#include "string.h"
#include "getchMethod.h"

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

void *passwordPrinting(char *pass)
{
  int i = 0;
  char a;

  for (i = 0;;)
  {
    a = getch();
    if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9'))
    {
      pass[i] = a;
      ++i;
      cout << "*";
    }
    if (i != 0)
    {
      if (a == '\n')
      {
        break;
      }
    }
  }
}

void split(char *string, char *deli)
{
  list<char *> buff;
  int i = 0;
  char *bgn = string;
  char *end;
  while ((end = strpbrk(bgn, " ")) != 0)
  {
    buff.push_back(bgn);
    *end = '\0';
    bgn = end + 1;
  }
  buff.push_back(bgn);

  for (char *temp : buff)
    cout << temp << endl;

  // return buff;
}