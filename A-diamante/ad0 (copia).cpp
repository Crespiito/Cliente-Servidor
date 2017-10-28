#include "graphreaderw.hh"
#include "stats.hh"
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <math.h>
#include <time.h>
#include <vector>

using namespace std;
using MatD = vector<unordered_map<int,int>>;

void ad0( MatD &m1, MatD &m2, MatD &res) {
  int i = m1.size();    // number of rows in m1
  int j = m1.size(); // number of cols in m1
  int k = m2.size();    // number of rows in m2
  int l = m2.size(); // number of cols in m2

  assert(j == k);

  for (int a = 0; a < i; a++){
    for (int b = 0; b < l; b++){
      auto c = m1[a].begin();
      auto d = 0;
      while(d < i  && c != m1[a].end()){
        if(d == c->first){
              if(res[a].find(b) == res[a].end()){
                res[a][b] = m1[d][b] + c->second;
                ++c;
                d++;
              }else{
                res[a][b] =min(res[a][b],m1[d][b] + c->second);
                ++c;
                d++;
              }
        }else if((c->first)<d){
          c++;
        }else{
          d++; 
        }

      }
    }
  }
}


void printMat( MatD &m) {
  for (int i = 1; i <= m.size(); i++) {
    for (auto j = m[i].begin(); j != m[i].end(); ++j){
        cout << i <<","<< j->first <<" : "<<  j->second << "  " ;
      }
    cout<<endl;
    }
}

void benchmark( string &fileName , MatD &MDispersa) {
  clock_t start, end;
  readGraph(fileName, MDispersa);
  printMat(MDispersa);
  printf("almenos lei el archivo :v\n");
  int times = ceil(log(MDispersa.size()));
  MatD r;
  for (int  i = 0 ; i<times ; i++ ){
    start = clock(); 
    ad0(MDispersa, MDispersa, r);
    end = clock();
    printf("The time was: %f\n", ((float)(end - start)) / CLOCKS_PER_SEC); 
    MDispersa = r;
    printMat(MDispersa);
    r.clear();  

  }
}

int main(int argc, char **argv) {
  if (argc != 2) {
    cerr << "Error!!" << endl;
  }
  string fileName(argv[1]);
  MatD Mat;
  benchmark(fileName,Mat);
  return 0;
}
