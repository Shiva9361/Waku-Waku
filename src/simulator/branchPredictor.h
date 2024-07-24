#ifndef BRANCHPREDICTOR
#define BRANCHPREDICTOR

#include <unordered_map>
#include <vector>
#include "state.h"

class BranchPredictor
{
private:
    std::unordered_map<int, int> BTB;
    std::vector<bool> states;
    bool prediction;

public:
    void predict(State &state);
    void BTB_access(State &state);
    void add(State &state);
    void update(bool resolve);
    BranchPredictor();
};

#endif