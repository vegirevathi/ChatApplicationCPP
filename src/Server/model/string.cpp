#include "string.h"

using namespace std;

void str_overwrite_stdout()
{
  printf("\r%s", "> ");
  fflush(stdout);
}

void str_trim_lf(char *arr, int length)
{
  int i;
  for (i = 0; i < length; i++)
  { // trim \n
    if (arr[i] == '\n')
    {
      arr[i] = '\0';
      break;
    }
  }
}

