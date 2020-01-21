/* For copyright information, see olden_v1.0/COPYRIGHT */

#include <cstdlib>

int NumNodes = 1;

int dealwithargs(int argc, char *argv[]) {
  int level;

  if (argc > 1)
    level = std::atoi(argv[1]);
  else
    level = 64;

  return level;
}
