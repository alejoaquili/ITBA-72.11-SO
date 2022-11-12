typedef struct Sem {
  int value;
} sem;

void my_sem_init(sem *s, int value);
void my_sem_wait(sem *s);
void my_sem_post(sem *s);
void acquire(int *lock);
void release(int *lock);
