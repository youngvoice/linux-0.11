#include <linux/sched.h>
#include <linux/kernel.h>
#include <asm/segment.h>
#include <asm/system.h>
typedef struct {
		char name[20];
		int value;
		struct task_struct *queue;
} sem_t; 
sem_t semtable[20] = {
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL},
		{{'\0',}, 0, NULL}
};

/*
 * if success return unique identifier > 0, else return NULL
 * */
//sem_t *sys_sem_open(const char *name, unsigned int value)
/*
int sys_sem_open(const char *name, unsigned int value, sem_t **sem)
{
		//find sem named name in semtable
		char myname[20];
		const char *cur = name;
		char c;
		int i = 0;
		while ((c = get_fs_byte(cur)) != '\0' && i < 19) {
				myname[i] = c;
				i++;
				cur++;
		}
		myname[i] = '\0';

		for (i = 0; i < 20; i++)
				if (!strcmp(semtable[i].name, myname)) {
						*sem = &semtable[i];
						return &semtable[i];
				}
		//if not find then create
		for (i = 0; i < 20; i++)
				if (!strcmp(semtable[i].name, "\0")) {
						strcpy(semtable[i].name, myname);
						semtable[i].value = value;
						*sem = &semtable[i];
						printk("create sucessful\n");
						return &semtable[i];
				}
		return NULL;

		//return index
}
*/
int sys_sem_open(const char *name, unsigned int value, sem_t **sem)
{
		//find sem named name in semtable
		char myname[20];
		int i = 0;

		for (i = 0; myname[i] = get_fs_byte(name + i); i++);
		for (i = 0; i < 20; i++) {
				if (!strcmp(semtable[i].name, myname)) {
						*sem = &semtable[i];
						return i;
				}
		}

		for (i = 0; i < 20; i++) {
				if (semtable[i].name[0] == '\0') {
						strcpy(semtable[i].name, myname);
						semtable[i].value = value;
						semtable[i].queue = NULL;
						*sem = &semtable[i];
						printk("%s %p\n", myname,*sem);
						return i;
				}
		}

		printk("kernel sem open fail\n");
		*sem = NULL;
		return -1;

}
/*
int sys_sem_wait(sem_t *sem)
{
		cli();
		
		while (sem->value-- < 0)
				sleep_on(&sem->queue);

		sti();
		return 0;
}
*/
int sys_sem_wait(sem_t *sem)
{
		cli();

		while (sem->value <= 0) {
				sleep_on(&(sem->queue));
		}
		sem->value--;
		sti();
		return 0;
}
/*
int sys_sem_wait(sem_t *sem)
{
		struct task_struct *tmp;
		cli();
		if (semtable[sd].value-- < 0)
		{
				//set self as blocked state
				//add self to semtable[sd].queue
				tmp = semtable[sd].queue;	
				semtable[sd].queue = current;
				//schedule()
				schedule();
		}
		sti();
}
*/
/*
int sys_sem_post(sem_t *sem)
{
		cli();
		//if (sem->value++ <= 0)
		sem->value++;
				wake_up(&sem->queue);
		sti();
		return 0;
}
*/
int sys_sem_post(sem_t *sem)
{
		cli();
		sem->value++;
		wake_up(&(sem->queue));

		sti();
		return 0;
}

int sys_sem_unlink(const char *name)
{
		//find sem named name in semtable
		char myname[20];
		const char *cur = name;
		char c;
		int i;
		while ((c = get_fs_byte(cur)) != '\0' && i < 19) {
				myname[i] = c;
				i++;
				cur++;
		}
		myname[i] = '\0';

		for (i = 0; i < 20; i++)
				if (!strcmp(semtable[i].name, myname)) {
						semtable[i].name[0] = '\0';
						semtable[i].value = 0;
						semtable[i].queue = NULL;
						return 0;
				}
		return -1;

}
