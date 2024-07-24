#ifndef ASSEMBLER
#define ASSEMBLER

#include <fstream>
#include <vector>
#include <map>
#include "util.h"

class Assembler
{
private:
    std::map<std::string, std::string> lookup_table;

public:
    std::ifstream File;

    void display(std::string file);
    std::pair<std::vector<int>, std::vector<int>> assemble(std::string file);
    Assembler();
};

#endif