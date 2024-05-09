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

BranchPredictor::BranchPredictor()
{
    states.resize(2, false);
    prediction = true;
}
void BranchPredictor::predict(State &state)
{
    if (state.is_dummy || state.predicted)
    {
        return;
    }
    if (prediction)
    {
        BTB_access(state);
    }

    else
    {
        state.next_pc = state.pc + 1;
    }
    state.predicted = true;
}
void BranchPredictor::BTB_access(State &state)
{
    if (BTB.find(state.pc) != BTB.end())
    {
        state.next_pc = BTB[state.pc];
    }
    else
    {
        state.BTB_miss = true;
        state.next_pc = state.pc + 1;
    }
}

void BranchPredictor::add(State &state)
{
    if (state.BTB_miss && state.next_pc != state.pc + 1)
    {
        BTB[state.pc] = state.next_pc;
    }
}

void BranchPredictor::update(bool resolve)
{
    states[0] = states[1];
    states[1] = resolve ? prediction : ~prediction;
    if ((~states[0] && ~states[1] && prediction) || (states[0] && states[1] && ~prediction))
    {
        prediction = ~prediction;
    }
}