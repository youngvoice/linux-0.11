#include <unistd.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/mm.h>
#include <errno.h>
/*
 * Create or open a page memory and return shmid of the shared page. If the page memory has been built then return shmid. 
 * @return shmid of the page, if there no free page the return -1 and set errno as ENOMEM. 
 * @key All process that share a same page use the same key.
 * @size if size is over the size of a page, then return -1 and set errno as EINVAL. 
 * @shmflg this parameter can be ignored.
 *
 * */
int sys_shmget(key_t key, size_t size, int shmflg);

/*
 * Map the shared page(shmid) into the virtual address space of the current process.
 * @return the begin address of the virtual address of the shared page.
 * @shmid If shmid is invalid, the return -1 and set errno as EINVAL.
 * @shmaddr: ignored
 * @shmflg: ignored
 * */
void *sys_shmat(int shmid, const void *shmaddr, int shmflg);

int sys_shmdt(void *shmaddr);

static shm_ds shm_list[SHM_SIZE] = {{0, 0, 0}};
int sys_shmget(key_t key, size_t size, int shmflg)
{
		int i;
		void *page;
		if (size > PAGE_SIZE)
				return -EINVAL;
		page = get_free_page();
		if (!page)
				return -ENOMEM;
		printk("shmget get memory's address is %p\n", page);
		for (i = 0; i < SHM_SIZE; i++)
		{
				if (shm_list[i].key == key)
						return i;
		}
		for (i = 0; i < SHM_SIZE; i++)
		{
				if (shm_list[i].key == 0)
				{
						shm_list[i].page = page;
						shm_list[i].key = key;
						shm_list[i].size = size;
						return i;
				}
		}
		return -1;
}


void *sys_shmat(int shmid, const void *shmaddr, int shmflg)
{
		unsigned long data_base, brk;
		if (shmid < 0 || SHM_SIZE <= shmid || shm_list[shmid].page == 0 || shm_list[shmid].key <= 0)
				return (void *)-EINVAL;

		data_base = get_base(current->ldt[2]);
		printk("current data_base = %p, new page = %p\n", data_base, shm_list[shmid].page);
		brk = current->brk + data_base;

		current->brk += PAGE_SIZE;
		if (put_page(shm_list[shmid].page, brk) == 0)
				return (void *)-ENOMEM;

		return (void *)(current->brk - PAGE_SIZE);
}
int sys_shmdt(void *shmaddr)
{
		return 0;
}
