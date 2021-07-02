#include "swfunix.h"
#include<time.h>
#include<unistd.h>

uint64_t GetTickCount64()
{
    clock_t t;
    t=clock();
    return static_cast<uint64_t>(t*1000/CLOCKS_PER_SEC);
}



