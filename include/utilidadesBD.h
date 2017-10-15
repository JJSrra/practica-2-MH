#include <fstream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iostream>
#include <random>
#include <chrono>
#include <algorithm>
#include <numeric>

using namespace std;

// Función que lee la base de datos del camino indicado y la carga
// en la matriz de datos pasada por referencia, con sus etiquetas por dato en otro vector.
void leerBD(vector<vector<double> >& datos, vector<int>& etiquetas, string path_bd_elegida);
// Función que normaliza la base de datos seleccionada (1 para sonar, 2 para WDBC, 3 para spam) y la mete
// en un fichero para poder ser leída desde la función leerBaseDatos.
// ATENCIÓN: sólo hace falta llamar a esta función una vez, no cada vez que se lanza el programa.
//           Es decir, cuando los ficheros estén normalizados no hace falta normalizarlos más veces.
void normalizarBD(int bd_elegida);
// Función que genera las particiones para los conjuntos de prueba y entrenamiento
void FiveFoldCrossValidation(int bd_elegida);
