#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <iostream>
#include "core.h"
#include "assembler.h"
#include <string>

namespace py = pybind11;
class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(0, 84), Core(856, 943)}; // .text is of size 84 words
    Cache *cache;
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
    std::vector<std::map<std::string, std::string>> memory_states;

public:
    Processor(std::string file1, std::string file2, bool pipeline, bool forwarding, std::map<std::string, int> latencies, std::vector<int> cache_parameters);
    void evaluate(std::vector<State> &pipelined_instructions, int core, std::map<std::string, int> latencies);
    void run_unpipelined(int instructions_1_length, int instructions_2_length, std::map<std::string, int> latencies);
    void run_pipelined_wo_forwarding(std::map<std::string, int> latencies);
    void run_pipelined_with_forwarding(std::map<std::string, int> latencies);
    void save_stats();
    void save_mem();
    void write_pipeline_state(int core, std::vector<State> states);
    void write_memory_state();
    void process_pipeline_wo_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy);
    void process_pipeline_with_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy);

    std::vector<std::map<std::string, float>> getStats();
    std::vector<std::map<std::string, std::string>> getMemory();
    std::vector<std::vector<std::map<std::string, std::string>>> getPipeline();
    std::vector<std::vector<std::map<std::string, std::string>>> getRegisters();
};

Processor::Processor(std::string file1, std::string file2, bool pipeline, bool forwarding, std::map<std::string, int> latencies, std::vector<int> cache_parameters)
{
    srand(time(NULL)); // For random replacement policy
    clocks[0] = 0;
    clocks[1] = 0;
    clock = 0;
    hazard_count_0 = 0;
    hazard_count_1 = 0;
    stall_count_0 = 0;
    stall_count_1 = 0;
    instruction_count[1] = 0;
    instruction_count[0] = 0;
    pipeline_states = {{},
                       {}};
    cache = new Cache(cache_parameters[0], cache_parameters[1], cache_parameters[2], cache_parameters[3]);
    /*
        Initialize by adding programs to memory
    */
    int dataloc1 = 84;
    int dataloc2 = 943;
    std::pair<std::vector<int>, std::vector<int>> result1 = assembler.assemble(file1);
    std::pair<std::vector<int>, std::vector<int>> result2 = assembler.assemble(file2);

    std::vector<int> instructions1 = result1.first;
    std::vector<int> instructions2 = result2.first;

    std::vector<int> data1 = result1.second;
    std::vector<int> data2 = result2.second;

    // Saving .data to memory
    for (auto i : data1)
    {
        memory[dataloc1++] = i;
    }

    for (auto i : data2)
    {
        memory[dataloc2++] = i;
    }

    for (int index = 0; index < instructions1.size(); index++)
    {
        memory[index] = instructions1.at(index);
    }
    for (int index = 856; index - 856 < instructions2.size(); index++)
    {
        memory[index] = instructions2.at(index - 856);
    }

    std::ofstream MembFile("data/memory_before.txt");

    for (int i = 0; i < 1024; i++)
    {
        MembFile << i << "," << memory[i] << std::endl;
    }
    MembFile.close();
    if (pipeline)
    {
        if (forwarding)
        {
            run_pipelined_with_forwarding(latencies);
        }
        else
        {
            run_pipelined_wo_forwarding(latencies);
        }
    }
    else
    {
        run_unpipelined(instructions1.size(), instructions2.size(), latencies);
    }
    save_mem();
}
std::vector<std::vector<std::map<std::string, std::string>>> Processor::getRegisters()
{
    std::vector<std::vector<std::map<std::string, std::string>>> register_states;
    register_states.push_back(cores[0].register_states);
    register_states.push_back(cores[1].register_states);
    return register_states;
}
std::vector<std::vector<std::map<std::string, std::string>>> Processor::getPipeline()
{
    return pipeline_states;
}
std::vector<std::map<std::string, std::string>> Processor::getMemory()
{
    return memory_states;
}
void Processor::write_pipeline_state(int core, std::vector<State> states)
{
    int index = 0;
    std::map<std::string, std::string> temp;
    for (auto i : states)
    {
        if (i.is_dummy)
        {
            temp[std::to_string(index++)] = "stall";
        }

        else
        {
            temp[std::to_string(index++)] = std::to_string(i.pc);
        }
    }
    pipeline_states[core].push_back(temp);
    return;
}
void Processor::write_memory_state()
{
    std::map<std::string, std::string> temp;
    for (int i = 0; i < 1024; i++)
    {
        temp[std::to_string(i)] = std::to_string(memory[i]);
    }
}

void Processor::save_stats()
{
    std::ofstream Stats("data/stats.txt");
    Stats << instruction_count[0] << std::endl;
    Stats << instruction_count[1] << std::endl;
    Stats << hazard_count_0 << std::endl;
    Stats << hazard_count_1 << std::endl;
    Stats << clocks[0] << std::endl;
    Stats << clocks[1] << std::endl;
}

std::vector<std::map<std::string, float>> Processor::getStats()
{
    std::vector<std::map<std::string, float>> stat;
    std::map<std::string, float> temp;
    temp["IC"] = instruction_count[0];
    temp["HC"] = hazard_count_0;
    temp["SC"] = stall_count_0;
    temp["Clock"] = clocks[0];
    temp["IPC"] = (float)instruction_count[0] / clocks[0];
    stat.push_back(temp);
    temp["IC"] = instruction_count[1];
    temp["HC"] = hazard_count_1;
    temp["SC"] = stall_count_1;
    temp["Clock"] = clocks[1];
    temp["IPC"] = (float)instruction_count[1] / clocks[1];
    stat.push_back(temp);

    return stat;
}

void Processor::save_mem()
{
    std::ofstream MemFile("data/memory_after.txt");

    for (int i = 0; i < 1024; i++)
    {
        MemFile << i << "," << memory[i] << std::endl;
    }
    MemFile.close();
}
void Processor::evaluate(std::vector<State> &pipelined_instructions, int core, std::map<std::string, int> latencies)
{

    cores[core].writeback(pipelined_instructions[0], instruction_count[core]);

    cores[core].mem(pipelined_instructions[1], memory, cache);

    cores[core].execute(pipelined_instructions[2]);

    cores[core].decode(pipelined_instructions[3]);

    cores[core].fetch(memory, pipelined_instructions[4], cache);

    pipelined_instructions.erase(pipelined_instructions.begin());

    /*
        Adding latencies
    */
    if (pipelined_instructions[3].miss && !pipelined_instructions[3].fetch_latency && !pipelined_instructions[3].is_dummy)
    {
        pipelined_instructions[3].latency = latencies["fmiss"] - 1;
        pipelined_instructions[3].miss = false;
        pipelined_instructions[3].fetch_latency = true;
    }
    else if (!pipelined_instructions[3].miss && !pipelined_instructions[3].fetch_latency && !pipelined_instructions[3].is_dummy)
    {
        pipelined_instructions[3].latency = latencies["fhit"] - 1;
        pipelined_instructions[3].fetch_latency = true;
    }

    if (pipelined_instructions[2].opcode == "0110011")
    {
        if (pipelined_instructions[2].func3 == "000" && pipelined_instructions[2].func7 == "0000000")
        {
            pipelined_instructions[2].latency = latencies["add"];
        }
        else if (pipelined_instructions[2].func3 == "000" && pipelined_instructions[2].func7 == "0100000")
        {
            pipelined_instructions[2].latency = latencies["sub"];
        }
        else if (pipelined_instructions[2].func3 == "000" && pipelined_instructions[2].func7 == "0000001")
        {
            pipelined_instructions[2].latency = latencies["mul"];
        }
        else if (pipelined_instructions[2].func3 == "100" && pipelined_instructions[2].func7 == "0000001")
        {
            pipelined_instructions[2].latency = latencies["div"];
        }
    }
    else if (pipelined_instructions[2].opcode == "0010011")
    {
        pipelined_instructions[2].latency = latencies["addi"];
    }
    if (pipelined_instructions[0].miss && !pipelined_instructions[0].mem_latency && !pipelined_instructions[0].is_dummy)
    {
        pipelined_instructions[0].latency = latencies["mmiss"] - 1;
        pipelined_instructions[0].miss = false;
        pipelined_instructions[0].mem_latency = true;
    }
    else if (!pipelined_instructions[0].miss && !pipelined_instructions[0].mem_latency && !pipelined_instructions[0].is_dummy)
    {
        pipelined_instructions[0].latency = latencies["mhit"] - 1;
        pipelined_instructions[0].mem_latency = true;
    }
    // clock++;
    clocks[core]++;
}

void Processor::run_unpipelined(int instructions1_length, int instructions2_length, std::map<std::string, int> latencies)
{

    int i = 0, j = 0;
    int counter_0, counter_1;
    while (i < instructions1_length && j < instructions2_length)
    {
        counter_0 = 1;
        counter_1 = 1;
        cores[0].fetch(memory);
        cores[0].savereg(0);
        cores[1].fetch(memory);
        cores[1].savereg(1);
        cores[0].decode();
        cores[0].savereg(0);
        cores[1].decode();
        cores[1].savereg(1);
        i = cores[0].execute(latencies, counter_0);
        j = cores[1].execute(latencies, counter_1) - 856;
        cores[0].mem(memory);
        cores[0].savereg(0);
        cores[0].savereg(0);
        cores[1].mem(memory);
        cores[1].savereg(1);
        cores[1].savereg(1);
        while (counter_0--)
        {
            cores[0].savereg(0);
        }
        while (counter_1--)
        {
            cores[1].savereg(1);
        }
        instruction_count[0]++;
        instruction_count[1]++;
        clock++;
    }
    while (i < instructions1_length)
    {

        counter_0 = 1;
        cores[0].fetch(memory);
        cores[0].savereg(0);
        cores[0].decode();
        cores[0].savereg(0);
        i = cores[0].execute(latencies, counter_0);
        cores[0].mem(memory);
        cores[0].savereg(0);
        cores[0].savereg(0);
        while (counter_0--)
        {
            cores[0].savereg(0);
        }
        clock++;
    }
    while (j < instructions2_length)
    {
        counter_1 = 1;
        cores[1].fetch(memory);
        cores[1].savereg(1);
        cores[1].decode();
        cores[1].savereg(1);
        j = cores[1].execute(latencies, counter_1) - 856;
        cores[1].mem(memory);
        cores[1].savereg(1);
        cores[1].savereg(1);
        while (counter_1--)
        {
            cores[1].savereg(1);
        }
        clock++;
    }
}
void Processor::process_pipeline_wo_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy)
{

    write_pipeline_state(core, states);

    int hazard_count1 = 0;
    hazardDetector.hazard_without_forwarding(states, hazard_count1);

    hazard_count += hazard_count1;

    std::vector<State> oldstates = states;
    evaluate(states, core, latencies);
    if (states[0].latency > 0 && !states[0].is_dummy) // mem latency
    {
        states = {State(0), states[0], states[1], states[2], states[3]};
        states[0].is_dummy = true;
        stall_count++;
    }
    else if ((states[1].opcode == "1101111" || states[1].opcode == "1100011") && !states[1].is_dummy)
    {
        // Flush
        states[2].is_dummy = true;
        states[3].is_dummy = true;
        states.push_back(State(states[1].next_pc));
        stall_count += 2;
    }
    else if ((states[1].opcode == "0110011" || states[1].opcode == "0010011") && states[1].latency > 0 && !states[1].is_dummy)
    {
        states = {states[0], State(0), states[1], oldstates[3], states[3]};
        states[1].is_dummy = true;
        stall_count++;
    }
    else if (hazard_count1 > 0)
    {
        states = {states[0], states[1], State(0), oldstates[3], states[3]};
        states[2].is_dummy = true;
        stall_count++;
    }
    else if (states[3].latency > 0 && !states[3].is_dummy)
    {
        states = {states[0], states[1], states[2], State(0), states[3]}; // fetch latency
        states[3].is_dummy = true;
        stall_count++;
    }
    else
    {

        if (states[3].is_dummy)
        {
            states.push_back(State(0));
            states[4].is_dummy = true;
        }
        else
        {
            states.push_back(State(states[3].next_pc));
            if (memory[states[3].next_pc] == 0)
            {
                states[4].is_dummy = true;
            }
        }
        /*
            Exit condition
        */
        all_dummy = true;
        for (auto i : states)
        {
            if (!i.is_dummy)
            {
                all_dummy = false;
            }
        }
    }
    cores[core].savereg(core);
}
void Processor::run_pipelined_wo_forwarding(std::map<std::string, int> latencies)
{
    std::vector<State> states1 = {State(0), State(0), State(0), State(0), State(0)};
    std::vector<State> states2 = {State(856), State(856), State(856), State(856), State(856)};

    for (int i = 0; i < 4; i++)
    {
        states1[i].is_dummy = true;
        states2[i].is_dummy = true;
    }
    while (!all_dummy1 && !all_dummy2)
    {
        write_memory_state();
        process_pipeline_wo_forwarding(hazard_count_0, states1, latencies, 0, stall_count_0, all_dummy1);
        process_pipeline_wo_forwarding(hazard_count_1, states2, latencies, 1, stall_count_1, all_dummy2);
    }

    while (!all_dummy1)
    {
        write_memory_state();
        process_pipeline_wo_forwarding(hazard_count_0, states1, latencies, 0, stall_count_0, all_dummy1);
    }
    while (!all_dummy2)
    {
        write_memory_state();
        process_pipeline_wo_forwarding(hazard_count_1, states2, latencies, 1, stall_count_1, all_dummy2);
    }
}
void Processor::process_pipeline_with_forwarding(int &hazard_count, std::vector<State> &states, std::map<std::string, int> latencies, int core, long long int &stall_count, bool &all_dummy)
{
    write_pipeline_state(core, states);

    int hazard_count1 = 0;
    bool if_stall = false;
    int stall_pos = 2;
    hazardDetector.hazard_with_forwarding(states, hazard_count1, if_stall, stall_pos);
    hazard_count += hazard_count1;

    std::vector<State> oldstates = states;

    evaluate(states, core, latencies);

    if (states[0].latency > 0 && !states[0].is_dummy) // mem latency
    {
        states = {State(0), states[0], states[1], states[2], states[3]};
        states[0].is_dummy = true;
        stall_count++;
    }
    else if (if_stall)
    {
        if (stall_pos == 0)
        {
            states = {states[0], State(0), oldstates[2], oldstates[3], states[3]};
            states[4].pc = states[3].next_pc;
            states[1].is_dummy = true;
            stall_count++;
        }
        else if (stall_pos == 1)
        {
            states = {states[0], states[1], State(0), oldstates[3], states[3]};
            states[4].pc = states[3].next_pc;
            states[2].is_dummy = true;
            stall_count++;
        }
    }
    else
    {
        if (states[1].opcode == "1101111" || states[1].opcode == "1100011")
        {
            // Flush
            if (states[1].next_pc == states[2].pc)
            {
                states.push_back(State(states[3].next_pc));
                if (memory[states[3].next_pc] == 0)
                {
                    states[4].is_dummy = true;
                }
            }
            else
            {
                states[2].is_dummy = true;
                states[3].is_dummy = true;
                states.push_back(State(states[1].next_pc));
                stall_count += 2;
            }
        }
        else if ((states[1].opcode == "0110011" || states[1].opcode == "0010011") && states[1].latency > 0 && !states[1].is_dummy)
        {
            states = {states[0], State(0), states[1], oldstates[3], states[3]};
            states[4].pc = states[3].next_pc;
            states[1].is_dummy = true;
            stall_count++;
        }
        else if (states[3].latency > 0 && !states[3].is_dummy) // fetch latency
        {
            states = {states[0], states[1], states[2], State(0), states[3]};
            states[3].is_dummy = true;
            stall_count++;
        }
        else if (states[3].is_dummy)
        {
            states.push_back(State(0));
            states[4].is_dummy = true;
        }
        else
        {
            states.push_back(State(states[3].next_pc));
            if (memory[states[3].next_pc] == 0)
            {
                states[4].is_dummy = true;
            }
        }
        all_dummy = true;
        for (auto i : states)
        {
            if (!i.is_dummy)
            {
                all_dummy = false;
            }
        }
    }
    cores[core].savereg(core);
}
void Processor::run_pipelined_with_forwarding(std::map<std::string, int> latencies)
{
    std::vector<State> states1 = {State(0), State(0), State(0), State(0), State(0)};
    std::vector<State> states2 = {State(856), State(856), State(856), State(856), State(856)};

    for (int i = 0; i < 4; i++)
    {
        states1[i].is_dummy = true;
        states2[i].is_dummy = true;
    }

    while (!all_dummy1 && !all_dummy2)
    {
        write_memory_state();
        process_pipeline_with_forwarding(hazard_count_0, states1, latencies, 0, stall_count_0, all_dummy1);
        process_pipeline_with_forwarding(hazard_count_1, states2, latencies, 1, stall_count_1, all_dummy2);
    }
    while (!all_dummy1)
    {
        write_memory_state();
        process_pipeline_with_forwarding(hazard_count_0, states1, latencies, 0, stall_count_0, all_dummy1);
    }
    while (!all_dummy2)
    {
        write_memory_state();
        process_pipeline_with_forwarding(hazard_count_1, states2, latencies, 1, stall_count_1, all_dummy2);
    }
}
/*
    Binding required functions, to be exposed to python.
*/
PYBIND11_MODULE(processor, handle)
{
    handle.doc() = "A c++ class for a RISC-V simulator with pipelining, data-forwarding, variable latencies and more !!!";
    py::class_<Processor>(
        handle, "Processor")
        .def(py::init<std::string, std::string, bool, bool, std::map<std::string, int>, std::vector<int>>())
        .def("getStats", &Processor::getStats)
        .def("getPipeline", &Processor::getPipeline)
        .def("getRegisters", &Processor::getRegisters)
        .def("getMemory", &Processor::getMemory);
}