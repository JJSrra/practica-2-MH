#include <vector>
#include <iostream>
#include <math.h>
#include <utilidadesBD.h>
#include <ctime>
#include <chrono>

using namespace std;

// Cálculo de distancia euclídea entre dos vectores
double distanciaEuclidea(vector<double>& v1, vector<double>& v2, vector<double>& pesos);

// Cálculo del porcentaje de etiquetas correctas
double porcentajeEtiquetas(vector<int>& etiquetas, vector<int>& etiquetas_calculadas);

// Cálculo de la tasa de reducción
double tasaReduccion(vector<double>& pesos, double cota);

// Cálculo de la función objetivo
double funcionObjetivo(double tasa_clas, double tasa_red, double alfa);

// Cálculo de la validez de una solución
double calcularValidezSolucion(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos, double cota, double alfa);

// Algoritmo k-NN
void kNN(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<vector<double> >& test, vector<double>& pesos, vector<int>& etiquetas_calculadas, bool leave_one_out);

// Función que realiza los cálculos pertinentes con kNN y muestra por pantalla los porcentajes
void evaluarkNN(int bd_elegida, int particion);

// Algoritmo RELIEF
void RELIEF(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados);

// Función que realiza los cálculos pertinentes con RELIEF y muestra por pantalla los porcentajes
void evaluarRELIEF(int bd_elegida, int particion);

// Algoritmo que genera un vecino para la Búsqueda Local
void generarVecino(vector<double>& solucion_base, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador, vector<int>& indices);

// Algoritmo de Búsqueda Local
void localSearch(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_inicial, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador, int max_iteraciones);

// Función que realiza los cálculos para la local search y muestra por pantalla los porcentajes
void evaluarLocalSearch(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Algoritmo de Enfriamiento Simulado
void simulatedAnnealing(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos pertinentes con Simulated Annealing y muestra por pantalla los porcentajes
void evaluarSimulatedAnnealing(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Algoritmo que genera un vecino para la Búsqueda Local Reiterada
void generarVecinoILS(vector<double>& solucion_base, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador, vector<int>& indices);

// Algoritmo de Búsqueda Local Reiterada
void iteratedLocalSearch(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador);

// Función que realiza los cálculos para la iterated local search y muestra por pantalla los porcentajes
void evaluarILS(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador);
