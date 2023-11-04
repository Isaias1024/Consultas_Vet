#include "framework.h"
#include <fstream>
#include <string>

using namespace std;

struct VETERINARIO {
	string nombreVeterinario;
	string cedulaVeterinario;
	string nombreUsuario;
	string passwordVeterinario;
};

struct NODOVET {
	VETERINARIO* veterinario;
	NODOVET* anterior;
	NODOVET* siguiente;
};

struct VETERINARIOS {
	NODOVET* origen;
	NODOVET* fin;
} LISTAVETERINARIOS;

NODOVET* nuevoNodoUsuario(VETERINARIO* veterinario);
NODOVET* buscarUsuario(const string& buscarUsuario);
VETERINARIO* crearUsuario(const string& nombreVet, const string& cedula, const string& nombreUsuario, const string& psw);
VETERINARIO* EliminarUsuarioInicio();
VETERINARIO* EliminarUsuarioMedio(const string& nombre);
VETERINARIO* EliminarUsuarioFinal();
void agregarUsuarioFinal(VETERINARIO* veterinario);
void guardarUsuariosEnArchivo(const char* nombreArchivo);
void cargarUsuariosDesdeArchivo(const char* nombreArchivo);

NODOVET* nuevoNodoUsuario(VETERINARIO* veterinario) {
	NODOVET* nodo = new NODOVET;
	nodo->veterinario = veterinario;
	nodo->siguiente = nullptr;
	return nodo;
}

NODOVET* buscarUsuario(const string& buscarUsuario) {
	if (LISTAVETERINARIOS.origen == nullptr)
		return nullptr;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if (indice->veterinario->nombreUsuario == buscarUsuario)
			break;
		indice = indice->siguiente;
	}
	return indice;
}

VETERINARIO* crearUsuario(const string& nombreVeterinario, const string& cedula, const string& nombreUsuario, const string& password) {
	VETERINARIO* nuevo = new VETERINARIO;
	nuevo->nombreVeterinario = nombreVeterinario;
	nuevo->cedulaVeterinario = cedula;
	nuevo->nombreUsuario = nombreUsuario;
	nuevo->passwordVeterinario = password;
	return nuevo;
}

VETERINARIO* EliminarUsuarioInicio() {
	if (LISTAVETERINARIOS.origen == nullptr)
		return nullptr;
	if (LISTAVETERINARIOS.origen == LISTAVETERINARIOS.fin)
		LISTAVETERINARIOS.fin = nullptr;
	NODOVET* anterior = LISTAVETERINARIOS.origen;
	LISTAVETERINARIOS.origen = LISTAVETERINARIOS.origen->siguiente;
	VETERINARIO* veterinario = anterior->veterinario;
	delete anterior;
	return veterinario;
}

VETERINARIO* EliminarUsuarioMedio(const string& nombre) {
	NODOVET* busqueda = buscarUsuario(nombre);
	if (busqueda == nullptr)
		return nullptr;
	if (busqueda == LISTAVETERINARIOS.origen)
		return EliminarUsuarioInicio();
	else if (busqueda == LISTAVETERINARIOS.fin)
		return EliminarUsuarioFinal();
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == busqueda)
			break;
		indice = indice->siguiente;
	}
	VETERINARIO* veterinario = busqueda->veterinario;
	if (indice != nullptr)
		indice->siguiente = busqueda->siguiente;
	delete busqueda;
	return veterinario;
}

VETERINARIO* EliminarUsuarioFinal() {
	if (LISTAVETERINARIOS.origen == nullptr)
		return nullptr;
	if (LISTAVETERINARIOS.origen == LISTAVETERINARIOS.fin)
		return EliminarUsuarioInicio();
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == LISTAVETERINARIOS.fin) {
			break;
		}
		indice = indice->siguiente;
	}
	VETERINARIO* veterinario = LISTAVETERINARIOS.fin->veterinario;
	delete LISTAVETERINARIOS.fin;
	LISTAVETERINARIOS.fin = indice;
	if (indice != nullptr)
		indice->siguiente = nullptr;
	return veterinario;
}

void agregarUsuarioFinal(VETERINARIO* cita)
{
	NODOVET* nodo = nuevoNodo(cita);
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

void EliminarListaCita() {
	while (LISTAVETERINARIOS.origen != NULL) {
		NODOVET* temporal = LISTAVETERINARIOS.origen;
		LISTAVETERINARIOS.origen = LISTAVETERINARIOS.origen->siguiente;
		if (temporal->veterinario != NULL)
			delete temporal->veterinario;
		delete temporal;
	}
}

void guardarUsuariosEnArchivo(const char* nombreArchivo) {
	// Intentar abrir el archivo para escritura binaria
	std::ofstream archivo(nombreArchivo, std::ios::binary);

	if (!archivo.is_open()) {
		// Manejar errores al abrir el archivo
		MessageBox(nullptr, L"No se pudo abrir el archivo para escritura.", L"Error", MB_OK);
		return; // Salir de la función si no se pudo abrir el archivo
	}

	NODOVET* nodo = LISTAVETERINARIOS.origen;

	while (nodo != nullptr) {
		// Comprobar si el nodo o la cita son nulos antes de intentar escribirlos
		if (nodo->veterinario != nullptr) {
			// Intentar escribir el nodo->cita en el archivo
			archivo.write(reinterpret_cast<const char*>(nodo->veterinario), sizeof(VETERINARIO));
		}
		nodo = nodo->siguiente;
	}

	archivo.close(); // Cerrar el archivo después de escribir

	if (archivo.fail()) {
		// Manejar errores al cerrar el archivo
		MessageBox(nullptr, L"No se pudo cerrar el archivo correctamente.", L"Error", MB_OK);
	}
}

void cargarUsuariosDesdeArchivo(const char* nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		// Primero, elimina todas las citas existentes en la lista para evitar duplicados
		EliminarListaCita();

		// Busca el tamaño del archivo para asignar memoria
		archivo.seekg(0, std::ios::end);
		std::streampos fileSize = archivo.tellg();
		archivo.seekg(0, std::ios::beg);

		if (fileSize <= 0) {
			// El archivo está vacío, no hay datos que leer.
			archivo.close();
			return;
		}

		// Crea un buffer para leer los datos
		char* buffer = new char[fileSize];

		// Lee los datos en el buffer
		archivo.read(buffer, fileSize);

		// Cerrar el archivo
		archivo.close();

		// Procesar los datos en el buffer
		const VETERINARIO* citaData = reinterpret_cast<const VETERINARIO*>(buffer);
		size_t numCitas = fileSize / sizeof(VETERINARIO);

		for (size_t i = 0; i < numCitas; ++i) {
			VETERINARIO* nuevoVet = crearUsuario(citaData[i].nombreVeterinario, citaData[i].cedulaVeterinario, citaData[i].nombreUsuario, citaData[i].passwordVeterinario);
			agregarUsuarioFinal(nuevoVet);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}
