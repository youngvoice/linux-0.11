#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/stat.h>

#define __LIBRARY__
#include <unistd.h>

#define __NR_sem_open	72
#define __NR_sem_wait	73
#define __NR_sem_post	74
#define __NR_sem_unlink	75
typedef struct {
		char *name;
		int value;
		struct task_struct *queue;
} sem_t; 

sem_t *sem;

_syscall3(int, sem_open, const char*, name, unsigned int, value, sem_t**, sem)
_syscall1(int, sem_wait, sem_t*, sem)
_syscall1(int, sem_post, sem_t*, sem)
_syscall1(int, sem_unlink, const char*, name)
	

/*

int main()
{
		if (sem_open("sem1", 1, &sem) == NULL)
				printf("open error\n");
		printf("wait\n");
		if (sem_wait(sem) < 0)
				printf("wait error \n");

		return 0;
}
*
*/

#define MAX 10


int fd;
int fill = 0;
int use = 0;

void put(int value)
{
		if (lseek(fd, fill*sizeof(int), SEEK_SET) == -1)
				printf("lseek error\n");
		if (write(fd, &value, sizeof(int)) < 0)
				printf("write error\n");
		fill = (fill + 1) % MAX;
}

int get()
{
		int tmp;
		if (lseek(fd, use*sizeof(int), SEEK_SET) == -1)
				printf("lseek error\n");
		if (read(fd, &tmp, sizeof(int)) < 0)
				printf("read error\n");
		use = (use + 1) % MAX;
		return tmp;
}


sem_t *empty;
sem_t *full;
sem_t *mutex;

void *producer(void *arg) 
{
		int i;
		int loops = 501;
		for (i = 0; i < loops; i++) {
				sem_wait(empty);
				sem_wait(mutex);
				put(i);
				sem_post(mutex);
				sem_post(full);
		}
}

void *consumer(void *arg)
{
		int i;
		int loops = 501;
		int tmp;
		pid_t pid;
		pid = getpid();
		/*
		for (i = 0; i < loops; i++) {
				sem_wait(&full);
				sem_wait(&mutex);
				int tmp = get();
				sem_post(&mutex);
				sem_post(&empty);
				printf("%lu %d\n", (unsigned long)pid,tmp);
		}
		*/
		while (1)
		{
				sem_wait(full);
				sem_wait(mutex);
				tmp = get();
				printf("%lu: %d\n", (unsigned long)pid,tmp);
				sem_post(mutex);
				sem_post(empty);
		}
}

int main(int argc, char *argv[])
{
		
		pid_t c1, c2;
		pid_t p1, p2;
		if ((fd = open("buffer.txt", O_RDWR | O_CREAT | O_TRUNC)) < 0)
				printf("open error\n");
		/*
		sem_init(&empty, 1, MAX);
		sem_init(&full, 1, 0);
		sem_init(&mutex, 1, 1);
		*/
		/*
		empty = sem_open("/sem_empty", O_CREAT | O_EXCL, O_RDWR, MAX);
		full = sem_open("/sem_full", O_CREAT | O_EXCL, O_RDWR, 0);
		mutex = sem_open("/sem_mutex", O_CREAT | O_EXCL, O_RDWR, 1);
		*/
		if (sem_open("sem_empty", MAX, &empty) == 0)
				printf("sem_empty error\n");
		if (sem_open("sem_full", 0, &full) == 0)
				printf("sem_full error\n");
		if (sem_open("sem_mutex", 1, &mutex) == 0)
				printf("sem_mutex error\n");



		if (!(p1 = fork()))
		{
				producer(NULL);
				exit(0);
		}

		if (!(c1 = fork()))
		{
				consumer(NULL);
				exit(0);
		}
		
		sem_unlink("sem_empty");
		sem_unlink("sem_full");
		sem_unlink("sem_mutex");

		wait(NULL);
		wait(NULL);


		if (close(fd) < 0)
				printf("close error\n");
		return 0;

}
