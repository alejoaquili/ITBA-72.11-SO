#include "sem.h"

void my_sem_init(sem *s, int value){
  s->value = value;
}

extern int _xchg(int *lock, int value);

void acquire(int *lock){
  while(_xchg(lock, 1) != 0);
}

void release(int *lock){
  _xchg(lock, 0);
}

#ifdef IMP_XCHG
// posible solución 100% userland: contiene busy waiting y spinlock clasico

int lock = 0;

void my_sem_wait(sem *s){
  while(1){
    acquire(&lock);
    if (s->value > 0){
      s->value--;
      release(&lock);
      break;
    }
    release(&lock);
  }
}

void my_sem_post(sem *s){
  acquire(&lock);
  s->value++;
  release(&lock);
}
#endif

#ifdef IMP_XADD
// posible solución 100% userland: contiene busy waiting y usa lock xadd
extern int _xadd(int inc, int *value);

void my_sem_wait(sem *s){
  while(_xadd(-1, &(s->value)) <= 0)
    _xadd(1, &(s->value));
}

void my_sem_post(sem *s){
  _xadd(1, &(s->value));
}
#endif

#ifdef IMP_XCHG_RACY
void my_sem_wait(sem *s){
  while(_xchg(&(s->value), s->value - 1) <= 0)
    _xchg(&(s->value), s->value + 1);
}
    
void my_sem_post(sem *s){
  _xchg(&(s->value), s->value + 1);
}
#endif

#ifdef IMP_XCHG_DEFENSA
void my_sem_wait(sem *s){
  int aux = 0;
  while((aux = _xchg(&(s->value), aux)) <= 0);
  _xchg(&(s->value), aux - 1);
}

void my_sem_post(sem *s){
  _xchg(&(s->value), s->value + 1);
}
#endif

#ifdef IMP_RACY
// posible solución 100% userland: contiene condiciones de carrera
void my_sem_wait(sem *s){
  while(s->value <= 0);
  s->value--;
}

void my_sem_post(sem *s){
  s->value++;
}
#endif
