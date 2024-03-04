#include <string>
#include <climits>
class State{
    public:
        int pc;
        int next_pc;
        int cycles_required;
        std::string instruction;
        int rs1;
        int rs2;
        long operand1;
        long operand2;
        bool is_operand1;
        bool is_operand2;
        int temp_reg;
        bool write;
        int rd;
        int imm;
        int latency;
        std::string opcode;
        std::string func3;
        std::string func7;
        bool is_dummy;
        bool branch_taken;
        State(int pc);

};

State::State(int pc){
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
}