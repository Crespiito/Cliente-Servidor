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
		
		cout <<"###### Esperando Coneccion  "<<endl; 
		s.receive(m);
		string Accion;
		m >> Accion;


		if (Accion == "Ingresar"){
			Nodo Entrante;

			message_Nodo(m,Entrante);
			int PesoEntrante = stoi(Entrante.getPeso());
			int PesoPropio = stoi(n.getPeso());
			int PesoSiguiente = stoi(n.getPesoSiguiente());
			cout <<"\n####### Pesos"<<Entrante.getPeso() <<"  "<< n.getPeso() <<"  "<< n.getPesoSiguiente()<<endl;

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
				P_Central << Entrante.getIpPropia() + Entrante.getPuertoPropio();
				cout <<"###### hola  1#"<< Entrante.getIpPropia() + Entrante.getPuertoPropio()<<endl;
				P_Central << n.getIpPropia() + n.getPuertoPropio();
				cout <<"###### hola  2"<< n.getIpPropia() + n.getPuertoPropio()<<endl;
				P_Central << n.getIpSiguiente();
				cout <<"###### hola  3"<<n.getIpSiguiente()<<endl;
				P_Central << n.getPuertoSiguiente();
				cout <<"###### hola  4"<<n.getPuertoSiguiente()<<endl;
				P_Central << n.getPesoSiguiente();
				cout <<"###### hola  5"<<n.getPesoSiguiente()<<endl;
				P_Central << n.getIpPropia();
				cout <<"###### hola  seis"<< Entrante.getIpPropia()<<endl;
				P_Central << n.getPuertoPropio();
				cout <<"###### hola  siente"<< Entrante.getPuertoPropio()<<endl;
				P_Central << n.getPeso();
				cout <<"###### hola  8"<< Entrante.getPeso()<<endl;

				
				S_Central.send(P_Central);
				S_Central.receive(P_Central);
				string mensaje_C;
				P_Central >> mensaje_C;
				cout << mensaje_C;
				S_Central.disconnect(DireccionCentral);

				cout << "Termino La Coneccion con el Central"<<endl;

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

		if (Accion == "Cambio"){

			cout <<"#### Realizado Cambio"<<endl;

			string Dato;
			m >> Dato; 
			n.setIpSiguiente(Dato);
			m >> Dato;
			n.setPuertoSiguiente(Dato);
			m >> Dato;
			n.setPesoSiguiente(Dato);
			message message_R;

			cout <<"###### Cambio Realizado"<<endl;
			message_R << "Cambio Realizado";
			s.send(message_R);

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

	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dis(1,1000);
	Nodo Cliente;

	context ctx_Espera;
	context ctx_Envio;

	socket S_Espera(ctx_Espera,socket_type::rep);
	socket S_Envio(ctx_Envio,socket_type::req);

	 
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

			S_Envio.connect(DCentral);

			message P_Central;

			P_Central << "Ingresar";
			P_Central << Cliente.getIpPropia() + Cliente.getPuertoPropio();
			P_Central << Cliente.getIpPropia() + Cliente.getPuertoPropio();

			P_Central << Cliente.getIpSiguiente();
			P_Central << Cliente.getPuertoSiguiente();
			P_Central << Cliente.getPesoSiguiente();
			P_Central << Cliente.getIpPropia();
			P_Central << Cliente.getPuertoPropio();
			P_Central << Cliente.getPeso();

			
			S_Envio.send(P_Central);
			S_Envio.receive(P_Central);
			string mensaje_C;
			P_Central >> mensaje_C;
			cout << mensaje_C;
			S_Envio.disconnect(DCentral);
		}
	}


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
				cout<< "i = " << i <<endl ;
				string Direccion = Cliente.getIpSiguiente() + Cliente.getPuertoSiguiente();
				S_Envio.connect(Direccion);

				cout <<"###### hola Direccion"<< Direccion <<endl;

				message M;
				message R;
				M << "Ingresar";
				nodo_Message(M,Cliente);

				cout << "###### Enviandooooooo"<<endl;
				S_Envio.send(M);
				cout << "###### Envio realizado y ahora a receive"<<endl;

				S_Envio.receive(R);

				cout <<"###### recepcion realizada!"<<endl;

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


		if (opcion == 2){
			string Direccion_C;
			Direccion_C = Cliente.getDireccionCentral();

			cout <<" Direccion Central" <<  Direccion_C << endl;

			S_Envio.connect(Direccion_C);
			message C;
			C << "Retirar";
			C << Cliente.getIpPropia() + Cliente.getPuertoPropio();
			S_Envio.send(C);	
			message R; 
			S_Envio.receive(R);
			string message_R;
			R >> message_R;
			cout << message_R;
			S_Envio.disconnect(Direccion_C);

		}
	}



}
