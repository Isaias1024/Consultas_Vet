#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fstream>

using namespace std;

struct VETERINARIO
{
	char nombreVeterinario[100];
	char cedula[100];
	//Foto
	char usuario[100];
	char contrasena[100];
};

struct NODOVET {
	VETERINARIO* veterinario; //datos de veterinario
	NODOVET* anterior; //nodo datos de veterinario anterior
	NODOVET* siguiente; //nodo datos de veterinario despues
};

struct VETERINARIOS {
	NODOVET* origen; //inicio del nodo(lista)
	NODOVET* fin; //final del nodo(lista)
}LISTAVETERINARIOS;

// Definición de la estructura Cita
struct CITA
{
	char nombreCliente[100];
	char nombreMascota[100];
	char motivo[100];
	float costo;
	char fechaHora[100];
	char status[100];
	char cedula[100];
};

// Definición de la estructura NODOCITA para apuntar a una cita

struct NODOCITA {
	CITA* cita;
	NODOCITA* anterior;
	NODOCITA* siguiente;
};

struct CITAS {
	NODOCITA* origen;
	NODOCITA* fin;
}LISTACITAS;

int mainVet();
NODOVET* nuevoNodoVeterinario(VETERINARIO* veterinario);

CITA* crearCita(char nombreCliente[100], char fechaHora[100], char nombreMascota[100], char motivo[100], float costo, char status[100], char cedulaVeterinario[100]) {
	CITA* nuevo = new CITA;
	nuevo->costo = costo;
	strcpy_s(nuevo->fechaHora, fechaHora);
	strcpy_s(nuevo->motivo, motivo);
	strcpy_s(nuevo->nombreCliente, nombreCliente);
	strcpy_s(nuevo->nombreMascota, nombreMascota);
	strcpy_s(nuevo->status, status);
	strcpy_s(nuevo->cedula, cedulaVeterinario);
	return nuevo;
}

NODOCITA* nuevoNodo(CITA* cita) {
	NODOCITA* nodo = new NODOCITA;
	nodo->cita = cita;
	nodo->siguiente = NULL;
	return nodo;
}

NODOCITA* buscarNombre(char* buscarNomCliente) {
	if (LISTACITAS.origen == NULL)
		return NULL;
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (strcmp(indice->cita->nombreCliente, buscarNomCliente) == 0)
			break;
		indice = indice->siguiente;
	}
	return indice;
}

CITA* EliminarCitaInicio() {
	if (LISTACITAS.origen == NULL)
		return NULL;
	if (LISTACITAS.origen == LISTACITAS.fin)
		LISTACITAS.fin = NULL;
	NODOCITA* anterior = LISTACITAS.origen;
	LISTACITAS.origen =
		LISTACITAS.origen->siguiente;
	CITA* cita = anterior->cita;
	delete anterior;
	return cita;
}

CITA* EliminarCitaFinal() {
	if (LISTACITAS.origen == NULL)
		return NULL;
	if (LISTACITAS.origen == LISTACITAS.fin)
		return EliminarCitaInicio();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (indice->siguiente == LISTACITAS.fin) {
			break;
		}
		indice = indice->siguiente;
	}

	CITA* cita = LISTACITAS.fin->cita;
	delete LISTACITAS.fin;
	LISTACITAS.fin = indice;
	if (indice != NULL)
		indice->siguiente = NULL;
	return cita;
}

CITA* EliminarCitaMedio(char* nombre) {
	NODOCITA* busqueda = buscarNombre(nombre);
	if (busqueda == NULL)
		return NULL;
	if (busqueda == LISTACITAS.origen)
		return EliminarCitaInicio();
	else if (busqueda == LISTACITAS.fin)
		return EliminarCitaFinal();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (indice->siguiente == busqueda)
			break;
		indice = indice->siguiente;
	}

	CITA* cita = busqueda->cita;
	if (indice != NULL)
		indice->siguiente = busqueda->siguiente;
	delete busqueda;
	return cita;
}

void AgregarCitaInicio(CITA* cita) {
	NODOCITA* nodo = nuevoNodo(cita);
	if (LISTACITAS.origen == NULL) {
		LISTACITAS.origen = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = NULL;
	}
	else {
		nodo->siguiente = LISTACITAS.origen;
		LISTACITAS.origen = nodo;
	}
}

void agregarCitaFinal(CITA* cita) {
	NODOCITA* nodo = nuevoNodo(cita);
	if (LISTACITAS.origen == NULL) {
		LISTACITAS.origen = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = NULL;
	}
	else {
		LISTACITAS.fin->siguiente = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = NULL;
	}
}

void AgregarCitaMedio(char* buscar, CITA* cita) {
	NODOCITA* busqueda = buscarNombre(buscar);
	if (busqueda == NULL)
		return;
	if (busqueda == LISTACITAS.fin)
		return agregarCitaFinal(cita);
	NODOCITA* nodo = nuevoNodo(cita);
	nodo->siguiente = busqueda->siguiente;
	busqueda->siguiente = nodo;
}

void ImprimirListaCita() {
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		cout << "(" << endl;
		cout << "cliente: " << indice->cita->nombreCliente << endl;
		cout << "Costo: " << indice->cita->costo << endl;
		cout << "Fecha: " << indice->cita->fechaHora << endl;
		cout << "Motivo: " << indice->cita->motivo << endl;
		cout << "Mascota: " << indice->cita->nombreMascota << endl;
		cout << "Estatus: " << indice->cita->status << endl;
		cout << "Veterinario: " << indice->cita->cedula << endl;
		cout << ")" << endl;
		indice = indice->siguiente;
	}
}

void EliminarListaCita() {
	while (LISTACITAS.origen != nullptr) {
		NODOCITA* temporal = LISTACITAS.origen;
		LISTACITAS.origen = LISTACITAS.origen->siguiente;
		if (temporal->cita != nullptr)
			delete temporal->cita;
		delete temporal;
	}
}

void EditarCita(char* nombreCliente) {

	NODOCITA* cita = buscarNombre(nombreCliente);
	if (cita == nullptr) {
		cout << "La cita no se encontró." << endl;
		return;
	}

	// Solicitar al usuario los nuevos valores
	char nuevoNombreMascota[100];
	char nuevaFechaHora[100];
	char nuevoMotivo[100];
	float nuevoCosto;
	char nuevoStatus[100];

	cout << "Nuevo nombre de la mascota: ";
	cin.ignore();
	cin.getline(nuevoNombreMascota, 50);

	cout << "Nueva fecha y hora de la consulta: ";
	cin.getline(nuevaFechaHora, 50);

	cout << "Nuevo motivo de la consulta: ";
	cin.getline(nuevoMotivo, 50);

	cout << "Nuevo costo de la consulta: ";
	cin >> nuevoCosto;

	cout << "Nuevo estatus de la consulta: ";
	cin.ignore();
	cin.getline(nuevoStatus, 50);

	// Actualizar los valores de la cita
	strcpy_s(cita->cita->nombreMascota, nuevoNombreMascota);
	strcpy_s(cita->cita->fechaHora, nuevaFechaHora);
	strcpy_s(cita->cita->motivo, nuevoMotivo);
	cita->cita->costo = nuevoCosto;
	strcpy_s(cita->cita->status, nuevoStatus);

	cout << "Cita editada exitosamente." << endl;
}

void EliminarCitaPorNombre(char* nombreCliente) {
	CITA* citaEliminada = EliminarCitaMedio(nombreCliente);
	if (citaEliminada != nullptr) {
		cout << "Cita eliminada exitosamente." << endl;
		delete citaEliminada;
	}
	else {
		cout << "La cita no se encontro." << endl;
	}
}

VETERINARIO* crearUsuario(char* nombreVeterinario, char* cedula, char* nombreUsuario, char* pswUsuario) {
	VETERINARIO* nuevo = new VETERINARIO;
	strcpy_s(nuevo->nombreVeterinario, nombreVeterinario);
	strcpy_s(nuevo->cedula, cedula);
	strcpy_s(nuevo->usuario, nombreUsuario);
	strcpy_s(nuevo->contrasena, pswUsuario);
	return nuevo;
}

void agregarVeterinarioFinal(VETERINARIO* veterinario) {
	NODOVET* nodo = nuevoNodoVeterinario(veterinario);
	if (LISTAVETERINARIOS.origen == nullptr) {
		LISTAVETERINARIOS.origen = nodo;
		LISTAVETERINARIOS.fin = nodo;
		nodo->siguiente = nullptr;
	}
	else {
		LISTAVETERINARIOS.fin->siguiente = nodo;
		LISTAVETERINARIOS.fin = nodo;
		nodo->siguiente = nullptr;
	}
}

NODOVET* nuevoNodoVeterinario(VETERINARIO* veterinario) {
	NODOVET* nodo = new NODOVET;
	nodo->veterinario = veterinario;
	nodo->siguiente = nullptr;
	return nodo;
}

bool verificarUsuario(char* usuario) {

	if (LISTAVETERINARIOS.origen == nullptr)
		return false;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if (strcmp(indice->veterinario->usuario, usuario) == 0) {
			return true;
		}
		indice = indice->siguiente;
	}
	return false;
}

//Validar canedas de texto
bool esTextoValido(const std::string& nombre) {
	for (char c : nombre) {
		if (!std::isalpha(c) && c != ' ') {
			return false;
		}
	}
	return true;
}

// Validar si una cadena contiene solo números

bool esNumeroValido(const std::string& str) {
	for (char c : str) {
		if (!std::isdigit(c)) {
			return false;
		}
	}
	return true;
}

bool buscarUsuario(char* usuario, char* psw, char* cedula) {

	if (LISTAVETERINARIOS.origen == nullptr)
		return false;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if (strcmp(indice->veterinario->usuario, usuario) == 0 && strcmp(indice->veterinario->contrasena, psw) == 0) {
			return true;
		}
		indice = indice->siguiente;
	}
	return false;
}

//NODOVET* buscarUsuario(char* usuario, char* psw) {
//	char cedula;
//	if (LISTAVETERINARIOS.origen == NULL)
//		return NULL;
//	NODOVET* indice = LISTAVETERINARIOS.origen;
//	while (indice != NULL) {
//		if (strcmp(indice->veterinario->usuario, usuario) == 0 && strcmp(indice->veterinario->contrasena, psw) == 0) {
//			strcpy_s(indice->veterinario->cedula, &cedula);
//			return cedula;
//		}
//		indice = indice->siguiente;
//	}
//
//	return "Falso";
//}

int main()
{
	//cargarCitasDesdeArchivo("citas.dat");
	while (true)
	{
		cout << "\nOpciones:" << endl;
		cout << "1 Inicio Sesion:" << endl;
		cout << "2 Registrar Usuario:" << endl;
		cout << "Seleccione una opción: ";

		int opcion;
		cin >> opcion;

		if (opcion == 1) { //Inicio Sesion

			char usuario[100];
			char psw[100];

			cout << "Usuario: " << endl;
			cin.ignore();
			cin.getline(usuario, 100);

			if (!esTextoValido(usuario)) {
				cout << "El nombre no es válido." << endl;
				continue;
			}

			cout << "psw: " << endl;
			cin.getline(psw, 100);
			char cedulaVeterinario;

			if (buscarUsuario) {
				mainVet();
			}

		}
		else if (opcion == 2) {

			char nombre[100];
			char cedula[100];
			char usuario[100];
			char psw[100];

			cout << "Ingrese el nombre: " << endl;
			cin.ignore();
			cin.getline(nombre, 100);

			if (!esTextoValido(nombre)) {
				cout << "El nombre no es válido. Debe contener solo letras y espacios, Ultima oportunidad" << endl;
				continue;
			}

			cout << "cedula: " << endl;
			cin.getline(cedula, 100);

			cout << "usuraio: " << endl;
			cin.getline(usuario, 100);


			if (verificarUsuario(usuario)) {
				cout << "Este Usuario ya existe, Ingrese de nuevo" << endl;
				continue;
			}

			cout << "Psw: " << endl;
			cin.getline(psw, 100);

			agregarVeterinarioFinal(crearUsuario(nombre, cedula, usuario, psw));
		}
		else
		{
			cout << "Opcion no valida. Intente de nuevo." << endl;
		}
	}
	return 0;
}

int mainVet()
{
	//cargarCitasDesdeArchivo("citas.dat");
	while (true)
	{
		cout << "\nOpciones:" << endl;
		cout << "1. Agregar cita" << endl;
		cout << "2. Mostrar citas" << endl;
		cout << "3. Modificar cita" << endl;
		cout << "4. Eliminar cita" << endl;
		cout << "5. Salir" << endl;
		cout << "6. Guardar" << endl;
		cout << "7 Cargar" << endl;
		cout << "Seleccione una opción: ";

		int opcion;
		cin >> opcion;

		if (opcion == 1)
		{
			char nombreCliente[100];
			char nombreMascota[100];
			char fechaHora[100];
			char motivo[100];
			float costo;
			char status[100];
			cout << "Ingrese el nombre del cliente: " << endl;
			cin.ignore();
			cin.getline(nombreCliente, 100);

			if (!esTextoValido(nombreCliente)) {
				cout << "El nombre no es válido. Debe contener solo letras y espacios, Ultima oportunidad" << endl;
				continue;
			}

			cout << "Ingrese el nombre de la mascota: " << endl;
			cin.getline(nombreMascota, 100);

			cout << "Ingrese la fecha y hora de la consulta: " << endl;
			cin.getline(fechaHora, 100);

			cout << "Ingrese el motivo de la consulta: " << endl;
			cin.getline(motivo, 100);

			cout << "Ingrese el costo de la consulta: ";
			cin >> costo;

			cout << "Ingrese el estatus de la consulta: ";
			cin.ignore();
			cin.getline(status, 100);

			if (!esTextoValido(status)) {
				cout << "El Status no es válido. Debe contener solo letras y espacios, Repita el proceso" << endl;
				continue;
			}

			agregarCitaFinal(crearCita(nombreCliente, fechaHora, nombreMascota, motivo, costo, status, cedulaVeterinario));
			cout << "Producto agregado al inventario." << endl;
		}
		else if (opcion == 2)
		{
			ImprimirListaCita();
		}
		else if (opcion == 3) {
			char nombreCliente[100];
			cout << "Ingrese el nombre del cliente de la cita que desea editar: ";
			cin.ignore();
			cin.getline(nombreCliente, 50);
			EditarCita(nombreCliente);
		}
		else if (opcion == 4) {
			char nombreCliente[100];
			cout << "Ingrese el nombre del cliente de la cita que desea eliminar: ";
			cin.ignore();
			cin.getline(nombreCliente, 50);
			EliminarCitaPorNombre(nombreCliente);
		}
		else if (opcion == 5)
		{
			break; // Salir del programa
		}
		else if (opcion == 6) {
			//guardarCitasEnArchivo("citas.dat");
		}
		else
		{
			cout << "Opcion no valida. Intente de nuevo." << endl;
		}
	}

	return 0;
}


