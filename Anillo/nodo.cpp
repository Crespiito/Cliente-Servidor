#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <set>
#include <random>
#include <zmqpp/zmqpp.hpp>
#include <math.h>

using namespace std;
using namespace zmqpp;

string Tam_Anillo = "99";




class Finger{
	private:
		string IP;
		string Llave;
		string Peso;
	public:
		string getIp(){
			return IP;
		}

		string getLlave(){
			return Llave;
		}

		string getPeso(){
			return Peso;
		}

		void setIP(string Ip){
			IP = Ip;
		}

		void setLlave(string llave){
			Llave = llave;
		}

		void setPeso(string peso){
			Peso = peso;
		}

};

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

		vector<Finger> FingerT;

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

		cout <<"###### Esperando Ingreso  "<<endl;
		s.receive(m);
		cout <<"###### Mensaje Entrante"<<endl;
		string Accion;
		m >> Accion;

		if (Accion == "Actualizar"){
			message R;
			R<<"Ok";
			s.send(R);

			context ctx;

			socket S_Central(ctx,socket_type::req);

			string DireccionCentral =  n.getDireccionCentral();

			S_Central.connect(DireccionCentral);

			message P_FinTable , P_Central;

			n.FingerT.clear();

			P_FinTable << "FingerT";

			int Tama = log2(stoi(Tam_Anillo));

			P_FinTable  << n.getPeso();
			P_FinTable << Tam_Anillo;

			for (int f=0; f< Tama;f++){
				int PesoFt = stoi(n.getPeso()) + pow(2,f);
				if (PesoFt > stoi(Tam_Anillo)) {
					PesoFt = log(PesoFt)/log(stoi(Tam_Anillo));
				}
				P_FinTable << to_string(PesoFt);
				Finger Fing;
				Fing.setPeso(to_string(PesoFt));
				n.FingerT.push_back(Fing);
			}

			S_Central.send(P_FinTable);
			S_Central.receive(P_Central);

			int  Cantidad;
			string Peso_FT , IP_FT;
			P_Central >> Cantidad;

			for (int i = 0; i < Cantidad; ++i)
			{
				P_Central >> Peso_FT;
				P_Central >> IP_FT;
				for (int j = 0; j <n.FingerT.size(); ++j)
				{
					if (n.FingerT[j].getPeso() == Peso_FT )
					{
						n.FingerT[j].setIP(IP_FT);
						n.FingerT[j].setLlave(n.getIpPropia() + n.getPuertoPropio());
					}
				}
			}

			string mensaje_C;
			P_Central >> mensaje_C;
			cout <<endl<<  mensaje_C <<endl<<endl;
			S_Central.disconnect(DireccionCentral);
		}

		if (Accion == "Ingresar"){

			Nodo Entrante;
			message R;

			message_Nodo(m,Entrante);
			int PesoEntrante = stoi(Entrante.getPeso());
			int PesoPropio = stoi(n.getPeso());
			int PesoSiguiente = stoi(n.getPesoSiguiente());

			// bool entranteDiferenteActual = (Entrante.getPeso() != n.getPeso());
			// bool entranteDiferenteSiguiente = (Entrante.getPeso() != n.getPesoSiguiente());
			// cout <<"\nPesos"<<Entrante.getPeso() <<" "<<n.getPeso()<<" "<< n.getPesoSiguiente()<<endl;

			// if (entranteDiferenteActual && entranteDiferenteSiguiente) {
				if ((PesoEntrante != PesoSiguiente) && (PesoEntrante != PesoPropio)) {

					if((PesoEntrante > PesoPropio) && (PesoEntrante < PesoSiguiente)) {

						R << "Ok";
						nodo_Message_Siguientes(R,n);
						s.send(R);

						context ctx;

						socket S_Central(ctx,socket_type::req);

						string DireccionCentral =  n.getDireccionCentral();

						S_Central.connect(DireccionCentral);

						message P_Central;

						// Modificadores del valor actual

						P_Central << "Ingresar";
						P_Central << Entrante.getIpPropia() + Entrante.getPuertoPropio();
						P_Central << n.getIpPropia() + n.getPuertoPropio();
						P_Central << n.getIpSiguiente();
						P_Central << n.getPuertoSiguiente();
						P_Central << n.getPesoSiguiente();


						//Modificadores del valor anterior

						P_Central << Entrante.getIpPropia();
						P_Central << Entrante.getPuertoPropio();
						P_Central << Entrante.getPeso();

						P_Central << Tam_Anillo;

						S_Central.send(P_Central);
						S_Central.receive(P_Central);

						cout <<" holi 1"<<endl;

						string mensaje_C;
						P_Central >> mensaje_C;
						cout << mensaje_C;
						S_Central.disconnect(DireccionCentral);

						cout << "Termino La Coneccion con el Central"<<endl;

						n.setIpSiguiente(Entrante.getIpPropia());
						n.setPuertoSiguiente(Entrante.getPuertoPropio());
						n.setPesoSiguiente(Entrante.getPeso());
						//FingerT
					}else{
				        R << "No";
				        cout<< "no";
						vector<Finger>::iterator it;
						for (it = n.FingerT.begin() ; it != n.FingerT.end(); ++it){
							if(PesoEntrante < stoi(it->getPeso())){
								R << it->getIp();
								break;
							}
						}

						if ( it == n.FingerT.end()){
							R << n.FingerT.back().getIp();
						}

						s.send(R);
					}
				}else{
					R << "PesoIgual";
					s.send(R);
				}
			}
			// 	else{
			//
			// 	R << "PesoIgual";
			// 	if (!entranteDiferenteActual){
			// 		R << "El Peso Entrante = Actual";
			// 	}else{
			// 		R << "El Peso Entrante = Siguiente";
			// 	}
			// 	s.send(R);
			// }


		if (Accion == "Cambio"){

			cout <<"#### Realizando Cambio"<<endl;

			string Dato;
			m >> Dato;
			n.setIpSiguiente(Dato);
			m >> Dato;
			n.setPuertoSiguiente(Dato);
			m >> Dato;
			n.setPesoSiguiente(Dato);
			message message_R;

			message_R << "#### Cambio Realizado";
			s.send(message_R);

		}
	}

}

//http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(1,(stoi(Tam_Anillo)-1));

string genId(){

	string temp = to_string(dis(gen));
	return temp;

}

int main(int argc, char const *argv[]){

	Nodo Cliente;

	context ctx_Espera;
	context ctx_Envio;

	socket S_Espera(ctx_Espera,socket_type::rep);
	socket S_Envio(ctx_Envio,socket_type::req);


	if (argc <= 4){

		string Tipo(argv[1]);

		// ./nodo.out C 4201 tcp://localhost:5500

		if (Tipo == "C"){
			string puerto(argv[2]);
			//string peso(argv[3]); //Modificar el arg
			string DCentral(argv[3]);
			Cliente.setIpPropia("tcp://localhost:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setPeso(genId());
			Cliente.setDireccionCentral(DCentral);


		}

		// ./nodo.out M 4800 tcp://localhost:5500

		if (Tipo == "M"){
			string puerto(argv[2]);
			string DCentral(argv[3]);
			Cliente.setPeso("0");  // organizar por random o shawan
			Cliente.setIpPropia("tcp://localhost:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setIpSiguiente("tcp://localhost:");
			Cliente.setPuertoSiguiente(puerto);
			Cliente.setPesoSiguiente(Tam_Anillo); // organizar por valor por defecto
			Cliente.setDireccionCentral(DCentral);

			S_Envio.connect(DCentral);

			message P_Central;

			P_Central << "Ingresar";
			//llave
			P_Central << Cliente.getIpPropia() + Cliente.getPuertoPropio();
			 //Direccion del Anterioir
			P_Central << Cliente.getIpPropia() + Cliente.getPuertoPropio();

			 //informacion para el siguiente nodo

			P_Central << Cliente.getIpSiguiente();
			P_Central << Cliente.getPuertoSiguiente();
			P_Central << Cliente.getPesoSiguiente();

			 //informacion para el nodo anterioir

			P_Central << Cliente.getIpPropia();
			P_Central << Cliente.getPuertoPropio();
			P_Central << Cliente.getPeso();

			P_Central << Tam_Anillo;



			S_Envio.send(P_Central);
			S_Envio.receive(P_Central);
			string mensaje_C;
			P_Central >> mensaje_C;
			cout <<endl<<  mensaje_C <<endl<<endl;

			//solicitar Ft

			//Creacion de la Finger Tb sin direcciones
/*
			message P_FinTable;

			P_FinTable << "FingerT";

			int Tama = log2(stoi(Tam_Anillo));

			P_FinTable  << Cliente.getPeso();
			P_FinTable << Tam_Anillo;

			for (int f=0; f< Tama;f++){
				int PesoFt = stoi(Cliente.getPeso()) + pow(2,f);
				if (PesoFt > stoi(Tam_Anillo)) {
					PesoFt = log(PesoFt)/log(stoi(Tam_Anillo));
				}
				P_FinTable << to_string(PesoFt);
				Finger Fing;
				Fing.setPeso(to_string(PesoFt));
				Cliente.FingerT.push_back(Fing);
			}

			S_Envio.send(P_FinTable);
			S_Envio.receive(P_Central);

			int  Cantidad;
			string Peso_FT , IP_FT;
			P_Central >> Cantidad;

			for (int i = 0; i < Cantidad; ++i)
			{
				P_Central >> Peso_FT;
				P_Central >> IP_FT;
				for (int j = 0; j <Cliente.FingerT.size(); ++j)
				{
					if (Cliente.FingerT[j].getPeso() == Peso_FT )
					{
						Cliente.FingerT[j].setIP(IP_FT);
					}
				}
			}
			P_Central >> mensaje_C;
			cout <<endl<<  mensaje_C <<endl<<endl;
			*/
			S_Envio.disconnect(DCentral);
		}
	}


	thread t_Escucha;

	t_Escucha = thread(Esperando,ref(S_Espera),ref(Cliente));

	while (true){

		int opcion;
		cout << endl << "########### Menu ###########" << endl;  // Organizar menu 1 ingresar 2 retirarce 3 imprimir hash
		cout << "1. Ingresar" << endl;
		cout << "2. Retirar" << endl;
		cout << "3. Imprimir hash" << endl;
		cout << "Digite alguna opcion (1,2,3) : ";
		cin >> opcion ;


		if (opcion == 1){

			cout << endl << " Ha seleccionado la opcion 1 (Ingresar). " <<endl;

			bool ubicado = true;
			string Direccion_P;

			cout << " Ingresar IP: tcp://localhost: "<<endl;
			cin >> Direccion_P;
			Cliente.setIpSiguiente(Direccion_P);
			cout << "ingresar puerto: "<<endl;
			cin >> Direccion_P;
			Cliente.setPuertoSiguiente(Direccion_P);
			//cout <<getIpSiguiente<<endl;

			int i =1 ;
			while(ubicado){
				cout<< "Nodos Recorridos = " << i <<endl ;
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
				if (Accion == "PesoIgual") {
					// string Dato;
					// R >> Dato;
					cout << "Error de Pesos" <<endl;
					// cout << Dato <<endl;
					cout << "No se pudo ubicar el nodo" <<endl;
					ubicado = false;
				}
				if ( Accion == "No"){
					string Dato,temp , Sub_ip ,Sub_puerto;
					R >> Dato;
					temp = Dato.substr(5);
					size_t pos = temp.find(":");
					Sub_ip = Dato.substr(0,pos+6);
					Sub_puerto = Dato.substr(pos+6);

					Cliente.setIpSiguiente(Sub_ip);
					Cliente.setPuertoSiguiente(Sub_puerto);

					cout << Sub_ip << Sub_puerto<<endl;
				}

				S_Envio.disconnect(Direccion);
				i++;

			}

		}

		if (opcion == 2){

			cout << endl << " Ha seleccionado la opcion 2 (Retirar). " <<endl;

			string Direccion_C;
			Direccion_C = Cliente.getDireccionCentral();

			S_Envio.connect(Direccion_C);
			message C;

			C << "Retirar";
			C << Cliente.getIpPropia() + Cliente.getPuertoPropio();

			S_Envio.send(C);
			message R;
			S_Envio.receive(R);

			string message_R;
			R >> message_R;
			cout << message_R <<endl;
			S_Envio.disconnect(Direccion_C);

		}

		if (opcion == 3){

			cout << endl << " Ha seleccionado la opcion 3 (Listar Hash). " <<endl;
	
			for (int i = 0; i < Cliente.FingerT.size(); ++i)
			{
			 	cout << Cliente.FingerT[i].getPeso()<<": : " << Cliente.FingerT[i].getIp()<<": : :" << Cliente.FingerT[i].getLlave()<<endl;
			}
			string Direccion_C;
			Direccion_C = Cliente.getDireccionCentral();

			S_Envio.connect(Direccion_C);
			message C;
			C << "Listar";
			S_Envio.send(C);

			message R;
			S_Envio.receive(R);
			string message_R;
			R >> message_R;
			cout << message_R <<endl;
			S_Envio.disconnect(Direccion_C);

		}
	}
}
