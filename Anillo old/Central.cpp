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
			return Peso;
		}
		string getDireccionAnterior(){
			return DireccionAnterior;
		}
		void setIpPropia(string n){
			IpPropia = n;
		}
		void setPuertoPropio(string n){
			PuertoPropio = n;
		}
		void setDireccionAnterior(string n){

			DireccionAnterior = n;
		}
		void setPeso(string n){
			Peso = n;
		}
};



int main(){
	context ctx_Espera, ctx_Envio;
	unordered_map<string, Nodo> Lista;
	Nodo Central;
	socket S_Espera(ctx_Espera,socket_type::rep);
	socket S_Envio(ctx_Envio,socket_type::req);

	Central.setIpPropia("tcp://localhost:");
	Central.setPuertoPropio("5500");

	string Direccion  = "tcp://*:" + Central.getPuertoPropio();

	S_Espera.bind(Direccion);
	while (true){
		message M;
		S_Espera.receive(M);
		string Accion;
		M >> Accion;
		cout << "####### Se Obtiene la Accion  "<<Accion<<endl;

		if (Accion == "Ingresar"){
				Nodo Dato;
				string llave, D_Nodo;
				M >> llave;
				M >> D_Nodo;
				Dato.setDireccionAnterior(D_Nodo);
				M >> D_Nodo;
				Dato.setIpPropia(D_Nodo);
				M >> D_Nodo;
				Dato.setPuertoPropio(D_Nodo);
				M >> D_Nodo;
				Dato.setPeso(D_Nodo);

				
				Lista[llave] = Dato;
				string D_Anterior = Dato.getDireccionAnterior();

				Lista[Dato.getIpPropia()+Dato.getPuertoPropio()].setDireccionAnterior(llave);

				M >> D_Nodo;
				Lista[D_Anterior].setIpPropia(D_Nodo);
				M >> D_Nodo;
				Lista[D_Anterior].setPuertoPropio(D_Nodo);
				M >> D_Nodo;
				Lista[D_Anterior].setPeso(D_Nodo);
				
				message R;
	
				R << "Nodo Agregado";
				S_Espera.send(R);
		}
		if (Accion == "Retirar"){
				string llave;
				Nodo Nodo_Retirar;
				M >> llave;
				
				if (Lista.find(llave) != Lista.end()){

					Nodo_Retirar = Lista[llave];
					string D_Anterior(Nodo_Retirar.getDireccionAnterior());
					
					S_Envio.connect(D_Anterior);
					
					message Cambio;
					Cambio << "Cambio";
					Cambio << Nodo_Retirar.getIpPropia();
					Cambio << Nodo_Retirar.getPuertoPropio();
					Cambio << Nodo_Retirar.getPeso();
					S_Envio.send(Cambio);
					message R_Cambio;
					S_Envio.receive(R_Cambio);
					string message_R;
					R_Cambio >> message_R;
					cout << message_R;
					S_Envio.disconnect(D_Anterior);
					
					Lista.erase(llave);
					Lista[D_Anterior].setIpPropia(Nodo_Retirar.getIpPropia());
					Lista[D_Anterior].setPuertoPropio(Nodo_Retirar.getPuertoPropio());
					Lista[D_Anterior].setPeso(Nodo_Retirar.getPeso());

					Lista[Nodo_Retirar.getIpPropia()+Nodo_Retirar.getPuertoPropio()].setDireccionAnterior(D_Anterior);
					
					message R;
					R << "Cambio Realizado";
					S_Espera.send(R);

				}else{
					message R;
					R << "No Perteneces al Anillo";
					S_Espera.send(R);
				}
		}

		if (Accion == "Listar"){
			for (auto  a = Lista.begin()  ; a != Lista.end() ; ++a ){
				cout<<"\n###############################"<<endl;
				cout<<"### Llave : " << a->first<<endl; 
				cout<<"### Direccion Anterior: "<<a->second.getDireccionAnterior() <<endl;
				cout<<"### IpSiguiente: "<<a->second.getIpPropia() <<endl;
				cout<<"### Puerto Siguiente "<<a->second.getPuertoPropio() <<endl;
				cout<<"### PesoSiuente: " <<a->second.getPeso()<<endl;
				cout<<"###############################"<<endl;
			}
			message R;
			R << "Listado Realizado";
			S_Espera.send(R);

		}

	}
}

