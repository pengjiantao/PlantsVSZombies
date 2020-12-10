#ifndef SWFUNIX_H
#define SWFUNIX_H
#include<stdint.h>

#define ULONGLONG unsigned long int
#define WORD uint8_t
#define STD_OUTPUT_HANDLE 0
typedef   struct {short X;short Y;} COORD;

ULONGLONG GetTickCount64();

void Sleep(const double n);

bool kbhit();

char getch();


#endif // SWFUNIX_H
