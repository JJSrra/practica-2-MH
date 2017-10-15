#include <geneticos.h>

using namespace std;

// Torneo binario
int torneoBinario(vector<double>& porcentajes_poblacion){

  int padre1 = rand()%porcentajes_poblacion.size();
  int padre2;

  do{
    padre2 = rand()%porcentajes_poblacion.size();
  }while (padre2 == padre1);

  if (porcentajes_poblacion[padre1] > porcentajes_poblacion[padre2])
    return padre1;
  else
    return padre2;
}

// Mutar cromosoma
void mutarCromosoma(vector<double>& individuo, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  int indice_cambio = rand()%individuo.size();
  double numero_generado = distribucion_normal(generador);

  individuo[indice_cambio] += numero_generado;
  if (individuo[indice_cambio] > 1)
    individuo[indice_cambio] = 1;
  else if (individuo[indice_cambio] < 0)
    individuo[indice_cambio] = 0;
}

// Cruce BLX
void cruceBLX(double alfa, vector<double>& padre1, vector<double>& padre2, vector<double>& hijo1, vector<double>& hijo2, default_random_engine& generador){
  hijo1.resize(padre1.size());
  hijo2.resize(padre1.size());
  double cmax, cmin, i;
  double car_hijo1, car_hijo2;

  for (int j=0; j < padre1.size(); ++j){
    if (padre1[j] < padre2[j]){
      cmax = padre2[j];
      cmin = padre1[j];
    }
    else{
      cmax = padre1[j];
      cmin = padre2[j];
    }

    i = cmax - cmin;

    uniform_real_distribution<double> dist_uniforme(cmin-(i*alfa), cmax+(i*alfa));
    car_hijo1 = dist_uniforme(generador);
    car_hijo2 = dist_uniforme(generador);
    if (car_hijo1 > 1)
      hijo1[j] = 1;
    else if (car_hijo1 < 0)
      hijo1[j] = 0;
    else
      hijo1[j] = car_hijo1;

    if (car_hijo2 > 1)
      hijo2[j] = 1;
    else if (car_hijo2 < 0)
      hijo2[j] = 0;
    else
      hijo2[j] = car_hijo2;
  }
}

// Cruce aritmético
void cruceAritmetico(vector<double>& padre1, vector<double>& padre2, vector<double>& hijo){
  hijo.resize(padre1.size());

  for(int i=0; i < padre1.size(); ++i)
    hijo[i] = (padre1[i]+padre2[i])/2.0;
}

// Algoritmo genético generacional BLX
void AGGBLX(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(30);
  vector<double> aux_poblacion(train[0].size());
  int mejor_individuo;
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual, mejor_porcentaje = 0;
  int evaluaciones = 0;

  for (int i=0; i < 30; ++i){
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
  }

  solucion_generada = poblacion[mejor_individuo];

}

// Función que realiza los cálculos pertinentes con AGGBLX y muestra los porcentajes obtenidos
void evaluarAGGBLX(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AGGBLX(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double tasa_clas = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  double tasa_red = tasaReduccion(pesos_calculados, 0.1);
  double agregado = funcionObjetivo(tasa_clas, tasa_red, 0.5);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << tasa_clas << " " << tasa_red << " " << agregado << " " << transcurrido.count() << endl;
}

// Algoritmo genético estacionario BLX
void AGEBLX(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(30);
  vector<double> aux_poblacion(train[0].size());
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual;
  int evaluaciones = 0;

  for (int i=0; i < 30; ++i){
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
  }

  int padre1, padre2;
  vector<double> hijo1, hijo2;
  int indice_peor_porcentaje;
  double peor_porcentaje, porcentaje_hijo1, porcentaje_hijo2;
  int genes_generados = 0, mejor_solucion;
  int mutaciones, indice_mutado;

  while (evaluaciones < 15000){
    padre1 = torneoBinario(porcentajes_poblacion);
    padre2 = torneoBinario(porcentajes_poblacion);

    cruceBLX(0.3, poblacion[padre1], poblacion[padre2], hijo1, hijo2, generador);

    kNN(train, etiquetas_train, train, hijo1, etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_hijo1 = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
    kNN(train, etiquetas_train, train, hijo2, etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_hijo2 = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);

    indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
    peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];

    if (peor_porcentaje < max(porcentaje_hijo1, porcentaje_hijo2)){
      if (porcentaje_hijo1 > porcentaje_hijo2){
        poblacion[indice_peor_porcentaje] = hijo1;
        porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo1;

        indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
        peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];
        if (peor_porcentaje < porcentaje_hijo2){
          poblacion[indice_peor_porcentaje] = hijo2;
          porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo2;
        }
      }
      else{
        poblacion[indice_peor_porcentaje] = hijo2;
        porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo2;

        indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
        peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];
        if (peor_porcentaje < porcentaje_hijo1){
          poblacion[indice_peor_porcentaje] = hijo1;
          porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo1;
        }
      }
    }

    genes_generados += 2*poblacion[0].size();
    mutaciones = genes_generados%1000;
    genes_generados -= mutaciones*1000;

    for (int i=0; i < mutaciones; ++i){
      indice_mutado = rand()%poblacion.size();
      mutarCromosoma(poblacion[indice_mutado], distribucion_normal, generador);
    }
  }

  mejor_solucion = distance(porcentajes_poblacion.begin(), max_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
  solucion_generada = poblacion[mejor_solucion];
}

// Función que realiza los cálculos pertinentes con AGEBLX y muestra los porcentajes obtenidos
void evaluarAGEBLX(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AGEBLX(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double porcentaje = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGEBLX train en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;

  t1 = chrono::high_resolution_clock::now();
  AGEBLX(test, etiquetas_test, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(test, etiquetas_test, train, pesos_calculados, etiquetas_calculadas, false);
  porcentaje = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGEBLX test en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;
}

// Algoritmo genético generacional con cruce aritmético
void AGGCA(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(30);
  vector<double> aux_poblacion(train[0].size());
  int mejor_individuo;
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual, mejor_porcentaje = 0;
  int evaluaciones = 0;

  for (int i=0; i < 30; ++i){
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
  vector<int> padres(poblacion.size()*1.7);
  vector<double> hijo1;
  int mejor_hijo, peor_hijo;
  double mejor_porcentaje_hijo, peor_porcentaje_hijo;
  int genes_generados = 0;
  int mutaciones, indice_mutado;
  vector<double> porcentajes_nuevos(poblacion.size());
  vector<bool> conozco_porcentaje(poblacion.size());
  int contador_hijos;

  while (evaluaciones < 15000){
    contador_hijos = 0;

    for (int i=0; i < poblacion.size()*1.7; ++i){
      padre_elegido = torneoBinario(porcentajes_poblacion);
      padres[i] = padre_elegido;
    }

    for (int i=0; i < poblacion.size()*0.7*2; i+=2){
      cruceAritmetico(poblacion[padres[i]], poblacion[padres[i+1]], hijo1);
      nueva_poblacion[contador_hijos] = hijo1;
      conozco_porcentaje[contador_hijos] = false;
      ++contador_hijos;
    }

    while (contador_hijos < poblacion.size()){
      nueva_poblacion[contador_hijos] = poblacion[padres[contador_hijos]];
      conozco_porcentaje[contador_hijos] = true;
      porcentajes_nuevos[contador_hijos] = porcentajes_poblacion[padres[contador_hijos]];
      ++contador_hijos;
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
  }

  solucion_generada = poblacion[mejor_individuo];

}

// Función que realiza los cálculos pertinentes con AGGCA y muestra los porcentajes obtenidos
void evaluarAGGCA(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AGGCA(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double porcentaje = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGGCA train en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;

  t1 = chrono::high_resolution_clock::now();
  AGGCA(test, etiquetas_test, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(test, etiquetas_test, train, pesos_calculados, etiquetas_calculadas, false);
  porcentaje = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGGCA test en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;
}

// Algoritmo genético estacionario con cruce aritmético
void AGECA(vector<vector<double> >& train, vector<int>& etiquetas_train, vector<double>& solucion_generada, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
  solucion_generada.resize(train[0].size());

  vector<vector<double> > poblacion(30);
  vector<double> aux_poblacion(train[0].size());
  vector<int> etiquetas_calculadas;
  vector<double> porcentajes_poblacion(poblacion.size());
  double porcentaje_actual;
  int evaluaciones = 0;

  for (int i=0; i < 30; ++i){
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
  }

  int padre1, padre2;
  vector<double> hijo1, hijo2;
  int indice_peor_porcentaje;
  double peor_porcentaje, porcentaje_hijo1, porcentaje_hijo2;
  int genes_generados = 0, mejor_solucion;
  int mutaciones, indice_mutado;

  while (evaluaciones < 15000){
    padre1 = torneoBinario(porcentajes_poblacion);
    padre2 = torneoBinario(porcentajes_poblacion);
    cruceAritmetico(poblacion[padre1], poblacion[padre2], hijo1);

    padre1 = torneoBinario(porcentajes_poblacion);
    padre2 = torneoBinario(porcentajes_poblacion);
    cruceAritmetico(poblacion[padre1], poblacion[padre2], hijo2);

    kNN(train, etiquetas_train, train, hijo1, etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_hijo1 = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
    kNN(train, etiquetas_train, train, hijo2, etiquetas_calculadas, true);
    ++evaluaciones;
    porcentaje_hijo2 = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);

    indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
    peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];

    if (peor_porcentaje < max(porcentaje_hijo1, porcentaje_hijo2)){
      if (porcentaje_hijo1 > porcentaje_hijo2){
        poblacion[indice_peor_porcentaje] = hijo1;
        porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo1;

        indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
        peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];
        if (peor_porcentaje < porcentaje_hijo2){
          poblacion[indice_peor_porcentaje] = hijo2;
          porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo2;
        }
      }
      else{
        poblacion[indice_peor_porcentaje] = hijo2;
        porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo2;

        indice_peor_porcentaje = distance(porcentajes_poblacion.begin(), min_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
        peor_porcentaje = porcentajes_poblacion[indice_peor_porcentaje];
        if (peor_porcentaje < porcentaje_hijo1){
          poblacion[indice_peor_porcentaje] = hijo1;
          porcentajes_poblacion[indice_peor_porcentaje] = porcentaje_hijo1;
        }
      }
    }

    genes_generados += 2*poblacion[0].size();
    mutaciones = genes_generados%1000;
    genes_generados -= mutaciones*1000;

    for (int i=0; i < mutaciones; ++i){
      indice_mutado = rand()%poblacion.size();
      mutarCromosoma(poblacion[indice_mutado], distribucion_normal, generador);
    }
  }

  mejor_solucion = distance(porcentajes_poblacion.begin(), max_element(porcentajes_poblacion.begin(), porcentajes_poblacion.end()));
  solucion_generada = poblacion[mejor_solucion];
}

// Función que realiza los cálculos pertinentes con AGECA y muestra los porcentajes obtenidos
void evaluarAGECA(int bd_elegida, int particion, normal_distribution<double>& distribucion_normal, default_random_engine& generador){
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
  AGECA(train, etiquetas_train, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(train, etiquetas_train, test, pesos_calculados, etiquetas_calculadas, false);
  double porcentaje = porcentajeEtiquetas(etiquetas_test, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGECA train en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;

  t1 = chrono::high_resolution_clock::now();
  AGECA(test, etiquetas_test, pesos_calculados, distribucion_normal, generador);
  t2 = chrono::high_resolution_clock::now();
  kNN(test, etiquetas_test, train, pesos_calculados, etiquetas_calculadas, false);
  porcentaje = porcentajeEtiquetas(etiquetas_train, etiquetas_calculadas);
  transcurrido = chrono::duration_cast<chrono::duration<double> >(t2 - t1);

  cout << "% AGECA test en " << nombre_bd << " para partición " << particion << ": " << porcentaje << endl;
  cout << "\tTiempo: " << transcurrido.count() << endl;
}
