#include <bitset>
#include <iostream>

#include <bits/stdc++.h>
#include <string>

int bin_to_int(std::string bin)
{
    int n = bin.length();
    int r = 0;
    int w = 1;
    for (int i = n - 1; i >= 0; i--)
    {
        if (i == 1)
        {
            if (bin[i] == '1')
            {
                r += w;
            }
            w *= -2;
            continue;
        }
        else if (bin[i] == '1')
        {
            r += w;
        }
        w *= 2;
    }
    std::cout << std::endl;
    return r;
}
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
    Core(int pc);
    void fetch(int *memory);
    void decode();
    int execute();
    void mem(int *memory);
};

Core::Core(int pc)
{
    // registers[2] = 2147483632; // Stack pointer
    registers[2] = 2;
    // registers[3] = 268435456;  // Global Pointer
    registers[3] = 3;
    registers[17] = 0;
    this->pc = pc;
}
void Core::fetch(int memory[])
{
    int_instruction = memory[pc];
    // std::cout << int_instruction << std::endl;
    pc++;
}
void Core::decode()
{
    std::bitset<32> bin_instruction(int_instruction);
    std::string instruction = bin_instruction.to_string();
    // std::cout << instruction << std::endl;

    opcode = instruction.substr(25, 7);
    func3 = instruction.substr(17, 3);
    rs1 = instruction.substr(12, 5);
    if (opcode == "0000011")
    {

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
    else if (opcode == "1101111")
    {
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 20);
    }
    else if (opcode == "1100011")
    {
        rs2 = instruction.substr(7, 5);
        imm = instruction.substr(0, 7) + instruction.substr(20, 5);
    }
}

int Core::execute()
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
    else if (opcode == "1101111")
    {
        registers[std::stoi(rd, nullptr, 2)] = pc;
        int int_imm = bin_to_int(imm);
        if (int_imm >= 0)
        {
            pc = pc + bin_to_int(imm) - 1;
        }
        else
        {
            pc = pc + bin_to_int(imm);
        }

        std::cout << "Jumped to " << pc << std::endl;
    }
    else if (opcode == "1100011")
    {
        if (func3 == "001")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] != registers[std::stoi(rs2, nullptr, 2)])
            {
            }
        }
    }
    return pc;
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