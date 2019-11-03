/*
#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <sys/types.h>
#include <fcntl.h>

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)

int main()
{
		sem_t *sem_empty, *sem_full, *sem_mutex;
		int fd;
		int buf_in = 0, buf_out = 0;
		const int bufSize = 10;
		const int cNum = 2;
		const int itemNum = 500;
		int data;

		int i, j, k;
		pid_t p;
		if ((sem_empty = sem_open("empty", 10)) == NULL) {
				perror("sem_empty error\n");
				return -1;
		}
		if ((sem_full = sem_open("full", 0)) == NULL) {
				perror("sem_empty error\n");
				return -1;
		}
		if ((sem_mutex = sem_open("mutex", 1)) == NULL) {
				perror("sem_empty error\n");
				return -1;
		}

		fd = open("buffer.dat", O_RDWR | O_CREAT | O_TRUNC, 777);

		lseek(fd, bufSize*sizeof(int),SEEK_SET);
		write(fd, &buf_out, sizeof(int));


		if (!(p = fork())) {
				for (i = 0; i < itemNum; i++) {
						sem_wait(sem_empty);
						sem_wait(sem_mutex);
						lseek(fd, buf_in*sizeof(int),SEEK_SET);
						write(fd, &i, sizeof(int));
						buf_in = (buf_in + 1)%bufSize;
						sem_post(sem_mutex);
						sem_post(sem_full);
				}
				return 0;
		}

		for (j = 0; j < cNum; j++) {
				if (!(p = fork())) {
						for (k = 0; k < itemNum/cNum; k++) {
								sem_wait(sem_full);
								sem_wait(sem_mutex);

								lseek(fd, bufSize*sizeof(int), SEEK_SET);
								read(fd, &buf_out, sizeof(int));

								lseek(fd, buf_out*sizeof(int), SEEK_SET);
								read(fd, &data, sizeof(int));

								buf_out = (buf_out + 1) % bufSize;

								lseek(fd, bufSize*sizeof(int), SEEK_SET);
								write(fd, &buf_out, sizeof(int));

								printf("%d: %d\n", getpid(), data);
								fflush(stdout);
								sem_post(sem_mutex);
								sem_post(sem_empty);
						}
						return 0;
				}
		}
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		wait(NULL);
		sem_unlink("empty");
		sem_unlink("full");
		sem_unlink("mutex");
		close(fd);
		return 0;
}
*/



#define __LIBRARY__
#include "unistd.h"

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define TEXT_SZ 2048

struct shared_use_st {
		int written;
		char text[TEXT_SZ];
};

/*
int shmget(key_t key, size_t size, int shmflag);
void *shmat(int shmid, const void *shmaddr, int shmflg);
*/

_syscall3(int, shmget, key_t, key, size_t, size, int, shmflag)
_syscall3(void*, shmat, int, shmid, const void*, shmaddr, int, shmflg)
_syscall1(int, shmdt, void *, shmaddr)

int read_process()
{
		void *shm = NULL;
		struct shared_use_st *shared;
		int shmid;

		shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0);
		if (shmid == -1)
		{
				fprintf(stderr, "shmat failed\n");
				exit(EXIT_FAILURE);
		}

		shm = shmat(shmid, 0, 0);
		if (shm == (void *)-1)
		{
				fprintf(stderr, "shmat failed\n");
				exit(EXIT_FAILURE);
		}

		printf("\nMemory attached at %X\n", (int)shm);

		shared = (struct shared_use_st *) shm;
		shared->written = 0;

		while(1)
		{
				if (shared->written == 1)
				{
						printf("You wrote: %s", shared->text);
						sleep(1);
						shared->written = 0;

						if (strncmp(shared->text, "end", 3) == 0)
						{
								break;
						}
				}
				else
				{
						sleep(1);
				}
		}
		if (shmdt(shm) == -1)
		{
				fprintf(stderr, "shmdt failed\n");
				exit(EXIT_FAILURE);
		}

		exit(EXIT_SUCCESS);
}


int write_process()
{
		void *shm = NULL;
		struct shared_use_st *shared = NULL;
		int shmid;

		char buffer[BUFSIZ + 1];

		shmid = shmget((key_t)1234, sizeof(struct shared_use_st), 0);
		if (shmid == -1)
		{
				fprintf(stderr, "shmget failed\n");
				exit(EXIT_FAILURE);
		}

		shm = shmat(shmid, (void *)0, 0);
		if (shm == (void *)-1)
		{
				fprintf(stderr, "shmat failed\n");
				exit(EXIT_FAILURE);
		}

		printf("Memory attached at %X\n", (int)shm);


		shared = (struct shared_use_st *)shm;
		while(1)
		{
				while (shared->written == 1)
				{
						sleep(1);
						printf("Waiting...\n");
				}
				printf("Enter some text: ");
				fgets(buffer, BUFSIZ, stdin);
				strncpy(shared->text, buffer, TEXT_SZ);
				shared->written = 1;

				if (strncmp(buffer, "end", 3) == 0)
				{
						break;
				}
		}

		if (shmdt(shm) == -1)
		{
				fprintf(stderr, "shmdt failed\n");
				exit(EXIT_FAILURE);
		}
		sleep(2);
		exit(EXIT_SUCCESS);


}

int main()
{
		pid_t p;
		if (!(p = fork())) {
				read_process();
		}

		if (!(p = fork())) {
				write_process();
		}

		wait(NULL);
		wait(NULL);
		return 0;
}
