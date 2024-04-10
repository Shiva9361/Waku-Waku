#include <cmath>
#include <bitset>
#include <iostream>
#include <bits/stdc++.h>

#define block_type std::pair<std::vector<int>, int>
#define set_type std::vector<std::pair<int, block_type>>
#define cache_type std::vector<set_type>
#define cache_state_type std::unordered_map<int, std::pair<std::vector<int>, std::vector<std::pair<int, int>>>>
class Cache
{
private:
    /* data */
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
    Cache(int cache_size, int block_size, int associativity, int policy);
    std::pair<int, bool> read(int address, int *memory, int cycle);
    void write(int address, int data, int cycle);
    void LRU(std::vector<int> block, set_type &set, int tag);
    void RRP(std::vector<int> block, set_type &set, int tag);
    cache_state_type getCache();
    std::pair<int, int> getHitsMisses();
    void writeCacheState(int data, int cycle, int location, int tag, int index);
};
std::pair<int, int> Cache::getHitsMisses()
{
    return {hits, misses};
}
Cache::Cache(int cache_size, int block_size, int associativity, int policy)
{
    this->cache_size = cache_size;
    this->block_size = block_size;
    this->associativity = associativity;
    this->sets = cache_size / block_size / associativity;
    this->number_of_index_bits = (int)log2(sets);
    this->number_of_offset_bits = (int)log2(block_size);
    this->number_of_tag_bits = 32 - this->number_of_index_bits - this->number_of_offset_bits;
    this->policy = policy;
#ifdef DEBUG
    std::cout << "ib " << number_of_index_bits << std::endl;
    std::cout << "ob " << number_of_offset_bits << std::endl;
    std::cout << "tb " << number_of_tag_bits << std::endl;
#endif
    hits = 0;
    misses = 0;
    set_type set;
    std::vector<int> block;
    block.resize(block_size);
    set.resize(associativity, {-1, {block, -1}});
    cache.resize(sets, set);

    // #ifdef DEBUG
    //     for (auto i : cache)
    //     {
    //         for (auto j : i)
    //         {
    //             auto x = j.second;
    //             for (auto k : x.first)
    //             {
    //                 std::cout << k << " " << x.second << " ";
    //             }
    //             std::cout << std::endl;
    //         }
    //         std::cout << std::endl;
    //     }
    // #endif
}

cache_state_type Cache::getCache()
{
    return cache_states;
}
void Cache::writeCacheState(int data, int cycle, int location, int tag, int index)
{
    cache_states[cycle].first.resize(2);
    cache_states[cycle].first[0] = index;
    cache_states[cycle].first[1] = tag;
    cache_states[cycle].second.push_back({data, location});
}
std::pair<int, bool> Cache::read(int address, int *memory, int cycle)
{

    int tag = address >> (32 - number_of_tag_bits);
    int offset = address & ((1 << number_of_offset_bits) - 1);
    int index = (address >> number_of_offset_bits) & ((1 << number_of_index_bits) - 1);

    bool found = false;
    int tag2 = 0;
    for (int i = 0; i < cache[index].size(); i++)
    {
        if (cache[index][i].first == tag)
        {
            tag2 = i;
            found = true;
        }
    }
    if (found)
    {
        hits++;
        for (auto block = cache[index].begin(); block != cache[index].end(); block++)
        {
            if (block->second.second != -1)
                block->second.second++;
        }
        cache[index][tag2].second.second = 1;
        return {cache[index][tag2].second.first[offset], true};
    }
    else
    {
        misses++;
        address = (address & ~((1 << number_of_offset_bits) - 1)); // offsetting to get block
        std::vector<int> block;
        for (int i = address; i < address + block_size; i++)
        {
            block.push_back(memory[i]);
        }
        if (policy == 0)
            LRU(block, cache[index], tag);
        else if (policy == 1)
            RRP(block, cache[index], tag);

        for (auto block = cache[index].begin(); block != cache[index].end(); block++)
        {
            if (block->second.second != -1)
                block->second.second++;
        }
        for (int i = 0; i < cache[index].size(); i++)
        {
            if (cache[index][i].first == tag)
            {
                tag2 = i;
                found = true;
            }
        }
        cache[index][tag2].second.second = 1;
    }

#ifdef DEBUG
    for (auto i : cache)
    {
        for (auto j : i)
        {
            auto x = j.second;
            for (auto k : x.first)
            {
                std::cout << k << " " << x.second << " ";
            }
            std::cout << std::endl;
        }
        std::cout << std::endl;
    }
    std::cout << "eoc";
#endif
    for (int i = 0; i < block_size; i++)
    {
        writeCacheState(cache[index][tag2].second.first[i], cycle, index * associativity * block_size + tag2 * block_size + i, tag, index);
    }

    return {cache[index][tag2].second.first[offset], false};
}

void Cache::LRU(std::vector<int> block, set_type &set, int tag)
{
    auto itr = set.begin();
    int MRU = itr->second.second;
    if (MRU == -1)
    {
        set[itr - set.begin()] = {tag, {block, 1}};
        return;
    }
    for (auto i = set.begin(); i != set.end(); i++)
    {
        if (i->second.second == -1)
        {
            set[i - set.begin()] = {tag, {block, 1}};
            return;
        }
        else if (i->second.second > MRU)
        {
            MRU = i->second.second;
            itr = i;
        }
    }
    set[itr - set.begin()] = {tag, {block, 1}};
}

void Cache::RRP(std::vector<int> block, set_type &set, int tag)
{
    int index = rand() % associativity;
    set[index] = {tag, {block, 1}};
}

void Cache::write(int address, int data, int cycle)
{
    int tag = address >> (32 - number_of_tag_bits);
    int offset = address & ((1 << number_of_offset_bits) - 1);
    int index = (address >> number_of_offset_bits) & ((1 << number_of_index_bits) - 1);

    bool found = false;
    int tag2 = 0;

    for (int i = 0; i < cache[index].size(); i++)
    {
        if (cache[index][i].first == tag)
        {
            tag2 = i;
            found = true;
        }
    }
    if (found)
    {
        hits++;
        for (auto block = cache[index].begin(); block != cache[index].end(); block++)
        {
            if (block->second.second != -1)
                block->second.second++;
        }
        cache[index][tag2].second.second = 1;
        cache[index][tag2].second.first[offset] = data;

        writeCacheState(cache[index][tag2].second.first[offset], cycle, index * associativity * block_size + tag2 * block_size + offset, tag, index);
    }
    else
    {
        misses++;
    }

    // We are not fetching if it is not there. We just update memory.
}
