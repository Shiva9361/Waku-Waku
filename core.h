#include<bitset>
#include<iostream>

#include<bits/stdc++.h>
#include<string>
class Core{
    private:
        //32 - 32 bit registers
        int registers[32]={0};
        int instruction_register;
        int pc;
    public:
        Core();
        void fetch(int *memory);
        void execute(std::string opcode,std::string rd,std::string func3,std::string rs1,std::string rs2,std::string func7);
        void decode();
    
};

Core::Core(){
    //registers = ;
    registers[2] = 2147483632; // Stack pointer
    registers[3] = 268435456; // Global Pointer
    pc = 0;
}
void Core::fetch(int memory[]){
    //instruction_register = memory[pc];pc++;
    instruction_register = 51;
}
void Core::decode(){
    std::bitset<32>instruction(instruction_register);
    std::string instruction_string = instruction.to_string();
    
    std::string opcode = instruction_string.substr(25,7);
    std::string rd = instruction_string.substr(20,5);
    std::string func3 = instruction_string.substr(17,3);
    std::string rs1 = instruction_string.substr(12,5);
    std::string rs2 = instruction_string.substr(7,5);
    std::string func7 = instruction_string.substr(0,7);
    execute(opcode,rd,func3,rs1,rs2,func7);
    
}
void Core::execute(std::string opcode,std::string rd,std::string func3,std::string rs1,std::string rs2,std::string func7){
    if(opcode=="0110011"){
        if (func3 == "000" && func7 == "0000000"){
            registers[std::stoi(rd,nullptr,2)] = registers[std::stoi(rs1,nullptr,2)] + registers[std::stoi(rs2,nullptr,2)]; 
        }
        else if (func3 == "000" && func7 == "0100000"){
            registers[std::stoi(rd,nullptr,2)] = registers[std::stoi(rs1,nullptr,2)] - registers[std::stoi(rs2,nullptr,2)]; 
        }
        else if (func3 == "000" && func7 == "0000001"){
            registers[std::stoi(rd,nullptr,2)] = registers[std::stoi(rs1,nullptr,2)] * registers[std::stoi(rs2,nullptr,2)]; 
        }
        else if (func3 == "100" && func7 == "0000001"){
            registers[std::stoi(rd,nullptr,2)] = registers[std::stoi(rs1,nullptr,2)] / registers[std::stoi(rs2,nullptr,2)]; 
        }
    }
}