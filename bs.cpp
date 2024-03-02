if (state.opcode == "0110011" && state.rd != 0)
    {
        if (state.func3 == "000" && state.func7 == "0000000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "add" << std::endl;
            #endif
            state.exe_temp = registers[state.rs1] + registers[state.rs2];
        }
        else if (state.func3 == "000" && state.func7 == "0100000")
        {   
            #ifdef PRINT
            std::cout << "ID: " << "sub" << std::endl;
            #endif
            state.exe_temp = registers[state.rs1] - registers[state.rs2];
            
        }
        else if (state.func3 == "000" && state.func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "mul" << std::endl;
            #endif
            state.exe_temp = registers[state.rs1] * registers[state.rs2];

        }
        else if (state.func3 == "100" && state.func7 == "0000001")
        {
            #ifdef PRINT
            std::cout << "ID: " << "div" << std::endl;
            #endif
            state.exe_temp = registers[state.rs1] / registers[state.rs2];
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << state.rs1 << ", " << "x" << state.rs2 << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    else if (opcode == "1101111")
    {
        if (rd != "00000")
        {
            state.exe_temp = pc;
        }
        int int_imm = std::stoi(imm, nullptr, 2);
        #ifdef PRINT
        std::cout << "ID: " << "jal " << "pc: " << int_imm + pc_i << std::endl;
        #endif

        pc = int_imm + pc_i;
        #ifdef PRINT
        std::cout << "RF: ---" << std::endl; 
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }
    else if (opcode == "1100011")
    {
        if (state.func3 == "001")
        {
            if (registers[state.rs1] != registers[state.rs2])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "bne " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "000")
        {
            if (registers[state.rs1] == registers[state.rs2])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "beq " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "100")
        {
            if (registers[state.rs1] < registers[state.rs2])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "blt " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        else if (state.func3 == "101")
        {
            if (registers[state.rs1] >= registers[state.rs2])
            {
                int int_imm = std::stoi(imm, nullptr, 2);
                #ifdef PRINT
                std::cout << "ID: " << "bge " << "pc: " << int_imm + pc_i << std::endl;
                #endif
                pc = int_imm + pc_i;
            }
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << state.rs1 << ", " << "x" << state.rs2 << std::endl;
        std::cout << "EXE: " << "executed & jumped to " << pc << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " "---" << std::endl;
        #endif
    }
    else if (opcode == "0010011")
    {
        if (state.func3 == "000")
        {
            #ifdef PRINT
            std::cout << "ID: " << "addi " << "imm: " << bin_to_int(imm) << std::endl;
            #endif
            if (state.rd != 0)
            {
                state.exe_temp = registers[state.rs1] + bin_to_int(imm);
            }
        }
        #ifdef PRINT
        std::cout << "RF: " << "x" << state.rs1 << std::endl;
        std::cout << "EXE: " << "executed" << std::endl;
        std::cout << "MEM: " << "---" << std::endl;
        std::cout << "WB: " << "x" << std::stoi(rd, nullptr, 2) << std::endl;
        #endif
    }