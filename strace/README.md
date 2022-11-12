# strace
**strace** (**s**yscall**trace**) es una herramienta de tracing de syscalls y señales que nos permite ver qué syscalls (con sus parámetros) ejecuta un programa cualquiera, como así también qué señales recibe. Sugerimos strace(1).

## Objetivo
Esta guía presenta los ejercicios realizados en clase con strace con una breve explicación y los resultados esperados. Usamos como casos de estudio la reflexión final del la práctica de POSIX.

El objetivo de este ejercicio es entender cómo la shell resuelve cada uno de estos comandos en término de syscalls vistas en clase, como fork (clone), execve, pipe, open, close, read, write, dup, dup2, wait, entre otras. Para ello nos enfocaremos en estas syscalls únicamente.

## Requerimientos
* Necesitamos tener 2 terminales abiertas en el mismo contenedor docker
* Con **ps aux** podemos descubrir el PID de bash.
* En la segunda terminal ejecutamos **strace -ff -o out -p \<bash pid\>**. Esto guarda las syscalls de bash y de todos los procesos hijos en archivos separados (**-ff -o out**).

En este punto bash está esperando por un comando y strace muestra algo como esto:

```
Process 1 attached
```

## Manos a la obra
En la terminal de bash podemos empezar a ejecutar los comandos de la guía de POSIX. No es necesario ejecutar los programas **p** y **c**, puede ser cualquier programa, por ejemplo **ls** y **wc**.

Al ejecutar **ls | wc** esperamos ver las syscalls de los 3 procesos (**bash**, **ls** y **wc**) separadas en los archivos **out.PID**. El output es muy extenso. Se puede configurar qué ver en la salida de strace -> strace(1).

En **out.\<PID de bash\>** esperamos ver la syscall **pipe** para conectar ambos procesos, inmediatamente después la syscall **clone** 2 veces (una por cada proceso) y finalmente la syscall **wait** 2 veces ya que bash espera a que ambos procesos hijos terminen (los `...` representan syscalls ignoradas en este ejercicio):

```
pipe([3, 4])                            = 0
...
clone(child_stack=0, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f6699d349d0) = 6307s
...
clone(child_stack=0, flags=CLONE_CHILD_CLEARTID|CLONE_CHILD_SETTID|SIGCHLD, child_tidptr=0x7f6699d349d0) = 6308
...
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], WSTOPPED|WCONTINUED, NULL) = 6307
wait4(-1, [{WIFEXITED(s) && WEXITSTATUS(s) == 0}], WSTOPPED|WCONTINUED, NULL) = 6308
```

En **out.\<PID de ls\>** esperamos ver las syscall **dup2** y **close** para conectar el proceso **ls** al pipe y finalmente **execve** para ejecutar **ls**:

```
...
close(3)                                = 0
dup2(4, 1)                              = 1
close(4)                                = 0
...
execve("/bin/ls", ["ls", "--color=auto"], [/* 10 vars */]) = 0
```

Finalmente en **out.\<PID de wc\>** esperamos ver una salida similar a la de **ls**, solo que en este caso **wc** se conecta al otro extremo del pipe

El resto de los ejemplos se puede reproducir de la misma manera, enfocándose siempre en las syscalls relevantes.

## ltrace

**ltrace** (**l**ibrary**trace**) es una herramienta similar a **strace**, solo que permite ver qué funciones de librería ejecuta un programa (además de syscalls).

## Conclusión
strace es una herramienta excelente no solo para aprender cómo se comunican los programas con el sistema operativo, sino también como herramienta de debugging.
