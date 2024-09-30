// p.c
// Prints numbers from 0 to MAX - 1 with a delay

#include <stdio.h>
#include <unistd.h>

#define MAX 5
#define DELAY 1

int main() {

  // Disable buffering on stdout
  setvbuf(stdout, NULL, _IONBF, 0);

  int i;
  for (i = 0; i < MAX; i++) {
    printf("%d\n", i);
    sleep(DELAY);
  }

  return 0;
}
