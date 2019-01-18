#include <unistd.h>	
#include <sys/syscall.h>
#include <stdio.h>
#include <linux/kernel.h>

#define insdump 359
#define	rmdump	360

int main(){

	char* var = "0xc1045ad0"; //address of getpid syscall
	int ret1, ret3;

	ret1 = syscall(insdump, var, 0);

	fork();

	getpid();

	ret3 = syscall(rmdump, ret1); // succeeds for parent, fails for child
	
	return ret3;
}