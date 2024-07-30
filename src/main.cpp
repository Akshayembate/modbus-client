#include "modbus.h"
#include <iostream>

int main() {
    ModbusClient client("192.168.1.5", 503);

    if (!client.connect()) {
        std::cerr << "Failed to connect to Modbus server." << std::endl;
        return 1;
    }

    while (1)
    {

       std::vector<uint16_t> registers;
    if (client.readHoldingRegisters(0, 5, registers)) {
        std::cout << "Read holding registers:" << std::endl;
        for (auto reg : registers) {
            std::cout << reg << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Failed to read holding registers." << std::endl;
    }

   std::vector<uint16_t> values = {123, 456, 789, 1011, 1213};
    int quantity = values.size(); // Determine the quantity from the size of the values vector
    if (client.writeMultipleRegisters(15, quantity, values)) {
        std::cout << "Successfully wrote multiple registers." << std::endl;
    } else {
        std::cerr << "Failed to write multiple registers." << std::endl;
    }

     // Writing floating-point numbers
    std::vector<float> writeValues = {1.23f, 4.56f, 7.89f};
    if (client.writeMultipleRegistersFloat(20, writeValues)) {
        std::cout << "Successfully wrote floating-point numbers." << std::endl;
    } else {
        std::cerr << "Failed to write floating-point numbers." << std::endl;
    }

    // Reading floating-point numbers
    std::vector<float> readValues;
    if (client.readHoldingRegistersFloat(5, 3, readValues)) {
        std::cout << "Successfully read floating-point numbers:" << std::endl;
        for (const auto& value : readValues) {
            std::cout << value << " ";
        }
        std::cout << std::endl;
    } else {
        std::cerr << "Failed to read floating-point numbers." << std::endl;
    } 
    }

    client.disconnect();
    return 0;
}
