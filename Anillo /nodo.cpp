#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <set>
#include <random>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;

class Nodo {
	private:
		string Peso;
		string PesoSiguiente;
		string IpPropia;
		string PuertoPropio;
		string IpSiguiente;
		string PuertoSiguiente;

	public:
	  string getPeso(){
			return Peso;
		}
		string getPesoSiguiente(){
			return PesoSiguiente;
		}
		string getIpPropia(){
			return IpPropia;
		}
		string getPuertoPropio(){
			return PuertoPropio;
		}
		string getIpSiguiente(){
			return IpSiguiente;
		}
		string getPuertoSiguiente(){
			return PuertoSiguiente;
		}

		void setPeso(string n){
			Peso = n;
		}
		void setPesoSiguiente(string n){
			PesoSiguiente = n;
		}
		void setIpPropia(string n){
			IpPropia = n;
		}
		void setPuertoPropio(string n){
			PuertoPropio = n;
		}
		void setIpSiguiente(string n){
			IpSiguiente = n;
		}
		void setPuertoSiguiente(string n){
			PuertoSiguiente = n;
		}
};


void message_Nodo(message &m, Nodo &n){

	string Dato;
	
	m >> Dato;
	n.setIpSiguiente(Dato);
	m >> Dato;
	n.setPuertoSiguiente(Dato);
	m >> Dato;
	n.setPesoSiguiente(Dato);
}

void nodo_Message(message &m, Nodo &n){

	string Dato;
	
	m << n.getIpSiguiente();
	m << n.getPuertoSiguiente();
	m << n.getPesoSiguiente();
}

void Esperando( socket &s , Nodo &n){
	string Direccion  = n.getIpPropia() + n.getPuertoPropio();
	
	s.bind(Direccion);
	while (true){
		message m;
		s.receive(m);
		string Accion;
		m >> Accion;

		if (Accion == "Ingresar"){
			Nodo Entrante;

			message_Nodo(m,Entrante);

			if(stoi(Entrante.getPeso()) > stoi(n.getPeso()) && stoi(Entrante.getPeso()) < stoi(n.getPesoSiguiente())){
				
				message R;
				R << "Ok";
				nodo_Message(R,n);
				s.send(R);
				n.setIpSiguiente(Entrante.getIpPropia());
				n.setPuertoSiguiente(Entrante.getPuertoPropio());
				n.setPesoSiguiente(Entrante.getPeso());

			}else{
				message R;
				R << "No";
				R << n.getIpSiguiente();
				R << n.getPuertoSiguiente();
				s.send(R);
			}
		}
	}

}

// void nodo_Message(message m, Nodo &n) {
// 	m << n.IpSiguiente;
// 	m << n.PuertoSiguiente;
// 	//m << to_string(n.PesoSiguiente);
// 	m << n.PesoSiguiente;
// }
	// int Peso;
	// int PesoSiguente;
	// string IpPropia;
	// string PuertoPropio;
	// string IpSiguiente;
	// string PuertoSiguiente;

// void message_Nodo(message m, Nodo &n) {
// 	m << n.;
// 	int Peso;
// 	int PesoSiguente;
// 	string IpPropia;
// 	string PuertoPropio;
// 	string IpSiguiente;
// 	string PuertoSiguiente;
// }


int main(int argc, char const *argv[]){

	context ctx;
	Nodo Cliente;

	// context Ctx_Espera;
	// context Ctx_Envio;
	if (argc <= 4){
		string Tipo(argv[1]);
		if (Tipo == "C"){
			string puerto(argv[2]);
			string peso(argv[3]);
			Cliente.setIpPropia("tcp://*:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setPeso(peso);
		}

		cout<<"gola"<<endl;
		if (Tipo == "M"){
			string puerto(argv[2]);
			Cliente.setPeso("0");  // organizar por random o shawan
			Cliente.setIpPropia("tcp://*:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setIpSiguiente("tcp://*:");
			Cliente.setPuertoSiguiente(puerto);			
			Cliente.setPesoSiguiente("9999"); // organizar por valor por defecto
		}
	}


	socket S_Espera(ctx,socket_type::rep);
	socket S_Envio(ctx,socket_type::req);

	thread t_Escucha;

	t_Escucha = thread(Esperando,ref(S_Espera),ref(Cliente));

	while (true){
		int opcion;
		cout << "menu";
		cin >> opcion;

		if (opcion == 1){
			bool ubicado = false;
			string Direccion_P;
			cin >> Direccion_P;
			Cliente.setIpSiguiente(Direccion_P);
			cin >> Direccion_P;
			Cliente.setPuertoSiguiente(Direccion_P);
			while(ubicado){

				string Direccion = Cliente.getIpSiguiente() + Cliente.getPuertoSiguiente();
				S_Envio.connect(Direccion);

				message M , R;


				M << "Ingresar";

				nodo_Message(M,Cliente);

				S_Envio.send(M);
				S_Envio.receive(R);
				string Accion;

				R >> Accion;

				if ( Accion == "Ok"){
					message_Nodo(R,Cliente);
					ubicado = true;
				}
				if ( Accion == "No"){
					string Dato;
					R >> Dato;
					Cliente.setIpSiguiente(Dato);
					R >> Dato;
					Cliente.setPuertoSiguiente(Dato);
				}
				S_Envio.disconnect(Direccion);

			}

		}
	}



}
