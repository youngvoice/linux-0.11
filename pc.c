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
