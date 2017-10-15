#include <vector>
#include <iostream>
#include <math.h>
#include <utilidadesBD.h>
#include <algoritmos.h>
#include <ctime>
#include <chrono>

using namespace std;

// Algoritmo de selección de padres para Differential Evolution
void seleccionar3Padres(int tam_poblacion, int& padre1, int& padre2, int& padre3);

// Algoritmo de selección de padres para Differential Evolution
void seleccionar2Padres(int tam_poblacion, int& padre1, int& padre2);

// Algoritmo de Evolución Diferencial Rand
void differentialEvolutionRand(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados);

// Función que realiza los cálculos pertinentes con Evolución Diferencial y muestra por pantalla los porcentajes
void evaluarDERand(int bd_elegida, int particion);

// Algoritmo de Evolución Diferencial Current to Best
void differentialEvolutionCtoBest(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados);

// Función que realiza los cálculos pertinentes con Evolución Diferencial Current to best y muestra por pantalla los porcentajes
void evaluarDECtoBest(int bd_elegida, int particion);
