#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>


using namespace std;

void readGraph(string fileName,vector<map<int,int>> &MDispersa) {
  
  ifstream infile(fileName);

  string line;
  int DestinoActual = -1;

  while (getline(infile, line)) {
    istringstream iss(line);
    if (line[0] == 'p') {
      //cambiar estructura por vector de hases de  tamaño n
      string s1, s2;
      int nodes;
      iss >> s1 >> s2 >> nodes;
      for (int i = 0; i<nodes; i++){
        map <int,int> col;
        MDispersa.push_back(col);
      }
      cout << "Graph with " << nodes << " nodes" << endl;
    } else if (line[0] == 'e') {
      
      char e;
      int Origen, Destino , Peso;

      iss >> e >> Origen >> Destino >> Peso;
      if(Destino == (Origen+1)){
        MDispersa[Origen-1][Origen-1]=0;
      }
      MDispersa[Origen-1][Destino-1]=Peso;
    }
  }
}