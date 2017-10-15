#include <algoritmos.h>

using namespace std;

// Cálculo de la distancia euclídea entre dos vectores
double distanciaEuclidea(vector<double>& v1, vector<double>& v2, vector<double>& pesos){
  if (v1.size() != v2.size()){
    cerr << "Los vectores no tienen el mismo tamaño" << endl;
    exit(-1);
  }
  double distancias = 0;
  for (int i=0; i < v1.size(); ++i){
    if (pesos[i] >= 0.1)
      distancias += pesos[i]*(v1[i] - v2[i])*(v1[i] - v2[i]);
  }

  distancias = sqrt(distancias);
  return distancias;
}

// Cálculo del porcentaje de etiquetas correctas
double porcentajeEtiquetas(vector<int>& etiquetas, vector<int>& etiquetas_calculadas){
  if (etiquetas.size() != etiquetas_calculadas.size()){
    cerr << "El tamaño de los vectores de etiquetas no coincide" << endl;
    exit(-1);
  }

  double porcentaje;
  int etiquetas_correctas = 0;

  for (int i=0; i < etiquetas.size(); ++i){
    if (etiquetas[i] == etiquetas_calculadas[i])
      ++etiquetas_correctas;
  }

  porcentaje = (etiquetas_correctas*1.0 / etiquetas.size()*1.0)*100.0;
  return porcentaje;
}

// Cálculo de la tasa de reducción
double tasaReduccion(vector<double>& pesos, double cota){
  double porcentaje;
  int pesos_bajo_cota = 0;

  for (int i=0; i < pesos.size(); ++i){
    if (pesos[i] < cota)
      ++pesos_bajo_cota;
  }

  porcentaje = (pesos_bajo_cota*1.0/pesos.size()*1.0)*100.0;
  return porcentaje;
}

// Cálculo de la función objetivo
double funcionObjetivo(double tasa_clas, double tasa_red, double alfa){
  return tasa_clas*alfa + tasa_red*(1-alfa);
}

// Cálculo de la validez de una solución
double calcularValidezSolucion(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos, double cota, double alfa){
  vector<int> etiquetas_calculadas;
  kNN(train, etiquetas_train, train, pesos, etiquetas_calculadas, true);
  double tasa_clas = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos, cota);
  return funcionObjetivo(tasa_clas, tasa_red, alfa);
}

// Algoritmo k-NN para el vecino más cercano (Nearest neighbour)
void kNN(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<vector<double> >& test, vector<double>& pesos, vector<int>& etiquetas_calculadas, bool leave_one_out){
  etiquetas_calculadas.resize(test.size());
  int etiqueta_minimo;
  double distancia_minima, distancia_actual;

  for (int i=0; i < test.size(); ++i){
    etiqueta_minimo = 0;
    distancia_minima = 100;
    for (int j=0; j < train.size(); ++j){
      if (leave_one_out){
        if (i!=j){
          distancia_actual = distanciaEuclidea(train[j],test[i],pesos);
          if (distancia_actual < distancia_minima){
            etiqueta_minimo = etiquetas_train[j];
            distancia_minima = distancia_actual;
          }
        }
      }
      else{
        distancia_actual = distanciaEuclidea(train[j],test[i],pesos);
        if (distancia_actual < distancia_minima){
          etiqueta_minimo = etiquetas_train[j];
          distancia_minima = distancia_actual;
        }
      }
    }
    etiquetas_calculadas[i] = etiqueta_minimo;
  }
}

// Función que realiza los cálculos pertinentes con kNN y muestra por pantalla los porcentajes
void evaluarkNN(int bd_elegida, int particion){
  string bd_test, bd_train, nombre_bd;
  vector<double> aux;
  vector<vector<double> > train, test;
  vector<int> etiquetas_train, etiquetas_test;
  chrono::high_resolution_clock::time_point t1, t2;
  chrono::duration<double> transcurrido;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_" + to_string(particion);
      bd_train = "partitions/sonar_train_" + to_string(particion);
      nombre_bd = "sonar";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_" + to_string(particion);
      bd_train = "partitions/wdbc_train_" + to_string(particion);
      nombre_bd = "wdbc";
      break;

    case 3:
      bd_test = "partitions/spam_test_" + to_string(particion);
      bd_train = "partitions/spam_train_" + to_string(particion);
      nombre_bd = "spam";
      break;
  }

  leerBD(train, etiquetas_train, bd_train);
  leerBD(test, etiquetas_test, bd_test);

  vector<double> pesos(train[0].size(), 1);

  vector<int> etiquetas_calculadas;
  t1 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos, etiquetas_calculadas, false);
  t2 = chrono::high_resolution_clock::now();
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = 0;
  double agregado = funcionObjetivo(tasa_clas, 0, 0.5);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo RELIEF
void RELIEF(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados){
  pesos_calculados.resize(train[0].size());
  for(int i=0; i < train[0].size(); ++i){pesos_calculados[i]=0;}
  int enemigo_cercano, amigo_cercano;
  double distancia_enemigo, distancia_amigo, distancia_actual;
  vector<double> pesos_1(train[0].size(),1);

  for (int i=0; i < train.size(); ++i){
    distancia_amigo = 1000000;
    distancia_enemigo = 1000000;
    enemigo_cercano = 0;
    amigo_cercano = 0;
    for (int j=0; j < train.size(); ++j){
      if(i!=j){
        distancia_actual = distanciaEuclidea(train[j],train[i],pesos_1);
        if (etiquetas_train[i] == etiquetas_train[j]){
          if (distancia_actual < distancia_amigo){
            distancia_amigo = distancia_actual;
            amigo_cercano = j;
          }
        }
        else{
          if (distancia_actual < distancia_enemigo){
            distancia_enemigo = distancia_actual;
            enemigo_cercano = j;
          }
        }
      }
    }

    for (int k=0; k < pesos_calculados.size(); ++k){
      pesos_calculados[k] = pesos_calculados[k] - abs(train[i][k]-train[amigo_cercano][k])
                          + abs(train[i][k]-train[enemigo_cercano][k]);
    }
  }

  double peso_maximo = *max_element(pesos_calculados.begin(), pesos_calculados.end());

  for (int i=0; i < pesos_calculados.size(); ++i){
    if (pesos_calculados[i] < 0)
      pesos_calculados[i] = 0;
    else
      pesos_calculados[i] = pesos_calculados[i]/peso_maximo;
  }
}

// Función que realiza los cálculos pertinentes con RELIEF y muestra por pantalla los porcentajes
void evaluarRELIEF(int bd_elegida, int particion){
  string bd_test, bd_train, nombre_bd;
  vector<double> aux;
  vector<vector<double> > train, test;
  vector<int> etiquetas_train, etiquetas_test;
  chrono::high_resolution_clock::time_point t1, t2;
  chrono::duration<double> transcurrido;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_" + to_string(particion);
      bd_train = "partitions/sonar_train_" + to_string(particion);
      nombre_bd = "sonar";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_" + to_string(particion);
      bd_train = "partitions/wdbc_train_" + to_string(particion);
      nombre_bd = "wdbc";
      break;

    case 3:
      bd_test = "partitions/spam_test_" + to_string(particion);
      bd_train = "partitions/spam_train_" + to_string(particion);
      nombre_bd = "spam";
      break;
  }

  leerBD(train, etiquetas_train, bd_train);
  leerBD(test, etiquetas_test, bd_test);

  vector<double> pesos_calculados;
  t1 = chrono::high_resolution_clock::now();
  RELIEF(train, etiquetas_train, pesos_calculados);
  t2 = chrono::high_resolution_clock::now();
  vector<int> etiquetas_calculadas;
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo que genera un vecino para la Búsqueda Local
void generarVecino(vector<double>& solucion_base, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador, vector<int>& indices){
  solucion_generada = solucion_base;
  double numero_generado = distribucion_normal(generador);

  if (indices.size() == 0){
    indices.resize(solucion_base.size());
    iota(indices.begin(), indices.end(), 0);
  }

  int indice_aleatorio = rand()%indices.size();
  int indice_cambio = indices[indice_aleatorio];
  indices.erase(indices.begin() + indice_aleatorio);

  double valor_cambiado = solucion_base[indice_cambio] + numero_generado;
  if (valor_cambiado > 1)
    solucion_generada[indice_cambio] = 1;
  else if (valor_cambiado < 0)
    solucion_generada[indice_cambio] = 0;
  else
    solucion_generada[indice_cambio] = valor_cambiado;

}

// Algoritmo de Búsqueda Local
void localSearch(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_inicial, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador, int max_iteraciones){
  vector<double> mejor_solucion = solucion_inicial, solucion_vecina;
  double mejor_validez, validez_actual;
  vector<int> indices;
  bool mejora;

  mejor_validez = calcularValidezSolucion(train, etiquetas_train, mejor_solucion, 0.1, 0.5);

  int iteraciones=1, sin_mejora=0;
  int max_sin_mejora = 20*train[0].size();

  while(iteraciones < max_iteraciones && sin_mejora < max_sin_mejora){
    mejora = false;
    for (int i=0; i < train[0].size() && !mejora; ++i){
      generarVecino(mejor_solucion, solucion_vecina, distribucion_normal, generador, indices);
      validez_actual = calcularValidezSolucion(train, etiquetas_train, solucion_vecina, 0.1, 0.5);
      ++iteraciones;
      if (validez_actual > mejor_validez){
        mejor_validez = validez_actual;
        mejor_solucion = solucion_vecina;
        sin_mejora = 0;
        mejora = true;
      }
      else
        ++sin_mejora;
    }
  }

  pesos_calculados = mejor_solucion;
}

// Función que realiza los cálculos pertinentes con RELIEF y muestra por pantalla los porcentajes
void evaluarLocalSearch(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  string bd_test, bd_train, nombre_bd;
  vector<double> aux;
  vector<vector<double> > train, test;
  vector<int> etiquetas_train, etiquetas_test;
  chrono::high_resolution_clock::time_point t1, t2;
  chrono::duration<double> transcurrido;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_" + to_string(particion);
      bd_train = "partitions/sonar_train_" + to_string(particion);
      nombre_bd = "sonar";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_" + to_string(particion);
      bd_train = "partitions/wdbc_train_" + to_string(particion);
      nombre_bd = "wdbc";
      break;

    case 3:
      bd_test = "partitions/spam_test_" + to_string(particion);
      bd_train = "partitions/spam_train_" + to_string(particion);
      nombre_bd = "spam";
      break;
  }

  leerBD(train, etiquetas_train, bd_train);
  leerBD(test, etiquetas_test, bd_test);

  int num_caracteristicas = train[0].size();
  vector<double> solucion_inicial(num_caracteristicas);
  for (int i=0; i < num_caracteristicas; ++i)
    solucion_inicial[i] = rand()/(double)RAND_MAX;

  vector<double> pesos_calculados;
  vector<int> etiquetas_calculadas;
  t1 = chrono::high_resolution_clock::now();
  localSearch(train, etiquetas_train, solucion_inicial, pesos_calculados, distribucion_normal, generador, 15000);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo de Enfriamiento Simulado
void simulatedAnnealing(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  int num_caracteristicas = train[0].size();
  vector<int> indices;
  vector<double> mejor_solucion(num_caracteristicas);
  for (int i=0; i < num_caracteristicas; ++i)
    mejor_solucion[i] = rand()/(double)RAND_MAX;

  vector<double> solucion_actual = mejor_solucion;

  double t_final = 0.001;
  double mejor_validez = calcularValidezSolucion(train, etiquetas_train, mejor_solucion, 0.1, 0.5);
  int evaluaciones = 1, exitos = 1;
  int max_vecinos = 10*num_caracteristicas, max_exitos = 0.1*max_vecinos;
  double temperatura = (0.3 * mejor_validez) / -log(0.3);
  double beta = (temperatura - t_final)/((15000/max_vecinos) * temperatura * t_final);
  vector<double> solucion_vecina(num_caracteristicas);

  double variacion_validez, validez_actual = mejor_validez, validez_vecino;

  while (evaluaciones < 15000 && exitos > 0 && temperatura > t_final){
    exitos = 0;
    for (int i=0; i < max_vecinos && exitos < max_exitos; ++i){
      generarVecino(solucion_actual, solucion_vecina, distribucion_normal, generador, indices);
      validez_vecino = calcularValidezSolucion(train, etiquetas_train, solucion_vecina, 0.1, 0.5);
      ++evaluaciones;
      variacion_validez = validez_vecino - validez_actual;

      if (variacion_validez > 0 || rand()/(double)RAND_MAX < exp(-variacion_validez/temperatura)){
        solucion_actual = solucion_vecina;
        validez_actual = validez_vecino;
        ++exitos;
        if (validez_actual > mejor_validez){
          mejor_solucion = solucion_actual;
          mejor_validez = validez_actual;
        }
      }
    }

    temperatura = temperatura / (1 + beta*temperatura);
  }

  pesos_calculados = mejor_solucion;
}

// Función que realiza los cálculos pertinentes con Simulated Annealing y muestra por pantalla los porcentajes
void evaluarSimulatedAnnealing(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  string bd_test, bd_train, nombre_bd;
  vector<double> aux;
  vector<vector<double> > train, test;
  vector<int> etiquetas_train, etiquetas_test;
  chrono::high_resolution_clock::time_point t1, t2;
  chrono::duration<double> transcurrido;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_" + to_string(particion);
      bd_train = "partitions/sonar_train_" + to_string(particion);
      nombre_bd = "sonar";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_" + to_string(particion);
      bd_train = "partitions/wdbc_train_" + to_string(particion);
      nombre_bd = "wdbc";
      break;

    case 3:
      bd_test = "partitions/spam_test_" + to_string(particion);
      bd_train = "partitions/spam_train_" + to_string(particion);
      nombre_bd = "spam";
      break;
  }

  leerBD(train, etiquetas_train, bd_train);
  leerBD(test, etiquetas_test, bd_test);

  vector<double> pesos_calculados;
  vector<int> etiquetas_calculadas;
  t1 = chrono::high_resolution_clock::now();
  simulatedAnnealing(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo que genera un vecino para la Búsqueda Local Reiterada
void generarVecinoILS(vector<double>& solucion_base, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador, vector<int>& indices){
  solucion_generada = solucion_base;
  double numero_generado, valor_cambiado;
  int indice_aleatorio, indice_cambio;

  for (int i=0; i < 0.1*solucion_base.size(); ++i){

    numero_generado = distribucion_normal(generador);

    if (indices.size() == 0){
      indices.resize(solucion_base.size());
      iota(indices.begin(), indices.end(), 0);
    }

    indice_aleatorio = rand()%indices.size();
    indice_cambio = indices[indice_aleatorio];
    indices.erase(indices.begin() + indice_aleatorio);

    valor_cambiado = solucion_base[indice_cambio] + numero_generado;
    if (valor_cambiado > 1)
      solucion_generada[indice_cambio] = 1;
    else if (valor_cambiado < 0)
      solucion_generada[indice_cambio] = 0;
    else
      solucion_generada[indice_cambio] = valor_cambiado;
  }
}


// Algoritmo de Búsqueda Local Reiterada
void iteratedLocalSearch(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  int num_caracteristicas = train[0].size();
  vector<int> indices;
  vector<double> mejor_solucion(num_caracteristicas);
  for (int i=0; i < num_caracteristicas; ++i)
    mejor_solucion[i] = rand()/(double)RAND_MAX;

  vector<double> solucion_generada, solucion_mutada;

  localSearch(train, etiquetas_train, mejor_solucion, mejor_solucion, distribucion_normal, generador, 1000);
  double mejor_validez = calcularValidezSolucion(train, etiquetas_train, mejor_solucion, 0.1, 0.5);
  double validez_actual;

  for (int busquedas=0; busquedas < 14; ++busquedas){
    generarVecinoILS(mejor_solucion, solucion_mutada, distribucion_normal, generador, indices);
    localSearch(train, etiquetas_train, solucion_mutada, solucion_generada, distribucion_normal, generador, 1000);
    validez_actual = calcularValidezSolucion(train, etiquetas_train, solucion_generada, 0.1, 0.5);

    if (validez_actual > mejor_validez){
      mejor_validez = validez_actual;
      mejor_solucion = solucion_generada;
    }
  }

  pesos_calculados = mejor_solucion;
}

// Función que realiza los cálculos para la iterated local search y muestra por pantalla los porcentajes
void evaluarILS(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  string bd_test, bd_train, nombre_bd;
  vector<double> aux;
  vector<vector<double> > train, test;
  vector<int> etiquetas_train, etiquetas_test;
  chrono::high_resolution_clock::time_point t1, t2;
  chrono::duration<double> transcurrido;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_" + to_string(particion);
      bd_train = "partitions/sonar_train_" + to_string(particion);
      nombre_bd = "sonar";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_" + to_string(particion);
      bd_train = "partitions/wdbc_train_" + to_string(particion);
      nombre_bd = "wdbc";
      break;

    case 3:
      bd_test = "partitions/spam_test_" + to_string(particion);
      bd_train = "partitions/spam_train_" + to_string(particion);
      nombre_bd = "spam";
      break;
  }

  leerBD(train, etiquetas_train, bd_train);
  leerBD(test, etiquetas_test, bd_test);

  vector<double> pesos_calculados;
  vector<int> etiquetas_calculadas;
  t1 = chrono::high_resolution_clock::now();
  iteratedLocalSearch(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;

}
