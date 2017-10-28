#include "graphreaderw.hh"
#include "stats.hh"
#include <cassert>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>
#include <math.h>
#include <time.h>
#include <vector>
#include "thread_pool.hh"

using namespace std;
using MatD = vector<map<int,int>>;

void printMat( MatD &m) {
  for (int i = 0; i < m.size(); i++) {
    for (auto j = m[i].begin(); j != m[i].end(); ++j){
        cout << i <<","<< j->first <<" : "<<  j->second << "  " ;
      }
    cout<<endl;
    }
}

void dot (MatD &res , MatD &m1 ,int a){
  int l = m1.size(); 
  for (int b = a; b < l; b++){
    auto c = m1[a].begin();
    auto d = m1[b].begin();
    while(d != m1[b].end()  && c != m1[a].end()){
        if(d->first == c->first){ 
                if(a == b){
                  res[a][b] = 0;
                }else if(res[a].find(b) != res[a].end() ){
                  res[a][b] =min(res[a][b],d->second + c->second);
                }else{
                  res[a][b] = d->second + c->second;
                }
                ++c;
                ++d;
        }else if((c->first)<(d->first)){
          ++c;
        }else{
          ++d; 
        }
      }

    if(a != b && res[a].find(b) != res[a].end() ){  
      if(a==0 && b == 2){
          printf(" d, c %d %d %d \n ", d->second , c->second , res[a][b]);
        }                
        res[b][a]=res[a][b];
      }
    }
}

void ad0( MatD &m1, MatD &res) {
  int i = m1.size();    // number of rows in m1

  for (int a = 0; a<i; a++){
        map <int,int> col;
        res.push_back(col);
      }

  thread_pool pool;
  for (int a = 0; a < i; a++){
      pool.submit( 
         [&res,&m1,a]() { dot(res,m1,a); });
      //dot(res,m1,a);  
    } 
}




void benchmark( string &fileName , MatD &MDispersa) {
  clock_t start, end;
  readGraph(fileName, MDispersa);
  int times = ceil(log(MDispersa.size()));
  MatD r;
  for (int  i = 0 ; i<times ; i++ ){
    start = clock(); 
    ad0(MDispersa, r);
    end = clock();
    MDispersa = r;
    r.clear();  
  }
  MDispersa.clear();
  printf("The time was: %f\n", ((float)(end - start)) / CLOCKS_PER_SEC); 
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
