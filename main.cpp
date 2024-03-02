#define PRINT ;
#include "processor.h"
int main(int argc,  char* argv [])
{
    std::string file1= "codes/bubble_sort.s",file2 = "codes/selection_sort.s";
    if (argc > 2){
        file1 = (std::string)argv[1];
        file2 = (std::string)argv[2];
    }
    bool pipelined = false;
    bool forwarding = false;
    if ((std::string) argv[3] == "true") pipelined = true;
    if ((std::string) argv[4] == "true") forwarding = true; 
    Processor processsor(file1, file2,pipelined,forwarding);
    return 0;
}