#include "processor.h"
int main(int argc, char *argv[])
{
    std::string file1 = "codes/bubble_sort.s", file2 = "codes/selection_sort.s";
    if (argc > 2)
    {
        file1 = (std::string)argv[1];
        file2 = (std::string)argv[2];
    }
    bool pipelined = false;
    bool forwarding = false;
    if ((std::string)argv[3] == "true")
        pipelined = true;
    if ((std::string)argv[4] == "true")
        forwarding = true;
    int addi_latency, add_latency, div_latency, mul_latency, sub_latency;
    std::cin >> addi_latency >> add_latency >> div_latency >> mul_latency >> sub_latency;
    std::map<std::string, int> latencies;

    latencies["addi"] = addi_latency;
    latencies["add"] = add_latency;
    latencies["div"] = div_latency;
    latencies["mul"] = mul_latency;
    latencies["sub"] = sub_latency;

    Processor processsor(file1, file2, pipelined, forwarding, latencies);
    return 0;
}