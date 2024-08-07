// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1993 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "system.h"
#include "syscall.h"
extern void StartProcess(int spaceID);
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// And don't forget to increment the pc before returning. (Or else you'll
// loop making the same system call forever!
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	are in machine.h.
//----------------------------------------------------------------------

// void
// ExceptionHandler(ExceptionType which)
// {
//     int type = machine->ReadRegister(2);

//     if ((which == SyscallException) && (type == SC_Halt)) {
// 	DEBUG('a', "Shutdown, initiated by user program.\n");
//    	interrupt->Halt();
//     } else {
// 	printf("Unexpected user mode exception %d %d\n", which, type);
// 	ASSERT(FALSE);
//     }
// }

void AdvancePC(){
    machine->WriteRegister(PCReg,machine->ReadRegister(PCReg)+4);
    machine->WriteRegister(NextPCReg,machine->ReadRegister(NextPCReg)+4);
}

void
ExceptionHandler(ExceptionType which)
{
    int type = machine->ReadRegister(2);

    if (which == SyscallException) {
	DEBUG('a', "Shutdown, initiated by user program.\n");
        switch (type)
        {
        case SC_Halt/* constant-expression */:
            /* code */{
                DEBUG('a',"Shutdown, initiated by user program.\n");
                
                interrupt->Halt();
                break;
            }
        case SC_Join:
        {
            int SpaceId = machine->ReadRegister(4); // ie. ThreadId or SpaceId
            currentThread->Join(SpaceId);
            // 返回 Joinee 的退出码 waitProcessExitCode
            machine->WriteRegister(2, currentThread->pcb->waitProcessExitCode);
            AdvancePC();
            break;
        }
        case SC_Exec:
        {
            //printf("expected syscall %d\t%d\n",which,type);
            char filename[128];
            int addr = machine->ReadRegister(4);
            int i = 0;
            do
            {
                machine->ReadMem(addr + i, 1, (int *)&filename[i]);
            } while (filename[i++] != '\0');
            OpenFile *executable = fileSystem->Open(filename);
            AddrSpace *space;
            printf("filename :%s\n",filename);
            //printf("111\n");
            if (executable == NULL)
            {
                printf("Unable to open file %s\n", filename);
                return;
            }
            // new address space
            space = new AddrSpace(executable);
            space->Print();
            delete executable; // close file
            // new and fork thread char *forkedThreadName = filename;
            Thread *thread = new Thread(filename);
           
            thread->Fork(StartProcess, space->getSpaceID());
            thread->pcb->space = space; // 用户线程映射到核心线程
            // return spaceID
            machine->WriteRegister(2, space->getSpaceID());

            AdvancePC();
            //currentThread->Yield();
            break;
        }

        case SC_Exit:
        {
            //printf("currentThread: ");
            //currentThread->Print();
            //printf("\n");
            //currentThread->space->Print();
            int ExitStatus = machine->ReadRegister(4);
            machine->WriteRegister(2, ExitStatus);
            currentThread->pcb->setExitStatus(ExitStatus);
           
            if (ExitStatus == 99) // parent process exit, delele all terminated threads
            {
                 List *terminatedList = scheduler->getTerminatedList();
                 terminatedList->empty();
                // Thread *thread;
                // while ((thread = (Thread *)(terminatedList->Remove())) != NULL)
                // {
                //     delete thread;
                // }
            }
            //delete currentThread->space;
            currentThread->Finish();

            AdvancePC();
            //break;

            // printf("Execute system call of Exit()\n");
            // interrupt->Exit();
            // AdvancePC();
            // currentThread->Finish();
            // break;
        }
        case SC_Yield:
        {
            currentThread->Yield();
            AdvancePC();
            break;
        }
        case SC_Create:
        {
            int base = machine->ReadRegister(4);
            int value;
            int count = 0;
            char *FileName = new char[128];
            do
            {
                machine->ReadMem(base + count, 1, &value);
                FileName[count] = *(char *)&value;
                count++;
            }
            while (*(char *)&value != '\0' && count < 128)
                ;
            int fileDescriptor = OpenForWrite(FileName);
            if (fileDescriptor == -1)
                printf("create file %s failed!\n", FileName);
            else
                printf("create file %s succeed!, the file id is %d\n", FileName, fileDescriptor);

            Close(fileDescriptor);
            // machine->WriteRegister(2,fileDescriptor);

            AdvancePC();
            break;
        }
        case SC_Open:
        {
            int base = machine->ReadRegister(4);
            int value;
            int count = 0;
            char *FileName = new char[128];
            do
            {
                machine->ReadMem(base + count, 1, &value);
                FileName[count] = *(char *)&value;
                count++;
            } while (*(char *)&value != '\0' && count < 128);
            int fileDescriptor = OpenForReadWrite(FileName, FALSE);
            if (fileDescriptor == -1)
                printf("Open file %s failed!\n", FileName);
            else
                printf("Open file %s succeed!, the file id is %d\n", FileName, fileDescriptor);
            machine->WriteRegister(2, fileDescriptor);
            AdvancePC();
            break;
        }
        case SC_Write:
        {
            //读取参数
            int base = machine->ReadRegister(4);   // buffer
            int size = machine->ReadRegister(5);   // bytes written to file
            int fileId = machine->ReadRegister(6); // fd
            int value;
            int count = 0;
            // printf("base=%d, size=%d, fileId=%d \n",base,size,fileId );
            OpenFile *openfile = new OpenFile(fileId);
            ASSERT(openfile != NULL);

            char *buffer = new char[128];
            do
            {
                machine->ReadMem(base + count, 1, &value);
                buffer[count] = *(char *)&value;
                count++;
            } while ((*(char *)&value != '\0') && (count < size));
            buffer[size] = '\0';

            int WritePosition;
            if (fileId == 1)
                WritePosition = 0;
            else
                WritePosition = openfile->Length();

            int writtenBytes = openfile->WriteAt(buffer, size, WritePosition);
            if ((writtenBytes) == 0)
                printf("write file failed!\n");
            else
                printf("\"%s\" has wrote in file %d succeed!\n", buffer, fileId);
                // machine->WriteRegister(2,size);
                AdvancePC();
            break;
        }
        case SC_Read:
        {
            int base = machine->ReadRegister(4);
            int size = machine->ReadRegister(5);
            int fileId = machine->ReadRegister(6);
            OpenFile *openfile = new OpenFile(fileId);
            char buffer[size];
            int readnum = 0;
            readnum = openfile->Read(buffer, size);

            for (int i = 0; i < size; i++)
                if (!machine->WriteMem(base, 1, buffer[i]))
                    printf("This is something wrong.\n");
            buffer[size] = '\0';
            printf("read succeed! the content is \"%s\" , the length is %d\n", buffer, size);
            machine->WriteRegister(2, readnum);
            AdvancePC();
            break;
        }
        case SC_Close:
        {
            int fileId = machine->ReadRegister(4);
            // printf("SC_Close: fileId in $4 = %d\n",fileId);
            // void Close(int fd) in sysdep.cc

            // OpenFile* openfile=new OpenFile(fileId);
            // delete openfile; //does not work well
            Close(fileId);
            printf("File %d closed succeed!\n", fileId);
            AdvancePC();
            break;
        }
        default:
            printf("Unexpected syscall %d\t%d\n",which,type);
            ASSERT(FALSE);
        }
   	//interrupt->Halt();
    } 
    // else {
	// printf("Unexpected user mode exception %d %d\n", which, type);
	// ASSERT(FALSE);
    // }
}


