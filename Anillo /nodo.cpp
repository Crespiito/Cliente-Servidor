#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <set>
#include <random>
#include <zmqpp/zmqpp.hpp>
#include <condition_variable>
#include <random>


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


//http://en.cppreference.com/w/cpp/numeric/random/uniform_int_distribution
//Funcion para generar numeros aleatorio con distribucion normal

random_device rd;
mt19937 gen(rd());
uniform_int_distribution<> dis(1,1000);
string generateId(int n){
  //cout<<"////////////////////////////////////////////////////"<<endl<<endl;
  //cout<<"Numero aleatorio-> "<<n<<endl;
  string temp = to_string(n);
  return temp;
}


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

			if( (PesoEntrante > PesoPropio) && (PesoEntrante < PesoSiguiente) ){

				message R;
				R << "Ok";
				nodo_Message_Siguientes(R,n);
				s.send(R);

				//Agregar el nodo a dht Central server
				context ctx;
				socket S_Central(ctx,socket_type::req);
				string DireccionCentral =  n.getDireccionCentral();
				S_Central.connect(DireccionCentral);
				message P_Central;
				//Mensaje de ingreso al Central server (IP,PORT)
				P_Central << "Ingresar";

				P_Central << Entrante.getIpPropia() + Entrante.getPuertoPropio();
				P_Central << n.getIpPropia() + n.getPuertoPropio();
				P_Central << Entrante.getIpSiguiente();
				P_Central << Entrante.getPuertoSiguiente();
				P_Central << Entrante.getPesoSiguiente();

				S_Central.send(P_Central);
				//Mensaje respuesta de ingreso al Central server (IP,PORT)
				S_Central.receive(P_Central);
				string mensaje_C;
				P_Central >> mensaje_C;
				cout << "###########";
				cout << mensaje_C;
				S_Central.close();
				//Actualizar los siguientes del nodo por el cual se accedio (n)
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

int main(int argc, char const *argv[]){

	Nodo Cliente;
	context ctx_Espera;
	context ctx_Envio;

	if (argc <= 5){

		string Tipo(argv[1]);

		//Nodo tipo 'C' se utilizar solo en el caso
		// de que ya existe un Nodo primario 'M'

		if (Tipo == "C"){
			string puerto(argv[2]);
			string peso(argv[3]);
			string DCentral(argv[4]);
			Cliente.setIpPropia("tcp://localhost:");
			Cliente.setPuertoPropio(puerto);
			Cliente.setPeso(peso);
			Cliente.setDireccionCentral(DCentral);
		}
		//Nodo tipo 'M' se utilizar solo en el caso
		// de que ya no exista ningun nodo

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

		socket S_Espera(ctx_Espera,socket_type::rep);
		socket S_Envio(ctx_Envio,socket_type::req);

		//Hilo que actua  como un servidor
		thread t_Escucha;
		t_Escucha = thread(Esperando,ref(S_Espera),ref(Cliente));
	}


	while (true){
		int opcion;
		cout << "--------Menu--------" << endl;
		cout << "Para Ingresar Digite (1) " << endl;
		cout << "Para Retirarse Digite (2) " << endl;
		cout << "Para Descargar un archivo Digite (3) " << endl;
		cin >> opcion;

		if (opcion == 1){

			cout << endl << "Ha seleccionado la opcion Ingresar" << endl;

			bool ubicado = true;
			string Direccion_P;
			cout << "Ingrese su ip : ( tcp://localhost: )" << endl;
			cin >> Direccion_P;
			Cliente.setIpSiguiente(Direccion_P);

			cout << "Ingrese su puerto :" << endl;
			cin >> Direccion_P;
			Cliente.setPuertoSiguiente(Direccion_P);

			int i =0 ;

			while(ubicado){

				cout<< "i = " << i <<endl ;
				//could
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
					cout<<"Entro Al ok"<<endl;
					cout<<"Ingreso el nodo correctamente"<<endl;

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

			cout << endl << "Ha seleccionado la opcion Retirar" << endl;
			bool ubicado = true;
   		message R;
			R << "Retirar";
			R << Cliente.getIpPropia();
			R << Cliente.getPuertoPropio();
			S_Envio.connect(getDireccionCentral());


	}



}
