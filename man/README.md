# Guía de uso de man para 72.11 - Sistemas Operativos

La siguiente guía cubre aspectos básicos del uso de man. Para más información consultar:

```plaintext
man man
```

### SECCIONES

man está organizado por secciones, las que más usaremos durante la materia son las siguientes (por orden de relevancia):

```plaintext
2   System calls (functions provided by the kernel)
3   Library calls (functions within program libraries)
7   Miscellaneous
1   Executable programs or shell commands
```

### ORGANIZACIÓN DE UNA ENTRADA DEL MANUAL

En el encabezado se puede observar el nombre de la función y la sección. Es lo primero que debemos mirar para saber si estamos en la sección correcta

```plaintext
READ(2)			Linux Programmer's Manual			READ(2)
```

Por ejemplo:

```plaintext
man printf
```

Nos lleva a la sección 1, en lugar de la 3 que es la que es más probable que estemos buscando si estamos programando en C.

A continuación del encabezado tenemos una (muy) breve descripción de la función

```plaintext
NAME
       read - read from a file descriptor
```

Seguido a esto, los headers y prototipos de las funciones involucradas. En esta misma sub-sección (y cuando aplica) tenemos los `Feature Test Macro Requirements`. Ignorarlos suele traer problemas de linkeo.

```plaintext
SYNOPSIS
       #include <unistd.h>

       ssize_t read(int fd, void *buf, size_t count);
```

Luego de la descripción y el retorno de la función podemos encontrar la sub-sección `SEE ALSO` que nos refiere a entradas relacionadas del manual. Esto es muy valioso para conocer nuevas funciones que posiblemente nos sirvan para lo que estamos haciendo.

```plaintext
SEE ALSO
       close(2), fcntl(2), ioctl(2), lseek(2), open(2), pread(2), readdir(2), readlink(2), readv(2), select(2), write(2), fread(3)
```

En algunas entradas del manual se pueden encontrar ejemplos de código, o referencias a entradas relacionadas que los contengan. Esto es muy útil para comprender el uso de una API específica.

```plaintext
FORK(2)

...

EXAMPLE
       See pipe(2) and wait(2).
```

### BÚSQUEDA

Dentro de una entrada específica se puede buscar usando `/` y escribiendo el término a buscar seguido de la tecla enter, por ejemplo

```plaintext
/open
```

Resaltará todas las coincidencias y es posible moverse entre ellas usando `n` o `N` dependiendo se sedesea buscar en orde descendente o ascendente.

man soporta la búsqueda en la sub-sección `NAME` de todas las entradas del manual:

```plaintext
man -k process
```

Remplazar `-k` por `-K` hará que la búsqueda se realice en todo en manual en lugar de solo la sub-sección `NAME`. `-K` permite especificar una sección específica del manual, por ejemplo

```plaintext
man -K 2 process
```

Buscará `process` en toda la sección 2.
