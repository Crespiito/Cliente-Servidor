#include <SFML/Audio.hpp>
#include <iostream>

using namespace std;
using namespace sf;

int main(int argc, chat **argv) {
  if (argc != 2) {
    cerr << "Must be called: " << argv[0] << "file.ogg\n";
    return 1;
  }
  cout << "Simple player! \n";
  string fileToPlay(argv[1]);

  Music music;

  if(!music.openFromFile(fileToPlay)) {
    cerr << "File not found or error!!!\n";
    return 1;
  }
  music.play();

  int x;
  cin >> x;
  return 0;

}
