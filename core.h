#include <bitset>
#include <iostream>

#include <bits/stdc++.h>
#include <string>
class Core
{
private:
    // 32 - 32 bit registers
    int registers[32] = {0};
    int int_instruction;
    int pc;

public:
    Core();
    void fetch(int *memory);
    void execute(std::string opcode, std::string rd, std::string func3, std::string rs1, std::string rs2, std::string func7);
    void decode();
};

Core::Core()
{
    // registers = ;
    // registers[2] = 2147483632; // Stack pointer
    registers[2] = 2;
    // registers[3] = 268435456;  // Global Pointer
    registers[3] = 3;
    pc = 0;
}
void Core::fetch(int memory[])
{
    int_instruction = memory[pc];
    pc++;
}
void Core::decode()
{
    std::bitset<32> bin_instruction(int_instruction);
    std::string instruction = bin_instruction.to_string();

    std::string opcode = instruction.substr(25, 7);
    std::string rd = instruction.substr(20, 5);
    std::string func3 = instruction.substr(17, 3);
    std::string rs1 = instruction.substr(12, 5);
    std::string rs2 = instruction.substr(7, 5);
    std::string func7 = instruction.substr(0, 7);
    execute(opcode, rd, func3, rs1, rs2, func7);
}
void Core::execute(std::string opcode, std::string rd, std::string func3, std::string rs1, std::string rs2, std::string func7)
{
    if (opcode == "0110011")
    {
        if (func3 == "000" && func7 == "0000000")
        {
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "000" && func7 == "0100000")
        {
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] - registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "000" && func7 == "0000001")
        {
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] * registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "100" && func7 == "0000001")
        {
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] / registers[std::stoi(rs2, nullptr, 2)];
        }
        std::cout << registers[std::stoi(rd, nullptr, 2)] << " " << rd << std::endl;
    }
}