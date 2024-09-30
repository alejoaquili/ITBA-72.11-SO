# Construcción del Kernel y mecanismos de administración de recursos del Sistema Operativo

## Objetivo
Esta carpeta engloba contenido de ayuda y soporte para el desarrollo del kernel en el segundo trabajo práctico especial de 72.11 Sistemas Operativos en el ITBA.

## Requerimientos
Se necesita tener un trabajo práctivo especial de la materia previa de *72.08 - Arquitectura de Computadoras*, el mismo se basa en el BareBones:

- [https://github.com/alejoaquili/x64BareBones](https://github.com/alejoaquili/x64BareBones)

El BareBones construido para ambas catedras, se basa en Pure64:

- [https://github.com/ReturnInfinity/Pure64](https://github.com/ReturnInfinity/Pure64)

## Algunas recomendaciones para el desarrollo
- Modularizar y segregar responsabilidades de administración de recursos dentro del Kernel
- Cuando sea posible probar abstracciones por fuera del Kernel. Por ejemplo si tienen componentes encapsulados y abstraidos (haciendo uso de TADs en C o similar) y estas abstracciones pueden correrse por fuera del Kernel, pueden utilizar herramientas de análisis como las que ya conocen, como Valgrind para evaluar memory leaks en su Memory Manager o en su estructura generica de colecciones o estructuras de datos como colas, stack, arboles o listas.
- Adicional a la modularización se recomienda utilizar test unitarios de las distintas abstracciones o módulos.
- Se recomienda utilizar carpetas para organizar los modulos dentro del Kernel.

## Unit testing
Para realizar unit tests en C de los módulos o abstracciones que construyan en su Kernel, se pueden basar en el siguiente ejemplo utilizado en la clase de unit testing.

- [https://github.com/alejoaquili/c-unit-testing-example](https://github.com/alejoaquili/c-unit-testing-example)

## Memory Management
Para la construcción del memory manager pueden usar de base la interfaz propuesta en TAD de Memory Manager que se encuentra en el repositorio anterior de unit testing en C. Más ejemplos:
- [https://wiki.osdev.org/Page_Frame_Allocation](https://wiki.osdev.org/Page_Frame_Allocation)
- [https://freertos.org/a00111.html](https://freertos.org/a00111.html)
- [https://github.com/jubalh/awesome-os](https://github.com/jubalh/awesome-os)

## Building del Kernel
En esta carpeta encontraran una guía que explica en mayor detalle como es el building del Kernel.

## Debugging del Kernel
En esta carpeta encontraran una guía que explica en mayor detalle como utilizar el debugger GDB en el Kernel.
