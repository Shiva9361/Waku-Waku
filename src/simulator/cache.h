#include <cmath>
#include "core.h"
class Cache
{
private:
    /* data */
    struct compare
    {
        bool operator()(std::pair<std::vector<int>, int> a, std::pair<std::vector<int>, int> b)
        {
            return a.second < b.second;
        }
    };
    int cache_size;
    int block_size;
    int associativity;
    int sets;
    int number_of_index_bits;
    int number_of_offset_bits;
    int number_of_tag_bits;
    int hits;
    int misses;
    std::vector<std::map<int, std::pair<std::vector<int>, int>, compare>> cache; // each vector is a block and has it's own recency

public:
    Cache(int cache_size, int block_size, int associativity);
    std::pair<int, bool> read(int address, int[] memory);
    void write(int address, int data);
    void LRU(std::vector<int> block, std::map<int, std::pair<std::vector<int>, int>, compare> &set, int tag);
};

Cache::Cache(int cache_size, int block_size, int associativity)
{
    this->cache_size = cache_size;
    this->block_size = block_size;
    this->associativity = associativity;
    this->sets = cache_size / block_size / associativity;
    this->number_of_index_bits = (int)log2(sets);
    this->number_of_offset_bits = (int)log2(block_size);
    this->number_of_tag_bits = 32 - this->number_of_index_bits - this->number_of_offset_bits;

    for (int i = 0; i < sets; i++)
    {
        std::map<int, std::pair<std::vector<int>, int>, compare> temp;
        cache.push_back(temp);
    }
}

std::pair<int, bool> Cache::read(int address, int[] memory)
{
    std::bitset<32> bin_address(address);
    std::string address_string = bin_address.to_string();
    std::string tag_string = address_string.substr(0, number_of_tag_bits);
    std::string index_string = address_string.substr(number_of_tag_bits, number_of_index_bits);
    std::string offset_string = address_string.substr(number_of_tag_bits + number_of_index_bits, number_of_offset_bits);

    int tag = bin_to_int(tag_string);
    int index = bin_to_int(index_string);
    int offset = bin_to_int(offset_string);
    std::map<int, std::pair<std::vector<int>, int>, compare> temp;
    temp = cache[index];
    if (temp.count(tag))
    {
        for (auto block : temp)
        {
            block.second++;
        }
        temp[tag].second = 1;
        return {temp[tag].first[offset], true};
    }

    address = (address / block_size) * block_size;
    std::vector<int> block;
    for (int i = address; i < address + block_size; i++)
    {
        block.push_back(memory[i]);
    }

    if (temp.size() == associativity)
    {
        LRU(block, temp, tag);
    }
    else
    {
        temp[tag].second = 1;
        temp[tag].first = block;
    }
    return {temp[tag].first[offset], false};
}

void Cache::LRU(std::vector<int> block, std::map<int, std::pair<std::vector<int>, int>, compare> &set, int tag)
{
    auto itr = set.begin();
    set.erase(itr);
    set.insert({tag, {block, 1}});
}

void Cache::write(int address, int data)
{
    std::bitset<32> bin_address(address);
    std::string address_string = bin_address.to_string();
    std::string tag_string = address_string.substr(0, number_of_tag_bits);
    std::string index_string = address_string.substr(number_of_tag_bits, number_of_index_bits);
    std::string offset_string = address_string.substr(number_of_tag_bits + number_of_index_bits, number_of_offset_bits);

    int tag = bin_to_int(tag_string);
    int index = bin_to_int(index_string);
    int offset = bin_to_int(offset_string);
    std::map<int, std::pair<std::vector<int>, int>, compare> temp;
    temp = cache[index];
    if (temp.count(tag))
    {
        for (auto block : temp)
        {
            block.second++;
        }
        temp[tag].second = 1;
        temp[tag].first[offset] = data;
    }
    // We are not fetching if it is not there. We just update memory.
}
