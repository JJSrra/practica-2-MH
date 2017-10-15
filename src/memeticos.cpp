#include <memeticos.h>

using namespace std;

// Memético con BLX y LS cada 10 generaciones
void AMtotal(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(10);
  vector<double> aux_poblacion(train[0].size());
  int mejor_individuo;
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual, mejor_porcentaje = 0;
  int evaluaciones = 0;
  int generaciones = 0;

  for (int i=0; i < poblacion.size(); ++i){
    for (int j=0; j < train[0].size(); ++j){
      aux_poblacion[j] = rand()/(double)RAND_MAX;
    }
    poblacion[i] = aux_poblacion;
  }

  for (int i=0; i < poblacion.size(); ++i){
    kNN(train, etiquetas_train, train, poblacion[i], etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_actual = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
    porcentajes_poblacion[i] = porcentaje_actual;

    if (porcentaje_actual > mejor_porcentaje){
      mejor_porcentaje = porcentaje_actual;
      mejor_individuo = i;
    }
  }

  vector<vector<double> > nueva_poblacion(poblacion.size());
  int padre_elegido;
  vector<int> padres(poblacion.size());
  vector<double> hijo1, hijo2;
  int mejor_hijo, peor_hijo;
  double mejor_porcentaje_hijo, peor_porcentaje_hijo;
  int genes_generados = 0;
  int mutaciones, indice_mutado;
  vector<double> porcentajes_nuevos(poblacion.size());
  vector<bool> conozco_porcentaje(poblacion.size());

  vector<double> mejor_solucion(train[0].size()), solucion_vecina;
  bool mejora;
  int iteraciones;
  int max_sin_mejora = 2*train[0].size();

  while (evaluaciones < 15000){

    for (int i=0; i < poblacion.size(); ++i){
      padre_elegido = torneoBinario(porcentajes_poblacion);
      padres[i] = padre_elegido;
    }

    for (int i=0; i < poblacion.size(); i+=2){
      if (i+1 < poblacion.size()*0.7){
        cruceBLX(0.3, poblacion[padres[i]], poblacion[padres[i+1]], hijo1, hijo2, generador);
        nueva_poblacion[i] = hijo1;
        conozco_porcentaje[i] = false;
        nueva_poblacion[i+1] = hijo2;
        conozco_porcentaje[i+1] = false;
      }
      else{
        nueva_poblacion[i] = poblacion[padres[i]];
        conozco_porcentaje[i] = true;
        porcentajes_nuevos[i] = porcentajes_poblacion[padres[i]];
        nueva_poblacion[i+1] = poblacion[padres[i+1]];
        conozco_porcentaje[i+1] = true;
        porcentajes_nuevos[i+1] = porcentajes_poblacion[padres[i+1]];
      }
    }

    genes_generados += poblacion.size() * poblacion[0].size();
    mutaciones = genes_generados%1000;
    genes_generados -= mutaciones*1000;

    for (int i=0; i < mutaciones; ++i){
      indice_mutado = rand()%poblacion.size();
      mutarCromosoma(nueva_poblacion[indice_mutado], distribucion_normal, generador);
    }

    mejor_porcentaje_hijo = 0;
    peor_porcentaje_hijo = 100;
    mejor_hijo = 0;
    peor_hijo = 0;

    for (int i=0; i < nueva_poblacion.size(); ++i){
      if (!conozco_porcentaje[i]){
        kNN(train, etiquetas_train, train, nueva_poblacion[i], etiquetas_calculadas, true);
        ++evaluaciones;
        porcentaje_actual = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
        porcentajes_nuevos[i] = porcentaje_actual;
      }

      if (porcentajes_nuevos[i] > mejor_porcentaje_hijo){
        mejor_porcentaje_hijo = porcentajes_nuevos[i];
        mejor_hijo = i;
      }
      else if (porcentajes_nuevos[i] < peor_porcentaje_hijo){
        peor_porcentaje_hijo = porcentajes_nuevos[i];
        peor_hijo = i;
      }
    }

    if (mejor_porcentaje_hijo < mejor_porcentaje){
      nueva_poblacion[peor_hijo] = poblacion[mejor_individuo];
      mejor_individuo = peor_hijo;
    }
    else{
      mejor_individuo = mejor_hijo;
      mejor_porcentaje = mejor_porcentaje_hijo;
    }

    poblacion = nueva_poblacion;
    porcentajes_poblacion = porcentajes_nuevos;
    ++generaciones;

    if (generaciones%10 == 0){
      for (int i=0; i < poblacion.size(); ++i){
        iteraciones = 0;
        vector<int> indices;

        while(iteraciones < max_sin_mejora){
          mejora = false;
          for (int j=0; j < train[0].size() && !mejora; ++j){
            generarVecino(poblacion[i], solucion_vecina, distribucion_normal, generador, indices);
            kNN(train, etiquetas_train, train, solucion_vecina, etiquetas_calculadas, true);
            ++iteraciones;
            porcentaje_actual = porcentajeEtiquetas(etiquetas_calculadas, etiquetas_train);
            if (porcentaje_actual > porcentajes_poblacion[i]){
              porcentajes_poblacion[i] = porcentaje_actual;
              poblacion[i] = solucion_vecina;
              mejora = true;
            }
          }
        }

        evaluaciones += iteraciones;
      }
    }
  }

  solucion_generada = poblacion[mejor_individuo];

}

// Función que realiza los cálculos pertinentes con AMtotal y muestra los porcentajes obtenidos
void evaluarAMtotal(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AMtotal(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double porcentaje = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AMtotal train en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;

  t1 = chrono::high_resolution_clock::now();
  AMtotal(test, etiquetas_test, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(test, etiquetas_test, train, pesos_calculados, etiquetas_calculadas, false);
  porcentaje = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AMtotal test en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;
}

// Memético con BLX y LS en el 10% de los cromosomas
void AM10(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(10);
  vector<double> aux_poblacion(train[0].size());
  int mejor_individuo;
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual, mejor_porcentaje = 0;
  int evaluaciones = 0;
  int generaciones = 0;

  for (int i=0; i < poblacion.size(); ++i){
    for (int j=0; j < train[0].size(); ++j){
      aux_poblacion[j] = rand()/(double)RAND_MAX;
    }
    poblacion[i] = aux_poblacion;
  }

  for (int i=0; i < poblacion.size(); ++i){
    kNN(train, etiquetas_train, train, poblacion[i], etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_actual = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
    porcentajes_poblacion[i] = porcentaje_actual;

    if (porcentaje_actual > mejor_porcentaje){
      mejor_porcentaje = porcentaje_actual;
      mejor_individuo = i;
    }
  }

  vector<vector<double> > nueva_poblacion(poblacion.size());
  int padre_elegido;
  vector<int> padres(poblacion.size());
  vector<double> hijo1, hijo2;
  int mejor_hijo, peor_hijo;
  double mejor_porcentaje_hijo, peor_porcentaje_hijo;
  int genes_generados = 0;
  int mutaciones, indice_mutado;
  vector<double> porcentajes_nuevos(poblacion.size());
  vector<bool> conozco_porcentaje(poblacion.size());

  vector<double> mejor_solucion(train[0].size()), solucion_vecina;
  bool mejora;
  int iteraciones;
  int max_sin_mejora = 2*train[0].size();

  while (evaluaciones < 15000){

    for (int i=0; i < poblacion.size(); ++i){
      padre_elegido = torneoBinario(porcentajes_poblacion);
      padres[i] = padre_elegido;
    }

    for (int i=0; i < poblacion.size(); i+=2){
      if (i+1 < poblacion.size()*0.7){
        cruceBLX(0.3, poblacion[padres[i]], poblacion[padres[i+1]], hijo1, hijo2, generador);
        nueva_poblacion[i] = hijo1;
        conozco_porcentaje[i] = false;
        nueva_poblacion[i+1] = hijo2;
        conozco_porcentaje[i+1] = false;
      }
      else{
        nueva_poblacion[i] = poblacion[padres[i]];
        conozco_porcentaje[i] = true;
        porcentajes_nuevos[i] = porcentajes_poblacion[padres[i]];
        nueva_poblacion[i+1] = poblacion[padres[i+1]];
        conozco_porcentaje[i+1] = true;
        porcentajes_nuevos[i+1] = porcentajes_poblacion[padres[i+1]];
      }
    }

    genes_generados += poblacion.size() * poblacion[0].size();
    mutaciones = genes_generados%1000;
    genes_generados -= mutaciones*1000;

    for (int i=0; i < mutaciones; ++i){
      indice_mutado = rand()%poblacion.size();
      mutarCromosoma(nueva_poblacion[indice_mutado], distribucion_normal, generador);
    }

    mejor_porcentaje_hijo = 0;
    peor_porcentaje_hijo = 100;
    mejor_hijo = 0;
    peor_hijo = 0;

    for (int i=0; i < nueva_poblacion.size(); ++i){
      if (!conozco_porcentaje[i]){
        kNN(train, etiquetas_train, train, nueva_poblacion[i], etiquetas_calculadas, true);
        ++evaluaciones;
        porcentaje_actual = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
        porcentajes_nuevos[i] = porcentaje_actual;
      }

      if (porcentajes_nuevos[i] > mejor_porcentaje_hijo){
        mejor_porcentaje_hijo = porcentajes_nuevos[i];
        mejor_hijo = i;
      }
      else if (porcentajes_nuevos[i] < peor_porcentaje_hijo){
        peor_porcentaje_hijo = porcentajes_nuevos[i];
        peor_hijo = i;
      }
    }

    if (mejor_porcentaje_hijo < mejor_porcentaje){
      nueva_poblacion[peor_hijo] = poblacion[mejor_individuo];
      mejor_individuo = peor_hijo;
    }
    else{
      mejor_individuo = mejor_hijo;
      mejor_porcentaje = mejor_porcentaje_hijo;
    }

    poblacion = nueva_poblacion;
    porcentajes_poblacion = porcentajes_nuevos;
    ++generaciones;

    if (generaciones%10 == 0){
      iteraciones = 0;
      vector<int> indices;
      int cromosoma_mutado = rand()%poblacion.size();

      while(iteraciones < max_sin_mejora){
        mejora = false;
        for (int j=0; j < train[0].size() && !mejora; ++j){
          generarVecino(poblacion[cromosoma_mutado], solucion_vecina, distribucion_normal, generador, indices);
          kNN(train, etiquetas_train, train, solucion_vecina, etiquetas_calculadas, true);
          ++iteraciones;
          porcentaje_actual = porcentajeEtiquetas(etiquetas_calculadas, etiquetas_train);
          if (porcentaje_actual > porcentajes_poblacion[cromosoma_mutado]){
            porcentajes_poblacion[cromosoma_mutado] = porcentaje_actual;
            poblacion[cromosoma_mutado] = solucion_vecina;
            mejora = true;
          }
        }

        evaluaciones += iteraciones;
      }
    }
  }

  solucion_generada = poblacion[mejor_individuo];

}

// Función que realiza los cálculos pertinentes con AM10 y muestra los porcentajes obtenidos
void evaluarAM10(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AM10(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double porcentaje = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AM10 train en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;

  t1 = chrono::high_resolution_clock::now();
  AM10(test, etiquetas_test, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(test, etiquetas_test, train, pesos_calculados, etiquetas_calculadas, false);
  porcentaje = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AM10 test en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;
}

// Memético con BLX y LS en el mejor cromosoma
void AMmejor(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(10);
  vector<double> aux_poblacion(train[0].size());
  int mejor_individuo;
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual, mejor_porcentaje = 0;
  int evaluaciones = 0;
  int generaciones = 0;

  for (int i=0; i < poblacion.size(); ++i){
    for (int j=0; j < train[0].size(); ++j){
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

  vector<vector<double> > nueva_poblacion(poblacion.size());
  int padre_elegido;
  vector<int> padres(poblacion.size());
  vector<double> hijo1, hijo2;
  int mejor_hijo, peor_hijo;
  double mejor_porcentaje_hijo, peor_porcentaje_hijo;
  int genes_generados = 0;
  int mutaciones, indice_mutado;
  vector<double> porcentajes_nuevos(poblacion.size());
  vector<bool> conozco_porcentaje(poblacion.size());

  vector<double> mejor_solucion(train[0].size()), solucion_vecina;
  bool mejora;
  int iteraciones;
  int max_sin_mejora = 2*train[0].size();

  while (evaluaciones < 15000){

    for (int i=0; i < poblacion.size(); ++i){
      padre_elegido = torneoBinario(porcentajes_poblacion);
      padres[i] = padre_elegido;
    }

    for (int i=0; i < poblacion.size(); i+=2){
      if (i+1 < poblacion.size()*0.7){
        cruceBLX(0.3, poblacion[padres[i]], poblacion[padres[i+1]], hijo1, hijo2, generador);
        nueva_poblacion[i] = hijo1;
        conozco_porcentaje[i] = false;
        nueva_poblacion[i+1] = hijo2;
        conozco_porcentaje[i+1] = false;
      }
      else{
        nueva_poblacion[i] = poblacion[padres[i]];
        conozco_porcentaje[i] = true;
        porcentajes_nuevos[i] = porcentajes_poblacion[padres[i]];
        nueva_poblacion[i+1] = poblacion[padres[i+1]];
        conozco_porcentaje[i+1] = true;
        porcentajes_nuevos[i+1] = porcentajes_poblacion[padres[i+1]];
      }
    }

    genes_generados += poblacion.size() * poblacion[0].size();
    mutaciones = genes_generados%1000;
    genes_generados -= mutaciones*1000;

    for (int i=0; i < mutaciones; ++i){
      indice_mutado = rand()%poblacion.size();
      mutarCromosoma(nueva_poblacion[indice_mutado], distribucion_normal, generador);
    }

    mejor_porcentaje_hijo = 0;
    peor_porcentaje_hijo = 100;
    mejor_hijo = 0;
    peor_hijo = 0;

    for (int i=0; i < nueva_poblacion.size(); ++i){
      if (!conozco_porcentaje[i]){
        porcentaje_actual = calcularValidezSolucion(train, etiquetas_train, nueva_poblacion[i], 0.1, 0.5);
        ++evaluaciones;
        porcentajes_nuevos[i] = porcentaje_actual;
      }

      if (porcentajes_nuevos[i] > mejor_porcentaje_hijo){
        mejor_porcentaje_hijo = porcentajes_nuevos[i];
        mejor_hijo = i;
      }
      else if (porcentajes_nuevos[i] < peor_porcentaje_hijo){
        peor_porcentaje_hijo = porcentajes_nuevos[i];
        peor_hijo = i;
      }
    }

    if (mejor_porcentaje_hijo < mejor_porcentaje){
      nueva_poblacion[peor_hijo] = poblacion[mejor_individuo];
      mejor_individuo = peor_hijo;
    }
    else{
      mejor_individuo = mejor_hijo;
      mejor_porcentaje = mejor_porcentaje_hijo;
    }

    poblacion = nueva_poblacion;
    porcentajes_poblacion = porcentajes_nuevos;
    ++generaciones;

    if (generaciones%10 == 0){
      iteraciones = 0;
      vector<int> indices;

      while(iteraciones < max_sin_mejora){
        mejora = false;
        for (int j=0; j < train[0].size() && !mejora; ++j){
          generarVecino(poblacion[mejor_individuo], solucion_vecina, distribucion_normal, generador, indices);
          porcentaje_actual = calcularValidezSolucion(train, etiquetas_train, solucion_vecina, 0.1, 0.5);
          ++iteraciones;
          if (porcentaje_actual > porcentajes_poblacion[mejor_individuo]){
            porcentajes_poblacion[mejor_individuo] = porcentaje_actual;
            poblacion[mejor_individuo] = solucion_vecina;
            mejora = true;
          }
        }

        evaluaciones += iteraciones;
      }
    }
  }

  solucion_generada = poblacion[mejor_hijo];

}

// Función que realiza los cálculos pertinentes con AMmejor y muestra los porcentajes obtenidos
void evaluarAMmejor(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AMmejor(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}
