#include "syscall.h"
int main()
{
    char fn[5];
    char cont[7];
    char buffer[50];
    int fd;
    char c;
    int i;
    OpenFileId input = ConsoleInput;
    OpenFileId output = ConsoleOutput;
    fn[0] = 'f'; fn[1] = 'i'; fn[2] = 'l'; fn[3] = 'e'; fn[4] = '\0';
    cont[0] = 'n'; cont[1] = 'a'; cont[2] = 'c'; cont[3] = 'h'; cont[4] = 'o'; cont[5] = 's'; 
    cont[6] = '\0'; 
    fd = Open(fn);
    //Write(cont, 11, output);
    Write(cont, 7, fd);

    Read(buffer, 7, fd);
    Close(fd);
    Exit(99);
}