#include "util.h"

int bin_to_int(std::string bin)
{
  int n = bin.length();
  int r = 0;
  int w = 1;
  for (int i = n - 1; i >= 0; i--)
  {
    if (i == 1)
    {
      if (bin[i] == '1')
      {
        r += w;
      }
      w *= -2;
      continue;
    }
    else if (bin[i] == '1')
    {
      r += w;
    }
    w *= 2;
  }
  return r;
}
