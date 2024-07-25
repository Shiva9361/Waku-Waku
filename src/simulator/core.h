#ifndef CORE
#define CORE
#include <unordered_map>
#include <map>
#include <memory>
#include <string>
#include "cache.h"
#include "state.h"

#define incremental_data std::unordered_map<int, std::pair<int, int>>

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
    std::unique_ptr<Cache> cache;
    Core(int pc, int dataloc);
    /*
        Unpipelined
    */
    void savereg();
    void fetch(int *memory);
    void init_cache(int cache_size, int block_size, int associativity, int policy, int alt_cache);
    void decode();
    int execute(std::map<std::string, int> latencies, int &counter);
    void mem(int *memory);
    std::vector<std::map<std::string, std::string>> register_states;
    /*
        Pipelined
    */
    void fetch(int *memory, State &instruction, int cycle);
    void decode(State &state);
    void execute(State &instruction);
    void mem(State &instruction, int *memory, std::unordered_map<int, std::pair<int, int>> &memorystate, int cycle);
    void writeback(State &instruction, int &instruction_count, int core);

    bool predict(int pc);

    incremental_data register_states_incremental;
};
#endif