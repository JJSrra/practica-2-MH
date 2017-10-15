#include <vector>
#include <iostream>
#include <math.h>
#include <utilidadesBD.h>
#include <algoritmos.h>
#include <geneticos.h>
#include <ctime>
#include <chrono>

using namespace std;

// Memético con BLX y LS cada 10 generaciones
void AMtotal(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AMtotal y muestra los porcentajes obtenidos
void evaluarAMtotal(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Memético con BLX y LS en el 10% de los cromosomas
void AM10(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AM10 y muestra los porcentajes obtenidos
void evaluarAM10(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Memético con BLX y LS en el mejor cromosoma
void AMmejor(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AMmejor y muestra los porcentajes obtenidos
void evaluarAMmejor(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);
