#include <vector>
#include <iostream>
#include "state.h"
class HazardDetector{
    public:
        void harzard_without_forwarding(std::vector<State> states,int &hazard_count);
        void hazard_with_forwarding(std::vector<State> &states, int &hazard_count);

};

void HazardDetector::harzard_without_forwarding(std::vector<State> states,int &hazard_count){
    // states of 2,3 we have to check
    /*  0 wb
        1 mem
        2 exe
        3 id/rf
        4 if

        we don't need to check for wb because wb will be done in eval before exe
    */
    /* 
        Decode has not yet been executed so need to get rs2 rs1 manually
    */

    if (states[3].is_dummy) return;
    states[3].opcode = states[3].instruction.substr(25, 7);
    if (states[3].opcode == "0110011" || states[3].opcode == "0100011"){
        states[3].rs1 = std::stoi(states[3].instruction.substr(12, 5),nullptr,2);
        states[3].rs2 = std::stoi(states[3].instruction.substr(7, 5),nullptr,2);
        
        if (!states[2].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[2].rd == states[3].rs1 || states[2].rd == states[3].rs2){
                hazard_count++;
                #ifdef DEBUG
                    std::cout<<"RAW hazard R"<<std::endl;
                #endif
            }
        }
    }
    else if (states[3].opcode == "0000011" || states[3].opcode == "0010011"){
        states[3].rs1 = std::stoi(states[3].instruction.substr(12, 5),nullptr,2);

        if (!states[2].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[2].rd == states[3].rs1){
                hazard_count++;
                #ifdef DEBUG
                    std::cout<<"RAW hazard I"<<std::endl;
                #endif
            }
        }
    }
    else if (states[3].opcode == "1100011"){
        states[3].rs1 = std::stoi(states[3].instruction.substr(12, 5),nullptr,2);
        states[3].rs2 = std::stoi(states[3].instruction.substr(7, 5),nullptr,2);
        if (!states[2].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[2].rd == states[3].rs1 || states[2].rd == states[3].rs2){
                    hazard_count++;
                    #ifdef DEBUG
                        std::cout<<"RAW hazard B"<<std::endl;
                    #endif
            }
        }
    }

    if ((states[3].opcode == "0110011"||states[3].opcode == "0100011") && !states[1].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[1].rd == states[3].rs1 || states[1].rd == states[3].rs2){
                hazard_count++;
                #ifdef DEBUG
                    std::cout<<"RAW hazard R in mem"<<std::endl;
                #endif
            }
    }
    else if (states[3].opcode == "1100011" && !states[1].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[1].rd == states[3].rs1 || states[1].rd == states[3].rs2){
                hazard_count++;
                #ifdef DEBUG
                    std::cout<<"RAW hazard B in mem"<<std::endl;
                #endif
            }
    }
    else if ((states[3].opcode == "0000011" || states[3].opcode == "0010011") && !states[1].is_dummy && states[3].rd !=0 && !states[3].is_dummy){
            if (states[1].rd == states[3].rs1){
                hazard_count++;
                #ifdef DEBUG
                    std::cout<<"RAW hazard I in mem"<<std::endl;
                #endif
            }
        }
    
    //std::cout<<states[2].rd<<" "<<states[3].rd<<std::endl;
    
}

void HazardDetector::hazard_with_forwarding(std::vector<State> &states,int &hazard_count){
    
    if (states[3].is_dummy) return;
    states[3].opcode = states[3].instruction.substr(25, 7);
    if (states[3].opcode == "0110011" || states[3].opcode == "0100011"){
        states[3].rs1 = std::stoi(states[3].instruction.substr(12, 5),nullptr,2);
        states[3].rs2 = std::stoi(states[3].instruction.substr(7, 5),nullptr,2);
}
}
