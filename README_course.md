# Waku-Waku

Please don't run the build_frontend script,
you will have to manually modify index.html to make
everything work again

Also prefer running in linux/docker

## 10/5/2034 11:00 pm

I was travelling back home. So Darshan added some stuff.
Our "Always-Not-Taken" predictor evolved into a "2-bit" predictor(ref-Pokemon) with 'true' have higher priority.
Also now both the cores have private caches.
MSI cache coherence protocol was implemented to reduce the amount of traffic in the coherency interconnect.

## 7/04/2024 11:00 pm

We made frontend for cache and memory.
We saw that it was very slow as we were sending all the states.
So we made it incremental and rendered it based on clock cycles.
Darshan added docker file, requirements and dependencies.

## 31/03/2024 8:00 pm

We added cache. We are storing both the addresses and data in the cache.
We implemented Write through and No-write allocate policies.
We implemented the LRU cache replacement policy.
We were thinking of what other policy to implement. Then we implemented Random replacement policy.
We came to know that even ripes has the same policies(lol).
Later that night we added stalls for the IF and MEM stages.
Cache is working fine. Spent lesser time than the previous phases

## 26/03/2024 10:45 pm

Soo, we still haven't started phase 3, youtube recomended me some videos related to pybind11, to import cpp to
python, which got my atttention. As I have nothing to do after messing up test2, I dove into the rabbit hole,
ended up adding pybind support and directly importing the processor into python.

The problem with the gui was the use of global variables which would never work if we wanted to deploy this project,
so I decided to add session management which was another rabbit-hole. CORS ended up creating soo much problem that at the
end I just merged both the servers and removed cors altogether. So after a day gone, sessions are working fine now.

Should start with phase 3 tomorrow after discussing with Sasaank.

## 10/03/2024 2:30 pm

We successfully completed the work. As the project is not deployed, it takes a few extra steps to run it.
first install the packages listed in requirements.md then run the frontend and backend servers seperately using the listed
commands. commands are with respect to the root folder.

## 10/03/2024 12:45 am

We spent a lot of time on the GUI, mostly done, will add remaining stuff after waking up.

## 8/03/2024 2:00pm

After spending hours on trying to do a post request from frontend to backend, finally found the bug and fixed it.
Now post requests are working and we can continue adding all features.

## 6/03/2024 10:00pm

Started installing vue pacakages, although vue2 reached eol, we decided to go with it as there weren't too many places
to go to for bugs in vue3. Doing work after NSO does seem very difficult.

## 4/03/2024 9:00 pm

Added latency support for arithmetic instructions.
We decided to screw more with our sanity by adding a gui to this simulator.
After going through pages of horror stories decided that we better not write a webserver in c++
After some thoughts, came up with quite a convoluted way of setting up a web app.
Basically python with Flask framework will be the backend, Vue.js will be front-end
Communication between frontend and backend will be using CRUD, python can then feed data into the
cpp binary execute it, return data which can then be displayed in the frontend in any way and any speed we want.

Although this might sound stupid, the alternatives were way too horrifying with only confusing documentation to
go off of.

## 4/03/2024 2:15 am

So we spent the whole day fixing stupid bugs that made us question overselves as to why we picked c++ instead of
warm and comforting python but I digress, Anyway data forwarding seems to be done and bubble / selection sorts
worked.. so, well the time we spent was not completely wasted ig. We thought of using btb and doing good predictions
called it of after spending too much time with bugs and just added a lame static predictor.

## 2/03/2024 2:00 am

So we started with pipelining. Fist we decided to pipeline only the 1st core. In order to simulate a pipeline,
we decided to have a execute function that takes a vector of 5 instructions and does the different stages
of execution on these instructions inorder to facilitate that we created a new class which was initially known as
InstructionState but then due to the verbose nature shortened to just state. We identified that for RAW hazard
we just need to stall the decode stage. In order to simulate stall, we decided to add a bool called is_dummy which
when set will not do anything with the instruction.
Stalling will happen as long as the dependent instruction is not written to memory.

## 25/02/2024 6:30 pm

Finally did some fixing, Now selection sort works.. Hurray!!. As the gui is still under development,
decided to print everything for now. After taking the size of memory and our sanity into consideration,
decided to write the memory before and after execution to a file to show the changes.
The code for both the sorts has been added into codes folder.

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
