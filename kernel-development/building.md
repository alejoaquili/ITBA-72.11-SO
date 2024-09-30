# Building del kernel

En esta guía vamos a explorar en detalle el proceso de compilación del kernel, los archivos que se generan y cómo se cargan en memoria. Comprender este proceso es fundamental ya que nos permite:

* Saber qué estamos haciendo y qué está pasando.
* Entender errores de compilación, de linkeo, de ejecución, etc.
* Conocer qué memoria está en uso y qué memoria está libre. Esto permite implementar el administrador de memoria.
* Integrar un debugger.

Para esta tarea resulta natural empezar por el makefile principal (los archivos utilizados en esta guía pueden no coincidir con los que utilicen)

## Makefile principal

**Makefile:**
```
all:  bootloader kernel userland image

bootloader:
	cd Bootloader; make all

kernel:
	cd Kernel; make all

userland:
	cd Userland; make all

image: kernel bootloader userland
	cd Image; make all
```
Las reglas bootloader, kernel y userland son compilaciones independientes y producen los siguientes binarios:

**Bootloader:**
```
Bootloader/Pure64/bmfs_mbr.sys
Bootloader/Pure64/pure64.sys
```

**Kernel:**
```
Kernel/kernel.bin
```

**Userland:**
```
Userland/0000-sampleCodeModule.bin
Userland/0001-sampleDataModule.bin
```

Finalmente, la regla image combina todos estos binarios en un único archivo el cual será utilizado por qemu.

**Image:**
```
Image/packedKernel.bin <- kernel.bin y sample*Module.bin
Image/x64BareBonesImage.img <- bmfs_mbr.sys pure64.sys y packedKernel.bin
Image/x64BareBonesImage.qcow2 <- otro formato de .img
Image/x64BareBonesImage.vmdk <- otro formato de .img
```

## Makefile Kernel

**Makefile:**
```
$(KERNEL): $(LOADEROBJECT) $(OBJECTS) $(OBJECTS_ASM)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^

%.o: %.c
	$(GCC) $(GCCFLAGS) -I./include -c $< -o $@

%.o : %.asm
	$(ASM) $(ASMFLAGS) $< -o $@
```

Las últimas 2 reglas solo compilan los fuentes en C y Assembler (unidades de compilación) y la primera regla los linkea. Reemplazando las variables de la primera regla obtenemos
```
kernel.bin: loader.o (*.c -> *.o) (*.asm -> *.o)
	$(LD) $(LDFLAGS) -T kernel.ld -o $@ $^
```
En esta regla se genera el archivo kernel.bin como se mencionó anteriormente, el cual contiene el código y datos del kernel. Es importante notar que el primer objeto de la lista es loader.o y que el linker recibe como parámetro el archivo kernel.ld.

**loader.asm:**
```
global loader
extern main
extern initializeKernelBinary

loader:
	call initializeKernelBinary
	mov rsp, rax
	call main
hang:
	cli
	hlt
	jmp hang
```

**kernel.ld:**
```
OUTPUT_FORMAT("binary")
ENTRY(loader)
SECTIONS
{
	.text 0x100000 :
	{
		text = .;
		*(.text*)
		. = ALIGN(0x1000);
		rodata = .;
		*(.rodata*)
	}
	.data ALIGN(0x1000) : AT(ADDR(.data))
	{
		data = .;
		*(.data*)
		endOfKernelBinary = .;
	}
	.bss ALIGN(0x1000) : AT(ADDR(.bss))
	{
		bss = .;
		*(.bss*)
		*(EXCLUDE_FILE (*.o) COMMON)
	}
	. = ALIGN(0x1000);
	endOfKernel = .;
}
```

El archivo kernel.ld es un script de linkeo y nos permite especificar qué “pinta” va a tener el archivo de salida:

* OUTPUT_FORMAT es la versión para scripts de la opción --oformat=output-format. Permite especificar el formato de salida. En este caso el formato es binary, el cual no contiene nada más que el código y los datos.
* ENTRY es la versión para scripts de la opción -e. Permite especificar el entry point, es decir, la primera instrucción a ser ejecutada. En este caso es la función loader de loader.asm.
* SECTIONS nos permite especificar qué secciones, en qué orden y en qué dirección se cargará cada una en memoria para que el linker acomode todas las referencias.
* En la sección .text tendremos el código. En la sección .data las variables globales inicializadas y en .bss las variables globales no inicializadas.
* El orden en el que aparecen las secciones en SECTIONS es relevante y no solo podemos especificar en qué dirección comienza una sección (como .text en 0x10000) sino que podemos modificar la variable especial “.” que representa la posición actual. Por ejemplo, . = ALIGN(0x1000); nos permite redondear la posición actual a la siguiente dirección alineada a 0x1000, de esta manera la sección siguiente comenzará en una dirección alineada. También podemos declarar variables, por ejemplo, bss = .; cuyo valor es la posición actual. Esta variable en particular nos permite saber dónde empieza la sección .bss, mientras que la variable endOfKernel nos permite saber dónde termina el kernel. Estas variables serán utilizadas para reubicar los módulos Sample*Module.bin. La sección .text se debe cargar en la dirección 0x100000 ya que el bootloader saltará (jmp) a esta dirección una vez que finalice su ejecución. De la misma manera, SampleCodeModule deberá ser cargado en la dirección 0x400000 ya que nuestro kernel saltará (jmp) a esta dirección para ejecutar la shell.

## Makefile Userland

La regla Userland es muy similar, y también tiene un loader y un script de linkeo:

**_loader.c:**
```
#include <stdint.h>

extern char bss;
extern char endOfBinary;

int _start() {
	memset(&bss, 0, &endOfBinary - &bss);
	return main();
}
```

**sampleCodeModule.ld**
```
OUTPUT_FORMAT("binary")
ENTRY(_start)
SECTIONS
{
	.text 0x400000 :
	{
		*(.text*)
		. = ALIGN(0x1000);
		*(.rodata*)
	}
	.data ALIGN(0x1000) :
	{
		*(.data*)
	}
	.bss ALIGN(0x1000) :
	{
		bss = .;
		*(.bss*)
	}
	endOfBinary = .;
}
```

## Makefile Image

Este makefile crea la imagen con bmfs.bin (BareMetal File System) y luego con qemu-img convierte esta imagen en el formato qcow2 que será el utilizado por qemu.

### Module Packer

Uno de los requerimientos de la regla image es el archivo packedKernel.bin el cual se genera con ModulePacker (mp.bin). ModulePacker toma como parámetro los archivos kernel.bin y sample*Module.bin y genera un único archivo (packedKernel.bin) el cual consiste en la concatenación de los 3 archivos pasados como parámetro sumado a metainformación de los mismos. packedKernel.bin se puede detallar como sigue:

```
packedKernel.bin	| kernel.bin
			        | # módulos extra (2 en este caso)
			        | Tamaño en bytes del primer módulo extra
			        | SampleCodeModule.bin
			        | Tamaño en bytes del segundo módulo extra
			        | SampleDataModule.bin
```

### x64BareBonesImage.img

La imagen final tiene esta pinta:
```
x64BareBonesImage.img	| bmfs_mbr.sys
                        | pure64.sys
                        | packedKernel.bin	| kernel.bin
						                    | # módulos extra
                                            | Tamaño en bytes
                                            | SampleCodeModule.bin
                                            | Tamaño en bytes
                                            | SampleDataModule.bin
```

EL bootloader se encargará de ubicar packedKernel.bin (por consiguiente kernel.bin) en la dirección 0x100000, sin embargo, SampleCodeModule.bin no quedará exactamente en la dirección 0x400000, ya que kernel.bin ocupa menos de 0x300000 bytes. La responsabilidad de acomodar SampleCodeModule.bin en la dirección 0x400000 (en el script de linkeo especificamos que iba a estar en esta dirección) es del kernel, en particular la función initializeKernelBinary.

**kernel.c**
```
static void *const sampleCodeModuleAddress = (void*) 0x400000;
static void *const sampleDataModuleAddress = (void*) 0x500000;

void *initializeKernelBinary(){
  void *moduleAddresses[] = {
    sampleCodeModuleAddress,
    sampleDataModuleAddress
  };

  loadModules(&endOfKernelBinary, moduleAddresses);
  clearBSS(&bss, &endOfKernel - &bss);
  return getStackBase();
}
```
La función loadModules toma como parámetros la variable endOfKernelBinary (declarada en el script de linkeo kernel.ld) y las direcciones en las que hay que ubicar cada módulo. Si detallamos las secciones en kernel.bin podemos observar que endOfKernelBinary apunta a donde termina el binario del kernel y comienza # modules extra:

```
| packedKernel.bin	| kernel.bin			| .text
                                            | .rodata
                                            | .data
                                            | <- endOfKernelBinary
                    | # módulos extra
                    | Tamaño en bytes
                    | SampleCodeModule.bin
                    | Tamaño en bytes 				
                    | SampleDataModule.bin
```

Es importante destacar que la sección .bss no se almacena en el binario, ya que corresponde a variables no inicializadas, sin embargo, cada acceso del kernel a estas variables se realizará en las direcciones de memoria que sigan a .data, i.e donde actualmente se encuentra # módulos extra, Tamaño en bytes, etc.

```
| packedKernel.bin	| kernel.bin			| .text
                                            | .rodata
                                            | .data
                                            | <- endOfKernelBinary
                    | # módulos extra
                    | Tamaño en bytes
                    | ... (Espacio libre)
        0x400000 ->	| SampleCodeModule.bin
			        | ... (Espacio libre)				
        0x500000 ->	| SampleDataModule.bin
```
