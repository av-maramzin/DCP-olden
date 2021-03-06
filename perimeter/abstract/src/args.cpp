/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <cstdlib>
#include <string>
#include <iostream>

int NumNodes;

bool parallel;

int dealwithargs(int argc, char *argv[])
{
  int level;

  if (argc > 3)
    if (std::atoi(argv[3]) == 1) {
        parallel = true;
    } else if (std::atoi(argv[3]) == 0) {
        parallel = false;
    } else {
        parallel = false;
    }
  else
    parallel = false;

  if (argc > 2)
    NumNodes = std::atoi(argv[2]);
  else
    NumNodes = 1;

  if (argc > 1)
    level = std::atoi(argv[1]);
  else
    level = 11;

  return level;

}

