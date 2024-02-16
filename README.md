# Waku-Waku

## 16/02/2024 5:30 pm

We decided to make an assembler which reads a text file having assembly code.
We encode each line/instruction in the code into binary format based on RISC-V ISA. 
We then convert the binary string to an integer and store it in the memory.
The core then fetches this instruction, converts it into binary string, decodes the instruction and executes it.

## 15/02/2024 10:15 pm

We decided not to have seperate ALU and CU abstractions, all of the functionality is added within the core.
How and where to store instructions is still not entirely resolved. Should consult sir.
Added base classes for core and processor.

## 15/02/2024 10:15 am

We decided to use C++ to simulate the RISC-V processor. Had a convo on how and where to store the instructions. Thought of encoding'em in binary.
