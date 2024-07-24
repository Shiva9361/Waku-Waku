#include "state.h"

State::State(int pc)
{
  this->pc = pc;
  is_dummy = false;
  rs1 = -1;
  rs2 = -1;
  rd = -1;
  operand1 = 0;
  is_operand1 = false;
  is_operand2 = false;
  latency = 1;
  operand2 = 0;
  write = false;
  next_pc = -1;
  branch_taken = false;
  miss = false;
  i_fetched = false;
  m_fetched = false;
  fetch_latency = false;
  mem_latency = false;
  is_mem_instruction = false;
  BTB_miss = false;
  predicted = false;
}