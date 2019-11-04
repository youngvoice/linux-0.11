#define __LIBRARY__
#include <stdio.h>
#include <stdlib.h>
#include "unistd.h"
#include <sys/types.h>
#include <fcntl.h>

#define BUFSIZE 10
struct shared_buf{
		int buf_in;
		int buf_out;
		int num_buf[BUFSIZE];
};

_syscall2(sem_t *, sem_open, const char *, name, unsigned int, value)
_syscall1(int, sem_wait, sem_t *, sem)
_syscall1(int, sem_post, sem_t *, sem)
_syscall1(int, sem_unlink, const char *, name)


_syscall3(int, shmget, key_t, key, size_t, size, int, shmflag)
_syscall3(void*, shmat, int, shmid, const void*, shmaddr, int, shmflg)
_syscall1(int, shmdt, void *, shmaddr)


int main()
{
		sem_t *sem_empty, *sem_full, *sem_mutex;
		void *shm = NULL;
		struct shared_buf *shared;
		int shmid;
		int buf_in = 0, buf_out = 0;
		const int bufSize = 10;
		const int cNum = 2;
		const int itemNum = 500;
		int data;

		int i, j, k;
		pid_t p;

		shmid = shmget((key_t)1234, sizeof(struct shared_buf), 0);
		if (shmid == -1)
		{
				fprintf(stderr, "shmat failed\n");
				exit(EXIT_FAILURE);
		}


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

		shm = shmat(shmid, 0, 0);
		if (shm == (void *)-1)
		{
				fprintf(stderr, "shmat failed\n");
				exit(EXIT_FAILURE);
		}

		shared = (struct shared_buf *) shm;
		shared->buf_in = 0;
		shared->buf_out = 0;


		if (!(p = fork())) {
				shm = shmat(shmid, 0, 0);
				shared = (struct shared_buf *) shm;
				for (i = 0; i < itemNum; i++) {
						sem_wait(sem_empty);
						sem_wait(sem_mutex);
						shared->num_buf[shared->buf_in] = i;
						shared->buf_in = (shared->buf_in + 1)%bufSize;
						sem_post(sem_mutex);
						sem_post(sem_full);
				}
				while(1);
				return 0;
		}

		for (j = 0; j < cNum; j++) {
				if (!(p = fork())) {
						shm = shmat(shmid, 0, 0);
						shared = (struct shared_buf *) shm;
						for (k = 0; k < itemNum/cNum; k++) {
								sem_wait(sem_full);
								sem_wait(sem_mutex);

								data = shared->num_buf[shared->buf_out];

								shared->buf_out = (shared->buf_out + 1) % bufSize;

								printf("%d: %d\n", getpid(), data);
								fflush(stdout);
								sem_post(sem_mutex);
								sem_post(sem_empty);
						}
						while(1);
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
		return 0;
}
