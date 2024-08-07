#ifndef PCB_H
#define PCB_H

#include "addrspace.h"
#include "machine.h"
#include "filesys.h"

class Pcb
{
public:
    int userRegisters[NumTotalRegs]; //寄存器
    AddrSpace *space; // 用户进程的地址空间
    int waitingProcessSpaceId;
    int waitProcessExitCode;
    Pcb();
    ~Pcb();
    void setExitStatus(int ExitStatus);
    int getExitStatus();
private:
    int exitStatus;
};

#endif 