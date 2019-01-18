# kernel-dynamic-stack-dumping

This software is a patch for enhancing the kernel with dynamic dump_stack system calls.
Two system calls are implemented:
1) insdump, with system call number 359 - this system call expects the text symbol address and mode of operation (dumpmode) and returns a unique dumpid. dump_stack is inserted using kprobes before the symbol and the dumpid is returned

2) rmdump, with system call number 360 - this system call expects the dumpid of the dump_stack which needs to be removed. kprobes are removed from the entered location and the dump_stack is disabled

Compilation and Usage:
----------------------
The kernel is modified using the patch file
The testapp file is compiled using "make" command
The testapp file is ran using ./testapp

Note:
1) The process which enables the dump_stack (insdump call) can only disable it (rmdump call)
2) The dump_stack is enabled for the owner process if dumpmode is 0
3) The dump_stack is enabled for the owner process and it's siblings if dumpmode is 1
4) The dump_stack is enabled for all process if dumpmode is anything else
