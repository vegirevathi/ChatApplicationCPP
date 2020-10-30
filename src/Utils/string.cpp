#include <iostream>
#include <string.h>
#include "string.h"
#include "getchMethod.h"

using namespace std;

void str_overwrite_stdout() {
    printf("\r%s", "> ");
    fflush(stdout);
}

void str_trim_lf (char* arr, int length) {
  int i;
  for (i = 0; i < length; i++) { // trim \n
    if (arr[i] == '\n') {
      arr[i] = '\0';
      break;
    }
  }
}

void passwordPrinting(char *password)
{
    int i = 0;
    char a;
    for (i = 0;;)
    {
        a = getch();
        if ((a >= 'a' && a <= 'z') || (a >= 'A' && a <= 'Z') || (a >= '0' && a <= '9'))
        {
            password[i] = a;
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
