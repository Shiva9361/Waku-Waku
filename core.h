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

    std::string opcode;
    std::string rs1;
    std::string rs2;
    std::string rd;
    std::string imm;
    std::string func3;
    std::string func7;

public:
    Core();
    void fetch(int *memory);
    void decode();
    void execute();
    void execute_R();
    void mem(int *memory);
};

Core::Core()
{
    // registers[2] = 2147483632; // Stack pointer
    registers[2] = 2;
    // registers[3] = 268435456;  // Global Pointer
    registers[3] = 3;
    registers[17] = 0;
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

    opcode = instruction.substr(25, 7);
    func3 = instruction.substr(17, 3);
    rs1 = instruction.substr(12, 5);
    if (opcode == "0000011")
    {
        std::cout << instruction << std::endl;
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 12);
    }
    else if (opcode == "0110011")
    {
        rd = instruction.substr(20, 5);
        rs2 = instruction.substr(7, 5);
        func7 = instruction.substr(0, 7);
    }
    else if (opcode == "0100011")
    {
        rs2 = instruction.substr(7, 5);
        imm = instruction.substr(0, 7) + instruction.substr(20, 5);
    }
}
void Core::execute()
{
    if (opcode == "0110011")
    {
        execute_R();
    }
}
void Core::execute_R()
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
    }
}
void Core::mem(int *memory)
{
    if (opcode == "0000011")
    {
        if (func3 == "010")
        {
            registers[std::stoi(rd, nullptr, 2)] = memory[registers[std::stoi(rs1, nullptr, 2)] + std::stoi(imm, nullptr, 2)];
        }
    }
    else if (opcode == "0100011")
    {
        if (func3 == "010")
        {
            memory[registers[std::stoi(rs2, nullptr, 2)] + std::stoi(imm, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)];
        }
    }
}