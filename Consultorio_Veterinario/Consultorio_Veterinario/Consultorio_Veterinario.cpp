#include <iostream>
#include <string.h>
#include <stdio.h>
#include <fstream>

using namespace std;

// Definición de la estructura Cita
struct CITA {
	char fechaHora[100];
	char nombreCliente[100];
	char nombreMascota[100];
	char motivo[100];
	float costo;
	char status[100];
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

CITA* crearCita(char nombreCliente[100], char fechaHora[100], char nombreMascota[100], char motivo[100], float costo, char status[100]) {
	CITA* nuevo = new CITA;
	nuevo->costo = costo;
	strcpy_s(nuevo->fechaHora, fechaHora);
	strcpy_s(nuevo->motivo, motivo);
	strcpy_s(nuevo->nombreCliente, nombreCliente);
	strcpy_s(nuevo->nombreMascota, nombreMascota);
	strcpy_s(nuevo->status, status);
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
		cout << ")" << endl;
		indice = indice->siguiente;
	}
}

void EliminarListaCita() {
	while (LISTACITAS.origen != NULL) {
		NODOCITA* temporal = LISTACITAS.origen;
		LISTACITAS.origen = LISTACITAS.origen->siguiente;
		if (temporal->cita != NULL)
			delete temporal->cita;
		delete temporal;
	}
}

void EditarCita(char* nombreCliente) {

	NODOCITA* cita = buscarNombre(nombreCliente);
	if (cita == NULL) {
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
	if (citaEliminada != NULL) {
		cout << "Cita eliminada exitosamente." << endl;
		delete citaEliminada;
	}
	else {
		cout << "La cita no se encontro." << endl;
	}
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

void guardarCitasEnArchivo(const char* nombreArchivo) {
	std::ofstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		NODOCITA* nodo = LISTACITAS.origen;

		while (nodo != NULL) {
			archivo.write(reinterpret_cast<const char*>(nodo->cita), sizeof(CITA));
			nodo = nodo->siguiente;
		}

		archivo.close();
		std::cout << "Citas guardadas en el archivo binario." << std::endl;
	}
	else {
		std::cerr << "No se pudo abrir el archivo para escritura." << std::endl;
	}
}

void cargarCitasDesdeArchivo(const char* nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		// Primero, elimina todas las citas existentes en la lista
		EliminarListaCita();

		CITA cita;
		while (archivo.read(reinterpret_cast<char*>(&cita), sizeof(CITA))) {
			CITA* nuevaCita = crearCita(cita.nombreCliente, cita.fechaHora, cita.nombreMascota, cita.motivo, cita.costo, cita.status);
			agregarCitaFinal(nuevaCita);
		}

		archivo.close();
		std::cout << "Citas cargadas desde el archivo binario." << std::endl;
	}
	else {
		std::cerr << "No se pudo abrir el archivo para lectura." << std::endl;
	}
}


int main()
{
	CITA citas;
	cargarCitasDesdeArchivo("citas.dat");
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

			agregarCitaFinal(crearCita(nombreCliente, fechaHora, nombreMascota, motivo, costo, status));
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
			guardarCitasEnArchivo("citas.dat");
		}
		else
		{
			cout << "Opcion no valida. Intente de nuevo." << endl;
		}
	}

	return 0;
}