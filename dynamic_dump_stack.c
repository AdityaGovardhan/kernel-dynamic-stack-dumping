#include<linux/syscalls.h>
#include<linux/kernel.h>
#include<linux/kprobes.h>
#include<linux/sched.h>
#include<linux/err.h>
#include<linux/slab.h>
#include<linux/uaccess.h>

static struct kprobe kp;

// owner process pid and tgid
static int pid;
static int ppid;

//operation mode
static int dumpmode;
static int enabler_dumpid;

int pre_handler(struct kprobe *p, struct pt_regs *regs){
	printk(KERN_INFO "dumpmode = %d\n", dumpmode);
	printk(KERN_INFO "current_pid = %d enabler_pid = %d\n", current->pid, pid);
	printk(KERN_INFO "current_ppid = %d enabler_ppid = %d", current->tgid, ppid);

	// enabled for the owner process
	if(dumpmode == 0){
		if(current->pid == pid){
			printk(KERN_INFO "thus probe enabled for same process\n");
			dump_stack();
		}
	}

	// enabled for the processes that have the same parent process as the owner
	else if(dumpmode == 1){
		if(current->tgid == ppid){
			printk(KERN_INFO "thus probe enabled for same parent\n");
			dump_stack();
		}
	}

	// enabled for any processes
	else{
		printk(KERN_INFO "thus probe enabled for any process\n");
		dump_stack();
	}

	return 0;
}

void post_handler(struct kprobe *p, struct pt_regs *regs, unsigned long flags){
}

SYSCALL_DEFINE2(insdump, const char __user *, symbolname, unsigned int, mode){

#ifdef CONFIG_DYNAMIC_DUMP_STACK
	char *addr_buf;
	unsigned int addr;

	if(!(addr_buf = kmalloc(sizeof(char) * 11, GFP_KERNEL))){
		return -ENOMEM;
	}
	strncpy_from_user(addr_buf, symbolname, 11);
	kstrtouint(addr_buf, 0, &addr);

	pid = current->pid;
	ppid = current->tgid;

	dumpmode = mode;
	enabler_dumpid = 0;

	kp.pre_handler = pre_handler; 
	kp.post_handler = post_handler; 
	kp.addr = (kprobe_opcode_t *)addr;
	
	register_kprobe(&kp);

	printk(KERN_INFO "pid = %d PROBE REGISTERED\n", current->pid);

	return enabler_dumpid; //return dumpid for rmdump

#else
	return 0;
#endif
}

SYSCALL_DEFINE1(rmdump, unsigned int, dumpid){

#ifdef CONFIG_DYNAMIC_DUMP_STACK
	//owner of the dump_stack operation is responsible to make a call to remove the dump_stack operation
	if(current->pid == pid && dumpid == enabler_dumpid){
		unregister_kprobe(&kp);
		printk(KERN_INFO "pid = %d PROBE UNREGISTERED\n", current->pid);
		return 0;
	}
	
	// else return -EINVAL
	else{
		printk(KERN_INFO "pid = %d probe unregistration failed, process not the owner\n", current->pid);
		return -EINVAL;
	}
#else
	return 0;
#endif
}
