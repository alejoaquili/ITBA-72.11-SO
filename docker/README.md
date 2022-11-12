# Guía de uso de Docker para 72.11 - Sistemas Operativos

La siguiente guía cubre los pasos necesarios para tener un entorno de trabajo docker tanto en una PC personal como en una PC de los laboratorios de ITBA.

## Instalación de Docker
### PC PERSONAL
#### Ejemplo para Linux Mint
```
  sudo apt-get install docker.io
```
Si al intentar ejecutar docker, por ejemplo "docker ps" se obtiene este error

```
Got permission denied while trying to connect to the docker daemon socket
```
Se deben seguir los pasos de el siguiente [enlace](https://docs.docker.com/install/linux/linux-postinstall/).
##### TL;DR
Ejecutar:
```
  sudo groupadd docker
  sudo usermod -aG docker $USER
```
Reiniciar sesión para que los cambios tengan efecto

#### Ejemplo para macOS
Pueden ingresar en este [enlace](https://docs.docker.com/desktop/mac/install/) y descargar el archivo `DMG` eligiendo versión para dispositivos con Chip Intel o con Chip Apple según corresponda. O bien se puede instalar a través del [Homebrew](https://brew.sh/): 
```
  brew install docker
```
#### Ejemplo para Windows con WSL
Pueden seguir las instrucciones en este [enlace](https://docs.docker.com/desktop/windows/wsl/)

### PC LABORATORIOS DEL ITBA
Ya está instalado, pero es necesario seguir los siguientes pasos para poder ejecutar docker
* Iniciar sesión en archlinux (en los laboratorios bootea por red, sino desde pampero vía ssh a alguna pc)
* Ejecutar los siguientes comandos:
```
  sudo /usr/bin/start_docker.sh
  sudo /usr/bin/enter_docker.sh
```
Esto debería mostrar un prompt como el siguiente:
```
root@d1:~#
```

## Descargar imagen

```
  docker pull agodio/itba-so:1.0
```
Y pueden revisar que la imagen aparezca utilizando:

```
  docker images
```
## Ejecutar el contenedor

```
  docker run -v "${PWD}:/root" --privileged -ti agodio/itba-so:1.0 
```
Esto debería mostrar un prompt como el siguiente:
```
root@c3285f863835:/#
```
El flag `--privileged` quita la restricción a syscalls utilizadas por `gdb`, `strace`, `ltrace`, `PVS-studio`, etc. En versiones anteriores de docker esto lo lográbamos con el flag `--security-opt seccomp:unconfined `.
El flag `-v "${PWD}:/root"` hace un `bind-mount` de la carpeta actual ($PWD) del host en la carpeta /root del guest, esto permite compartir archivos entre el host y el guest.
**¡CUIDADO!** Los archivos **NO SE COPIAN**, sino que se comparten, es decir que cualquier cambio tanto desde el host como desde el guest se podrá ver en el host y el guest.

### PC PERSONAL

Una alternativa práctica y segura es cambiar el directorio de trabajo del host al proyecto en el cual se esté trabajando, por ejemplo "cd /home/TP1", y luego ejecutar el contenedor, de esta manera en la carpeta /root del contenedor se verán los archivos del TP1

### PC LABORATORIOS DEL ITBA

En los laboratorios se inicia un contenedor con `enter_docker.sh` y dentro de este ejecutamos otro contenedor con `docker run`.
El primer contenedor realiza un `bind-mount` de la carpeta `/lab/tmp` -> `/shared`, por este motivo para poder compartir archivos entre el host y el guest, antes de ejecutar `docker run` es necesario cambiar el directorio de trabajo a `/shared` y luego ejecutar `docker run`.

Si al intentar ejecutar `docker run` se obtiene el siguiente error
```
docker: Error response from daemon: failed to start shim: exec: "docker-containerd-shim": executable file not found in $PATH: unknown.
```
Se puede resolver reiniciando el servicio de docker
```
    service docker restart
```

## Guía práctica
Crear un programa en C en el HOST, iniciar docker haciendo el bind-mount correspondiente y compilarlo y ejecutarlo dentro del contenedor.

### PC PERSONAL
```
    mkdir test-docker
    cd test-docker
    echo '#include <stdio.h>\nint main(){ printf("Challenge completed\\n"); return 0;}' > main.c
    docker run -v "${PWD}:/root" --privileged -ti agodio/itba-so:1.0
    cd /root
    gcc -Wall main.c
    ./a.out
```
### PC LABORATORIOS DEL ITBA

```
    cd /lab/tmp
    echo '#include <stdio.h>\nint main(){ printf("Challenge completed\\n"); return 0;}' > main.c
    sudo /usr/bin/start_docker.sh
    sudo /usr/bin/enter_docker.sh
    cd /shared
    docker run -v "${PWD}:/root" --privileged -ti agodio/itba-so:1.0
    cd root
    gcc -Wall main.c
    ./a.out
```

## Extras

### Agregar color al output de GCC:
```
    export GCC_COLORS='error=01;31:warning=01;35:note=01;36:caret=01;32:locus=01:quote=01'
```
### Agregar color al output de ls
```
    alias ls='ls --color'
```
Los comandos para los colores (y cualquier otra configuración que quieran agregar) se puede agregar en un archivo `.bashrc` en la carpeta que van a montar en el docker, de esta manera cuando inicie bash en el docker va a levantar esta configuración automáticamente. Pueden por ejemplo incluir también el `cd /root` si así lo desean.

### Abrir otra terminal en un contenedor que ya esté corriendo:
Obtener el id del contenedor que esté corriendo con el siguiente comando (Este comando NO debe ejecutarse dentro del contenedor, sino en el host)
```
    docker ps
```
Obteniendo un output como el siguiente:
```
    CONTAINER ID        IMAGE                COMMAND             CREATED             STATUS              PORTS               NAMES
    a3b852a22b2c        agodio/itba-so:1.0   "bash"              17 seconds ago      Up 16 seconds                           goofy_hamilton
```
El id del contenedor es `a3b852a22b2c`, para iniciar una terminal en el mismo contenedor ejecutar el siguiente comando:
```
    docker exec -ti a3b852a22b2c bash
```
#### Armar alias "dexec"
Pueden armar un alias llamado `dexec` o como sea de su preferencia para:
```
  docker exec -ti $(docker ps | tail -n1 | cut -d ' ' -f 1) bash
```
