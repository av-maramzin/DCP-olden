/* For copyright information, see olden_v1.0/COPYRIGHT */

#include "em3d.h"

int NumNodes;

extern int DebugFlag;

void dealwithargs(int argc, char *argv[])
{
  if (argc > 5)
    DebugFlag = atoi(argv[5]);
  else
    DebugFlag = 0;

  if (argc > 4)
    NumNodes = atoi(argv[4]);
  else
    NumNodes = 1;

  if (argc > 1)
    n_nodes = atoi(argv[1]);
  else
    n_nodes = 64;

  if (argc > 2)
    d_nodes = atoi(argv[2]);
  else
    d_nodes = 3;

  if (argc > 3)
    local_p = atoi(argv[3]);
  else
    local_p = 75;
}


