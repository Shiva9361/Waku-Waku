#ifndef CACHE
#define CACHE
#include <cmath>
#include <iostream>
#include <vector>
#include <unordered_map>

#define block_type std::pair<std::vector<int>, std::pair<int, int>> // 0-s,1-m, 2-i
#define set_type std::vector<std::pair<int, block_type>>
#define cache_type std::vector<set_type>
#define cache_state_type std::unordered_map<int, std::pair<std::vector<int>, std::vector<std::pair<int, int>>>>
class Cache
{
private:
    /* data */
    static inline std::vector<std::unordered_map<int, int>> change_history = {{}, {}};
    int cache_number;
    int cache_size;
    int block_size;
    int associativity;
    int sets;
    int number_of_index_bits;
    int number_of_offset_bits;
    int number_of_tag_bits;
    int hits;
    int misses;
    int policy;
    cache_type cache; // each vector is a block and has it's own recency, -1 recency denotes invalid
    cache_state_type cache_states;

public:
    Cache(int cache_size, int block_size, int associativity, int policy, int cache_number);
    std::pair<int, bool> read(int address, int *memory, int cycle);
    void write(int address, int data, int cycle);
    void LRU(std::vector<int> block, set_type &set, int tag);
    void RRP(std::vector<int> block, set_type &set, int tag);
    void update_MSI();
    cache_state_type getCache();
    std::pair<int, int> getHitsMisses();
    void writeCacheState(int data, int cycle, int location, int tag, int index);
};

#endif