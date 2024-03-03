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
    bool all_dummy1;
    bool all_dummy2;
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
    //std::cout<<"did wb"<<std::endl;
    cores[core].mem(pipelined_instructions[1],memory);
    //std::cout<<"did mem"<<std::endl;
    cores[core].execute(pipelined_instructions[2]);
    //std::cout<<"did exe"<<std::endl;
    cores[core].decode(pipelined_instructions[3]);
    //std::cout<<"did dec"<<std::endl;
    cores[core].fetch(memory,pipelined_instructions[4]);
    //std::cout<<"did fe"<<std::endl;
    pipelined_instructions.erase(pipelined_instructions.begin());

}
Processor::Processor(std::string file1, std::string file2,bool pipeline,bool forwarding)
{
    // memory[0] = 69;
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
    
    std::ofstream MembFile("data/memory_before.txt");

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
            std::vector<State> states1 = {State(0),State(0),State(0),State(0),State(0)};
            std::vector<State> states2 = {State(856),State(856),State(856),State(856),State(856)};
            
            for (int i=0;i<4;i++){
                states1[i].is_dummy = true;
            }
            for (int i=0;i<4;i++){
                states2[i].is_dummy = true;
            }
            std::ofstream File;
            while(!all_dummy1 && !all_dummy2){
                
                File.open("data/core0_pipe.txt",std::ios_base::app);
                for (auto i:states1){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                File.open("data/core1_pipe.txt",std::ios_base::app);
                for (auto i:states2){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                int hazard_count1 =0;
                hazardDetector.hazard_without_forwarding(states1,hazard_count1);
                int hazard_count2 =0;
                hazardDetector.hazard_without_forwarding(states2,hazard_count2);

                std::vector<State> oldstates1 = states1;
                std::vector<State> oldstates2 = states2;
    
                evaluate(states1,0);

                if (hazard_count1>0){
                    hazard_count_f += hazard_count1;
                    states1 = {states1[0],states1[1],State(0),oldstates1[3],oldstates1[4]};
                    states1[4].pc = states1[3].next_pc;
                    std::cout<<states1[4].pc<<std::endl;
                    states1[2].is_dummy = true;
                }
                else {
                    if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011"){
                        // Flush
                        states1[2].is_dummy = true;
                        states1[3].is_dummy = true;
                        states1.push_back(State(states1[1].next_pc));
                    }
                    else if (states1[3].is_dummy){
                        states1.push_back(State(0));
                        states1[4].is_dummy = true;
                    }
                    else {
                        states1.push_back(State(states1[3].next_pc));
                        if (memory[states1[3].next_pc] == 0){
                            states1[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy1 = true;
                    for (auto i:states1){
                        if(!i.is_dummy){
                            all_dummy1 = false;
                        }
                    }
                   
                }
                cores[0].savereg(0);
                
                evaluate(states2,1);

                if (hazard_count2>0){
                    hazard_count_f += hazard_count2;
                    states2 = {states2[0],states2[1],State(0),oldstates2[3],oldstates2[4]};
                    states2[4].pc = states2[3].next_pc;
                    std::cout<<states2[4].pc<<std::endl;
                    states2[2].is_dummy = true;
                }
                else {
                    if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011"){
                        // Flush
                        states2[2].is_dummy = true;
                        states2[3].is_dummy = true;
                        states2.push_back(State(states2[1].next_pc));
                    }
                    else if (states2[3].is_dummy){
                        states2.push_back(State(0));
                        states2[4].is_dummy = true;
                    }
                    else {
                        states2.push_back(State(states2[3].next_pc));
                        if (memory[states2[3].next_pc] == 0){
                            states2[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy2 = true;
                    for (auto i:states2){
                        if(!i.is_dummy){
                            all_dummy2 = false;
                        }
                    }
                   
                }
                cores[1].savereg(1);

            }

            while(!all_dummy1){
                File.open("data/core0_pipe.txt",std::ios_base::app);
                for (auto i:states1){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                int hazard_count1 =0;
                hazardDetector.hazard_without_forwarding(states1,hazard_count1);
                std::vector<State> oldstates1 = states1;
                evaluate(states1,0);

                if (hazard_count1>0){
                    hazard_count_f += hazard_count1;
                    states1 = {states1[0],states1[1],State(0),oldstates1[3],oldstates1[4]};
                    states1[4].pc = states1[3].next_pc;
                    std::cout<<states1[4].pc<<std::endl;
                    states1[2].is_dummy = true;
                }
                else {
                    if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011"){
                        // Flush
                        states1[2].is_dummy = true;
                        states1[3].is_dummy = true;
                        states1.push_back(State(states1[1].next_pc));
                    }
                    else if (states1[3].is_dummy){
                        states1.push_back(State(0));
                        states1[4].is_dummy = true;
                    }
                    else {
                        states1.push_back(State(states1[3].next_pc));
                        if (memory[states1[3].next_pc] == 0){
                            states1[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy1 = true;
                    for (auto i:states1){
                        if(!i.is_dummy){
                            all_dummy1 = false;
                        }
                    }
                   
                }
                cores[0].savereg(0);
            }
            while(!all_dummy2){
                File.open("data/core1_pipe.txt",std::ios_base::app);
                for (auto i:states2){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                int hazard_count2 =0;
                hazardDetector.hazard_without_forwarding(states2,hazard_count2);
                std::vector<State> oldstates2 = states2;
                
                evaluate(states2,1);
                if (hazard_count2>0){
                    hazard_count_f += hazard_count2;
                    states2 = {states2[0],states2[1],State(0),oldstates2[3],oldstates2[4]};
                    states2[4].pc = states2[3].next_pc;
                    std::cout<<states2[4].pc<<std::endl;
                    states2[2].is_dummy = true;
                }
                else {
                    if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011"){
                        // Flush
                        states2[2].is_dummy = true;
                        states2[3].is_dummy = true;
                        states2.push_back(State(states2[1].next_pc));
                    }
                    else if (states2[3].is_dummy){
                        states2.push_back(State(0));
                        states2[4].is_dummy = true;
                    }
                    else {
                        states2.push_back(State(states2[3].next_pc));
                        if (memory[states2[3].next_pc] == 0){
                            states2[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy2 = true;
                    for (auto i:states2){
                        if(!i.is_dummy){
                            all_dummy2 = false;
                        }
                    }
                   
                }
                cores[1].savereg(1);
            }

        }
        else{
            std::vector<State> states1 = {State(0),State(0),State(0),State(0),State(0)};
            std::vector<State> states2 = {State(856),State(856),State(856),State(856),State(856)};

            for (int i=0;i<4;i++){
                states1[i].is_dummy = true;
            }
            for (int i=0;i<4;i++){
                states2[i].is_dummy = true;
            }
            std::ofstream File;
            while(!all_dummy1 && !all_dummy2){
                File.open("data/core0_pipe.txt",std::ios_base::app);
                for (auto i:states1){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                File.open("data/core1_pipe.txt",std::ios_base::app);
                for (auto i:states2){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();
                
                int hazard_count_1=0; bool if_stall_1 = false; int stall_pos_1 = 2;
                hazardDetector.hazard_with_forwarding(states1,hazard_count_1,if_stall_1,stall_pos_1);
                
                int hazard_count_2 =0; bool if_stall_2 = false; int stall_pos_2 = 2;
                hazardDetector.hazard_with_forwarding(states2,hazard_count_2,if_stall_2,stall_pos_2);

                std::vector<State> oldstates1 = states1;
                std::vector<State> oldstates2 = states2;
    
                evaluate(states1,0);
                evaluate(states2,1);

                if (if_stall_1){
                    if (stall_pos_1==0){
                        states1 = {states1[0],State(0),oldstates1[2],oldstates1[3],oldstates1[4]};
                        states1[4].pc = states1[3].next_pc;
                        states1[1].is_dummy = true;
                    } 
                    else if (stall_pos_1==1){
                        states1 = {states1[0],states1[1],State(0),oldstates1[3],oldstates1[4]};
                        states1[4].pc = states1[3].next_pc;
                        states1[2].is_dummy = true;
                    }
                }
                else {
                    if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011"){
                        // Flush
                        if (states1[1].next_pc == states1[2].pc){
                            states1.push_back(State(states1[3].next_pc));
                            if (memory[states1[3].next_pc] == 0){
                                states1[4].is_dummy = true;
                            }
                        }
                        else {
                            states1[2].is_dummy = true;
                            states1[3].is_dummy = true;
                            states1.push_back(State(states1[1].next_pc));
                        }
                    }
                    else if (states1[3].is_dummy){
                        states1.push_back(State(0));
                        states1[4].is_dummy = true;
                    }
                    else {
                        states1.push_back(State(states1[3].next_pc));
                        if (memory[states1[3].next_pc] == 0){
                            states1[4].is_dummy = true;
                        }
                    }
                    
                   
                }
                cores[0].savereg(0);
                
                if (if_stall_2){
                    if (stall_pos_2==0){
                        states2 = {states2[0],State(0),oldstates2[2],oldstates2[3],oldstates2[4]};
                        states2[4].pc = states2[3].next_pc;
                        states2[1].is_dummy = true;
                    } 
                    else if (stall_pos_2==1){
                        states2 = {states2[0],states2[1],State(0),oldstates2[3],oldstates2[4]};
                        states2[4].pc = states2[3].next_pc;
                        states2[2].is_dummy = true;
                    }
                }
                else {
                    if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011"){
                        // Flush
                        if (states2[1].next_pc == states2[2].pc){
                            states2.push_back(State(states2[3].next_pc));
                            if (memory[states2[3].next_pc] == 0){
                                states2[4].is_dummy = true;
                            }
                        }
                        else {
                            states2[2].is_dummy = true;
                            states2[3].is_dummy = true;
                            states2.push_back(State(states2[1].next_pc));
                        }
                    }
                    else if (states2[3].is_dummy){
                        states2.push_back(State(0));
                        states2[4].is_dummy = true;
                    }
                    else {
                        states2.push_back(State(states2[3].next_pc));
                        if (memory[states2[3].next_pc] == 0){
                            states2[4].is_dummy = true;
                        }
                    }
                    /*
                        Exit condition
                    */
                    all_dummy1 = true;
                    for (auto i:states1){
                        if(!i.is_dummy){
                            all_dummy1 = false;
                        }
                    }
                    all_dummy2 = true;
                    for (auto i:states2){
                        if(!i.is_dummy){
                            all_dummy2 = false;
                        }
                    }
                   
                }
                cores[1].savereg(1);
            }
            while(!all_dummy1){
                File.open("data/core0_pipe.txt",std::ios_base::app);
                for (auto i:states1){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                int hazard_count_1=0; bool if_stall_1 = false; int stall_pos_1 = 2;
                hazardDetector.hazard_with_forwarding(states1,hazard_count_1,if_stall_1,stall_pos_1);
                std::vector<State> oldstates1 = states1;

                evaluate(states1,0);

                if (if_stall_1){
                    if (stall_pos_1==0){
                        states1 = {states1[0],State(0),oldstates1[2],oldstates1[3],oldstates1[4]};
                        states1[4].pc = states1[3].next_pc;
                        states1[1].is_dummy = true;
                    } 
                    else if (stall_pos_1==1){
                        states1 = {states1[0],states1[1],State(0),oldstates1[3],oldstates1[4]};
                        states1[4].pc = states1[3].next_pc;
                        states1[2].is_dummy = true;
                    }
                }
                else {
                    if (states1[1].opcode == "1101111" || states1[1].opcode == "1100011"){
                        // Flush
                        if (states1[1].next_pc == states1[2].pc){
                            states1.push_back(State(states1[3].next_pc));
                            if (memory[states1[3].next_pc] == 0){
                                states1[4].is_dummy = true;
                            }
                        }
                        else {
                            states1[2].is_dummy = true;
                            states1[3].is_dummy = true;
                            states1.push_back(State(states1[1].next_pc));
                        }
                    }
                    else if (states1[3].is_dummy){
                        states1.push_back(State(0));
                        states1[4].is_dummy = true;
                    }
                    else {
                        states1.push_back(State(states1[3].next_pc));
                        if (memory[states1[3].next_pc] == 0){
                            states1[4].is_dummy = true;
                        }
                    }
                    all_dummy1 = true;
                    for (auto i:states1){
                        if(!i.is_dummy){
                            all_dummy1 = false;
                        }
                    }
                   
                }
                cores[0].savereg(0);
            }
            while(!all_dummy2){
                File.open("data/core1_pipe.txt",std::ios_base::app);
                for (auto i:states2){
                    if (i.is_dummy) File<<"0 ";
                    else File<<1<<" ";
                }
                File<<std::endl;
                File.close();

                int hazard_count_2=0; bool if_stall_2 = false; int stall_pos_2 = 2;
                hazardDetector.hazard_with_forwarding(states2,hazard_count_2,if_stall_2,stall_pos_2);
                std::vector<State> oldstates2 = states2;

                evaluate(states2,1);

                if (if_stall_2){
                    if (stall_pos_2==0){
                        states2 = {states2[0],State(0),oldstates2[2],oldstates2[3],oldstates2[4]};
                        states2[4].pc = states2[3].next_pc;
                        states2[1].is_dummy = true;
                    } 
                    else if (stall_pos_2==1){
                        states2 = {states2[0],states2[1],State(0),oldstates2[3],oldstates2[4]};
                        states2[4].pc = states2[3].next_pc;
                        states2[2].is_dummy = true;
                    }
                }
                else {
                    if (states2[1].opcode == "1101111" || states2[1].opcode == "1100011"){
                        // Flush
                        if (states2[1].next_pc == states2[2].pc){
                            states2.push_back(State(states2[3].next_pc));
                            if (memory[states2[3].next_pc] == 0){
                                states2[4].is_dummy = true;
                            }
                        }
                        else {
                            states2[2].is_dummy = true;
                            states2[3].is_dummy = true;
                            states2.push_back(State(states2[1].next_pc));
                        }
                        
                    }
                    else if (states2[3].is_dummy){
                        states2.push_back(State(0));
                        states2[4].is_dummy = true;
                    }
                    else {
                        states2.push_back(State(states2[3].next_pc));
                        if (memory[states2[3].next_pc] == 0){
                            states2[4].is_dummy = true;
                        } if (memory[states2[3].next_pc] == 0){
                                states2[4].is_dummy = true;
                            }
                    }
                    all_dummy2 = true;
                    for (auto i:states2){
                        if(!i.is_dummy){
                            all_dummy2 = false;
                        }
                    }
                   
                }
                cores[1].savereg(1);
            }
        }
    }
    std::cout<<instruction_count<<std::endl;
    std::ofstream MemFile("data/memory_after.txt");

    
    for (int i=0;i<1024;i++){
        MemFile << i<<","<<memory[i]<<std::endl;
    }
    MemFile.close();
}

