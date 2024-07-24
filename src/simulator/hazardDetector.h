#ifndef HAZARD
#define HAZARD

#include <vector>
#include "state.h"

class HazardDetector
{
public:
    void hazard_without_forwarding(std::vector<State> states, int &hazard_count);
    void hazard_with_forwarding(std::vector<State> &states, int &hazard_count, bool &if_stall, int &stall_pos);
};

#endif