#include "hazardDetector.h"
#include "cache.h"

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
    Core(int pc, int dataloc);
    void savereg(int core);
    void fetch(int *memory);
    void fetch(int *memory, State &instruction, Cache *cache);
    void decode();
    void decode(State &state);
    int execute(std::map<std::string, int> latencies, int &counter);
    void execute(State &instruction);
    void mem(int *memory);
    void mem(State &instruction, int *memory, Cache *cache);
    void writeback(State &instruction, int &instruction_count);
    bool predict(int pc);

    std::vector<std::map<std::string, std::string>> register_states;
};

Core::Core(int pc, int dataloc)
{
    registers[0] = 0; // x0 is hardwired to 0
    this->pc = pc;
    this->pc_i = pc;
    this->dataloc = dataloc;
}
void Core::fetch(int memory[])
{
    int_instruction = memory[pc];
    pc++;
}

void Core::fetch(int memory[], State &state, Cache *cache)
{
    if (state.is_dummy)
    {
        return;
    }
#ifdef PRINT
    std::cout << state.pc << std::endl;
#endif
    if (state.i_fetched)
    {
        state.latency--;
        return;
    }

    std::pair<int, bool> cache_result = cache->read(state.pc, memory);
    int instruction = cache_result.first;
    state.miss = !cache_result.second;
    std::bitset<32> bin_instruction(instruction);
    std::string instruction_string = bin_instruction.to_string();

    state.instruction = instruction_string;
    state.i_fetched = true;

    if (!predict(state.pc))
    {
        state.next_pc = state.pc + 1;
    }
    else
    {
        // Nvm
    }
#ifdef PRINT
    std::cout << "fetched" << std::endl;
#endif
}

void Core::decode()
{
    std::bitset<32> bin_instruction(int_instruction);
    std::string instruction = bin_instruction.to_string();

#ifdef PRINT
    std::cout << "IF: " << instruction << std::endl;
#endif

    opcode = instruction.substr(25, 7);
    func3 = instruction.substr(17, 3);
    rs1 = instruction.substr(12, 5);
    if (opcode == "0000011")
    {

        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 12);
    }
    else if (opcode == "0110011")
    {
        rd = instruction.substr(20, 5);
        rs2 = instruction.substr(7, 5);
        func7 = instruction.substr(0, 7);
    }
    else if (opcode == "0100011")
    {
        rs2 = instruction.substr(7, 5);
        imm = instruction.substr(0, 7) + instruction.substr(20, 5);
    }
    else if (opcode == "1101111")
    {
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 20);
    }
    else if (opcode == "1100011")
    {
        rs2 = instruction.substr(7, 5);
        imm = instruction.substr(0, 7) + instruction.substr(20, 5);
    }
    else if (opcode == "0010011")
    {
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 12);
    }
    else if (opcode == "0010111")
    {
        rd = instruction.substr(20, 5);
        imm = instruction.substr(0, 20);
    }
}

void Core::decode(State &state)
{
    /*
    Control hazard for branch
    */
    if (state.is_dummy)
    {
        return;
    }
    state.opcode = state.instruction.substr(25, 7);
    state.func3 = state.instruction.substr(17, 3);
    state.rs1 = std::stoi(state.instruction.substr(12, 5), nullptr, 2);
    if (state.opcode == "0000011")
    {

        state.rd = std::stoi(state.instruction.substr(20, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 12));
        state.write = true;
    }
    else if (state.opcode == "0110011")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5), nullptr, 2);
        state.rs2 = std::stoi(state.instruction.substr(7, 5), nullptr, 2);
        state.func7 = state.instruction.substr(0, 7);
        state.write = true;
    }
    else if (state.opcode == "0100011")
    {
        state.rs2 = std::stoi(state.instruction.substr(7, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 7) + state.instruction.substr(20, 5));
    }
    else if (state.opcode == "1101111")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 20));
        state.write = true;
    }
    else if (state.opcode == "1100011")
    {
        state.rs2 = std::stoi(state.instruction.substr(7, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 7) + state.instruction.substr(20, 5));
    }
    else if (state.opcode == "0010011")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 12));
        state.write = true;
    }
    else if (state.opcode == "0010111")
    {
        state.rd = std::stoi(state.instruction.substr(20, 5), nullptr, 2);
        state.imm = bin_to_int(state.instruction.substr(0, 20));
        state.write = true;
    }
}

int Core::execute(std::map<std::string, int> latencies, int &counter)
{
    if (opcode == "0110011" && rd != "00000")
    {
        if (func3 == "000" && func7 == "0000000")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "add" << std::endl;
#endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + registers[std::stoi(rs2, nullptr, 2)];
            counter += (latencies["add"] - 1);
        }
        else if (func3 == "000" && func7 == "0100000")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "sub" << std::endl;
#endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] - registers[std::stoi(rs2, nullptr, 2)];
            counter += (latencies["sub"] - 1);
        }
        else if (func3 == "000" && func7 == "0000001")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "mul" << std::endl;
#endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] * registers[std::stoi(rs2, nullptr, 2)];
            counter += (latencies["mul"] - 1);
        }
        else if (func3 == "100" && func7 == "0000001")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "div" << std::endl;
#endif
            registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] / registers[std::stoi(rs2, nullptr, 2)];
            counter += (latencies["div"] - 1);
        }
#ifdef PRINT
        std::cout << "RF: "
                  << "x" << std::stoi(rs1, nullptr, 2) << ", "
                  << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: "
                  << "executed" << std::endl;
        std::cout << "MEM: "
                  << "---" << std::endl;
        std::cout << "WB: "
                  << "x" << std::stoi(rd, nullptr, 2) << std::endl;
#endif
    }
    else if (opcode == "1101111")
    {
        if (rd != "00000")
        {
            registers[std::stoi(rd, nullptr, 2)] = pc;
        }
        int int_imm = std::stoi(imm, nullptr, 2);
#ifdef PRINT
        std::cout << "ID: "
                  << "jal "
                  << "pc: " << int_imm + pc_i << std::endl;
#endif

        pc = int_imm + pc_i;
#ifdef PRINT
        std::cout << "RF: ---" << std::endl;
        std::cout << "EXE: "
                  << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: "
                  << "---" << std::endl;
        std::cout << "WB: "
                  << "x" << std::stoi(rd, nullptr, 2) << std::endl;
#endif
    }
    else if (opcode == "1100011")
    {
        if (func3 == "001")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] != registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
#ifdef PRINT
                std::cout << "ID: "
                          << "bne "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "000")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] == registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
#ifdef PRINT
                std::cout << "ID: "
                          << "beq "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "100")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] < registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
#ifdef PRINT
                std::cout << "ID: "
                          << "blt "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                pc = int_imm + pc_i;
            }
        }
        else if (func3 == "101")
        {
            if (registers[std::stoi(rs1, nullptr, 2)] >= registers[std::stoi(rs2, nullptr, 2)])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
#ifdef PRINT
                std::cout << "ID: "
                          << "bge "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                pc = int_imm + pc_i;
            }
        }
#ifdef PRINT
        std::cout << "RF: "
                  << "x" << std::stoi(rs1, nullptr, 2) << ", "
                  << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
        std::cout << "EXE: "
                  << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: "
                  << "---" << std::endl;
        std::cout << "WB: "
                     "---"
                  << std::endl;
#endif
    }
    else if (opcode == "0010011")
    {
        if (func3 == "000")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "addi "
                      << "imm: " << bin_to_int(imm) << std::endl;
#endif
            if (rd != "00000")
            {
                registers[std::stoi(rd, nullptr, 2)] = registers[std::stoi(rs1, nullptr, 2)] + bin_to_int(imm);
                counter += (latencies["addi"] - 1);
            }
        }
#ifdef PRINT
        std::cout << "RF: "
                  << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: "
                  << "executed" << std::endl;
        std::cout << "MEM: "
                  << "---" << std::endl;
        std::cout << "WB: "
                  << "x" << std::stoi(rd, nullptr, 2) << std::endl;
#endif
    }
    return pc;
}
/*
    fix for branch instructions
*/
void Core::execute(State &state)
{

    if (state.is_dummy)
    {
        return;
    }
    int temp_operand1, temp_operand2;
    if (state.is_operand1)
    {
        temp_operand1 = registers[state.rs1];
        registers[state.rs1] = state.operand1;
    }

    if (state.is_operand2)
    {
        temp_operand2 = registers[state.rs2];
        registers[state.rs2] = state.operand2;
    }

    if (state.opcode == "0110011")
    {

        state.latency -= 1;
        if (state.func3 == "000" && state.func7 == "0000000")
        {

#ifdef PRINT
            std::cout << "ID: "
                      << "add" << std::endl;
#endif
            state.temp_reg = registers[state.rs1] + registers[state.rs2];
        }
        else if (state.func3 == "000" && state.func7 == "0100000")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "sub" << std::endl;
#endif
            state.temp_reg = registers[state.rs1] - registers[state.rs2];
        }
        else if (state.func3 == "000" && state.func7 == "0000001")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "mul" << std::endl;
#endif
            state.temp_reg = registers[state.rs1] * registers[state.rs2];
        }
        else if (state.func3 == "100" && state.func7 == "0000001")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "div" << std::endl;
#endif
            state.temp_reg = registers[state.rs1] / registers[state.rs2];
        }
    }
    else if (state.opcode == "1101111")
    {

        state.temp_reg = state.pc;
        int int_imm = state.imm;
#ifdef PRINT
        std::cout << "IMM " << int_imm << std::endl;
        std::cout << "ID: "
                  << "jal "
                  << "pc: " << int_imm + pc_i << std::endl;
#endif
        state.next_pc = int_imm + pc_i;
    }
    else if (state.opcode == "1100011")
    {
        if (state.func3 == "001")
        {
            if (registers[state.rs1] != registers[state.rs2])
            {
                int int_imm = state.imm;
#ifdef PRINT
                std::cout << "ID: "
                          << "bne "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "000")
        {
            if (registers[state.rs1] == registers[state.rs2])
            {
                int int_imm = state.imm;
#ifdef PRINT
                std::cout << "ID: "
                          << "beq "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "100")
        {
            if (registers[state.rs1] < registers[state.rs2])
            {
                int int_imm = state.imm;
#ifdef PRINT
                std::cout << "ID: "
                          << "blt "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                state.next_pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "101")
        {
            if (registers[state.rs1] >= registers[state.rs2])
            {
                int int_imm = state.imm;
#ifdef PRINT
                std::cout << "ID: "
                          << "bge "
                          << "pc: " << int_imm + pc_i << std::endl;
#endif
                state.next_pc = int_imm + pc_i;
            }
        }
    }
    else if (state.opcode == "0010011")
    {
        state.latency -= 1;
        if (state.func3 == "000")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "addi "
                      << "imm: " << bin_to_int(imm) << std::endl;
#endif
            if (state.rd != 0)
            {
                state.temp_reg = registers[state.rs1] + state.imm;
            }
        }
    }
    if (state.is_operand1)
    {
        registers[state.rs1] = temp_operand1;
    }

    if (state.is_operand2)
    {
        registers[state.rs2] = temp_operand2;
    }

    return;
}
void Core::mem(int *memory)
{
    if (opcode == "0000011")
    {
        if (func3 == "010")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "lw "
                      << "imm: " << bin_to_int(imm) << std::endl;
#endif

            if (rd != "00000")
            {
                registers[std::stoi(rd, nullptr, 2)] = memory[registers[std::stoi(rs1, nullptr, 2)] / 4 + bin_to_int(imm) / 4];
            }
        }
#ifdef PRINT
        std::cout << "RF: "
                  << "x" << std::stoi(rs1, nullptr, 2) << std::endl;
        std::cout << "EXE: "
                  << "executed" << std::endl;
        std::cout << "MEM: "
                  << "loaded from index " << registers[std::stoi(rs1, nullptr, 2)] / 4 + bin_to_int(imm) / 4 << std::endl;
        std::cout << "WB: "
                  << "x" << std::stoi(rd, nullptr, 2) << std::endl;
#endif
    }
    else if (opcode == "0100011")
    {
        if (func3 == "010")
        {
#ifdef PRINT
            std::cout << "ID: "
                      << "sw "
                      << "imm: " << bin_to_int(imm) << std::endl;
#endif
            memory[registers[std::stoi(rs2, nullptr, 2)] / 4 + bin_to_int(imm) / 4] = registers[std::stoi(rs1, nullptr, 2)];
#ifdef PRINT
            std::cout << "RF: "
                      << "x" << std::stoi(rs1, nullptr, 2) << ", "
                      << "x" << std::stoi(rs2, nullptr, 2) << std::endl;
            std::cout << "EXE: "
                      << "executed" << std::endl;
            std::cout << "MEM: "
                      << "stored at index " << registers[std::stoi(rs2, nullptr, 2)] / 4 + bin_to_int(imm) / 4 << std::endl;
            std::cout << "WB: "
                         "---"
                      << std::endl;
#endif
        }
    }
    else if (opcode == "0010111")
    {
#ifdef PRINT
        std::cout << "ID: "
                  << "la "
                  << "imm: " << bin_to_int(imm) << std::endl;
        std::cout << "RF: "
                  << "---" << std::endl;
        std::cout << "EXE: "
                  << "---" << std::endl;
#endif
        registers[std::stoi(rd, nullptr, 2)] = (dataloc + bin_to_int(imm)) * 4;

#ifdef PRINT
        std::cout << "MEM: "
                  << "loaded address from index " << dataloc + bin_to_int(imm) << std::endl;
        std::cout << "WB: "
                  << "x" << std::stoi(rd, nullptr, 2) << std::endl;
#endif
    }
#ifdef PRINT
    std::cout << std::endl;
#endif
}

void Core::mem(State &state, int *memory, Cache *cache)
{

    if (state.is_dummy)
    {
        return;
    }
    if (state.m_fetched)
    {
        state.latency--;
        return;
    }

    int temp_operand1, temp_operand2;
    if (state.is_operand1)
    {
        temp_operand1 = registers[state.rs1];
        registers[state.rs1] = state.operand1;
    }

    if (state.is_operand2)
    {
        temp_operand2 = registers[state.rs2];
        registers[state.rs2] = state.operand2;
    }

    if (state.opcode == "0000011")
    {
        if (state.func3 == "010")
        {
            if (state.rd != 0)
            {
                if (state.is_operand1)
                {
                    std::pair<int, bool> cache_result = cache->read(state.operand1 / 4 + state.imm / 4, memory);
                    state.temp_reg = cache_result.first;
                    state.miss = !cache_result.second;
                }
                else
                {
                    std::pair<int, bool> cache_result = cache->read(registers[state.rs1] / 4 + state.imm / 4, memory);
                    state.temp_reg = cache_result.first;
                    state.miss = !cache_result.second;
                }
            }
        }
    }
    else if (state.opcode == "0100011")
    {
        if (state.func3 == "010")
        {
            if (state.is_operand1 == true)
            {
                memory[state.operand1 / 4 + state.imm / 4] = registers[state.rs1];
                cache->write(state.operand1 / 4 + state.imm / 4, registers[state.rs1]);
            }
            memory[registers[state.rs2] / 4 + state.imm / 4] = registers[state.rs1];
            cache->write(registers[state.rs2] / 4 + state.imm / 4, registers[state.rs1]);
#ifdef PRINT
            std::cout << "Wrote to mem" << registers[state.rs1] << std::endl;
#endif
        }
    }
    else if (state.opcode == "0010111")
    {
        state.temp_reg = (dataloc + state.imm) * 4;
    }

    if (state.is_operand1)
    {
        registers[state.rs1] = temp_operand1;
    }

    if (state.is_operand2)
    {
        registers[state.rs2] = temp_operand2;
    }
}

void Core::writeback(State &state, int &instruction_count)
{
    if (state.is_dummy)
    {
        return;
    }
    instruction_count += 1;
    if (state.write && state.rd != 0)
    {
        registers[state.rd] = state.temp_reg;
#ifdef PRINT
        std::cout << "Wrote " << state.temp_reg << " to " << state.rd << std::endl;
#endif
    }
}
void Core::savereg(int core)
{
    std::map<std::string, std::string> temp;

    for (int i = 0; i < 32; i++)
    {
        temp[std::to_string(i)] = std::to_string(registers[i]);
    }
    register_states.push_back(temp);
}

bool Core::predict(int pc)
{
    return false; // Always not taken , Static predictor
}