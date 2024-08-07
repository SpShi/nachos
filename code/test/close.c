#include "syscall.h"
int main()
{
    char fn[5];
        int fd;
    fn[0] = 'f';
    fn[1] = 'i';
    fn[2] = 'l';
    fn[3] = 'e';
    fn[4] = '\0';
    //Create(fn);

    fd=Open(fn);
    Close(fd);
    Exit(1);
}