#include "swfunix.h"
#include<time.h>
#include<unistd.h>

ULONGLONG GetTickCount64()
{
    clock_t t;
    t=clock();
    return static_cast<double>(t*1000/CLOCKS_PER_SEC);
}

void Sleep(const double n)
{
    sleep(n/1000);
}
bool kbhit(){return false;}

char getch(){return 0;}

