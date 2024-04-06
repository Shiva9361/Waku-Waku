#include <string>
#include <climits>
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
    State(int pc);
};

State::State(int pc)
{
    this->pc = pc;
    is_dummy = false;
    rs1 = -1;
    rs2 = -1;
    rd = -1;
    operand1 = 0;
    is_operand1 = false;
    is_operand2 = false;
    latency = 1;
    operand2 = 0;
    write = false;
    next_pc = -1;
    branch_taken = false;
    miss = false;
    i_fetched = false;
    m_fetched = false;
    fetch_latency = false;
    mem_latency = false;
    is_mem_instruction = false;
}