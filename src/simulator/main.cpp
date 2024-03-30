#define DEBUG 1
#include "processor.h"
int main(int argc, char const *argv[])
{
    std::map<std::string, int> latencies = {{"add", 1}, {"addi", 1}, {"sub", 1}, {"mul", 1}, {"div", 1}, {"fmiss", 3}, {"fhit", 3}};
    // std::cout << "bef p" << std::endl;
    Processor processor("codes/slot0.s", "codes/slot1.s", true, true, latencies, {2, 1, 2, 0});
    std::cout << "af p" << std::endl;
    std::vector<std::vector<std::map<std::string, std::string>>> pipe = processor.getPipeline();
    std::vector<std::vector<std::map<std::string, std::string>>> reg = processor.getRegisters();
    for (auto i : pipe[0])
    {
        for (auto j : i)
        {
            std::cout << j.first << " " << j.second << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "hello" << std::endl;
    return 0;
}
