# Guía de uso de CPPCHECK

CPPCHECK es una herramienta simple de análisis de código estático para los lenguajes de programación C y C ++.

## Instalación
La imagen de docker provista tiene una versión instalada de CPPCHECK. Para instalar en su PC PERSONAL, ejecutar según corresponda:

```
# Debian
    sudo apt-get install cppcheck
# Fedora
    sudo yum install cppcheck
# macOS
    brew install cppcheck
```

## Ejecución 
```
    cppcheck --quiet --enable=all --force --inconclusive .
```
Esto habilita todos los chequeos y analiza los archivos fuente ubicados en el directorio actual recursivamente.


