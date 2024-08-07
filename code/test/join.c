#include "syscall.h"
int main()
{
    SpaceId pid;
    pid = Exec("../test/bar.noff");
    Join(pid);
    Exit(1);
    /* not reached */
}