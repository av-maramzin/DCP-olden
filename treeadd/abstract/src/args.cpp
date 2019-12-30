/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "tree.h"

int NumNodes;

int runs;

extern int atoi(const char *);

int dealwithargs(int argc, char *argv[])
{
  int level;

  if (argc > 3)
    runs = atoi(argv[3]);
  else
    runs = 100;

  if (argc > 2) 
    NumNodes = atoi(argv[2]);
  else 
    NumNodes = 4;

  if (argc > 1)
    level = atoi(argv[1]);
  else
    level = 16;

  return level;
}

