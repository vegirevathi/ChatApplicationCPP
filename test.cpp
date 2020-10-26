#include <iostream>
#include <string.h>
#include <list>

using namespace std;

list<char *> split(char *string)
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
    return buff;
}

char names[10][10] = {"Sudha", "Shasha"};

char *get_online_clients()
{
	char buff[100] = {};
	for (int i = 0; i < 2; ++i)
		if (names[i])
		{
			strcat(buff, names[i]);
			strcat(buff, ",");
		}
        strcat(buff, "\0");
        cout << buff;
	return buff;
}
 
int main()
{
    // Initializing victory string
    char s1[] = "victory sudha fart sams";
    // vector<char *> buffer;
    // list<char *> buffer;

    char* buff;

    buff = get_online_clients();
    // buffer = split(s1);
    // Declaring lottery strings
    // char s2[] = "a23";
    // char s3[] = "i22";
    // char *r, *t;

    // // Use of strpbrk()
    // // r = strpbrk(s1, s2);

    // while ((r = strpbrk(s1, " ")) != 0)
    // {
    //     cout << s1 << endl;
    //     buffer.push_back(s1);
    // }

    // for (char *str : buffer)
    //     cout << str << endl;

    // Checks if player 2 has won lottery
    // printf("%s", t);

    // if (t != 0)
    //     printf("\nCongrats u have won");
    // else
    //     printf("Better luck next time");

    return (0);
}