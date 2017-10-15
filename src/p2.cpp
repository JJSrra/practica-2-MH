#include <iostream>
#include <utilidadesBD.h>
#include <vector>
#include <algoritmos.h>
#include <random>
#include <geneticos.h>
#include <memeticos.h>
#include <evolutivos.h>

using namespace std;

int main(){
  vector<vector<double> > datos;
  bool primeraVez;
  long int semilla;
  bool knn=false, relief=false, localsearch=false, aggblx=false, ils=false, simulatedannealing=false, derand=false, dectobest=false, am10=false, ammejor=false;
  char opcion, opcionPrimeraVez;

  cout << "Para las siguientes opciones ->" << endl
       << "\t0-KNN" << endl
       << "\t1-RELIEF" << endl
       << "\t2-Local Search" << endl
       << "\t3-Simulated Annealing" << endl
       << "\t4-ILS" << endl
       << "\t5-DE Rand" << endl
       << "\t6-DE Current to Best" << endl
       << "\t7-AGGBLX" << endl
       << "\t8-AM 10-0.1mejor" << endl
       << "\tt-Ejecutar todos los algoritmos" << endl
       << "Seleccione cuál debe realizar el programa: ";
  cin >> opcion;
  cout << endl;

  cout << "¿Es la primera vez que ejecuta este programa? Si es así, o quiere volver a restablecer las particiones, indíquelo (s/n): ";
  cin >> opcionPrimeraVez;
  cout << endl;

  if (opcionPrimeraVez == 's' || opcionPrimeraVez == 'S')
    primeraVez = true;

  cout << "Por último, inicialice el valor de la semilla para generar números aleatorios: ";
  cin >> semilla;
  cout << endl << endl;

  srand(semilla);
  default_random_engine generador;
  normal_distribution<double> distribucion_normal(0.0,0.09);
  normal_distribution<double> distribucion_normal_ILS(0.0,0.16);

  if (primeraVez){
    normalizarBD(1);
    normalizarBD(2);
    normalizarBD(3);
    FiveFoldCrossValidation(1);
    FiveFoldCrossValidation(2);
    FiveFoldCrossValidation(3);
  }

  switch (opcion){
    case '0':
      knn = true;
      break;

    case '1':
      relief = true;
      break;

    case '2':
      localsearch = true;
      break;

    case '3':
      simulatedannealing = true;
      break;

    case '4':
      ils = true;
      break;

    case '5':
      derand = true;
      break;

    case '6':
      dectobest = true;
      break;

    case '7':
      aggblx = true;
      break;

    case '8':
      ammejor = true;
      break;

    case 't':
      knn = true;
      relief = true;
      localsearch = true;
      aggblx = true;
      simulatedannealing = true;
      ils = true;
      derand = true;
      dectobest = true;
      ammejor = true;
      break;
  }

  if (knn){
    cout << "KNN";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarkNN(i,j);

      cout << endl;
    }
  }

  if (relief){
    cout << "RELIEF";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarRELIEF(i,j);

      cout << endl;
    }
  }

  if (localsearch){
    cout << "Local Search";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarLocalSearch(i,j,distribucion_normal,generador);

      cout << endl;
    }
  }

  if (simulatedannealing){
    cout << "Simulated Annealing";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarSimulatedAnnealing(i,j,distribucion_normal,generador);

      cout << endl;
    }
  }

  if (ils){
    cout << "ILS";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarILS(i,j,distribucion_normal_ILS,generador);

      cout << endl;
    }
  }

  if (derand){
    cout << "Differential Evolution Rand";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarDERand(i,j);

      cout << endl;
    }
  }

  if (dectobest){
    cout << "Differential Evolution Current to Best";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarDECtoBest(i,j);

      cout << endl;
    }
  }

  if (aggblx){
    cout << "AGGBLX";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarAGGBLX(i,j,distribucion_normal,generador);

      cout << endl;
    }
  }

  if (ammejor){
    cout << "AMmejor";
    for (int i=1; i < 4; ++i){
      cout << endl;
      for (int j=1; j < 6; ++j)
        evaluarAMmejor(i,j,distribucion_normal,generador);

      cout << endl;
    }
  }

}
