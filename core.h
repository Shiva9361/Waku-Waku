#include <bitset>
#include <iostream>
#include "hazardDetector.h"
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
    int dataloc;
    int pc_i;

    std::string opcode;
    std::string rs1;
    std::string rs2;
    std::string rd;
    std::string imm;
    std::string func3;
    std::string func7;

public:
    int pc;
    Core(int pc,int dataloc);
    void savereg(int core);
    void fetch(int *memory);
    void fetch(int *memory,State &instruction);
    void decode();
    void decode(State &state);
    int execute();
    void execute(State &instruction);
    void mem(int *memory);
    void mem(State &instruction,int *memory);
    void writeback(State &instruction,int &instruction_count);
};

Core::Core(int pc,int dataloc)
{
    registers[0] = 0; // x0 is hardwired to 0
    // registers[2] = 2147483632; // Stack pointer
    registers[1] = 111;
    // registers[5] = 84*4;
    // // registers[3] = 268435456;  // Global Pointer
    // registers[3] = 3;
    // registers[17] = 8;
    this->pc = pc;
    this->pc_i = pc;
    this->dataloc = dataloc;
}
void Core::fetch(int memory[])
{
    int_instruction = memory[pc];
    pc++;
}

void Core::fetch(int memory[],State &state)
{
    if(state.is_dummy) return;
    int_instruction = memory[state.pc];
    std::bitset<32> bin_instruction(int_instruction);
    std::string instruction_string = bin_instruction.to_string();
 
    state.instruction = instruction_string;
    state.next_pc = state.pc+1; // no prediction for now
}

void Core::decode()
{
    std::bitset<32> bin_instruction(int_instruction);
    std::string instruction = bin_instruction.to_string();
    
    #ifdef PRINT
    std::cout << "IF: " << instruction << std::endl;
    #endif

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

void Core::decode(State &state){
    /*
    Control hazard for branch
    */
    if (state.is_dummy) return;
    state.opcode = state.instruction.substr(25, 7);
    state.func3 = state.instruction.substr(17, 3);
    state.rs1 = std::stoi(state.instruction.substr(12, 5),nullptr,2);
    if (state.opcode == "0000011")
    {

        state.rd = std::stoi(state.instruction.substr(20, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 12));
        state.write = true;
    }
    else if (state.opcode == "0110011")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5),nullptr,2);
        state.rs2 = std::stoi(state.instruction.substr(7, 5),nullptr,2);
        state.func7 = state.instruction.substr(0, 7);
        state.write = true;
    }
    else if (state.opcode == "0100011")
    {
        state.rs2 = std::stoi(state.instruction.substr(7, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 7) + state.instruction.substr(20, 5));
    }
    else if (state.opcode == "1101111")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 20));
        state.write = true;
    }
    else if (state.opcode == "1100011")
    {
        state.rs2 = std::stoi(state.instruction.substr(7, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 7) + state.instruction.substr(20, 5));
    }
    else if (state.opcode == "0010011")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 12));
        state.write = true;
    }
    else if (state.opcode == "0010111"){
        state.rd = std::stoi(state.instruction.substr(20, 5),nullptr,2);
        state.imm = bin_to_int(state.instruction.substr(0, 20));
        state.write = true;
    }
}

int Core::execute()
{
    if (opcode == "0110011" && rd != "00000")
    {
        if (func3 == "000" && func7 == "0000000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "add" << std::endl;
            #endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + registers[std::stoi(rs2, nullptr, 2)];
        }
        else if (func3 == "000" && func7 == "0100000")
        {   
            #ifdef PRINT
            std::cout << "ID: " << "sub" << std::endl;
            #endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] - registers[std::stoi(rs2, nullptr, 2)];
            
        }
        else if (func3 == "000" && func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "mul" << std::endl;
            #endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] * registers[std::stoi(rs2, nullptr, 2)];

        }
        else if (func3 == "100" && func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "div" << std::endl;
            #endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] / registers[std::stoi(rs2, nullptr, 2)];
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    else if (opcode == "1101111")
    {
        if (rd != "00000")
        {
            registers[std::stoi(rd, nullptr, 2)] = pc;
        }
        int int_imm = std::stoi(imm, nullptr, 2);
        #ifdef PRINT
        std::cout << "ID: " << "jal " << "pc: " << int_imm + pc_i << std::endl;
        #endif

        pc = int_imm + pc_i;
        #ifdef PRINT
        std::cout << "RF: ---" << std::endl; 
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    else if (opcode == "1100011")
    {
        if (func3 == "001")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] != registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "bne " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "000")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] == registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "beq " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "100")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] < registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "blt " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "101")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] >= registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "bge " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " "---" << std::endl;
        #endif
    }
    else if (opcode == "0010011")
    {
        if (func3 == "000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "addi " << "imm: " << bin_to_int(imm) << std::endl;
            #endif
            if (rd != "00000")
            {
                registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + bin_to_int(imm);
            }
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    return pc;
}
/*
    fix for branch instructions
*/
void Core::execute(State &state){
    
    if (state.is_dummy) return;
    if (state.opcode == "0110011")
    {
        
        if (state.func3 == "000" && state.func7 == "0000000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "add" << std::endl;
            #endif
            if (state.operand1 !=-1 && state.operand2!=-1){
                state.temp_reg = state.operand1 + state.operand2;
            }
            else if (state.operand1 !=-1){
                state.temp_reg = state.operand1 + registers[state.rs2];
            }
            else if (state.operand2 !=-1){
                state.temp_reg = registers[state.rs1] + state.operand2;
            }
            else {
                state.temp_reg = registers[state.rs1] + registers[state.rs2];
            }
            
            std::cout << state.temp_reg << " reg"<<std::endl;
        }
        else if (state.func3 == "000" && state.func7 == "0100000")
        {   
            #ifdef PRINT
            std::cout << "ID: " << "sub" << std::endl;
            #endif
            state.temp_reg = registers[state.rs1] - registers[state.rs2];
            
        }
        else if (state.func3 == "000" && state.func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "mul" << std::endl;
            #endif
            state.temp_reg = registers[state.rs1] * registers[state.rs2];

        }
        else if (state.func3 == "100" && state.func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "div" << std::endl;
            #endif
            state.temp_reg = registers[state.rs1] / registers[state.rs2];
        }

    }
    else if (state.opcode == "1101111")
    {
        
        state.temp_reg = state.pc;
        int int_imm = state.imm;
        #ifdef PRINT
        std::cout<<"IMM "<<int_imm<<std::endl;
        std::cout << "ID: " << "jal " << "pc: " << int_imm + pc_i << std::endl;
        #endif

        state.next_pc = int_imm + pc_i;
    
    }
    else if (state.opcode == "1100011")
    {
        if (state.func3 == "001")
        {
            if (registers[state.rs1] != registers[state.rs2])
            {
                int int_imm = state.imm;
                #ifdef PRINT
                std::cout << "ID: " << "bne " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "000")
        {
            if (registers[state.rs1] == registers[state.rs2])
            {
                int int_imm = state.imm;
                #ifdef PRINT
                std::cout << "ID: " << "beq " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "100")
        {
            if (registers[state.rs1] < registers[state.rs2])
            {
                int int_imm = state.imm;
                #ifdef PRINT
                std::cout << "ID: " << "blt " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "101")
        {
            if (registers[state.rs1] >= registers[state.rs2])
            {
                int int_imm = state.imm;
                #ifdef PRINT
                std::cout << "ID: " << "bge " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                state.next_pc = int_imm + pc_i;
            }
        }

    }
    else if (state.opcode == "0010011")
    {
        if (state.func3 == "000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "addi " << "imm: " << bin_to_int(imm) << std::endl;
            #endif
            if (state.rd != 0)
            {
                state.temp_reg = registers[state.rs1] + state.imm;
            }
        }

    }
    return;
}
void Core::mem(int *memory)
{
    if (opcode == "0000011")
    {
        if (func3 == "010")
        {
            #ifdef PRINT
            std::cout << "ID: " << "lw " << "imm: " << bin_to_int(imm) << std::endl;
            #endif

            if (rd != "00000"){
                registers[std::stoi(rd, nullptr, 2)] = memory[registers[std::stoi(rs1, nullptr, 2)]/4 + bin_to_int(imm)/4];
            }
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "loaded from index " << registers[std::stoi(rs1, nullptr, 2)]/4 + bin_to_int(imm)/4 << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    else if (opcode == "0100011")
    {
        if (func3 == "010")
        {
            #ifdef PRINT
            std::cout << "ID: " << "sw " << "imm: " << bin_to_int(imm) << std::endl;
            #endif
            memory[registers[std::stoi(rs2, nullptr, 2)]/4 + bin_to_int(imm)/4] = registers[std::stoi(rs1, nullptr, 2)];
            #ifdef PRINT
            std::cout << "RF: " << "x" << std::stoi(rs1, nullptr, 2) << ", " << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
            std::cout << "EXE: " << "executed" << std::endl;
            std::cout << "MEM: " << "stored at index " << registers[std::stoi(rs2, nullptr, 2)]/4 + bin_to_int(imm)/4 << std::endl;
            std::cout << "WB: " "---" << std::endl;
            #endif 
        }
    }
    else if (opcode == "0010111"){
        #ifdef PRINT
        std::cout << "ID: " << "la " << "imm: " << bin_to_int(imm) << std::endl;
        std::cout << "RF: " << "---" << std::endl;
        std::cout << "EXE: " << "---" << std::endl;
        #endif
        registers[std::stoi(rd, nullptr, 2)] = (dataloc+bin_to_int(imm))*4;
        
        #ifdef PRINT
        std::cout << "MEM: " << "loaded address from index " << dataloc+bin_to_int(imm) << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    #ifdef PRINT
    std::cout << std::endl;
    #endif
}

void Core::mem(State &state,int *memory){
    
    if (state.is_dummy) return;
    
    if (state.opcode == "0000011")
    {
        if (state.func3 == "010")
        {
            if (state.rd != 0){
                state.temp_reg = memory[registers[state.rs1]/4 + state.imm/4];
            }
        }
    }
    else if (state.opcode == "0100011")
    {
        if (state.func3 == "010")
        {
            memory[registers[state.rs2]/4 + state.imm/4] = registers[state.rs1];
            std::cout<<"Wrote to mem"<<registers[state.rs1]<<std::endl;
        }
    }
    else if (state.opcode == "0010111"){
        state.temp_reg = (dataloc+state.imm)*4;
    } 
}

void Core::writeback(State &state,int &instruction_count){
    if (state.is_dummy) return;
    instruction_count+=1;
    if (state.write && state.rd!=0){
        registers[state.rd] = state.temp_reg; 
        std::cout<<"Wrote "<<state.temp_reg<<" TO "<<state.rd<<std::endl;
    }
}
void Core::savereg(int core){
    std::ofstream File("core0.txt",std::ios_base::app);
    if (core == 1) {
        File.close();
        File.open("core1.txt",std::ios_base::app);
    }

    for (int i = 0; i<32;i++){
        File<< "x"<<i<<" "<<registers[i]<<"\t";
    }
    File<<"\n";
    File.close();
}