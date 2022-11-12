/*
Ilustrar el uso de diferentes primitivas de sincronización

Instrucciones:
Compilar según corresponda:
make IMP=XCHG
make IMP=XADD
make IMP=XCHG_RACY
make IMP=XCHG_DEFENSA
make IMP=RACY

./a.out

Output indicando un problema:
s.value: 50608 (1)
global: 219659 (300000)

Output indicando que todo anduvo bien (OJO! No es prueba de que es correcto)
s.value: 1 (1)
global: 300000 (300000)
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sem.h"

int global = 0;

#define N_THREADS 10
#define INCS 30000

void *inc(void *p) {
  sem *s = (sem *)p;
  int i;
  for (i = 0; i < INCS; i++) {
    my_sem_wait(s);
    global++;
    my_sem_post(s);
  }

  pthread_exit(NULL);
  return NULL;
}

int main(void) {
  pthread_t id[N_THREADS];
  sem s;

  my_sem_init(&s, 1);

  int i;
  for (i = 0; i < N_THREADS; i++)
    pthread_create(&id[i], NULL, inc, &s);

  for (i = 0; i < N_THREADS; i++)
    pthread_join(id[i], NULL);

  printf("s.value: %d (1)\nglobal: %d (%d)\n", s.value, global, N_THREADS * INCS);

  return 0;
}

/*

sem chop_stick[N_THREADS];
int philos[N_THREADS];
sem lock_global;
sem lock_print;

void printTable(){
  int i,last;

  my_sem_wait(&lock_print);

  last = philos[N_THREADS - 1];
  for (i = 0; i<N_THREADS; i++){
    printf("%d ", philos[i]);
    if (last && last == philos[i])
      printf("ERROR\n");
    last = philos[i];
  }
  printf("\n");

  my_sem_post(&lock_print);
}

void* philo_defensa_propuesta(void* p){
  int id = *(int *) p;
  time_t t;

  srand((unsigned) time(&t));

  while (1){
    usleep(rand() % 20000); //think

    my_sem_wait(&lock_global);
    my_sem_wait(&chop_stick[id]);
    my_sem_wait(&chop_stick[(id + 1) % N_THREADS]);
    my_sem_post(&lock_global);

    //EAT
    philos[id] = 1;
    usleep(rand() % 20000);

    printTable();

    //THINK
    philos[id] = 0;

    //my_sem_wait(&lock_global);
    my_sem_post(&chop_stick[(id + 1) % N_THREADS]);
    my_sem_post(&chop_stick[id]);
    //my_sem_post(&lock_global);
  }

  pthread_exit(NULL);
  return NULL;
}

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))
#define MAX(X, Y) (((X) > (Y)) ? (X) : (Y))

void* philo_defensa(void* p){
  int id = *(int *) p;
  time_t t;

  srand((unsigned) time(&t));

  while (1){
    usleep(rand() % 20000); //think

    my_sem_wait(&chop_stick[MIN(id,(id+1)%N_THREADS)]);
    if(id == 2) usleep(rand() % 100000);
    my_sem_wait(&chop_stick[MAX(id,(id+1)%N_THREADS)]);
    //my_sem_wait(&chop_stick[id]);
    //if(id == 2) usleep(rand() % 100000);
    //my_sem_wait(&chop_stick[(id + 1) % N_THREADS]);

    //EAT
    philos[id] = 1;
    usleep(rand() % 20000);

    printTable();

    //THINK
    philos[id] = 0;

    my_sem_post(&chop_stick[MAX(id,(id+1)%N_THREADS)]);
    if(id == 2) usleep(rand() % 100000);
    my_sem_post(&chop_stick[MIN(id,(id+1)%N_THREADS)]);
   // my_sem_post(&chop_stick[(id + 1) % N_THREADS]);
   // if(id == 2) usleep(rand() % 100000);
   // my_sem_post(&chop_stick[id]);
  }

  pthread_exit(NULL);
  return NULL;
}

void* philo(void* p){
  int id = *(int *) p;
  time_t t;

  srand((unsigned) time(&t));

  while (1){
    usleep(rand() % 20000); //think

    if (id % 2 == 0){
      my_sem_wait(&chop_stick[id]);
      my_sem_wait(&chop_stick[(id + 1) % N_THREADS]);
    }else{
      my_sem_wait(&chop_stick[(id + 1) % N_THREADS]);
      my_sem_wait(&chop_stick[id]);
    }

    //EAT
    philos[id] = 1;
    usleep(rand() % 20000);

    printTable();

    //THINK
    philos[id] = 0;

    if (id % 2 != 0){
      my_sem_post(&chop_stick[id]);
      my_sem_post(&chop_stick[(id + 1) % N_THREADS]);
    }else{
      my_sem_post(&chop_stick[(id + 1) % N_THREADS]);
      my_sem_post(&chop_stick[id]);
    }
  }

  pthread_exit(NULL);
  return NULL;
}

int main(void){
  pthread_t id[N_THREADS];
  int ids[N_THREADS];

  my_sem_init(&lock_print, 1);
  my_sem_init(&lock_global, 1);

  int i;
  for (i = 0; i < N_THREADS; i++){
    my_sem_init(&chop_stick[i], 1);
    ids[i] = i;
    philos[i] = 0;
  }

  for (i = 0; i < N_THREADS; i++)
    pthread_create(&id[i], NULL, philo_defensa, &ids[i]);

  for (i = 0; i < N_THREADS; i++)
    pthread_join(id[i], NULL);

  return 0;
}

#define PIPESIZE 500

typedef struct Pipe {
  int lock;
  char data[PIPESIZE];
  unsigned int nread;     // number of bytes read
  unsigned int nwrite;    // number of bytes written
} pipe_t;

int pipeget(pipe_t *p){
  acquire(&p->lock);
  int r = p->nwrite - p->nread;
  release(&p->lock);
  return r;
}

int pipewrite(pipe_t *p, char *addr, int n){
  int i;

  for(i = 0; i < n; i++){
    while (1){ //wait until there is space to write
      acquire(&p->lock);
      if (p->nwrite < p->nread + PIPESIZE)
        break;
      release(&p->lock);
    }
    p->data[p->nwrite++ % PIPESIZE] = addr[i];
    release(&p->lock);
  }

  return n;
}

int piperead(pipe_t *p, char *addr, int n){
  int i;

  for(i = 0; i < n; i++){
    while(1){ //wait until there is something to read
      acquire(&p->lock);
      if (p->nread < p->nwrite)
        break;
      release(&p->lock);
    }
    addr[i] = p->data[p->nread++ % PIPESIZE];
    release(&p->lock);
  }

  return i;
}

#define S "Hola Mundo"

void* w(void* param){
  pipe_t *p = (pipe_t *)param;

  while(1){
    pipewrite(p, S, strlen(S));
    usleep(90000);
  }
}

void* r(void* param){
  pipe_t *p = (pipe_t *)param;
  int len = strlen(S);
  char buf[len + 1];
  int count = 0;

  buf[len] = 0;

  while(1){
    piperead(p, buf, len);
    printf("%s %d\n", buf, ++count);
    usleep(100000);
  }
}

int main_pipe(void){
  pthread_t id[2];

  pipe_t p;
  p.lock = 0;
  p.nread = 0;
  p.nwrite = 0;

  pthread_create(&id[0], NULL, w, &p);
  pthread_create(&id[1], NULL, r, &p);

  int used;
  while(1){
    used = pipeget(&p);
    printf("%d\t\t", used);
    for (used = used / 10; used > 0; used--)
      printf("-");

    printf("\n");
    usleep(500000);
  }

  int i;
  for (i = 0; i < 2; i++)
    pthread_join(id[i], NULL);

  return 0;
}
*/
