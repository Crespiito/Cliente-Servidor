#include "safeQueue.h"
#include <iostream>
#include <fstream>
#include <string>
#include <zmqpp/zmqpp.hpp>
#include <SFML/Audio.hpp>

using namespace std;
using namespace zmqpp;
using namespace sf;


bool chekSong(string Song,SafeQueue<string> &Queue, socket &s){
	message m;
	string operation("add");
	m << operation;
	m << Song;
	s.send(m);
	message answer;
	s.receive(answer);
	string result;
	answer >> result;
	if (result == "Ns"){
		return false;
	}
	return true;
}
void messageAddToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 2);
	size_t size = msg.size(2);
	cout <<size;
	ofstream ofs;
	ofs.open (fileName, std::ofstream::out | std::ofstream::app);
	ofs.write((char*)data, size);
}

void addFile(message& answer,Music *music){
	string result;
	answer >> result;
	if (result == "file"){
			messageAddToFile(answer,"song.ogg");
		} else {
			cout << "Don't know what to do!!!" << endl;
				}
}

void messageToFile(const message& msg, const string& fileName) {
	const void *data;
	msg.get(&data, 2);
	size_t size = msg.size(2);
	cout << size;

	ofstream ofs(fileName, ios::binary);
	ofs.write((char*)data, size);
}

void OpenFile(message& answer,Music *music){
	string result;
	answer >> result;
	if (result == "file"){
			messageToFile(answer,"song.ogg");
			music->openFromFile("song.ogg");
		} else {
			cout << "Don't know what to do!!!" << endl;
				}
}



void ReproducirMusica(Music *music , SafeQueue<string> &Queue , socket &s){
		while(true){	
			string cancion;
			if(Queue.isEmpty()){
				break;
			}	
			cancion = Queue.dequeue();
			int part = 1;
			int n = 10;
			message m;
			m<<"play";
			m<<cancion;
			m<<part;
			s.send(m);
			message answer;
			s.receive(answer);
			OpenFile(answer,music);
			music->play();
			while (music->getStatus() == SoundSource::Playing) {
				if (part>=n){
				continue;
				}else
				message m;
				m<<"play";
				m<<cancion;
				part = part +1;
				m<<part;
				s.send(m);
				message answer;
				s.receive(answer);
				addFile(answer,music);
				int x;
				answer >> x;
				n = x;
				cout << "estas son las partes recibidas :" << part <<endl;
				}
			if(music->getStatus() == SoundSource::Stopped){
				break;
			}
		}
	}




int main() {

	cout << "This is the client\n";

	context ctx;
	socket s(ctx, socket_type::req);
	cout << "Connecting to tcp port 5555\n";
	s.connect("tcp://localhost:5555");

	Music music;
	SafeQueue<string> pl;
	string Orden;
	message m;
	thread t[2];

	while(true){

			cout <<"Seleccione una opcion\n1- list \n2- play \n3- add \n";

			int argc;
			bool Mensaje = false;

			cin >> argc;

			if(argc == 1) {
				string operation("list");
				m << operation;
				Mensaje = true;
			}
			
			else if(argc == 2) {
				string operation("add");
				message l;
				l << operation;
				cout << "Nombre de la cancion:  ";
				string file;
				cin >> file;      
				l << file;
				if (chekSong(file,ref(pl),ref(s))) {
					pl.enqueue(file);
					string play("file");
					m << play;
					m << file;
					Mensaje = true;

				}else{
					cout<<"invalid Song";
				}
			}

			else if(argc == 3) {
				string operation("add");
				m << operation;
				cout << "Nombre de la cancion:  ";
				Mensaje = true;
				string file;
				cin >> file;
				m << file;

			}else if (argc == 4){
				Mensaje = false;
				music.pause();
				if (pl.isEmpty() && t[0].joinable()){
					music.stop();
					t[0].join();
				}
			}

			cout << "Sending  some work!\n";

			string result;
			message answer;
			
			if(Mensaje){	
				s.send(m);
				s.receive(answer);
				answer >> result;
			}else{
				result="pass";	
			}



			cout <<"Result:  "<<result<<endl;

			if (result == "add"){
				string song;
				answer >> song;
				pl.enqueue(song);
			}

			if (result == "list") {
				size_t numSongs;
				answer >> numSongs;
				cout << "Available songs: " << numSongs << endl;
				for(size_t i = 0; i < numSongs; i++) {
					string s;
					answer >> s;
					cout << s << endl;

				}
			}
			
			if (result == "file"){
				cout << " holiii";
				if(t[0].joinable()){
					music.stop();
					t[0].join();
					t[0] = thread(ReproducirMusica,&music,ref(pl),ref(s));
				}else{
				t[0] = thread(ReproducirMusica,&music,ref(pl),ref(s));	
				}
			}
	}

	return 0;
}




