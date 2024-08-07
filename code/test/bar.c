#include "syscall.h"

int main(){
	Exec("../test/exec.noff");
	Exit(1);
	Halt();
}