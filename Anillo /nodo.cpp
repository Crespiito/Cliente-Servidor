#include <iostream>
#include <fstream>
#include <string>
#include <assert.h> 
#include <zmqpp/zmqpp.hpp>

using namespace std;

class Nodo {
	public:
		int Peso;
		int PesoSiguente;
		string IpPropia;
		string PuertoPropio;
		string IpSiguiente;
		string PuertoSiguiente
}

void Espera(socket &s , Nodo &n){
	Direccion  = n.IpPropia + n.PuertoPropio;
	s.bind(Direccion);
	while (true){
		message m;
		s.recive(m);

		string Accion;

		m >> Accion;

		if (Accion == "Ingresar"){
			Nodo Entrante; 
			m >> Entrante;
			if(Entrante.Peso > n.Peso && Entrante.Peso < n.PesoSiguente ){
				message R;
				R << "Ok";
				R << n.IpSiguiente;
				R << n.PuertoSiguiente;
				R << n.PesoSiguente;
				s.send(R);
				n.IpSiguiente = Entrante.IpPropia;
				n.PuertoSiguiente = Entrante.PuertoPropio;
				n.PesoSiguente = Entrante.Peso;
				
			}else{
				message R;
				R << "No";
				R << n.IpSiguiente;
				R << n.PuertoSiguiente;
				s.send(R);
			}
		}
	}

}

int main(){

	context Ctx_Espera;
	context Ctx_Envio;

	Nodo Cliente;
	Cliente.Peso = 0;  // organizar por random o shawan 
	Cliente.PesoSiguente = 9999; // organizar por valor por defecto 
	Cliente.IpPropia = "tcp://*:";
	Cliente.Puerto = "5550";

	socket S_Espera(Ctx_Espera,socket_type::rep);
	socket S_Envio(Ctx_Envio,socket_type::req);

	thread t_Escucha;

	t_Escucha = thread(Espera,ref(S_Espera),ref(Cliente));

	while (true){
		int opcion;
		cout << "menu"
		cin >> opcion;

		if (opcion == 1){
			bool ubicado = false;
			cin >> Cliente.IpSiguiente;
			cin >> Cliente.PuertoSiguiente;
			while(ubicado){
				int coneccion ;
				string Direccion = Cliente.IpSiguiente + Cliente.PuertoSiguiente;
				coneccion == S_Envio.conect(Direccion);
				assert(coneccion == 0);



			}

		}
	} 



}