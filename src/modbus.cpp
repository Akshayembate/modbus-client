#include "modbus.h"
#include <ws2tcpip.h>  // Include this header for InetPton
#include <stdexcept>
#include <cstring> 

#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0601 // Define the minimum platform as Windows 7 or higher
#endif

ModbusClient::ModbusClient(const std::string& ip, int port) : ip(ip), port(port), sock(INVALID_SOCKET) {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        throw std::runtime_error("WSAStartup failed");
    }
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    serverAddr.sin_addr.s_addr = inet_addr(ip.c_str());
    if (serverAddr.sin_addr.s_addr == INADDR_NONE) {
        throw std::runtime_error("Invalid IP address");
    }
}

ModbusClient::~ModbusClient() {
    disconnect();
    WSACleanup();
}

bool ModbusClient::connect() {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        return false;
    }
    if (::connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        closesocket(sock);
        sock = INVALID_SOCKET;
        return false;
    }
    return true;
}

void ModbusClient::disconnect() {
    if (sock != INVALID_SOCKET) {
        closesocket(sock);
        sock = INVALID_SOCKET;
    }
}

bool ModbusClient::sendRequest(const std::vector<uint8_t>& request, std::vector<uint8_t>& response) {
    if (send(sock, reinterpret_cast<const char*>(request.data()), request.size(), 0) == SOCKET_ERROR) {
        return false;
    }

    char buffer[256];
    int bytesReceived = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesReceived <= 0) {
        return false;
    }

    response.assign(buffer, buffer + bytesReceived);
    return true;
}

bool ModbusClient::readHoldingRegisters(int address, int quantity, std::vector<uint16_t>& registers) {
    std::vector<uint8_t> request = {
        0x00, 0x01,             // Transaction ID
        0x00, 0x00,             // Protocol ID
        0x00, 0x06,             // Length
        0x01,                   // Unit ID
        0x03,                   // Function Code
        static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF)
    };

    std::vector<uint8_t> response;
    if (!sendRequest(request, response)) {
        return false;
    }

    if (response.size() < 9 || response[7] != 0x03) {
        return false;
    }

    registers.resize(quantity);
    for (int i = 0; i < quantity; ++i) {
        registers[i] = (response[9 + i * 2] << 8) | response[10 + i * 2];
    }
    return true;
}

bool ModbusClient::writeSingleRegister(int address, uint16_t value) {
    std::vector<uint8_t> request = {
        0x00, 0x01,             // Transaction ID
        0x00, 0x00,             // Protocol ID
        0x00, 0x06,             // Length
        0x01,                   // Unit ID
        0x06,                   // Function Code
        static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>(value >> 8), static_cast<uint8_t>(value & 0xFF)
    };

    std::vector<uint8_t> response;
    return sendRequest(request, response) && response.size() >= 12;
}

// write multiple holding registers 
bool ModbusClient::writeMultipleRegisters(int address, int quantity, const std::vector<uint16_t>& values) {
    std::vector<uint8_t> request = {
        0x00, 0x01,             // Transaction ID
        0x00, 0x00,             // Protocol ID
        0x00, static_cast<uint8_t>(7 + quantity * 2), // Length
        0x01,                   // Unit ID
        0x10,                   // Function Code for writing multiple registers
        static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>(quantity >> 8), static_cast<uint8_t>(quantity & 0xFF),
        static_cast<uint8_t>(quantity * 2)  // Byte count
    };

    for (const auto& value : values) {
        request.push_back(static_cast<uint8_t>(value >> 8));
        request.push_back(static_cast<uint8_t>(value & 0xFF));
    }

    std::vector<uint8_t> response;
    return sendRequest(request, response) && response.size() >= 12;
}

//read flotting point values 
bool ModbusClient::readHoldingRegistersFloat(int address, int quantity, std::vector<float>& values) {
    std::vector<uint16_t> tempRegisters(quantity * 2); // each float occupies 2 registers
    if (!readHoldingRegisters(address, quantity * 2, tempRegisters)) {
        return false;
    }

    values.resize(quantity);
    for (int i = 0; i < quantity; ++i) {
        uint32_t temp = (tempRegisters[i * 2] << 16) | tempRegisters[i * 2 + 1];
        float value;
        std::memcpy(&value, &temp, sizeof(float));
        values[i] = value;
    }
    return true;
}

// write float
bool ModbusClient::writeMultipleRegistersFloat(int address, const std::vector<float>& values) {
    int quantity = values.size();
    std::vector<uint8_t> request = {
        0x00, 0x01,             // Transaction ID
        0x00, 0x00,             // Protocol ID
        0x00, static_cast<uint8_t>(7 + quantity * 4), // Length (each float is 4 bytes)
        0x01,                   // Unit ID
        0x10,                   // Function Code for writing multiple registers
        static_cast<uint8_t>(address >> 8), static_cast<uint8_t>(address & 0xFF),
        static_cast<uint8_t>((quantity * 2) >> 8), static_cast<uint8_t>((quantity * 2) & 0xFF),
        static_cast<uint8_t>(quantity * 4)  // Byte count
    };

    for (const auto& value : values) {
        uint32_t temp;
        std::memcpy(&temp, &value, sizeof(float));
        request.push_back(static_cast<uint8_t>(temp >> 24));
        request.push_back(static_cast<uint8_t>((temp >> 16) & 0xFF));
        request.push_back(static_cast<uint8_t>((temp >> 8) & 0xFF));
        request.push_back(static_cast<uint8_t>(temp & 0xFF));
    }

    std::vector<uint8_t> response;
    return sendRequest(request, response) && response.size() >= 12;
}