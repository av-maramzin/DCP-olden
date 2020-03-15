/* For copyright information, see olden_v1.0/COPYRIGHT */

extern int atoi(const char *);

int NumNodes;

int dealwithargs(int argc, char *argv[])
{
  int level;

  if (argc > 2)
    NumNodes = atoi(argv[2]);
  else
    NumNodes = 1;

  if (argc > 1)
    level = atoi(argv[1]);
  else
    level = 11;

  return level;

}
