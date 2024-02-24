# Waku-Waku

## 25/02/2024

## 24/02/2024 10 pm

We added 'j' and print statements for a cleaner output.
same shit regarding selection sort.

## 23/02/2024 5 pm

Bubble sort was working!!!
Fed up while trying to write assembly code for selection sort.

## 22/02/2024 7pm

Sasaank was in the 'Convocation Committee'. so he was not available.
Shiva changed the way memory is accessed.
Later that day, we added '.data' segment.

## 17/02/2024 1 am

We decided to partition the memory for each core based on RISC-V ISA.
Learnt to close the current file before opening the next one (lol).
Decided to read the input file once before encoding it to find and store the labels.
To execute branch and jump instructions, we decided to use 'std::stoi()' to retrieve the immediate values from the encoded instructions.
But the program counter started to act as weird as possible.
Took an eternity to figure out wth is happening.
The culprit turns out to be 'std::stoi()'(lol).
It is converting the binary string to its unsigned value.
So had to create our own function to convert a binary string to its 2's complement value.
Now the processor also supports labels, 'lw', 'sw', 'jal', 'bne'.

## 16/02/2024 5:30 pm

We decided to make an assembler which reads a text file having assembly code.
We encode each line/instruction of the code into binary format based on RISC-V ISA. 
We then convert the binary string to an integer and store it in the memory.
The core then fetches this instruction, converts it back into binary string, decodes the instruction and executes it.
The processor at this point supports 'add', 'sub', 'mul', 'div'.

## 15/02/2024 10:15 pm

We decided not to have seperate ALU and CU abstractions, all of the functionality is added within the core.
How and where to store instructions is still not entirely resolved. Should consult sir.
Added base classes for core and processor.

## 15/02/2024 10:15 am

We decided to use C++ to simulate the RISC-V processor. Had a convo on how and where to store the instructions. Thought of encoding'em in binary.
