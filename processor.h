#include <iostream>
#include "core.h"
#include "assembler.h"

class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(0, 84), Core(856, 943)}; // .text is of size 84 words
    int clock_0;
    int clock_1;
    int clock;
    bool all_dummy1;
    bool all_dummy2;
    int instruction_count_1;
    int instruction_count_0;
    int hazard_count_1;
    int hazard_count_0;
    Assembler assembler;
    HazardDetector hazardDetector;

public:
    Processor(std::string file1, std::string file2, bool pipeline, bool forwarding, std::map<std::string, int> latencies);
    void evaluate(std::vector<State> &pipelined_instructions, int core, std::map<std::string, int> latencies);
    void run_unpipelined(int instructions_1_length, int instructions_2_length, std::map<std::string, int> latencies);
    void run_pipelined_wo_forwarding(std::map<std::string, int> latencies);
    void run_pipelined_with_forwarding(std::map<std::string, int> latencies);
    void save_stats();
    void save_mem();
    void write_pipeline_file(int core, std::vector<State> states);
};

Processor::Processor(std::string file1, std::string file2, bool pipeline, bool forwarding, std::map<std::string, int> latencies)
{

    clock_0 = 0;
    clock = 0;
    clock_1 = 0;
    hazard_count_0 = 0;
    hazard_count_1 = 0;
    instruction_count_1 = 0;
    instruction_count_0 = 0;
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

void Processor::write_pipeline_file(int core, std::vector<State> states)
{
    std::ofstream pipefile;
    pipefile.open("data/core1_pipe.txt", std::ios_base::app);
    if (core == 0)
    {
        pipefile.close();
        pipefile.open("data/core0_pipe.txt", std::ios_base::app);
    }
    for (auto i : states)
    {
        if (i.is_dummy)
            pipefile << "-1 ";
        else
            pipefile << i.pc << " ";
    }
    pipefile << std::endl;
    pipefile.close();
    return;
}

void Processor::save_stats()
{
    std::ofstream Stats("data/stats.txt");
    Stats << instruction_count_0 << std::endl;
    Stats << instruction_count_1 << std::endl;
    Stats << hazard_count_0 << std::endl;
    Stats << hazard_count_1 << std::endl;
    Stats << clock_0 << std::endl;
    Stats << clock_1 << std::endl;
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
    if (core == 0)
    {
        cores[core].writeback(pipelined_instructions[0], instruction_count_0);
    }
    else
    {
        cores[core].writeback(pipelined_instructions[0], instruction_count_1);
    }
    // std::cout<<"did wb"<<std::endl;
    cores[core].mem(pipelined_instructions[1], memory);
    // std::cout<<"did mem"<<std::endl;
    cores[core].execute(pipelined_instructions[2]);
    // std::cout<<"did exe"<<std::endl;
    cores[core].decode(pipelined_instructions[3]);
    // std::cout<<"did dec"<<std::endl;
    cores[core].fetch(memory, pipelined_instructions[4]);
    // std::cout<<"did fe"<<std::endl;
    pipelined_instructions.erase(pipelined_instructions.begin());

    /*
        Adding latencies
    */

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

    // clock++;
    if (core == 0)
    {
        clock_0++;
    }
    else
    {
        clock_1++;
    }
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
        instruction_count_0++;
        instruction_count_1++;
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

void Processor::run_pipelined_wo_forwarding(std::map<std::string, int> latencies)
{
    std::vector<State> states1 = {State(0), State(0), State(0), State(0), State(0)};
    std::vector<State> states2 = {State(856), State(856), State(856), State(856), State(856)};

    for (int i = 0; i < 4; i++)
    {
        states1[i].is_dummy = true;
    }
    for (int i = 0; i < 4; i++)
    {
        states2[i].is_dummy = true;
    }
    std::ofstream File;
    while (!all_dummy1 && !all_dummy2)
    {

        write_pipeline_file(0, states1);
        write_pipeline_file(1, states2);

        int hazard_count1 = 0;
        hazardDetector.hazard_without_forwarding(states1, hazard_count1);
        int hazard_count2 = 0;
        hazardDetector.hazard_without_forwarding(states2, hazard_count2);

        hazard_count_0 += hazard_count1;
        hazard_count_1 += hazard_count2;
        std::vector<State> oldstates1 = states1;
        std::vector<State> oldstates2 = states2;

        evaluate(states1, 0, latencies);

        if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011")
        {
            // Flush
            states1[2].is_dummy = true;
            states1[3].is_dummy = true;
            states1.push_back(State(states1[1].next_pc));
        }
        else if ((states1[1].opcode == "0110011" || states1[1].opcode == "0010011") && states1[1].latency > 0 && !states1[1].is_dummy)
        {
            states1 = {states1[0], State(0), states1[1], oldstates1[3], oldstates1[4]};
            states1[4].pc = states1[3].next_pc;
            states1[1].is_dummy = true;
        }
        else if (hazard_count1 > 0)
        {
            states1 = {states1[0], states1[1], State(0), oldstates1[3], oldstates1[4]};
            states1[4].pc = states1[3].next_pc;
            states1[2].is_dummy = true;
        }
        else
        {

            if (states1[3].is_dummy)
            {
                states1.push_back(State(0));
                states1[4].is_dummy = true;
            }
            else
            {
                states1.push_back(State(states1[3].next_pc));
                if (memory[states1[3].next_pc] == 0)
                {
                    states1[4].is_dummy = true;
                }
            }
            /*
                Exit condition
            */
            all_dummy1 = true;
            for (auto i : states1)
            {
                if (!i.is_dummy)
                {
                    all_dummy1 = false;
                }
            }
        }
        cores[0].savereg(0);

        evaluate(states2, 1, latencies);

        if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011")
        {
            // Flush
            states2[2].is_dummy = true;
            states2[3].is_dummy = true;
            states2.push_back(State(states2[1].next_pc));
        }
        else if ((states2[1].opcode == "0110011" || states2[1].opcode == "0010011") && states2[1].latency > 0 && !states2[1].is_dummy)
        {
            states2 = {states2[0], State(0), states2[1], oldstates2[3], oldstates2[4]};
            states2[4].pc = states2[3].next_pc;
            states2[1].is_dummy = true;
        }
        else if (hazard_count2 > 0)
        {
            states2 = {states2[0], states2[1], State(0), oldstates2[3], oldstates2[4]};
            states2[4].pc = states2[3].next_pc;
            states2[2].is_dummy = true;
        }
        else
        {

            if (states2[3].is_dummy)
            {
                states2.push_back(State(0));
                states2[4].is_dummy = true;
            }
            else
            {
                states2.push_back(State(states2[3].next_pc));
                if (memory[states2[3].next_pc] == 0)
                {
                    states2[4].is_dummy = true;
                }
            }
            /*
                Exit condition
            */
            all_dummy2 = true;
            for (auto i : states2)
            {
                if (!i.is_dummy)
                {
                    all_dummy2 = false;
                }
            }
        }
        cores[1].savereg(1);
    }

    while (!all_dummy1)
    {
        write_pipeline_file(0, states1);

        int hazard_count1 = 0;
        hazardDetector.hazard_without_forwarding(states1, hazard_count1);

        hazard_count_0 += hazard_count1;

        std::vector<State> oldstates1 = states1;
        evaluate(states1, 0, latencies);

        if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011")
        {
            // Flush
            states1[2].is_dummy = true;
            states1[3].is_dummy = true;
            states1.push_back(State(states1[1].next_pc));
        }
        else if ((states1[1].opcode == "0110011" || states1[1].opcode == "0010011") && states1[1].latency > 0 && !states1[1].is_dummy)
        {
            states1 = {states1[0], State(0), states1[1], oldstates1[3], oldstates1[4]};
            states1[4].pc = states1[3].next_pc;
            states1[1].is_dummy = true;
        }
        else if (hazard_count1 > 0)
        {
            states1 = {states1[0], states1[1], State(0), oldstates1[3], oldstates1[4]};
            states1[4].pc = states1[3].next_pc;
            states1[2].is_dummy = true;
        }
        else
        {

            if (states1[3].is_dummy)
            {
                states1.push_back(State(0));
                states1[4].is_dummy = true;
            }
            else
            {
                states1.push_back(State(states1[3].next_pc));
                if (memory[states1[3].next_pc] == 0)
                {
                    states1[4].is_dummy = true;
                }
            }
            /*
                Exit condition
            */
            all_dummy1 = true;
            for (auto i : states1)
            {
                if (!i.is_dummy)
                {
                    all_dummy1 = false;
                }
            }
        }
        cores[0].savereg(0);
    }
    while (!all_dummy2)
    {
        write_pipeline_file(1, states2);

        int hazard_count2 = 0;
        hazardDetector.hazard_without_forwarding(states2, hazard_count2);

        hazard_count_1 += hazard_count2;
        std::vector<State> oldstates2 = states2;

        evaluate(states2, 1, latencies);

        if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011")
        {
            // Flush
            states2[2].is_dummy = true;
            states2[3].is_dummy = true;
            states2.push_back(State(states2[1].next_pc));
        }
        else if ((states2[1].opcode == "0110011" || states2[1].opcode == "0010011") && states2[1].latency > 0 && !states2[1].is_dummy)
        {
            states2 = {states2[0], State(0), states2[1], oldstates2[3], oldstates2[4]};
            states2[4].pc = states2[3].next_pc;
            states2[1].is_dummy = true;
        }
        else if (hazard_count2 > 0)
        {
            states2 = {states2[0], states2[1], State(0), oldstates2[3], oldstates2[4]};
            states2[4].pc = states2[3].next_pc;
            states2[2].is_dummy = true;
        }
        else
        {

            if (states2[3].is_dummy)
            {
                states2.push_back(State(0));
                states2[4].is_dummy = true;
            }
            else
            {
                states2.push_back(State(states2[3].next_pc));
                if (memory[states2[3].next_pc] == 0)
                {
                    states2[4].is_dummy = true;
                }
            }
            all_dummy2 = true;
            for (auto i : states2)
            {
                if (!i.is_dummy)
                {
                    all_dummy2 = false;
                }
            }
        }
        cores[1].savereg(1);
    }
    save_stats();
}

void Processor::run_pipelined_with_forwarding(std::map<std::string, int> latencies)
{
    std::vector<State> states1 = {State(0), State(0), State(0), State(0), State(0)};
    std::vector<State> states2 = {State(856), State(856), State(856), State(856), State(856)};

    for (int i = 0; i < 4; i++)
    {
        states1[i].is_dummy = true;
    }
    for (int i = 0; i < 4; i++)
    {
        states2[i].is_dummy = true;
    }

    while (!all_dummy1 && !all_dummy2)
    {
        write_pipeline_file(0, states1);
        write_pipeline_file(1, states2);

        int hazard_count1 = 0;
        bool if_stall_1 = false;
        int stall_pos_1 = 2;
        hazardDetector.hazard_with_forwarding(states1, hazard_count1, if_stall_1, stall_pos_1);

        int hazard_count2 = 0;
        bool if_stall_2 = false;
        int stall_pos_2 = 2;
        hazardDetector.hazard_with_forwarding(states2, hazard_count2, if_stall_2, stall_pos_2);

        hazard_count_0 += hazard_count1;
        hazard_count_1 += hazard_count2;

        std::vector<State> oldstates1 = states1;
        std::vector<State> oldstates2 = states2;

        evaluate(states1, 0, latencies);
        evaluate(states2, 1, latencies);

        if (if_stall_1)
        {
            if (stall_pos_1 == 0)
            {
                states1 = {states1[0], State(0), oldstates1[2], oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[1].is_dummy = true;
            }
            else if (stall_pos_1 == 1)
            {
                states1 = {states1[0], states1[1], State(0), oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[2].is_dummy = true;
            }
        }
        else
        {
            if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011")
            {
                // Flush
                if (states1[1].next_pc == states1[2].pc)
                {
                    states1.push_back(State(states1[3].next_pc));
                    if (memory[states1[3].next_pc] == 0)
                    {
                        states1[4].is_dummy = true;
                    }
                }
                else
                {
                    states1[2].is_dummy = true;
                    states1[3].is_dummy = true;
                    states1.push_back(State(states1[1].next_pc));
                }
            }
            else if ((states1[1].opcode == "0110011" || states1[1].opcode == "0010011") && states1[1].latency > 0 && !states1[1].is_dummy)
            {
                states1 = {states1[0], State(0), states1[1], oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[1].is_dummy = true;
            }

            else if (states1[3].is_dummy)
            {
                states1.push_back(State(0));
                states1[4].is_dummy = true;
            }
            else
            {
                states1.push_back(State(states1[3].next_pc));
                if (memory[states1[3].next_pc] == 0)
                {
                    states1[4].is_dummy = true;
                }
            }
        }
        cores[0].savereg(0);

        if (if_stall_2)
        {
            if (stall_pos_2 == 0)
            {
                states2 = {states2[0], State(0), oldstates2[2], oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[1].is_dummy = true;
            }
            else if (stall_pos_2 == 1)
            {
                states2 = {states2[0], states2[1], State(0), oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[2].is_dummy = true;
            }
        }
        else
        {
            if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011")
            {
                // Flush
                if (states2[1].next_pc == states2[2].pc)
                {
                    states2.push_back(State(states2[3].next_pc));
                    if (memory[states2[3].next_pc] == 0)
                    {
                        states2[4].is_dummy = true;
                    }
                }
                else
                {
                    states2[2].is_dummy = true;
                    states2[3].is_dummy = true;
                    states2.push_back(State(states2[1].next_pc));
                }
            }
            else if ((states2[1].opcode == "0110011" || states2[1].opcode == "0010011") && states2[1].latency > 0 && !states2[1].is_dummy)
            {
                states2 = {states2[0], State(0), states2[1], oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[1].is_dummy = true;
            }
            else if (states2[3].is_dummy)
            {
                states2.push_back(State(0));
                states2[4].is_dummy = true;
            }
            else
            {
                states2.push_back(State(states2[3].next_pc));
                if (memory[states2[3].next_pc] == 0)
                {
                    states2[4].is_dummy = true;
                }
            }
            /*
                Exit condition
            */
            all_dummy1 = true;
            for (auto i : states1)
            {
                if (!i.is_dummy)
                {
                    all_dummy1 = false;
                }
            }
            all_dummy2 = true;
            for (auto i : states2)
            {
                if (!i.is_dummy)
                {
                    all_dummy2 = false;
                }
            }
        }
        cores[1].savereg(1);
    }
    while (!all_dummy1)
    {
        write_pipeline_file(0, states1);

        int hazard_count1 = 0;
        bool if_stall_1 = false;
        int stall_pos_1 = 2;
        hazardDetector.hazard_with_forwarding(states1, hazard_count1, if_stall_1, stall_pos_1);
        hazard_count_0 += hazard_count1;

        std::vector<State> oldstates1 = states1;

        evaluate(states1, 0, latencies);

        if (if_stall_1)
        {
            if (stall_pos_1 == 0)
            {
                states1 = {states1[0], State(0), oldstates1[2], oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[1].is_dummy = true;
            }
            else if (stall_pos_1 == 1)
            {
                states1 = {states1[0], states1[1], State(0), oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[2].is_dummy = true;
            }
        }
        else
        {
            if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011")
            {
                // Flush
                if (states1[1].next_pc == states1[2].pc)
                {
                    states1.push_back(State(states1[3].next_pc));
                    if (memory[states1[3].next_pc] == 0)
                    {
                        states1[4].is_dummy = true;
                    }
                }
                else
                {
                    states1[2].is_dummy = true;
                    states1[3].is_dummy = true;
                    states1.push_back(State(states1[1].next_pc));
                }
            }
            else if ((states1[1].opcode == "0110011" || states1[1].opcode == "0010011") && states1[1].latency > 0 && !states1[1].is_dummy)
            {
                states1 = {states1[0], State(0), states1[1], oldstates1[3], oldstates1[4]};
                states1[4].pc = states1[3].next_pc;
                states1[1].is_dummy = true;
            }
            else if (states1[3].is_dummy)
            {
                states1.push_back(State(0));
                states1[4].is_dummy = true;
            }
            else
            {
                states1.push_back(State(states1[3].next_pc));
                if (memory[states1[3].next_pc] == 0)
                {
                    states1[4].is_dummy = true;
                }
            }
            all_dummy1 = true;
            for (auto i : states1)
            {
                if (!i.is_dummy)
                {
                    all_dummy1 = false;
                }
            }
        }
        cores[0].savereg(0);
    }
    while (!all_dummy2)
    {
        write_pipeline_file(1, states2);

        int hazard_count2 = 0;
        bool if_stall_2 = false;
        int stall_pos_2 = 2;
        hazardDetector.hazard_with_forwarding(states2, hazard_count2, if_stall_2, stall_pos_2);
        hazard_count_1 += hazard_count2;

        std::vector<State> oldstates2 = states2;

        evaluate(states2, 1, latencies);

        if (if_stall_2)
        {
            if (stall_pos_2 == 0)
            {
                states2 = {states2[0], State(0), oldstates2[2], oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[1].is_dummy = true;
            }
            else if (stall_pos_2 == 1)
            {
                states2 = {states2[0], states2[1], State(0), oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[2].is_dummy = true;
            }
        }
        else
        {
            if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011")
            {
                // Flush
                if (states2[1].next_pc == states2[2].pc)
                {
                    states2.push_back(State(states2[3].next_pc));
                    if (memory[states2[3].next_pc] == 0)
                    {
                        states2[4].is_dummy = true;
                    }
                }
                else
                {
                    states2[2].is_dummy = true;
                    states2[3].is_dummy = true;
                    states2.push_back(State(states2[1].next_pc));
                }
            }
            else if ((states2[1].opcode == "0110011" || states2[1].opcode == "0010011") && states2[1].latency > 0 && !states2[1].is_dummy)
            {
                states2 = {states2[0], State(0), states2[1], oldstates2[3], oldstates2[4]};
                states2[4].pc = states2[3].next_pc;
                states2[1].is_dummy = true;
            }
            else if (states2[3].is_dummy)
            {
                states2.push_back(State(0));
                states2[4].is_dummy = true;
            }
            else
            {
                states2.push_back(State(states2[3].next_pc));
                if (memory[states2[3].next_pc] == 0)
                {
                    states2[4].is_dummy = true;
                }
                if (memory[states2[3].next_pc] == 0)
                {
                    states2[4].is_dummy = true;
                }
            }
            all_dummy2 = true;
            for (auto i : states2)
            {
                if (!i.is_dummy)
                {
                    all_dummy2 = false;
                }
            }
        }
        cores[1].savereg(1);
    }
    save_stats();
}
