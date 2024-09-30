# Depuración del kernel

## Introducción
En esta guía presentamos los pasos necesarios para poder utilizar gdb con el kernel que están desarrollando.
Habitualmente iniciamos gdb pasándole como parámetro el programa a depurar o bien iniciamos el programa y luego conectamos el debugger:
```
gdb mi_programa
```
o bien:
```
./mi_programa
# ps nos indica que mi_programa tiene pid 15
gdb -p 15
```
El sistema operativo en el cual corre tanto mi_programa como gdb provee todos los servicios necesarios para que un proceso (gdb) pueda depurar otro programa (mi_prorama). Sin embargo, en el caso del kernel no tenemos un sistema operativo en el cual esté corriendo su propio kernel. Es decir, su kernel no es un proceso de otro sistema operativo que podamos ejecutar de esta manera:
```
gdb mi_kernel
```
Entonces, para poder depurar el kernel necesitamos que qemu nos permita hacerlo desde afuera.

## QEMU
Es necesario indicarle a qemu que en cuanto inicie, congele el CPU. Esto nos dará tiempo para conectar gdb y, por ejemplo, establecer los breakpoints de nuestro interés antes de que se ejecuten:
```
-S  Do not start CPU at startup (you must type 'c' in the monitor).
```
También es necesario indicarle a qemu que espere una conexión de gdb:
```
-s  Shorthand for -gdb tcp::1234, i.e. open a gdbserver on TCP port 1234.
```

## GDB

### Información de depuración
Para que gdb nos muestre línea por línea de nuestro código C mientras ejecuta necesita la información de depuración, de lo contrario solo puede mostrar el assembler. Para generar esta información durante la compilación necesitamos la siguiente opción de gcc:
```
-g  Produce debugging information in the operating system's native format (stabs, COFF, XCOFF, or DWARF 2). GDB can work with this debugging information.
```
Sin embargo, por más que compilen el kernel con esta opción, toda la información de depuración se pierde durante la etapa de linkeo ya que el formato de salida que le especificamos al linker en los scripts de linkeo (archivos .ld) tanto para el kernel como para sampleCodeModule es binary:
```
OUTPUT_FORMAT("binary")
```
Una solución simple es linkear 2 veces, una con formato binary y otra con formato elf64-x86-64 que preserva la información de depuración. Así tendrán 2 binarios para kernel y 2 binarios para sampleCodeModule:
```
kernel.bin
kernel.elf
sampleCodeModule.bin
sampleCodeModule.elf
```
Los binarios con extensión .bin serán utilizados para crear la imagen para qemu como es habitual. Los binarios con extensión .elf serán utilizados únicamente para proveerle la información de depuración a gdb, es decir, no serán ejecutados.
Otra opción es linkear una única vez con formato elf64-x86-64 y luego extraer el binario con objcopy:
```
# Linkeo
kernel.elf
sampleCodeModule.elf
# Extracción del binario
objcopy -O binary kernel.elf kernel.bin
objcopy -O binary sampleCodeModule.elf sampleCodeModule.bin
```
Una vez que tienen los binarios con información de depuración de kernel y userland, es necesario indicarle a gdb que cargue los símbolos desde estos archivos con la opción add-symbol-file. Esta opción necesita la dirección de memoria donde será cargado en cada módulo:
```
(gdb) add-symbol-file Kernel/kernel.elf 0x100000
(gdb) add-symbol-file Userland/sampleCodeModule.elf 0x400000
```
Las direcciones 0x100000 y 0x400000 son las mismas que especificamos en los scripts de linkeo tanto para kernel como para sampleCodeModule.

### Conexión con qemu
Qemu se quedó a la espera de una conexión TCP (opción -s), gdb necesita la dirección IP y el puerto donde esta escuchando qemu para conectarse con la opción target remote IP:PORT.
```
(gdb) help target remote
Use a remote computer via a serial line, using a gdb-specific protocol.
Specify the serial device it is connected to
(e.g. /dev/ttyS0, /dev/ttya, COM1, etc.).
```
A partir de este punto suponemos que qemu será ejecutado en el host y gdb en docker (el contenedor provisto ya tienen gdb instalado). Dado que la IP del host puede variar, es conveniente ejecutar un contenedor con la IP del host mapeada a un nombre específico:
```
--add-host list	Add a custom host-to-IP mapping (host:ip)
```
Para esto es necesario agregar este parámetro al comando run:
```
docker run --add-host=host.docker.internal:host-gateway
```
Con este parámetro, la IP del host se puede referenciar desde el contenedor como host.docker.internal. El puerto que abre qemu al utilizar la opción -s es 1234. El siguiente comando le indica a gdb que se conecte a la instancia de gdbserver iniciada por qemu: 
```
(gdb) target remote host.docker.internal:1234
```

## Extras

Ya vimos cómo indicarle a qemu que nos permita conectar un depurador, cómo generar la información de depuración y utilizarla con gdb. También vimos cómo conectar gdb a qemu (desde docker al host). Con esto es suficiente para depurar el kernel, sin embargo podemos realizar unos pasos extra para simplificar la tarea de depuración.

### run.sh
Sería interesante que el script de ejecución del kernel (run.sh) permita especificar si queremos depurar el kernel o simplemente ejecutarlo:
```
#!/bin/bash
if [[ "$1" = "gdb" ]]; then
  qemu-system-x86_64 -s -S -hda Image/x64BareBonesImage.qcow2 -m 512
else
  qemu-system-x86_64 -hda Image/x64BareBonesImage.qcow2 -m 512
fi
```

### .gdbinit
gdb puede leer comandos directamente de un archivo además de stdin:
```
source -- Read commands from a file named FILE
```
Además existe un archivo que gdb, por defecto, leerá cada vez que inicie:
```
~/.gdbinit
```
Este comportamiento es similar al de un intérprete de comandos como bash, que posee el comando builtin source y lee comandos del archivo `~/.bashrc` cada vez que inicia.
En este archivo podemos especificar la conexión a qemu y la carga de símbolos, ya que cada vez que iniciamos gdb necesitamos realizar estas operaciones;
```
~/.gdbinit
target remote host.docker.internal:1234
add-symbol-file Kernel/kernel.elf 0x100000
add-symbol-file Userland/sampleCodeModule.elf 0x400000
```

### Python
gdb provee una API para python que permite controlarlo desde este lenguaje. Existen diversas implementaciones disponibles altamente configurables. En esta guía les presentamos una en particular y les indicamos cómo utilizarla. Del siguiente repositorio pueden descargar el script python que deberán copiar a su archivo .gdbinit:

- [https://github.com/cyrus-and/gdb-dashboard](https://github.com/cyrus-and/gdb-dashboard)

Cada vez que inicie gdb, cargará este script. gdb-dashboard permite visualizar múltiples layouts de gdb (asm, source, stack, memory, etc) simultáneamente y permite cambiar los layouts durante la sesión de depuración. Por ejemplo, para depurar código C podemos especificar la siguiente configuración que dedica más espacio al código C y solo muestra los layouts relevantes:
```
define src-prof
    dashboard -layout source expressions stack variables
    dashboard source -style height 20
end
```
Para depurar assembler podemos utilizar la siguiente configuración:
```
define asm-prof
    dashboard -layout registers assembly memory stack
    dashboard registers -style list 'rax rbx rcx rdx rsi rdi rbp rsp r8 r9 r10 r11 r12 r13 r14 r15 rip eflags cs ss ds es fs gs fs_base gs_base k_gs_base cr0 cr2 cr3 cr4 cr8 efer'
end
```
Ambas configuraciones se pueden establecer directamente utilizando el comando dashboard, o bien se pueden definir funciones como en el ejemplo anterior para intercambiar entre ellas rápidamente. Estas funciones deberán agregarlas al archivo .gdbinit.

### Linkeo con múltiples formatos
En esta sección presentamos un ejemplo de cómo quedaría el Makefile para linkear en un formato diferente que el especificado en los scripts de linkeo.
Si bien en la etapa de linkeo se especifica el script con la opción -T, es posible ignorar un comando del script, haciéndolo explícito en los parámetros del linker. Por ejemplo OUTPUT_FORMAT("binary") se puede ignorar con --oformat TARGET:
```
-T FILE, --script FILE	Read linker script
--oformat TARGET		Specify target of output file
```
Si linkean de esta manera:
```
$(LD) ... -T kernel.ld -o $(KERNEL) ...
```
Se puede agregar un segundo linkeo para generar el formato elf:
```
$(LD) ... -T kernel.ld --oformat=elf64-x86-64 -o kernel.elf ...
```
Si compilan y linkean en un solo paso (gcc por defecto compila y linkea) es posible indicarle a gcc que le envíe instrucciones directamente al linker con el siguiente flag:
```
-Wl,<options>	Pass comma-separated <options> on to the linker.
```
Así, si compilan y linkean de esta manera:
```
$(GCC) ... -T sampleCodeModule.ld ... -o ../$(MODULE)
```
Se puede agregar un segundo linkeo para generar el formato elf:
```
$(GCC) ... -T sampleCodeModule.ld -Wl,--oformat=elf64-x86-64 ... -o ../sampleCodeModule.elf
```

### Monitoreo de interrupciones

qemu permite imprimir todas las interrupciones que ocurren con la siguiente opción:
```
-d item1[,...]	Enable logging of specified items. Use '-d help' for a list of log items.

-d help
int	show interrupts/exceptions in short format

qemu-system-x86_6 ... -d int
```
Esta opción imprime muchísima información (en stderr) por cada interrupción que ocurre:
```
    63: v=20 e=0000 i=0 cpl=0 IP=0008:0000000000105d26 pc=0000000000105d26 SP=0000:000000000014d6b0 env->regs[R_EAX]=0000000000000000
RAX=0000000000000000 RBX=0000000000000002 RCX=0000000000000000 RDX=0000000000000000
RSI=0000000000000000 RDI=0000000000000000 RBP=000000000014d6b8 RSP=000000000014d6b0
R8 =0000000000000006 R9 =0000000000000007 R10=0000000000000008 R11=0000000000000009
R12=000000000000000a R13=000000000000000b R14=000000000000000c R15=000000000000000d
RIP=0000000000105d26 RFL=00000202 [-------] CPL=0 II=0 A20=1 SMM=0 HLT=0
ES =0000 0000000000000000 00000000 00000000
CS =0008 0000000000000000 00000000 00209800 DPL=0 CS64 [---]
SS =0000 0000000000000000 ffffffff 00c09300 DPL=0 DS   [-WA]
DS =0000 0000000000000000 00000000 00000000
FS =0000 0000000000000000 00000000 00000000
GS =0000 0000000000000000 00000000 00000000
LDT=0000 0000000000000000 0000ffff 00008200 DPL=0 LDT
TR =0000 0000000000000000 0000ffff 00008b00 DPL=0 TSS64-busy
GDT=     0000000000001000 00000017
IDT=     0000000000000000 00000fff
CR0=80000013 CR2=0000000000000000 CR3=0000000000002008 CR4=00000630
DR0=0000000000000000 DR1=0000000000000000 DR2=0000000000000000 DR3=0000000000000000
DR6=00000000ffff0ff0 DR7=0000000000000400
CCS=0000000000000000 CCD=00000000001453d0 CCO=EFLAGS
EFER=0000000000000501
```
En la primera línea podemos observar qué interrupción ocurrió en hexadecimal (v=20) y los registros IP y SP con sus respectivos segmentos IP=8:105d26 SP=0:14d6b0. Si solo estamos interesados en esta información, podemos filtrar la salida con grep:
```
qemu-system-x86_6 ... -d int 2>&1 | grep "v="
```

### Control de interrupciones durante la depuración
Durante la depuración, qemu mantiene las interrupciones deshabilitadas. Esto no está relacionado con el bit IF del registro RFLAGS, sino que el timer simplemente no generará interrupciones mientras estemos depurando el código. Esto es así ya que al depurar paso a paso esperamos ejecutar la instrucción actual y no que nos interrumpa un timer y nos lleve a la rutina de servicio correspondiente.
Debido a que hay circunstancias excepcionales en las que se desea cambiar este comportamiento por defecto, existen 3 comandos con los que se puede consultar y establecer el comportamiento de qemu durante la depuración:
```
# Muestra la máscara utilizada para controlar este comportamiento
(gdb) maintenance packet qqemu.sstepbits
sending: "qqemu.sstepbits"
received: "ENABLE=1,NOIRQ=2,NOTIMER=4"

# Consulta la máscara actual
(gdb) maintenance packet qqemu.sstep
sending: "qqemu.sstep"
received: "0x7"

# Establece una nueva máscara, habilitando únicamente los timers
(gdb) maintenance packet Qqemu.sstep=0x3
sending: "qemu.sstep=0x3"
received: "OK"
```
Al habilitar las interrupciones de los timers notaremos que mientras depuramos paso a paso una función arbitraria, eventualemente saltaremosa la rutina de servicio del timer de manera inesperada.

### Dispositivo de video para docker
Habitualmente ejecutamos qemu (run.sh) directamente en el host con la imagen compilada en el guest (docker). Sin embargo, si intentamos ejecutar qemu desde docker obtendremos el siguiente error:
```
Could not initialize SDL(No available video device) - exiting
```
Esto se debe a que no existe un dispositivo de video disponible para que qemu ejecute el kernel. En caso de ser necesario ejecutar qemu en docker, es posible iniciar el contenedor con un dispositivo de video utilizando XQuartz:

1. En el host, Instalar XQuartz
2. En preferencias -> seguridad activar la opción para permitir conexiones por la red
3. Iniciar el contenedor: `docker run --env="DISPLAY=host.docker.internal:0" ...`
4. En el host ejecutar (este comando no retorna) `socat TCP-LISTEN:6000,reuseaddr,fork UNIX-CLIENT:\"$DISPLAY\"`
5. Ejecutar `./run.sh` dentro del contenedor.

### Error packet too long
(Buscar referencias en el foro) Se pueden copiar estos comandos directamente en la ventana de GDB:
```
gdb
  b main 
  c
  disconnect
  set arch i386:x86-64
  target remote localhost:1234
```
## Demostración
En el siguiente link encontrarán un video con una demostración ilustrando los contenidos presentados en esta guía:

-[DEMOSTRACIÓN GDB](https://drive.google.com/file/d/1IPtHNc34D96nuE1bXpwTVGD2PdRZCEN-/view?usp=sharing)