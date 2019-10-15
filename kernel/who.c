//kernel/who.c
//_syscall1(int,iam,const char*,name)
//_syscall2(int, whoami, char*, name, unsigned int, size)
#include <linux/kernel.h>
#include <asm/segment.h>
char myname[24];
int sys_whoami(char *name, unsigned int size)
{
		int mynameStrlen = 0;
		int i = 0;
		for (mynameStrlen = 0; myname[mynameStrlen] != '\0'; mynameStrlen++)
		//printk("enter sys_whoami\n");
		//printk("whoami name is %s\n",myname);
		if (size < mynameStrlen)
				return -1;
		while ( i < mynameStrlen) {
				put_fs_byte(myname[i], &name[i]);
				i++;
		}
		return mynameStrlen; 
}
int sys_iam(const char* name)
{
		int i = 0;
		const char *cur = name;
		char c;
		char temp[24];
		
		//printk("enter sys_iam\n");
		while ((c = get_fs_byte(cur)) != '\0' && i < 23) {
				temp[i] = c;
				i++;
				cur++;
		}
		//if (i >= 23 && c != '\0')
		if (i >= 23)
				return -1;
		temp[i] = '\0';
		i = 0;
		while ((myname[i] = temp[i]) != '\0') i++;
		//printk("iam name is %s\n", myname);
		return i; // i doesn't contain '\0'
}
