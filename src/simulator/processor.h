#ifndef PROCESSOR
#define PROCESSOR

#include <iostream>
#include <string>
#include "cache.h"
#include "core.h"
#include "assembler.h"
#include "branchPredictor.h"
#include "hazardDetector.h"

class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(0, 84), Core(856, 943)}; // .text is of size 84 words
    int clocks[2];
    int clock;
    bool all_dummy1;
    bool all_dummy2;
    int instruction_count[2];
    int hazard_count_1;
    int hazard_count_0;
    long long int stall_count_1;
    long long int stall_count_0;
    Assembler assembler;
    HazardDetector hazardDetector;
    std::vector<std::vector<std::map<std::string, std::string>>> pipeline_states;
    std::vector<std::unordered_map<int, std::pair<int, int>>> memory_states;
    std::unordered_map<int, int> initial_memory;
    BranchPredictor branchPredictor;

public:
    Processor(std::string file1, std::string file2, bool pipeline, bool forwarding, std::map<std::string, int> latencies, std::vector<int> cache_parameters);
    void evaluate(std::vector<State> &pipelined_instructions, int core, std::map<std::string, int> latencies);
    void run_unpipelined(int instructions_1_length, int instructions_2_length, std::map<std::string, int> latencies);
    void run_pipelined_wo_forwarding(std::map<std::string, int> latencies);
    void run_pipelined_with_forwarding(std::map<std::string, int> latencies);
    void save_stats();
    void save_mem();
    void write_pipeline_state(int core, std::vector<State> states);
    void process_pipeline_wo_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy);
    void process_pipeline_with_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy);
    void resolve_branch_instructions(std::vector<State> &states, long long int &stall_count);
    /*
        GUI Specific functions
    */
    std::vector<cache_state_type> getCache();
    std::vector<std::map<std::string, float>> getStats();
    std::vector<incremental_data> getMemory();
    std::vector<incremental_data> getRegisters();
    std::vector<std::vector<std::map<std::string, std::string>>> getPipeline();
    std::unordered_map<int, int> getInitialMemory();
};
#endif