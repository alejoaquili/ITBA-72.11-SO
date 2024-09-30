#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdio.h>

void type_prompt(){
  // Deshabilitamos el buffering en stdout para que type_prompt parezca real
  setvbuf(stdout, NULL, _IONBF, 0);
  
  char *prompt = "$ cat\n";

  while (*prompt){
    printf("%c", *prompt);
    sleep(1);

    prompt++;
  }
}

// El siguiente programa ilustra un uso muy básico de fork execve y waitpid.
// Las pausas están pensadas para ejecutar ps auxf en otra terminal DURANTE
// esas pausas para ver los procesos

int main(){
  int status;
  pid_t ret;

  while (1){
    type_prompt();

    printf("Antes del fork. Espero ver solo main\n");
    sleep(10);

    ret = fork();

    // No seamos tan cavernícolas y agreguemos el chequeo de errores. perror muy útil para esto!
    if (-1 == ret){
      perror("fork");
      return -1;
    }

    if (0 != ret) {
      // parent
      printf("Despues del fork (padre). Espero ver 2 main\n");
      sleep(10);

      printf("En este punto seguro ya se ejecutó execve, ps debería mostrar main y cat\n");
      sleep(10);

      waitpid(-1, &status, 0);

      printf("Despues del waitpid. El hijo termino con el siguiente exit status: %d\n", status);
      sleep(10);

    } else {
      //child

      printf("Despues del fork (hijo)\n");
      sleep(7);

      char *const parmList[] = {"/bin/cat", NULL};
      ret = execve("/bin/cat", parmList, 0);

      if (-1 == ret){
        perror("execve");
        return -1;
      }

      //0xDEADCODE
      printf("Despues del execve\n");
      sleep(10);
    }

    // No vamos a iterar como lo hace una shell real
    return 0;
  }

}
