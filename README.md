# practica-2-MH
Práctica 2 de la asignatura Metaheurísticas de la Universidad de Granada (UGR)

- En el directorio bin podemos encontrar el ejecutable del programa, es el que deberemos utilizar para probar los algoritmos.

- En el directorio include aparecen las cabeceras de las funciones de la práctica, agrupadas por clase (utilidades de las bases de datos, algoritmos genéticos, etc.)

- En el directorio src aparecen los distintos códigos fuente de la práctica realizada, con todas las funciones desarrolladas.

- En el directorio db aparecen las bases de datos tal y como yo las he almacenado, extrayendo los valores de los ficheros arff y separándolos por espacios en lugar de por comas.

- También aparecen las bases de datos normalizadas con los valores entre 0 y 1 y las etiquetas cambiadas para que aparezcan al final y sean binarias 0-1.

- Es necesario que existan estos ficheros normalizados para poder sacar las particiones de ellos. Si no se tienen, se debe indicar al inicio del programa para que se realicen previamente a los cálculos.

- En el directorio partitions se almacenarán las distintas particiones para cada base de datos, y según sean test o train.

- En el directorio obj se guardan los ficheros .o generados a partir de la compilación de los .cpp.

- En la raíz se encuentra el fichero Makefile, que permite la compilación del programa con un simple comando "make".
