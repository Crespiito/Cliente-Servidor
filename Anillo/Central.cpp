#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <set>
#include <unordered_map>
#include <map>
#include <random>
#include <zmqpp/zmqpp.hpp>

using namespace std;
using namespace zmqpp;


class Finger{
	private:
		string IP;
		string Puerto;
		String Peso;
	public:
		String getIp(){
			return IP;
		}

		String getPuerto(){
			return Puerto;
		}

		String getPeso(){
			return Peso;
		}

		void setIP(String Ip){
			IP = Ip;
		}

		void setPuerto(String puerto){
			Puerto = puerto;
		}

		void setPeso(String peso){
			Peso = peso;
		}

}


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
	map<int, Finger> Ft;
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

				// Se agrega a la tabla hash con los datos del sigiente

				M >> llave;
				M >> D_Nodo;
				Dato.setDireccionAnterior(D_Nodo);
				M >> D_Nodo;
				Dato.setIpPropia(D_Nodo);
				M >> D_Nodo;
				Dato.setPuertoPropio(D_Nodo);
				M >> D_Nodo;
				Dato.setPeso(D_Nodo);

				//se toa el peso del seiguiente como indice de la FT


				Lista[llave] = Dato;

				string D_Anterior = Dato.getDireccionAnterior();

				//se actualizan los datos de la tabla  hash del antesesor al ingresado

				Lista[Dato.getIpPropia()+Dato.getPuertoPropio()].setDireccionAnterior(llave);

				M >> D_Nodo;
				Lista[D_Anterior].setIpPropia(D_Nodo);
				//se toma la ip del que ingresa para la Ft

				M >> D_Nodo;
				Lista[D_Anterior].setPuertoPropio(D_Nodo);
				//se toma el pierto de la ip de el que ingresa para la Ft

				M >> D_Nodo;
				Lista[D_Anterior].setPeso(D_Nodo);

				String PosFt = D_Nodo;
				Ft[stoi(PosFt)] = Finger nodo;
				Ft[stoi(PosFt)].setIP(Dato.getDireccionAnterior());


				//Se responde con los valores de la FT segun su peso
				message R;
				R << "Nodo Agregado";
				S_Espera.send(R);
		}

		if (Accion == "FingerT") {
			message R;

			M >> D_Nodo;
			int PEntrante = stoi(D_Nodo);

			M >> D_Nodo;
			int Cantidad = log2(stoi(D_Nodo))

			R << Cantidad;

			vector<string> Pesos;

			// se sacan los pesos siguientes

			for (int i = 0; i < Cantidad; ++i)
			{
				M >> D_Nodo;
				Pesos.push_back(D_Nodo);
			}

			bool Check = false;

			for (auto it = Ft.begin() ; it != <Ft.end(); ++it){
						if ( PEntrante == it->first){
							Check = true;
						}

						if (Check){
								for (int i = 0; i < Pesos.size(); ++i)
								{
									if(it->first > stoi(Pesos[i])){
										R << Pesos[i];
										R << it->second.getIp();
										Pesos.erase(i);
										i = 0;
									}
								}
						}

						if(Pesos.size() == 0){
							break;
						}
			}

			if (Pesos.size() > 0) {
					for (auto it = Ft.begin() ; it != <Ft.end(); ++it){
						for (int i = 0; i < Pesos.size(); ++i)
						{
							if(it->first > stoi(Pesos[i])){
								R << Pesos[i];
								R << it->second.getIp();
								Pesos.erase(i);
								i = 0;
							}
						}
						if(Pesos.size() == 0){
							break;
						}
					}
			}

			if (Pesos.size() > 0){
				auto it = Ft.begin();
					for (int i = 0; i < Pesos.size(); ++i)
					{
							R << Pesos[i];
							R << it->second.getIp();
							Pesos.erase(i);
							i = 0;
					}
					if(Pesos.size() == 0){
						break;
					}
			}

			R << "Ft agregada";


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
					Ft.erase(stoi(Lista[D_Anterior].getPeso()))
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
