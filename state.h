#include <string>
class State{
    public:
        int pc;
        int next_pc;
        int cycles_required;
        std::string instruction;
        int rs1;
        int rs2;
        int temp_reg;
        bool write;
        int rd;
        int imm;
        std::string opcode;
        std::string func3;
        std::string func7;
        bool is_dummy;
        State(int pc);

};

State::State(int pc){
    this->pc = pc;
    is_dummy = false;
    rs1 = -1;
    rs2 = -1;
    rd = -1; 
    write = false;   
    next_pc = -1;
}