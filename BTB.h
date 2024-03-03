#include <map>
#include <vector>

class BTB{
    private:
        std::map<int, std::vector<int>> table;
    public:
        BTB();
        bool find(int pc);
        void enter(int pc);
        bool predict(int pc);
        int getTarget(int pc);
};
bool BTB::find(int pc){
    return table.find(pc) != table.end();
}
void BTB::enter(){
    
}
bool BTB::predict(int pc){
    return table[pc][0];
}
int BTB::getTarget(int pc){
    return table[pc][1];
}