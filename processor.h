#include <iostream>
#include "core.h"
#include "assembler.h"

class Processor
{
private:
    int memory[1024] = {0};
    Core cores[2] = {Core(0,84), Core(856,943)}; // .text is of size 84 words
    int clock;
    bool pipeline;
    bool all_dummy;
    int instruction_count;
    int hazard_count_f;
    Assembler assembler;
    HazardDetector hazardDetector;

public:
    Processor(std::string file1, std::string file2,bool pipeline,bool forwarding);
    void evaluate(std::vector<State> &pipelined_instructions,int core);
};

void Processor::evaluate(std::vector<State> &pipelined_instructions,int core){
    cores[core].writeback(pipelined_instructions[0],instruction_count);
    cores[core].mem(pipelined_instructions[1],memory);
    cores[core].execute(pipelined_instructions[2]);
    cores[core].decode(pipelined_instructions[3]);
    cores[core].fetch(memory,pipelined_instructions[4]);
    pipelined_instructions.erase(pipelined_instructions.begin());

}
Processor::Processor(std::string file1, std::string file2,bool pipeline,bool forwarding)
{
    clock = 0;
    hazard_count_f = 0;
    instruction_count = 0;
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
    
    std::ofstream MembFile("memory_before.txt");

    for (int i=0;i<1024;i++){
        MembFile << i<<","<<memory[i]<<std::endl;
    }
    MembFile.close();

    if (!pipeline){
    int i = 0, j = 0;
    while (i < instructions1.size() && j < instructions2.size())
    {
        cores[0].fetch(memory);
        cores[1].fetch(memory);
        cores[0].decode();
        cores[1].decode();
        i = cores[0].execute();
        j = cores[1].execute() - 856;
        cores[0].mem(memory);
        cores[1].mem(memory);
        cores[0].savereg(0);
        cores[1].savereg(1);
        clock++;
    }
    while (i < instructions1.size())
    {
        cores[0].fetch(memory);
        cores[0].decode();
        i = cores[0].execute();
        cores[0].mem(memory);
        cores[0].savereg(0);
        cores[1].savereg(1);
        clock++;
    }
    while (j < instructions2.size())
    {
        cores[1].fetch(memory);
        cores[1].decode();
        j = cores[1].execute() - 856;
        cores[1].mem(memory);
        cores[0].savereg(0);
        cores[1].savereg(1);
        clock++;
    }
    }
    /*
        Pipeline
    */
    else{
        if(!forwarding){
            std::vector<State> states = {State(0),State(0),State(0),State(0),State(0)};
            
            for (int i=0;i<4;i++){
                states[i].is_dummy = true;
            }
            
            while(!all_dummy){
                for (auto i:states){
                    if (i.is_dummy) std::cout<<"stall ";
                    else std::cout<<i.instruction<<" ";
                }
                std::cout<<std::endl;

                int hazard_count =0;
                hazardDetector.hazard_without_forwarding(states,hazard_count);

                std::vector<State> oldStates = states;
    
                evaluate(states,0);

                if (hazard_count>0){
                    hazard_count_f += hazard_count;
                    states = {states[0],states[1],State(0),oldStates[3],oldStates[4]};
                    states[4].pc = states[3].next_pc;
                    std::cout<<states[4].pc<<std::endl;
                    states[2].is_dummy = true;
                }
                else {
                    if (states[1].opcode == "1101111" || states[1].opcode == "1100011"){
                        // Flush
                        states[2].is_dummy = true;
                        states[3].is_dummy = true;
                        states.push_back(State(states[1].next_pc));
                    }
                    else if (states[3].is_dummy){
                        states.push_back(State(0));
                        states[4].is_dummy = true;
                    }
                    else {
                        states.push_back(State(states[3].next_pc));
                        if (memory[states[3].next_pc] == 0){
                            states[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy = true;
                    for (auto i:states){
                        if(!i.is_dummy){
                            all_dummy = false;
                        }
                    }
                   
                }
                cores[0].savereg(0);

            }
        }
        else{
            std::vector<State> states = {State(0),State(0),State(0),State(0),State(0)};

            for (int i=0;i<4;i++){
                states[i].is_dummy = true;
            }
            while(!all_dummy){
                for (auto i:states){
                    if (i.is_dummy) std::cout<<"stall ";
                    else std::cout<<i.instruction<<" ";
                }
                std::cout<<std::endl;

                int hazard_count =0; bool if_stall = false; int stall_pos = 0;
                hazardDetector.hazard_with_forwarding(states,hazard_count,if_stall,stall_pos);

                std::vector<State> oldStates = states;
    
                evaluate(states,0);

                if (hazard_count>0){
                    hazard_count_f += hazard_count;
                    states.push_back(State(states[3].next_pc));
                    if (memory[states[3].next_pc] == 0){
                        states[4].is_dummy = true;
                    }
                    states = {states[0],State(0),oldStates[2],oldStates[3],oldStates[4]};
                    states[4].pc = states[3].next_pc;
                    states[1].is_dummy = true;
                }
                else {
                    if (states[1].opcode == "1101111" || states[1].opcode == "1100011"){
                        // Flush
                        states[2].is_dummy = true;
                        states[3].is_dummy = true;
                        states.push_back(State(states[1].next_pc));
                    }
                    else if (states[3].is_dummy){
                        states.push_back(State(0));
                        states[4].is_dummy = true;
                    }
                    else {
                        states.push_back(State(states[3].next_pc));
                        if (memory[states[3].next_pc] == 0){
                            states[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy = true;
                    for (auto i:states){
                        if(!i.is_dummy){
                            all_dummy = false;
                        }
                    }
                   
                }
                cores[0].savereg(0);
            }
        }
    }
    std::cout<<instruction_count<<std::endl;
    std::ofstream MemFile("memory_after.txt");

    
    for (int i=0;i<1024;i++){
        MemFile << i<<","<<memory[i]<<std::endl;
    }
    MemFile.close();
}

