#include <iostream>
#include <fstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace zmqpp;
using namespace sf;

void messageToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 1);
	size_t size = msg.size(1);

	ofstream ofs(fileName, ios::binary);
	ofs.write((char*)data, size);
}

int main(int argc, char** argv) {
	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);
	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://localhost:5555");

	Music music;

	cout << "Sending  some work!\n";

	message m;
	string operation(argv[1]);

	m << operation;

	if(argc == 3) {
		string file(argv[2]);
		m << file;
	}

	s.send(m);

	message answer;
	s.receive(answer);

	string result;
	answer >> result;
	if (result == "list") {
		int numSongs;
		answer >> numSongs;
		cout << "Available songs: " << numSongs << endl;
		for(int i = 0; i < numSongs; i++) {
			string s;
			answer >> s;
			cout << s << endl;
		}

	} else if (result == "file"){
		messageToFile(answer,"song.ogg");
		music.openFromFile("song.ogg");
		music.play();
		int x;
		cin >> x;
	} else {
		cout << "Don't know what to do!!!" << endl;
	}

	return 0;
}
