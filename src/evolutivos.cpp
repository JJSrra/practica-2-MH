#include <evolutivos.h>

using namespace std;

// Algoritmo de selección de padres para Differential Evolution
void seleccionar3Padres(int tam_poblacion, int& padre1, int& padre2, int& padre3){
  vector<int> indices(tam_poblacion);
  iota(indices.begin(), indices.end(), 0);

  padre1 = rand()%indices.size();

  do{
    padre2 = rand()%indices.size();
  } while (padre2==padre1);

  do{
    padre3 = rand()%indices.size();
  } while (padre3==padre2 || padre3==padre1);
}

// Algoritmo de selección de padres para Differential Evolution
void seleccionar2Padres(int tam_poblacion, int& padre1, int& padre2){
  vector<int> indices(tam_poblacion);
  iota(indices.begin(), indices.end(), 0);

  padre1 = rand()%indices.size();

  do{
    padre2 = rand()%indices.size();
  } while (padre2==padre1);
}

// Algoritmo de Evolución Diferencial Rand
void differentialEvolutionRand(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados){
  int tam_poblacion = 50, num_caracteristicas = train[0].size();
  vector<vector<double> > poblacion(tam_poblacion);
  vector<double> aux_poblacion(num_caracteristicas);
  vector<double> porcentajes_poblacion(tam_poblacion);
  int evaluaciones = 0, mejor_individuo = 0;
  double porcentaje_actual, mejor_porcentaje = 0.0;

  for (int i=0; i < tam_poblacion; ++i){
    for (int j=0; j < num_caracteristicas; ++j){
      aux_poblacion[j] = rand()/(double)RAND_MAX;
    }
    poblacion[i] = aux_poblacion;
  }

  for (int i=0; i < poblacion.size(); ++i){
    porcentaje_actual = calcularValidezSolucion(train, etiquetas_train, poblacion[i], 0.1, 0.5);
    ++evaluaciones;
    porcentajes_poblacion[i] = porcentaje_actual;

    if (porcentaje_actual > mejor_porcentaje){
      mejor_porcentaje = porcentaje_actual;
      mejor_individuo = i;
    }
  }

  int padre1, padre2, padre3;
  vector<vector<double> > nueva_poblacion(tam_poblacion);
  vector<double> nuevos_porcentajes(tam_poblacion);

  while (evaluaciones < 15000){
    for (int i=0; i < tam_poblacion; ++i){
      seleccionar3Padres(tam_poblacion, padre1, padre2, padre3);
      for (int k=0; k < num_caracteristicas; ++k){
        if (rand()/(double)RAND_MAX < 0.5)
          aux_poblacion[k] = poblacion[padre1][k] + 0.5 * (poblacion[padre2][k] - poblacion[padre3][k]);
        else
          aux_poblacion[k] = poblacion[i][k];
      }
      nueva_poblacion[i] = aux_poblacion;
      nuevos_porcentajes[i] = calcularValidezSolucion(train, etiquetas_train, nueva_poblacion[i], 0.1, 0.5);
      ++evaluaciones;
    }

    for (int i=0; i < tam_poblacion; ++i){
      if (nuevos_porcentajes[i] > porcentajes_poblacion[i]){
        poblacion[i] = nueva_poblacion[i];
        porcentajes_poblacion[i] = nuevos_porcentajes[i];

        if (nuevos_porcentajes[i] > mejor_porcentaje){
          mejor_porcentaje = nuevos_porcentajes[i];
          mejor_individuo = i;
        }
      }
    }
  }

  pesos_calculados = poblacion[mejor_individuo];
}

// Función que realiza los cálculos pertinentes con Evolución Diferencial y muestra por pantalla los porcentajes
void evaluarDERand(int bd_elegida, int particion){
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
  differentialEvolutionRand(train, etiquetas_train, pesos_calculados);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo de Evolución Diferencial Current to Best
void differentialEvolutionCtoBest(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& pesos_calculados){
  int tam_poblacion = 50, num_caracteristicas = train[0].size();
  vector<vector<double> > poblacion(tam_poblacion);
  vector<double> aux_poblacion(num_caracteristicas);
  vector<double> porcentajes_poblacion(tam_poblacion);
  int evaluaciones = 0, mejor_individuo = 0;
  double porcentaje_actual, mejor_porcentaje = 0.0;

  for (int i=0; i < tam_poblacion; ++i){
    for (int j=0; j < num_caracteristicas; ++j){
      aux_poblacion[j] = rand()/(double)RAND_MAX;
    }
    poblacion[i] = aux_poblacion;
  }

  for (int i=0; i < poblacion.size(); ++i){
    porcentaje_actual = calcularValidezSolucion(train, etiquetas_train, poblacion[i], 0.1, 0.5);
    ++evaluaciones;
    porcentajes_poblacion[i] = porcentaje_actual;

    if (porcentaje_actual > mejor_porcentaje){
      mejor_porcentaje = porcentaje_actual;
      mejor_individuo = i;
    }
  }

  int padre1, padre2;
  vector<vector<double> > nueva_poblacion(tam_poblacion);
  vector<double> nuevos_porcentajes(tam_poblacion);

  while (evaluaciones < 15000){
    for (int i=0; i < tam_poblacion; ++i){
      seleccionar2Padres(tam_poblacion, padre1, padre2);
      for (int k=0; k < num_caracteristicas; ++k){
        if (rand()/(double)RAND_MAX < 0.5)
          aux_poblacion[k] = poblacion[i][k] + 0.5 * (poblacion[mejor_individuo][k] - poblacion[i][k]) + 0.5 * (poblacion[padre1][k] - poblacion[padre2][k]);
        else
          aux_poblacion[k] = poblacion[i][k];
      }
      nueva_poblacion[i] = aux_poblacion;
      nuevos_porcentajes[i] = calcularValidezSolucion(train, etiquetas_train, nueva_poblacion[i], 0.1, 0.5);
      ++evaluaciones;
    }

    for (int i=0; i < tam_poblacion; ++i){
      if (nuevos_porcentajes[i] > porcentajes_poblacion[i]){
        poblacion[i] = nueva_poblacion[i];
        porcentajes_poblacion[i] = nuevos_porcentajes[i];

        if (nuevos_porcentajes[i] > mejor_porcentaje){
          mejor_porcentaje = nuevos_porcentajes[i];
          mejor_individuo = i;
        }
      }
    }
  }

  pesos_calculados = poblacion[mejor_individuo];
}

// Función que realiza los cálculos pertinentes con Evolución Diferencial Current to Best y muestra por pantalla los porcentajes
void evaluarDECtoBest(int bd_elegida, int particion){
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
  differentialEvolutionCtoBest(train, etiquetas_train, pesos_calculados);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}
