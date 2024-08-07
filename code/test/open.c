#include "syscall.h"
int main()
{
    char fn[5];
    fn[0] = 'f';
    fn[1] = 'i';
    fn[2] = 'l';
    fn[3] = 'e';
    fn[4] = '\0';
    Open(fn);
    Exit(1);
}