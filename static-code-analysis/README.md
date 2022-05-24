# Herramientas de análisis estático de código.
El análisis estático de código es un tipo de análisis de software que se realiza sin ejecutar el programa.

## Lenguaje de programación C
En 72.11 Sistemas Operativos vamos a programar en lenguaje de programación C y sugerimos utilizar las herramientas de análisis estático de código:
* **[PVS-Studio](https://pvs-studio.com/en/):** es un analizador de código estático patentado que protege la calidad del código, la seguridad y la protección del código.
* **[CPPCHECK](https://cppcheck.sourceforge.io/):** es una herramienta de análisis de código estático para los lenguajes de programación C y C ++.

Es importante utilizar estas herramientas **"a conciencia"**, esto implica 2 cosas:
* La ausencia de advertencia no significa que no haya problemas (falso negativo)
* La presencia de advertencias no necesariamente es un problema (falso positivo)

## Guías de herramientas recomendadas para lenguaje de programación C

En los archivos `pvs-studio.md` y `cppcheck.md` pueden encontrar guías para el uso de esta herramientas.

## Otros lenguajes de programación
Existen infinidades de herramientas de este estilo, para los diversos lenguajes de programación. En particular para lenguaje de programación Java también se puede utilizar `PVS-Studio` para detectar bugs y potenciales vulnerabilidades (lo mismo ocurre con C++ y C#). También pueden encontrar herramientas como [Checkstyle](https://checkstyle.sourceforge.io/), [PMD](https://github.com/pmd/pmd), [SpotBugs](https://spotbugs.github.io/), etc. Les acercamos también un plugin que combina varia de ellas [Static Code Analysis](https://github.com/Monits/static-code-analysis-plugin). 
En cuanto al lenguaje de programación Golang y JavaScript pueden usar linters [golangci-lint](https://golangci-lint.run/) y [eslint](https://eslint.org/) respectivamente.
