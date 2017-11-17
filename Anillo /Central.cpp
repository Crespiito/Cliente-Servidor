#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <set>
#include <unordered_map>
#include <random>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;


class Nodo {
	private:
		string IpPropia;
		string DireccionAnterior;
		string PuertoPropio;
		string Peso;
	public:
		string getIpPropia(){
			return IpPropia;
		}
		string getPuertoPropio(){
			return PuertoPropio;
		}
		string getPeso(){
			return PuertoPropio;
		}
		string getDireccionAnterior(){
			return PuertoPropio;
		}
		void setIpPropia(string n){
			IpPropia = n;
		}
		void setPuertoPropio(string n){
			PuertoPropio = n;
		}
		void setDireccionAnterior(string n){
			IpPropia = n;
		}
		void setPeso(string n){
			IpPropia = n;
		}
};



int main(){
	context ctx;
	unordered_map<string, nodo> Lista;
	Nodo Central;
	socket S_Espera(ctx,socket_type::rep);
	socket S_Envio(ctx,socket_type::req);

	Central.setIpPropia("tcp://localhost:");
	Central.setPuertoPropio("5500");

	string Direccion  = "tcp://*:" + Central.getPuertoPropio();

	S_Espera.bind(Direccion);
	while (true){
		message M;
		S_Espera.receive(M);
		string Accion;
		cout <<"gola new client: "<<Direccion <<endl;
		M >> Accion;

		if (Accion == "Ingresar"){
				Nodo Dato;
				string llave, D_Nodo;
				M >> llave;
				M >> D_Nodo;
				Dato.setDireccionAnterior(D_Nodo);
				M >> D_Nodo;
				Dato.setIpPropia(D_Nodo);
				M >> D_Nodo;
				Dato.setIpPuerto(D_Nodo);
				M >> D_Nodo;
				Dato.setPeso(D_Nodo);

				Lista[llave] = Dato;
				message R;
				R << "Nodo Agregado";
				S_Espera.send(R);
		}
		if (Accion == "Retirar"){
				string llave, Dato;
				M >> llave;
				M >> Dato;
				Lista.erase(llave);
			}

				Lista[llave] = Dato;
				message R;
				R << "Nodo Retirado";
				S_Espera.send(R);
		}


	}

}
