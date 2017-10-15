#include <utilidadesBD.h>

using namespace std;

// Función que lee la base de datos del camino indicado y la carga
// en la matriz de datos pasada por referencia.
void leerBD(vector<vector<double> >& datos, vector<int>& etiquetas, string path_bd_elegida){
  ifstream fe;
  string cadena;
  vector<double> aux;
  datos.clear();
  etiquetas.clear();

  fe.open(path_bd_elegida, ifstream::in);
	if (fe.fail()){
		cerr << "Error al abrir el fichero " << path_bd_elegida << endl;
		exit(-1);
	} else{
    while (!fe.eof()){
			getline(fe,cadena,'\n');
      if (!fe.eof()){
        istringstream iss(cadena);
        aux.clear();
        for(string s; iss >> s;){
          aux.push_back(stof(s));
        }

        datos.push_back(aux);
  		}
    }
	}
	fe.close();

  for (int i=0; i < datos.size(); ++i){
    etiquetas.push_back(datos[i][datos[i].size()-1]);
    datos[i].pop_back();
  }
}

// Función que normaliza la base de datos seleccionada (1 para sonar, 2 para WDBC, 3 para spam) y la mete
// en un fichero para poder ser leída desde la función leerBaseDatos.
void normalizarBD(int bd_elegida){
  ifstream fe;
  string cadena;
  vector<double> aux;
  ofstream fs;
  vector<vector<double> > datos;
  double maximo, minimo;
  string basedatos, bd_norm;
  double label;

  switch(bd_elegida){
    case 1:
      basedatos = "db/sonar_data";
      bd_norm = "db/sonar_norm_data";
      break;

    case 2:
      basedatos = "db/wdbc_data";
      bd_norm = "db/wdbc_norm_data";
      break;

    case 3:
      basedatos = "db/spam_data";
      bd_norm = "db/spam_norm_data";
      break;
  }

  fs.open(bd_norm);
  if(!fs.is_open()){
    cerr << "Error al abrir el fichero de salida " << bd_norm << endl;
    exit(-1);
  }

  fe.open(basedatos, ifstream::in);
	if (fe.fail()){
		cerr << "Error al abrir el fichero " << basedatos << endl;
		exit(-1);
	} else{
    while (!fe.eof()){
			getline(fe,cadena,'\n');
      if(!fe.eof()){
        label = -1;
        istringstream iss(cadena);
        aux.clear();
        for(string s; iss >> s;){
          if (s != "M" && s != "B"){
            if (s == "Mine")
              s = "0";
            else if (s == "Rock")
              s = "1";

            aux.push_back(stof(s));
          }
          else{
            if (s == "M")
              label = 0;
            else if (s == "B")
              label = 1;
          }
        }
        if (label != -1)
          aux.push_back(label);

        datos.push_back(aux);
  		}
    }
	}
  for(int i=0; i < datos[0].size(); ++i){
    maximo = datos[0][i];
    minimo = datos[0][i];
    for (int j=1; j < datos.size(); ++j){
      if (datos[j][i] < minimo)
        minimo = datos[j][i];
      if (datos[j][i] > maximo)
        maximo = datos[j][i];
    }

    for (int j=0; j<datos.size(); ++j){
      datos[j][i] = (datos[j][i] - minimo) / (maximo - minimo);
    }
  }

  for (int i=0; i < datos.size(); ++i){
    for (int j=0; j < datos[0].size()-1; ++j){
      fs << datos[i][j] << " ";
    }
    fs << datos[i][datos[0].size()-1] << endl;
  }

	fe.close();
  fs.close();
}

// Función que genera las particiones para los conjuntos de prueba y entrenamiento
void FiveFoldCrossValidation(int bd_elegida){
  vector<vector<double> > datos;
  vector<int> etiquetas_datos;
  string bd_test, bd_train, part_actual_train, part_actual_test, path_bd_elegida;

  switch(bd_elegida){
    case 1:
      bd_test = "partitions/sonar_test_";
      bd_train = "partitions/sonar_train_";
      path_bd_elegida = "db/sonar_norm_data";
      break;

    case 2:
      bd_test = "partitions/wdbc_test_";
      bd_train = "partitions/wdbc_train_";
      path_bd_elegida = "db/wdbc_norm_data";
      break;

    case 3:
      bd_test = "partitions/spam_test_";
      bd_train = "partitions/spam_train_";
      path_bd_elegida = "db/spam_norm_data";
      break;
  }

  leerBD(datos,etiquetas_datos,path_bd_elegida);
  ofstream fs_train, fs_test;

  vector<int> indices_1, indices_0;
  for (int i=0; i < etiquetas_datos.size(); ++i){
    if (etiquetas_datos[i] == 1)
      indices_1.push_back(i);
    else
      indices_0.push_back(i);
  }

  vector<int> indices_escogidos;
  long int seed = 1031;

  shuffle(indices_1.begin(), indices_1.end(), std::default_random_engine(seed));
  shuffle(indices_0.begin(), indices_0.end(), std::default_random_engine(seed));

  int tam_0 = indices_0.size()/5, tam_1 = indices_1.size()/5;
  vector<vector<int> > test_0(5), test_1(5);

  for (int part=0; part < 5; ++part){
    for (int j=tam_0*part; j < tam_0*(part+1); ++j){
      test_0[part].push_back(indices_0[j]);
    }

    for (int j=tam_1*part; j < tam_1*(part+1); ++j){
      test_1[part].push_back(indices_1[j]);
    }
  }

  for (int i=0; i < indices_0.size()%5; ++i){
    test_0[i].push_back(indices_0[(tam_0*5)+i]);
  }

  int part_insercion, nuevos_0=indices_0.size()%5;
  for (int i=0; i < indices_1.size()%5; ++i){
    part_insercion = (i+nuevos_0)%5;
    test_1[part_insercion].push_back(indices_1[(tam_1*5)+i]);
  }

  vector<bool> indices_train;
  vector<vector<int> > train(5), test(5);

  for (int part=0; part < 5; ++part){
    for (int i=0; i < test_0[part].size(); ++i){
      test[part].push_back(test_0[part][i]);
    }
    for (int i=0; i < test_1[part].size(); ++i){
      test[part].push_back(test_1[part][i]);
    }

    indices_train.clear();
    indices_train.resize(etiquetas_datos.size(), true);

    for (int i=0; i < test[part].size(); ++i){
      indices_train[test[part][i]] = false;
    }

    for (int i=0; i < indices_train.size(); ++i){
      if (indices_train[i] == true)
        train[part].push_back(i);
    }

  }

  for (int part=0; part < 5; ++part){
    part_actual_train = bd_train;
    part_actual_test = bd_test;

    part_actual_train = part_actual_train + to_string(part+1);
    part_actual_test = part_actual_test + to_string(part+1);

    fs_train.open(part_actual_train);
    if(!fs_train.is_open()){
      cerr << "Error al abrir el fichero de salida " << part_actual_train << endl;
      exit(-1);
    }

    fs_test.open(part_actual_test);
    if(!fs_test.is_open()){
      cerr << "Error al abrir el fichero de salida " << part_actual_test << endl;
      exit(-1);
    }

    for (int i=0; i < test[part].size(); ++i){
      for (int j=0; j < datos[test[part][i]].size(); ++j)
        fs_test << datos[test[part][i]][j] << " ";

      fs_test << etiquetas_datos[test[part][i]] << endl;
    }

    for (int i=0; i < train[part].size(); ++i){
      for (int j=0; j < datos[train[part][i]].size(); ++j)
        fs_train << datos[train[part][i]][j] << " ";

      fs_train << etiquetas_datos[train[part][i]] << endl;
    }

    fs_train.close();
    fs_test.close();
  }

}
