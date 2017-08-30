#include <zmqpp/zmqpp.hpp>
#include <iostream>
#include <string>
#include <cassert>
#include <unordered_map>
#include <fstream>
#include <dirent.h>


using namespace std;
using namespace zmqpp;

vector<char> readFileToBytes(const string& fileName,int part) {
  int parts;
  ifstream ifs(fileName, ios::binary | ios::ate);
  ifstream::pos_type pos = ifs.tellg();
  ifs.seekg(512000, ios::beg);
  ifstream::pos_type Spart = ifs.tellg();
  parts = pos/512000 + 1;
  cout<<"hasta aqui llego!!  "<<part<<" "<<Spart<<endl;
  if(part<parts){
      vector<char> result(Spart);
      if(part==1){
        ifs.seekg(0, ios::beg);
        ifs.read(result.data(), Spart);      
      }else{
        ifs.seekg((Spart*(part-1)),ios::beg);
        ifs.read(result.data(), Spart);
      }
      return result;
    }      
  else{
  vector<char> result((pos-(Spart*(part-1))));
  ifs.seekg((Spart*(part-1)),ios::beg);
  ifs.read(result.data(), (pos-(Spart*(part-1))));
  return result;
  }
}

void fileToMesage(const string& fileName, message& msg,int part) {

  vector<char> bytes = readFileToBytes(fileName,part);
  msg.add_raw(bytes.data(), bytes.size());
}

string split(string s, char del){
  string nameSong = "";
  for (int i = 0; i < int(s.size()); i++) {
    if (s[i] != del)
      nameSong += s[i];
    else
      break;
   }
   return nameSong;
}


  
unordered_map<string,string> ReadDir(string path , unordered_map<string,string> songs){
  
  DIR *dir;

  struct dirent *ent;

  string file_name,key;

  dir = opendir(path.c_str());

  if (dir == NULL){
      cout<<"no se pudo abrir el directorio";
  }
while ((ent = readdir (dir)) != NULL)
    {
      file_name = ent->d_name;
      if ( file_name.find(".ogg") != string::npos) {
        key = split(file_name,'.');
        songs[key] = path + file_name;
          }
    }
  closedir (dir);
  return songs;

}


int main(int argc, char** argv) {


  context ctx;
  socket s(ctx, socket_type::rep);
  s.bind("tcp://*:5555");

  string path(argv[1]);

  unordered_map<string,string> songs;
  
  songs = ReadDir(path,songs);

  cout << "Start serving requests!\n";
  while(true) {
    message m;
    s.receive(m);

    string op;
    m >> op;

    cout << "----Action:  " << op << endl;
    if (op == "list") {  // Use case 1: Send the songs
      message n;
      n << "list" << songs.size();
      for(const auto& p : songs)
        n << p.first;
      s.send(n);
    } else if(op == "file") {
      message n;
      n << "file";
      s.send(n);

    }
      else if(op == "play") {
      // Use case 2: Send song file
      string songName;
      int part;
      m >> songName;
      cout << "sending song " << songName
           << " at " << songs[songName] << endl;
      m >> part;
      cout << part;
      message n;
      n << "file";
      
      int parts;
      ifstream ifs(songs[songName], ios::binary | ios::ate);
      ifstream::pos_type pos = ifs.tellg();
      parts = pos/512000 + 1;
      cout <<" estas son als partes : "<< parts << endl;
      n << parts;

      fileToMesage(songs[songName], n , part);

      s.send(n);
    } else if(op == "add"){
      string songName;
      m >> songName;
      cout << songName;
      if (( songs.count(songName)) == 1){
        cout << "enqueuing song " << songName;
        message n;
        n << "add";
        n << songName;
        s.send(n);
      }else{
        cout << "Invalid song requested!!\n";
        message n;
        n<<"Ns";
        s.send(n);
      }
    } else {
      cout << "Invalid operation requested!!\n";
      message n;
      n<<"Nop";
      s.send(n);
    }
  }

  cout << "Finished\n";
  return 0;
}