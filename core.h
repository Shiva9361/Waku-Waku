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
    return r;
}

class Core
{
private:
    // 32 - 32 bit registers
    int registers[32] = {0};
    int int_instruction;
    int pc;
    int dataloc;

    std::string opcode;
    std::string rs1;
    std::string rs2;
    std::string rd;
    std::string imm;
    std::string func3;
    std::string func7;

public:
    Core(int pc,int dataloc);
    void fetch(int *memory);
    void decode();
    int execute();
    void mem(int *memory);
};

Core::Core(int pc,int dataloc)
{
    registers[0] = 0; // x0 is hardwired to 0
    // registers[2] = 2147483632; // Stack pointer
    registers[5] = 84*4;
    // registers[3] = 268435456;  // Global Pointer
    registers[3] = 3;
    registers[17] = 8;
    this->pc = pc;
    this->dataloc = dataloc;
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
    std::cout << "IF: " << instruction << std::endl;

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
    else if (opcode == "0010011")
    {
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 12);
    }
    else if (opcode == "0010111"){
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 20);
    }
}

int Core::execute()
{
    if (opcode == "0110011" && rd != "00000")
    {
        if (func3 == "000" && func7 == "0000000")
        {
            std::cout << "ID: " << "add" << std::endl;
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "000" && func7 == "0100000")
        {
            std::cout << "ID: " << "sub" << std::endl;
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] - registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "000" && func7 == "0000001")
        {
            std::cout << "ID: " << "mul" << std::endl;
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] * registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "100" && func7 == "0000001")
        {
            std::cout << "ID: " << "div" << std::endl;
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] / registers[std::stoi(rs2, nullptr, 2)];
        }
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
    }
    else if (opcode == "1101111")
    {
        if (rd != "00000")
        {
            registers[std::stoi(rd, nullptr, 2)] = pc;
        }
        int int_imm = bin_to_int(imm);
        std::cout << "ID: " << "jal " << "imm: " << int_imm << std::endl;
        if (int_imm >= 0)
        {
            pc = pc + (bin_to_int(imm) - 2);
        }
        else
        {
            pc = pc + (bin_to_int(imm)) ;
        }
        std::cout << "RF: ---" << std::endl; 
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
    }
    else if (opcode == "1100011")
    {
        if (func3 == "001")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] != registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = bin_to_int(imm);
                std::cout << "ID: " << "bne " << "imm: " << int_imm << std::endl;
                if (int_imm >= 0)
                {
                    pc = pc + (bin_to_int(imm) - 2);
                }
                else
                {
                    pc = pc + (bin_to_int(imm));
                }
                
            }
        }
        else if (func3 == "000")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] == registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = bin_to_int(imm);
                std::cout << "ID: " << "beq " << "imm: " << int_imm << std::endl;
                if (int_imm >= 0)
                {
                    pc = pc + (bin_to_int(imm) - 2);
                }
                else
                {
                    pc = pc + (bin_to_int(imm));
                }
                
            }
        }
        else if (func3 == "100")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] < registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = bin_to_int(imm);
                std::cout << "ID: " << "blt " << "imm: " << int_imm << std::endl;
                if (int_imm >= 0)
                {
                    pc = pc + (bin_to_int(imm) - 2);
                }
                else
                {
                    pc = pc + (bin_to_int(imm));
                }
                
            }
        }
        else if (func3 == "101")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] >= registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = bin_to_int(imm);
                std::cout << "ID: " << "bge " << "imm: " << int_imm << std::endl;
                if (int_imm >= 0)
                {
                    pc = pc + (bin_to_int(imm) - 2);
                }
                else
                {
                    pc = pc + (bin_to_int(imm));
                }
                
            }
        }
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " "---" << std::endl;
    }
    else if (opcode == "0010011")
    {
        if (func3 == "000")
        {
            std::cout << "ID: " << "addi " << "imm: " << bin_to_int(imm) << std::endl;
            if (rd != "00000")
            {
                registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + bin_to_int(imm);
            }
        }
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
    }
    return pc;
}
void Core::mem(int *memory)
{
    if (opcode == "0000011")
    {
        if (func3 == "010")
        {
            std::cout << "ID: " << "lw " << "imm: " << bin_to_int(imm) << std::endl;
            if (rd != "00000"){
                registers[std::stoi(rd, nullptr, 2)] = memory[registers[std::stoi(rs1, nullptr, 2)]/4 + bin_to_int(imm)/4];
            }
        }
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "loaded from index " << registers[std::stoi(rs1, nullptr, 2)]/4 + bin_to_int(imm)/4 << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
    }
    else if (opcode == "0100011")
    {
        if (func3 == "010")
        {
            std::cout << "ID: " << "sw " << "imm: " << bin_to_int(imm) << std::endl;
            memory[registers[std::stoi(rs2, nullptr, 2)]/4 + bin_to_int(imm)/4] = registers[std::stoi(rs1, nullptr, 2)];
            std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
            std::cout << "EXE: " << "executed" << std::endl;
            std::cout << "MEM: " << "stored at index " << registers[std::stoi(rs2, nullptr, 2)]/4 + bin_to_int(imm)/4 << std::endl;
            std::cout << "WB: " "---" << std::endl;
        }
    }
    else if (opcode == "0010111"){
        std::cout << "ID: " << "la " << "imm: " << bin_to_int(imm) << std::endl;
        std::cout << "RF: " << "---" << std::endl;
        std::cout << "EXE: " << "---" << std::endl;
        registers[std::stoi(rd, nullptr, 2)] = (dataloc+bin_to_int(imm))*4;
        std::cout << "MEM: " << "loaded address from index " << dataloc+bin_to_int(imm) << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
    }
    std::cout << std::endl;
}