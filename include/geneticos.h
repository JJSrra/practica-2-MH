#include <vector>
#include <iostream>
#include <math.h>
#include <utilidadesBD.h>
#include <algoritmos.h>
#include <ctime>
#include <chrono>

using namespace std;

// Torneo binario
int torneoBinario(vector<double>& porcentajes_poblacion);

// Mutar cromosoma
void mutarCromosoma(vector<double>& individuo, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Cruce BLX
void cruceBLX(double alfa, vector<double>& padre1, vector<double>& padre2, vector<double>& hijo1, vector<double>& hijo2, default_random_engine& generador);

// Cruce aritmético
void cruceAritmetico(vector<double>& padre1, vector<double>& padre2, vector<double>& hijo);

// Algoritmo genético generacional BLX
void AGGBLX(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AGGBLX y muestra los porcentajes obtenidos
void evaluarAGGBLX(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Algoritmo genético estacionario BLX
void AGEBLX(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AGEBLX y muestra los porcentajes obtenidos
void evaluarAGEBLX(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Algoritmo genético generacional con cruce aritmético
void AGGCA(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AGGCA y muestra los porcentajes obtenidos
void evaluarAGGCA(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Algoritmo genético estacionario con cruce aritmético
void AGECA(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con AGECA y muestra los porcentajes obtenidos
void evaluarAGECA(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);
