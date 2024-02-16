#include <iostream>
#include "core.h"
#include "assembler.h"

class Processor
{
private:
    int memory[4096] = {0};
    Core cores[2] = {Core(), Core()};
    Assembler assembler;

public:
    Processor(std::string file1, std::string file2);
};

Processor::Processor(std::string file1, std::string file2)
{
    std::vector<int> instructions = assembler.assemble(file1);
    int index;
    for (int index = 0; index < instructions.size(); index++)
    {
        memory[index] = instructions.at(index);
    }
    for (int i = 0; i < 3; i++)
    {
        cores[0].fetch(memory);
        cores[0].decode();
    }
}
