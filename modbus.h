#ifndef MODBUS_H
#define MODBUS_H

#include <winsock2.h>
#include <iostream>
#include <vector>

class ModbusClient {
public:
    ModbusClient(const std::string& ip, int port);
    ~ModbusClient();

    bool connect();
    void disconnect();

    bool readHoldingRegisters(int address, int quantity, std::vector<uint16_t>& registers);
    bool writeSingleRegister(int address, uint16_t value);
    bool writeMultipleRegisters(int address, int quantity, const std::vector<uint16_t>& values);
    bool writeMultipleRegistersFloat(int address, const std::vector<float>& values);
    bool readHoldingRegistersFloat(int address, int quantity, std::vector<float>& values);


private:
    std::string ip;
    int port;
    SOCKET sock;
    sockaddr_in serverAddr;

    bool sendRequest(const std::vector<uint8_t>& request, std::vector<uint8_t>& response);
};

#endif // MODBUS_H
