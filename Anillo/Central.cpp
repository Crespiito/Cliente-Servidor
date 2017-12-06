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

				// Se agrega a la tabla hash con los datos del siguiente

				M >> llave;
				M >> D_Nodo;
				Dato.setDireccionAnterior(D_Nodo);
				M >> D_Nodo;
				Dato.setIpPropia(D_Nodo);
				M >> D_Nodo;
				Dato.setPuertoPropio(D_Nodo);
				M >> D_Nodo;
				Dato.setPeso(D_Nodo);

				//se toma el peso del siguiente como indice de la FT


				Lista[llave] = Dato;

				string D_Anterior = Dato.getDireccionAnterior();

				//se actualizan los datos de la tabla  hash del antecesor al ingresado

				Lista[Dato.getIpPropia()+Dato.getPuertoPropio()].setDireccionAnterior(llave);

				M >> D_Nodo;
				Lista[D_Anterior].setIpPropia(D_Nodo);
				//se toma la ip del que ingresa para la Ft

				M >> D_Nodo;
				Lista[D_Anterior].setPuertoPropio(D_Nodo);
				//se toma el puerto de la ip de el que ingresa para la Ft

				M >> D_Nodo;
				Lista[D_Anterior].setPeso(D_Nodo);

				string PosFt = D_Nodo;
				Finger node;
				Ft[stoi(PosFt)] =node ;
				Ft[stoi(PosFt)].setIP(Dato.getDireccionAnterior());
				Ft[stoi(PosFt)].setLlave(llave);

				M >> D_Nodo;

				vector<string> Actualizar;

				int Cantidad = log2(stoi(D_Nodo));
				for (auto i = Ft.begin(); i != Ft.end(); ++i)
				{	
					i->second.setIP(Lista[i->second.getLlave()].getDireccionAnterior());
					Actualizar.push_back(i->second.getLlave());
				}
				//Se responde con los valores de la FT segun su peso
				message R;
				R << "Nodo Agregado";
				S_Espera.send(R);

				cout <<" holi "<<endl;

				for (int i = 0; i < Actualizar.size(); ++i)
				{
					S_Envio.connect(Actualizar[i]);
					message Act;
					Act << "Actualizar";
					S_Envio.send(Act);
					S_Envio.receive(Act);
					S_Envio.disconnect(Actualizar[i]);
				}




		}

		if (Accion == "FingerT") {
			message R;
			string D_Nodo;

			M >> D_Nodo;
			int PEntrante = stoi(D_Nodo);

			M >> D_Nodo;
			int Cantidad = log2(stoi(D_Nodo));

			R << Cantidad;

			vector<string> Pesos;

			// se sacan los pesos siguientes

			for (int i = 0; i < Cantidad; ++i)
			{
				M >> D_Nodo;
				Pesos.push_back(D_Nodo);

			}

			bool Check = false;

			for (auto it = Ft.begin() ; it != Ft.end(); ++it){
						if ( PEntrante == it->first){
							Check = true;
						}

						if (Check){
								for (int i = 0; i < Pesos.size(); ++i)
								{
									cout << it->first <<"  " << stoi(Pesos[i])<<endl ;
									if(it->first > stoi(Pesos[i])){
										cout << "Cambio en ciclo 1 "<< it->second.getIp()<<endl;
										R << Pesos[i];
										R << it->second.getIp();
										Pesos.erase(Pesos.begin()+i);
										i = -1;
									}
								}
						}

						if(Pesos.size() == 0){
							break;
						}
			}


			if (Pesos.size() > 0) {
					for (auto it = Ft.begin() ; it != Ft.end(); ++it){
						for (int i = 0; i < Pesos.size(); ++i)
						{
							cout << "ciclo 2 "<< it->first <<"  " << stoi(Pesos[i])<<endl ;
							if(it->first > stoi(Pesos[i])){
								cout << "Cambio en ciclo 1 "<< it->second.getIp()<<endl;
								R << Pesos[i];
								R << it->second.getIp();
								Pesos.erase(Pesos.begin()+i);
								i = -1;
							}
						}
						if(Pesos.size() == 0){
							break;
						}
					}
			}

			if (Pesos.size() > 0){
				auto it = Ft.rbegin();
					int i = 0;
					while(true)
					{
							if(Pesos.size() == 0){
								break;
							}
							cout<<"Ciclo 3"<< it->first <<"  " << stoi(Pesos[i])<<endl ;
							cout << "Cambio en ciclo 1 "<< it->second.getLlave()<<endl;
							R << Pesos[i];
							R << it->second.getLlave();
							Pesos.erase(Pesos.begin());
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
					Ft.erase(stoi(Lista[D_Anterior].getPeso()));
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

			for (auto  a = Ft.begin()  ; a != Ft.end() ; ++a ){
				cout<<"\n###############################"<<endl;
				cout<<"### Peso : " << a->first<<endl;
				cout<<"### Direccion Anterior: "<<a->second.getIp() <<endl;
				cout<<"### Llave : "<<a->second.getLlave()<<endl;
				cout<<"###############################"<<endl;
			}


			message R;
			R << "Listado Realizado";
			S_Espera.send(R);

		}

	}
}
