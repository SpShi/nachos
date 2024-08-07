#include "pcb.h"

Pcb::Pcb()
{
    // for (int i = 0; i < MaxFileId; i++)
    //     fileIdUse[i] = false;
}

void Pcb::setExitStatus(int ExitStatus)
{
    this->exitStatus = ExitStatus;
}

int Pcb::getExitStatus()
{
    return exitStatus;
}

Pcb::~Pcb()
{
    if (space != NULL)
    {
        DEBUG('x', "delete space\n");
        delete space;
        space = NULL;
    }
}
