//_syscall1(int,iam,const char*,name)
//_syscall2(int, whoami, char*, name, unsigned int, size)
#include <linux/kernel.h>
int sys_whoami(char *name, unsigned int size)
{}
int sys_iam(const char* name)
{
		printk("I am xjk");
}
