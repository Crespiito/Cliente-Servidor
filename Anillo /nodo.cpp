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
		string DireccionCentral;

	public:
	  	string getPeso(){
			return Peso;
		}
		string getDireccionCentral(){
			return DireccionCentral;
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

		void setDireccionCentral(string n){
			DireccionCentral = n;
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
	n.setIpPropia(Dato);
	m >> Dato;
	n.setPuertoPropio(Dato);
	m >> Dato;
	n.setPeso(Dato);
}

void message_Nodo_Siguientes(message &m, Nodo &n){

	string Dato;
	
	m >> Dato;
	n.setIpSiguiente(Dato);
	m >> Dato;
	n.setPuertoSiguiente(Dato);
	m >> Dato;
	n.setPesoSiguiente(Dato);
}

void nodo_Message_Siguientes(message &m, Nodo &n){
	
	m << n.getIpSiguiente();
	m << n.getPuertoSiguiente();
	m << n.getPesoSiguiente();
}

void nodo_Message(message &m, Nodo &n){
	
	m << n.getIpPropia();
	m << n.getPuertoPropio();
	m << n.getPeso();

}

void Esperando( socket &s , Nodo &n){
	string Direccion  = "tcp://*:" + n.getPuertoPropio();
	
	s.bind(Direccion);
	while (true){
		message m;
		s.receive(m);
		string Accion;
		m >> Accion;

		if (Accion == "Ingresar"){
			Nodo Entrante;

			message_Nodo(m,Entrante);
			int PesoEntrante = stoi(Entrante.getPeso());
			int PesoPropio = stoi(n.getPeso());
			int PesoSiguiente = stoi(n.getPesoSiguiente());
			cout << Entrante.getPeso() <<"  "<< n.getPeso() <<"  "<< n.getPesoSiguiente()<<endl;

			if((PesoEntrante > PesoPropio) && (PesoEntrante < PesoSiguiente) ) {
				
				message R;
				R << "Ok";
				nodo_Message_Siguientes(R,n);
				s.send(R);

				context ctx;

				socket S_Central(ctx,socket_type::req);

				string DireccionCentral =  n.getDireccionCentral();

				S_Central.connect(DireccionCentral);

				message P_Central;

				P_Central << "Ingresar";
				P_Central << Entrante.getIpPropia();
				P_Central << Entrante.getPuertoPropio();

				S_Central.send(P_Central);
				S_Central.receive(P_Central);
				string mensaje_C;
				P_Central >> mensaje_C;
				cout << mensaje_C;
				S_Central.close();

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

	//context ctx;
	Nodo Cliente;

	 
	context ctx_Espera;
	context ctx_Envio;
	if (argc <= 5){
		string Tipo(argv[1]);
		if (Tipo == "C"){
			string puerto(argv[2]);
			string peso(argv[3]);
			string DCentral(argv[4]);
			Cliente.setIpPropia("tcp://localhost:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setPeso(peso);
			Cliente.setDireccionCentral(DCentral);
		}

		if (Tipo == "M"){
			string puerto(argv[2]);
			string DCentral(argv[3]);
			Cliente.setPeso("0");  // organizar por random o shawan
			Cliente.setIpPropia("tcp://localhost:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setIpSiguiente("tcp://localhost:");
			Cliente.setPuertoSiguiente(puerto);			
			Cliente.setPesoSiguiente("9999"); // organizar por valor por defecto
			Cliente.setDireccionCentral(DCentral);
		}
	}


	socket S_Espera(ctx_Espera,socket_type::rep);
	socket S_Envio(ctx_Envio,socket_type::req);

	thread t_Escucha;

	t_Escucha = thread(Esperando,ref(S_Espera),ref(Cliente));

	while (true){
		int opcion;
		cout << "menu";
		cin >> opcion;

		if (opcion == 1){
			bool ubicado = true;
			string Direccion_P;
			cout << " ingresar : tcp://localhost:"<<endl;
			cin >> Direccion_P;
			Cliente.setIpSiguiente(Direccion_P);
			cout << "ingresar : puerto"<<endl;
			cin >> Direccion_P;
			Cliente.setPuertoSiguiente(Direccion_P);
			int i =0 ;
			while(ubicado){
				cout<< "i = " << i <<endl ;could
				string Direccion = Cliente.getIpSiguiente() + Cliente.getPuertoSiguiente();
				S_Envio.connect(Direccion);
				message M;
				message R;
				M << "Ingresar";
				nodo_Message(M,Cliente);

				S_Envio.send(M);
				S_Envio.receive(R);
				string Accion;

				R >> Accion;

				if ( Accion == "Ok"){
					message_Nodo_Siguientes(R,Cliente);
					ubicado = false;
				}
				if ( Accion == "No"){
					cout<<"Entro Al No"<<endl;
					string Dato;
					R >> Dato;
					Cliente.setIpSiguiente(Dato);
					R >> Dato;
					Cliente.setPuertoSiguiente(Dato);
				}
				S_Envio.disconnect(Direccion);
				cout << "paso El disconnect"<<endl;
				i++;

			}

		}
	}



}
