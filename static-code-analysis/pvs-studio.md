# Guía de uso de PVS-Studio

PVS-Studio es una herramienta de análisis estático, es decir, no requiere ejecutar el código para hacer el análisis. Durante este análisis es capaz de detectar múltiples errores y de advertir sobre código sospechoso.

## Requisitos
Para poder utilizar la versión gratuita deben agregar 2 comentarios en el encabezado de cada archivo .c del proyecto. Una forma de hacer esto es ejecutar lo siguiente:

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

## Depuración
PVS-Studio provee documentación muy clara y con ejemplo correctos e incorrectos para cda uno de los resultados que reporta. Por ejemplo, para el error V1003 se puede visitar https://pvs-studio.com/en/docs/warnings/v1003/

