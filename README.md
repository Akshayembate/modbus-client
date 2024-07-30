# Modbus Client

This project provides a Modbus client implementation in C++ for communicating with a Modbus server. It supports reading and writing multiple holding registers, including handling floating-point numbers.

## Features

- Connect to a Modbus server
- Read multiple holding registers
- Write multiple holding registers
- Read floating-point numbers from holding registers
- Write floating-point numbers to holding registers

## Requirements

- MinGW (for Windows)
- g++
- Windows Sockets 2 (WinSock2) library

## Setup

### Prerequisites

1. Install MinGW:
   - Download from [MinGW official website](http://www.mingw.org/).
   - Follow the installation instructions.

2. Ensure MinGW `bin` directory is added to your system's PATH.

### Project Structure

modbus_client/
├── src/
│ ├── main.cpp
│ ├── modbus.cpp
│ ├── modbus.h
├── .vscode/
│ ├── c_cpp_properties.json
│ ├── launch.json
│ ├── tasks.json
├── README.md


### Configuration Files

- `.vscode/c_cpp_properties.json`: Configures IntelliSense for the project.
- `.vscode/launch.json`: Configures the debugger for the project.
- `.vscode/tasks.json`: Configures build tasks for the project.

### Building the Project

1. Open a terminal or command prompt.
2. Navigate to the root directory of the project.
3. Run the following command to compile the project:

    ```sh
    g++ -fdiagnostics-color=always -g -IC:\path\to\modbus_client\src C:\path\to\modbus_client\src\main.cpp C:\path\to\modbus_client\src\modbus.cpp -o C:\path\to\modbus_client\src\main.exe -lws2_32
    ```

   Replace `C:\path\to\modbus_client` with the actual path to your project directory.

### Running the Project

1. After compiling, navigate to the `src` directory.
2. Run the executable:

    ```sh
    ./main.exe
    ```

## Usage

### ModbusClient Class

The `ModbusClient` class provides methods to connect to a Modbus server and perform read/write operations on holding registers.

#### Connecting to the Server

```cpp
ModbusClient client("192.168.0.1", 502);

if (!client.connect()) {
    std::cerr << "Failed to connect to Modbus server." << std::endl;
    return 1;
}

Writing Multiple Holding Registers

std::vector<uint16_t> values = {123, 456, 789, 1011, 1213};
if (client.writeMultipleRegisters(15, values.size(), values)) {
    std::cout << "Successfully wrote multiple registers." << std::endl;
} else {
    std::cerr << "Failed to write multiple registers." << std::endl;
}

Reading Multiple Holding Registers
std::vector<uint16_t> readValues;
if (client.readHoldingRegisters(15, 5, readValues)) {
    std::cout << "Successfully read multiple registers:" << std::endl;
    for (const auto& value : readValues) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
} else {
    std::cerr << "Failed to read multiple registers." << std::endl;
}

Writing Floating-Point Numbers

std::vector<float> writeValues = {1.23f, 4.56f, 7.89f};
if (client.writeMultipleRegistersFloat(15, writeValues)) {
    std::cout << "Successfully wrote floating-point numbers." << std::endl;
} else {
    std::cerr << "Failed to write floating-point numbers." << std::endl;
}

Reading Floating-Point Numbers
std::vector<float> readFloatValues;
if (client.readHoldingRegistersFloat(15, 3, readFloatValues)) {
    std::cout << "Successfully read floating-point numbers:" << std::endl;
    for (const auto& value : readFloatValues) {
        std::cout << value << " ";
    }
    std::cout << std::endl;
} else {
    std::cerr << "Failed to read floating-point numbers." << std::endl;
}

