/*
Ilustrar la creación de un proceso zombie

Instrucciones:
make
./a.out &
ps aux
<esperar primer sleep>
ps aux
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
  pid_t child_pid = fork();

  if (child_pid == -1) {
    perror("fork");
    return 1;
  }

  sleep(10);
  printf("First sleep done\n");

  if (child_pid == 0)
    return 0;
  else
    sleep(10);

  printf("Second sleep done\n");

  return 0;
}
