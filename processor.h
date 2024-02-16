#include <iostream>
#include "core.h"
#include "assembler.h"

class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(), Core()};
    int clock;
    Assembler assembler;

public:
    Processor(std::string file1, std::string file2);
};

Processor::Processor(std::string file1, std::string file2)
{
    clock = 0;
    std::vector<int> instructions = assembler.assemble(file1);
    int index;
    for (int index = 0; index < instructions.size(); index++)
    {
        memory[index] = instructions.at(index);
    }
    memory[8] = 7;
    for (int i = 0; i < 4; i++)
    {
        cores[0].fetch(memory);
        cores[0].decode();
        cores[0].execute();
        cores[0].mem(memory);
        clock++;
    }
}
