#ifndef STATE
#define STATE
#include <string>
/*
    Every Instruction is an object of the state class.
*/
class State
{
public:
    int pc;
    int next_pc;
    int cycles_required;
    std::string instruction;
    int rs1;
    int rs2;
    int operand1;
    int operand2;
    bool is_operand1;
    bool is_operand2;
    int temp_reg;
    bool write;
    bool BTB_miss;
    int rd;
    int imm;
    int latency;
    bool miss;
    std::string opcode;
    std::string func3;
    std::string func7;
    bool is_dummy;
    bool branch_taken;
    bool i_fetched;
    bool m_fetched;
    bool fetch_latency;
    bool mem_latency;
    bool is_mem_instruction;
    bool predicted;
    State(int pc);
};
#endif