#include <fstream>
#include <iostream>
#include <vector>
#include <sstream>
#include <map>
#include <bitset>

class Assembler
{
private:
    std::map<std::string, std::string> lookup_table;

public:
    std::ifstream File;

    void display(std::string file);
    std::pair<std::vector<int>, std::vector<int>> assemble(std::string file);
    Assembler();
};
Assembler::Assembler()
{
    lookup_table["x0"] = "00000";
    lookup_table["x1"] = "00001";
    lookup_table["x2"] = "00010";
    lookup_table["x3"] = "00011";
    lookup_table["x4"] = "00100";
    lookup_table["x5"] = "00101";
    lookup_table["x6"] = "00110";
    lookup_table["x7"] = "00111";
    lookup_table["x8"] = "01000";
    lookup_table["x9"] = "01001";
    lookup_table["x10"] = "01010";
    lookup_table["x11"] = "01011";
    lookup_table["x12"] = "01100";
    lookup_table["x13"] = "01101";
    lookup_table["x14"] = "01110";
    lookup_table["x15"] = "01111";
    lookup_table["x16"] = "10000";
    lookup_table["x17"] = "10001";
    lookup_table["x18"] = "10010";
    lookup_table["x19"] = "10011";
    lookup_table["x20"] = "10100";
    lookup_table["x21"] = "10101";
    lookup_table["x22"] = "10110";
    lookup_table["x23"] = "10111";
    lookup_table["x24"] = "11000";
    lookup_table["x25"] = "11001";
    lookup_table["x26"] = "11010";
    lookup_table["x27"] = "11011";
    lookup_table["x28"] = "11100";
    lookup_table["x29"] = "11101";
    lookup_table["x30"] = "11110";
    lookup_table["x31"] = "11111";
    lookup_table["x0\r"] = "00000";
    lookup_table["x1\r"] = "00001";
    lookup_table["x2\r"] = "00010";
    lookup_table["x3\r"] = "00011";
    lookup_table["x4\r"] = "00100";
    lookup_table["x5\r"] = "00101";
    lookup_table["x6\r"] = "00110";
    lookup_table["x7\r"] = "00111";
    lookup_table["x8\r"] = "01000";
    lookup_table["x9\r"] = "01001";
    lookup_table["x10\r"] = "01010";
    lookup_table["x11\r"] = "01011";
    lookup_table["x12\r"] = "01100";
    lookup_table["x13\r"] = "01101";
    lookup_table["x14\r"] = "01110";
    lookup_table["x15\r"] = "01111";
    lookup_table["x16\r"] = "10000";
    lookup_table["x17\r"] = "10001";
    lookup_table["x18\r"] = "10010";
    lookup_table["x19\r"] = "10011";
    lookup_table["x20\r"] = "10100";
    lookup_table["x21\r"] = "10101";
    lookup_table["x22\r"] = "10110";
    lookup_table["x23\r"] = "10111";
    lookup_table["x24\r"] = "11000";
    lookup_table["x25\r"] = "11001";
    lookup_table["x26\r"] = "11010";
    lookup_table["x27\r"] = "11011";
    lookup_table["x28\r"] = "11100";
    lookup_table["x29\r"] = "11101";
    lookup_table["x30\r"] = "11110";
    lookup_table["x31\r"] = "11111";
}
void Assembler::display(std::string file)
{
    File.open(file);
    std::string line;
    while (std::getline(File, line))
    {
        std::cout << line << std::endl;
    }
    File.close();
}

std::pair<std::vector<int>, std::vector<int>> Assembler::assemble(std::string file)
{
    std::pair<std::vector<int>, std::vector<int>> result_pair;
    std::vector<int> data;
    std::vector<int> result;
    std::map<std::string, int> labels;
    File.open(file);
    std::string line;
    int lc = 0;
    // Label parser
    while (std::getline(File, line))
    {
        if (line == "")
        {
            continue;
        }
        std::vector<std::string> tokens;
        std::stringstream stream(line);
        std::string token;

        while (std::getline(stream, token, ' '))
        {
            tokens.push_back(token);
        }
        if (tokens[0][tokens[0].length() - 1] == '\r')
        {
            if (tokens[0][tokens[0].length() - 2] == ':')
            {
                if (tokens.size() > 3)
                {
                    labels[tokens[0].substr(0, tokens[0].length() - 1)] = lc; // offset for same line
                }
                else
                {
                    labels[tokens[0].substr(0, tokens[0].length() - 1)] = lc + 1;
                }
            }
        }
        else if (tokens[0][tokens[0].length() - 1] == ':')
        {
            if (tokens.size() > 3)
            {
                labels[tokens[0].substr(0, tokens[0].length() - 1)] = lc; // offset for same line
            }
            else
            {
                labels[tokens[0].substr(0, tokens[0].length() - 1)] = lc + 1;
            }
        }
        else if(tokens[0] == "#" || tokens[0][0] == '#'){
            continue;
        }
        lc++;
    }
    File.close();

    File.open(file);
    int offset = 0;
    while (std::getline(File, line))
    {
        if (line == "")
        {
            continue;
        }
        std::vector<std::string> tokens;
        std::stringstream stream(line);
        std::string token;
        while (std::getline(stream, token, ' '))
        {
            tokens.push_back(token);
        }
        if (tokens.size() > 1)
        {
            if (tokens[1] == ".word")
            {
                labels[tokens[0].substr(0,tokens[0].length()-1)] = offset;
                for (int i = 2; i < tokens.size(); i++)
                {
                    data.push_back(std::stoi(tokens[i], nullptr, 10));
                    offset++;
                }
                
            }
        }
        else if (tokens[0] == ".text")
        {
            break;
        }
        else if(tokens[0] == "#" || tokens[0][0] == '#'){
            continue;
        }
    }
    File.close();

    File.open(file);
    int ic = 0;
    while (std::getline(File, line))
    {
        if (line == "")
        {
            continue;
        }
        std::vector<std::string> tokens;
        std::stringstream stream(line);
        std::string token;
        while (std::getline(stream, token, ' '))
        {
            tokens.push_back(token);
        }
        int index = 0;
        // if (tokens[0][tokens[0].length() - 1] == ':' && tokens[1] != "")
        // {
        //     index = 1;
        // }
        if (tokens[index] == "add")
        {
            std::string opcode = "0110011";
            std::string rd = lookup_table[tokens[1]];
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[2]];
            std::string rs2 = lookup_table[tokens[3]];
            std::string func7 = "0000000";
            std::string instruction = func7 + rs2 + rs1 + func3 + rd + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "sub")
        {
            std::string opcode = "0110011";
            std::string rd = lookup_table[tokens[index + 1]];
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[index + 2]];
            std::string rs2 = lookup_table[tokens[index + 3]];
            std::string func7 = "0100000";
            std::string instruction = func7 + rs2 + rs1 + func3 + rd + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "mul")
        {
            std::string opcode = "0110011";
            std::string rd = lookup_table[tokens[index + 1]];
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[index + 2]];
            std::string rs2 = lookup_table[tokens[index + 3]];
            std::string func7 = "0000001";
            std::string instruction = func7 + rs2 + rs1 + func3 + rd + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "div")
        {
            std::string opcode = "0110011";
            std::string rd = lookup_table[tokens[index + 1]];
            std::string func3 = "100";
            std::string rs1 = lookup_table[tokens[index + 2]];
            std::string rs2 = lookup_table[tokens[index + 3]];
            std::string func7 = "0000001";
            std::string instruction = func7 + rs2 + rs1 + func3 + rd + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "lw")
        {
            std::string opcode = "0000011";
            std::string rd = lookup_table[tokens[index + 1]];
            std::string func3 = "010";
            std::vector<std::string> sub_tokens;
            std::stringstream stream2(tokens[index + 2]);
            while (std::getline(stream2, token, '('))
            {
                sub_tokens.push_back(token);
            }
            std::string rs1 = lookup_table[sub_tokens[1].substr(0, (sub_tokens[1].length() - 1))]; // why -2? fing \r
            if (sub_tokens[1][(sub_tokens[1].length() - 1)] == '\r')
            {
                std::string rs1 = lookup_table[sub_tokens[1].substr(0, (sub_tokens[1].length() - 2))];
            }
            std::bitset<12> bin_imm(std::stoi(sub_tokens[0], nullptr, 10));
            std::string imm = bin_imm.to_string();
            std::string instruction = imm + rs1 + func3 + rd + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "sw")
        {
            std::string opcode = "0100011";
            std::string rs1 = lookup_table[tokens[index + 1]];
            std::string func3 = "010";
            std::vector<std::string> sub_tokens;
            std::stringstream stream2(tokens[index + 2]);
            while (std::getline(stream2, token, '('))
            {
                sub_tokens.push_back(token);
            }
            std::string rs2 = lookup_table[sub_tokens[1].substr(0, (sub_tokens[1].length() - 1))];
            if (sub_tokens[1][(sub_tokens[1].length() - 1)] == '\r')
            {
                rs2 = lookup_table[sub_tokens[1].substr(0, (sub_tokens[1].length() - 2))];
            }
            std::bitset<12> bin_imm(std::stoi(sub_tokens[0], nullptr, 10));
            std::string imm = bin_imm.to_string();
            std::string imm2 = imm.substr(0, 7);
            std::string imm1 = imm.substr(7, 5);
            std::string instruction = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
            int bin_instruction = std::stoi(instruction, nullptr, 2);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "jal")
        {
            std::string opcode = "1101111";
            std::string rd = lookup_table[tokens[index + 1]];
            std::cout << ic << " IC" << std::endl;
            int int_imm = labels[tokens[index + 2]] - ic;
            std::cout << int_imm << " imm" << std::endl;
            std::bitset<20> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string instruction = imm + rd + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "bne")
        {
            std::string opcode = "1100011";
            std::string func3 = "001";
            std::string rs1 = lookup_table[tokens[index + 1]];
            std::string rs2 = lookup_table[tokens[index + 2]];
            int int_imm = labels[tokens[index + 3]] - ic;
            std::bitset<12> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string imm2 = imm.substr(0, 7);
            std::string imm1 = imm.substr(7, 5);
            std::string instruction = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "beq")
        {
            std::string opcode = "1100011";
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[index + 1]];
            std::string rs2 = lookup_table[tokens[index + 2]];
            int int_imm = labels[tokens[index + 3]] - ic;
            std::bitset<12> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string imm2 = imm.substr(0, 7);
            std::string imm1 = imm.substr(7, 5);
            std::string instruction = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "blt")
        {
            std::string opcode = "1100011";
            std::string func3 = "100";
            std::string rs1 = lookup_table[tokens[index + 1]];
            std::string rs2 = lookup_table[tokens[index + 2]];
            int int_imm = labels[tokens[index + 3]] - ic;
            std::bitset<12> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string imm2 = imm.substr(0, 7);
            std::string imm1 = imm.substr(7, 5);
            std::string instruction = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "bge")
        {
            std::string opcode = "1100011";
            std::string func3 = "101";
            std::string rs1 = lookup_table[tokens[index + 1]];
            std::string rs2 = lookup_table[tokens[index + 2]];
            int int_imm = labels[tokens[index + 3]] - ic;
            std::bitset<12> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string imm2 = imm.substr(0, 7);
            std::string imm1 = imm.substr(7, 5);
            std::string instruction = imm2 + rs2 + rs1 + func3 + imm1 + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "addi")
        {
            std::string opcode = "0010011";
            std::string rd = lookup_table[tokens[index + 1]];
            std::string func3 = "000";
            std::string rs1 = lookup_table[tokens[index + 2]];
            int int_imm = std::stoi(tokens[index + 3]);
            std::bitset<12> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string instruction = imm + rs1 + func3 + rd + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if (tokens[index] == "la")
        {
            std::string opcode = "0010111";
            std::string rd = lookup_table[tokens[index + 1]];
            int int_imm = labels[tokens[index + 2]];
            std::bitset<20> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string instruction = imm + rd + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if(tokens[index] == "j"){
            std::string opcode = "1101111";
            std::string rd = "00000";
            std::cout << ic << " IC" << std::endl;
            int int_imm = labels[tokens[index + 1]] - ic;
            std::cout << int_imm << " imm" << std::endl;
            std::bitset<20> bin_imm(int_imm);
            std::string imm = bin_imm.to_string();
            std::string instruction = imm + rd + opcode;
            int bin_instruction = bin_to_int(instruction);
            result.push_back(bin_instruction);
        }
        else if(tokens[index] == "#" || tokens[index][0] == '#'){
            continue;
        }
        ic++;
    }
    File.close();
    result_pair.first = result;
    result_pair.second = data;
    return result_pair;
}
