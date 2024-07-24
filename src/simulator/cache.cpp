#include "cache.h"

std::pair<int, int> Cache::getHitsMisses()
{
  return {hits, misses};
}
Cache::Cache(int cache_size, int block_size, int associativity, int policy, int cache_number)
{
  this->cache_size = cache_size;
  this->block_size = block_size;
  this->associativity = associativity;
  this->sets = cache_size / block_size / associativity;
  this->number_of_index_bits = (int)log2(sets);
  this->number_of_offset_bits = (int)log2(block_size);
  this->number_of_tag_bits = 32 - this->number_of_index_bits - this->number_of_offset_bits;
  this->policy = policy;
  this->cache_number = cache_number;
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
  set.resize(associativity, {-1, {block, {-1, 0}}});
  cache.resize(sets, set);
}

cache_state_type Cache::getCache()
{
  return cache_states;
}

void Cache::update_MSI()
{
  for (int index = 0; index < sets; index++)
  {

    for (int i = 0; i < cache[index].size(); i++)
    {
      if (change_history[cache_number].count((cache[index][i].first << number_of_index_bits) + index))
      {
        if (change_history[cache_number][(cache[index][i].first << number_of_index_bits) + index] == 1)
        {
          cache[index][i].second.second.second = 2;
        }
        if (change_history[cache_number][(cache[index][i].first << number_of_index_bits) + index] == 0)
        {
          cache[index][i].second.second.second = 0;
        }
        change_history[cache_number].erase(cache[index][i].first); // used it
      }
    }
  }
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
  update_MSI();
  int number = cache_number ? 0 : 1;
  int tag = address >> (32 - number_of_tag_bits);
  int offset = address & ((1 << number_of_offset_bits) - 1);
  int index = (address >> number_of_offset_bits) & ((1 << number_of_index_bits) - 1);
  int msi_address = (address & ~((1 << number_of_offset_bits) - 1)) >> number_of_offset_bits;

  bool found = false;
  int tag2 = 0;
  for (int i = 0; i < cache[index].size(); i++)
  {
    if (cache[index][i].first == tag && cache[index][i].second.second.first != -1)
    {
      tag2 = i;
      found = true;
    }
  }
  if (cache[index][tag2].second.second.second == 2 && found)
  {
    cache[index][tag2].second.second.first = -1;
  }
  else if (found)
  {

    hits++;
    for (auto block = cache[index].begin(); block != cache[index].end(); block++)
    {
      if (block->second.second.first != -1)
        block->second.second.first++;
    }
    cache[index][tag2].second.second.first = 1;
    return {cache[index][tag2].second.first[offset], true};
  }

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
    if (block->second.second.first != -1)
      block->second.second.first++;
  }
  for (int i = 0; i < cache[index].size(); i++)
  {
    if (cache[index][i].first == tag)
    {
      tag2 = i;
      found = true;
    }
  }
  cache[index][tag2].second.second.first = 1;
  cache[index][tag2].second.second.second = 0;
  change_history[number][msi_address] = 0;

  for (int i = 0; i < block_size; i++)
  {
    writeCacheState(cache[index][tag2].second.first[i], cycle, index * associativity * block_size + tag2 * block_size + i, tag, index);
  }

  return {cache[index][tag2].second.first[offset], false};
}

void Cache::LRU(std::vector<int> block, set_type &set, int tag)
{
  auto itr = set.begin();
  int MRU = itr->second.second.first;
  if (MRU == -1)
  {
    set[itr - set.begin()] = {tag, {block, {1, 0}}};
    return;
  }
  for (auto i = set.begin(); i != set.end(); i++)
  {
    if (i->second.second.first == -1)
    {
      set[i - set.begin()] = {tag, {block, {1, 0}}};
      return;
    }
    else if (i->second.second.first > MRU)
    {
      MRU = i->second.second.first;
      itr = i;
    }
  }
  set[itr - set.begin()] = {tag, {block, {1, 0}}};
}

void Cache::RRP(std::vector<int> block, set_type &set, int tag)
{
  int index = rand() % associativity;
  set[index] = {tag, {block, {1, 0}}};
}

void Cache::write(int address, int data, int cycle)
{
  update_MSI();
  int number = cache_number ? 0 : 1;
  int tag = address >> (32 - number_of_tag_bits);
  int offset = address & ((1 << number_of_offset_bits) - 1);
  int msi_address = (address & ~((1 << number_of_offset_bits) - 1)) >> number_of_offset_bits;
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
      if (block->second.second.first != -1)
        block->second.second.first++;
    }
    cache[index][tag2].second.second.first = 1;
    cache[index][tag2].second.second.second = 1; // modified
    cache[index][tag2].second.first[offset] = data;

    change_history[number][msi_address] = 1;

    writeCacheState(cache[index][tag2].second.first[offset], cycle, index * associativity * block_size + tag2 * block_size + offset, tag, index);
  }
  else
  {
    misses++;
  }

  // We are not fetching if it is not there. We just update memory.
}