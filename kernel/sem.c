#include <unistd.h>
#include <errno.h>
#include <asm/segment.h>
#include <asm/system.h>
#define SEM_LIST_LENGTH 5
#define SEM_NAME_LENGTH 20

sem_t sem_list[SEM_LIST_LENGTH] = 
{
		{{'\0',},0,NULL},
		{{'\0',},0,NULL},
		{{'\0',},0,NULL},
		{{'\0',},0,NULL},
		{{'\0',},0,NULL}
};

sem_t *sys_sem_open(const char *name, unsigned int value)
{
		char nbuf[SEM_NAME_LENGTH];
		int i = 0;
		for (i = 0; nbuf[i] = get_fs_byte(name + i); i++);

		sem_t *result = NULL;

		for (i = 0; i < SEM_LIST_LENGTH; i++) {
				if (!strcmp(sem_list[i].name, nbuf)) {
						result = &sem_list[i];
						printk("kernel sem %s found\n",result->name);
						return result;
				}
		}

		for (i = 0; i < SEM_LIST_LENGTH; i++) {
				if (sem_list[i].name[0] == '\0') {
						strcpy(sem_list[i].name, nbuf);
						sem_list[i].value = value;
						sem_list[i].queue = NULL;
						result = &sem_list[i];
						printk("sem %s created, value = %d\n", result->name, result->value);
						return result;

				}
		}
		return result;
}

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
		char nbuf[SEM_NAME_LENGTH];
		int i = 0;
		for (i = 0; nbuf[i] = get_fs_byte(name + i); i++);

		for (i = 0; i < SEM_LIST_LENGTH; i++) {
				if (!strcmp(sem_list[i].name, nbuf)) {
						printk("sem %s unlinked\n", sem_list[i].name);
						sem_list[i].name[0] = '\0';
						sem_list[i].value = 0;
						sem_list[i].queue = NULL;
						return 0;
				}
		}
}
