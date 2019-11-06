#include <linux/kernel.h>
#include <stdarg.h>
#include <linux/sched.h>
#include <asm/segment.h>
extern int vsprintf(char * buf, const char * fmt, va_list args);
int sprintf(char *buf, const char *fmt, ...)
{
		va_list args;
		int i;
		va_start(args, fmt);
		i = vsprintf(buf, fmt, args);
		va_end(args);
		return i;
}
/*
int proc_read(int dev, char * buf, int count, off_t * pos)
{
		struct task_struct **p;
		char *kbuf = (char *)malloc(1024*sizeof(char));
		int mesg_len = 40, mesg_times = 0;
		int i = *pos;
		sprintf((kbuf + mesg_times*mesg_len), "pid\tstate\tfather\tcounter\tstart_time\n");
		mesg_times++;
		for (p = &LAST_TASK; p > &FIRST_TASK; --p)
				if (*p)
				{
						sprintf((kbuf + mesg_times*mesg_len), "%d\t%d\t%d\t%d\t%d\n", (*p)->pid, (*p)->state, (*p)->father, (*p)->counter, (*p)->start_time);
						mesg_times++;
				}

		while (count-- > 0 && i < 65536)
		{
				put_fs_byte(*(kbuf + i), buf++);
				i++;

		}
		i -= *pos;
		*pos += i;
		free(kbuf);
		return i;
}
*/
int proc_read(struct m_inode *inode, int dev, char * buf, int count, off_t * pos)
{
		struct super_block *sb;
		struct task_struct **p;
		char *kbuf = (char *)malloc(1024*sizeof(char));
		int i = *pos;
		int kbuf_len = 0;

		if (dev == 0) {
				kbuf_len += sprintf((kbuf + kbuf_len), "pid\tstate\tfather\tcounter\tstart_time\n");
				for (p = &LAST_TASK; p > &FIRST_TASK; --p)
						if (*p)
						{
								kbuf_len += sprintf((kbuf + kbuf_len), "%d\t%d\t%d\t%d\t%d\n", (*p)->pid, (*p)->state, (*p)->father, (*p)->counter, (*p)->start_time);
						}


		}
		else if (dev == 1) {
				sb = get_super(inode->i_dev);
				kbuf_len += sprintf((kbuf + kbuf_len), "total blocks: \t%d\n", sb->s_nzones);
				
		}
		else ;
		while (count-- > 0 && i < 65536)
		{
				put_fs_byte(*(kbuf + i), buf++);
				i++;

		}
		i -= *pos;
		*pos += i;
		free(kbuf);
		return i;
}
