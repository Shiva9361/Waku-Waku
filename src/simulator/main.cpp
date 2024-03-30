// #define DEBUG 1
#define PRINT 1
#include "processor.h"
int main(int argc, char const *argv[])
{
    std::map<std::string, int> latencies = {{"add", 1}, {"addi", 1}, {"sub", 1}, {"mul", 1}, {"div", 1}, {"fmiss", 4}, {"fhit", 4}, {"mhit", 4}, {"mmiss", 4}};

    Processor processor("codes/selection_sort.s", "codes/bubble_sort.s", true, false, latencies, {4, 2, 2, 1});
    std::vector<std::vector<std::map<std::string, std::string>>> pipe = processor.getPipeline();
    std::vector<std::vector<std::map<std::string, std::string>>> reg = processor.getRegisters();
    for (auto i : pipe[1])
    {
        for (auto j : i)
        {
            std::cout << j.first << " " << j.second << std::endl;
        }
        std::cout << std::endl;
    }
    // for (auto i : reg[1])
    // {
    //     for (auto j : i)
    //     {
    //         std::cout << j.first << " " << j.second << std::endl;
    //     }
    //     std::cout << std::endl;
    // }

    return 0;
}
