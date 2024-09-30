/*
Ilustrar la sobreescritura de variables en makefile combinada con definición de macros en gcc por linea de comandos.

Instrucciones:

Para compilar con free lists:

make ; ./a.out

Para compilar con buddy:

make MM=BUDDY; ./a.out
*/

#include <stdio.h>

int main() {

#ifdef MM_BUDDY
  printf("Usando buddy\n");
#else
  printf("Usando free list\n");
#endif

  return 0;
}
