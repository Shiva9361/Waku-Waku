#include <iostream>
#include "core.h"
class Processor{
    private:
        int memory[4096] = {0};
        Core cores[2] = {Core(),Core()};
    public:
        Processor();
};

Processor::Processor(){
    cores[0].fetch(memory);
    cores[0].decode();
}
