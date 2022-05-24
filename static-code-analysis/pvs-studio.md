# Guía de uso de PVS-Studio

PVS-Studio es una herramienta de análisis estático, es decir, no requiere ejecutar el código para hacer el análisis. Durante este análisis es capaz de detectar múltiples errores y de advertir sobre código sospechoso.

## Instalación
La imagen de docker provista tiene una versión antigua y es necesario actualizarla. Para esto deben ejecutar lo siguiente dentro de docker:
```
    wget -q -O - https://files.pvs-studio.com/etc/pubkey.txt | apt-key add -
    wget -O /etc/apt/sources.list.d/viva64.list https://files.pvs-studio.com/etc/viva64.list
    apt-get install apt-transport-https
	apt-get update
	apt-get install pvs-studio
```
La nueva versión requiere que se ingrese una licencia:
```
    pvs-studio-analyzer credentials "PVS-Studio Free" "FREE-FREE-FREE-FREE"
```

## Requisitos
Una vez instalado, para poder utilizar la versión gratuita deben agregar 2 comentarios en el encabezado de cada archivo .c del proyecto. Una forma de hacer esto es ejecutar lo siguiente:

**¡¡¡CUIDADO!!!** -> esto agrega el encabezado a TODOS los archivos con extensión .c desde el directorio actual, recursivamente. Asegúrense de estar en el directorio correcto antes de ejecutarlo.
```
	find . -name "*.c" | while read line; do sed -i '1s/^\(.*\)$/\/\/ This is a personal academic project. Dear PVS-Studio, please check it.\n\1/' "$line"; done
	find . -name "*.c" | while read line; do sed -i '2s/^\(.*\)$/\/\/ PVS-Studio Static Code Analyzer for C, C++ and C#: http:\/\/www.viva64.com\n\1/' "$line"; done
```

Finalmente, pvs-studio requiere el comando de compilación para realizar el análisis. En este ejemplo el comando es make. También requiere permisos para ejecutar la syscall trace, la cual se habilita en docker con el flag `--privileged`.

## Ejecución 
```
	pvs-studio-analyzer trace -- make
	pvs-studio-analyzer analyze
	plog-converter -a '64:1,2,3;GA:1,2,3;OP:1,2,3' -t tasklist -o report.tasks PVS-Studio.log
```
Estos pasos generar un archivo llamado `report.tasks` el cual tiene el reporte con los errores / advertencias / notas.
