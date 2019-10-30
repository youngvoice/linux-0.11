/*
 * create a sem or open a sem that already exist.
 * @sem_t is sem type, defined by you self.
 * @return if success return unique identifier, else return NULL.
 * @name different process can share the same sem by same name. If the sem doesn't exist, then create sem named name. If the sem exists, then open the sem.
 * @value is the initial value of the sem. Only when creare new sem, it have meaning. Otherwise it is ignored. 
 * */
sem_t *sem_open(const char *name, unsigned int value);

/*
 * @return if success return 0, else return -1.
 * P operate, if the condition is not meet, then the process is wait on sem.
 * */
int sem_wait(sem_t *sem);
/*
 * @return if success return 0, else return -1.
 * V operate, if there are wait process, then one of the processes is waked up.
 * */
int sem_post(sem_t *sem);
/*
 * @return if success return 0, else return -1.
 * delete the sem named name.
 * */
int sem_unlink(const char *name);
