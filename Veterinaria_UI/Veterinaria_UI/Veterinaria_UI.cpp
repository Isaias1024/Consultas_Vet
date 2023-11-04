#include "framework.h"
#include "Veterinaria_UI.h" //La creamos para aqui referenciar los IDs de todos los recursos
#include <fstream>
#include <string>

using namespace std;
#define MAX_LOADSTRING 100

// Variables globales:
HINSTANCE hInst;			// instancia actual, para saber que ventana es la que se esta utilizando

// Definición de la estructura Cita
struct CITA {
	string nombreCliente;
	string nombreMascota;
	string motivo;
	float costo;
	string fechaHora;
	string status;
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
} LISTACITAS;

struct VETERINARIO {
	string nombreVeterinario;
	string cedulaVeterinario;
	string nombreUsuario;
	string passwordVeterinario;
};

// Definición de la estructura NODOCITA para apuntar a una cita
struct NODOVET{
	VETERINARIO* cita;
	NODOVET* anterior;
	NODOVET* siguiente;
};

struct VETERINARIOS {
	NODOVET* origen;
	NODOVET* fin;
} LISTAVETERINARIOS;

NODOCITA* nuevoNodo(CITA* cita);
NODOCITA* buscarNombre(const string& buscarNomCliente);
CITA* crearCita(const string& nombreCliente, const string& fechaHora, const string& nombreMascota, const string& motivo, float costo, const string& status);
CITA* EliminarCitaMedio(const string& nombre);
CITA* EliminarCitaFinal();
CITA* EliminarCitaInicio();
void agregarCitaFinal(CITA* cita);

NODOVET* nuevoNodoVet(VETERINARIO* veterinario);
NODOVET* buscarVet(const string& buscarNomVet);
VETERINARIO* crearVet(const string& buscarNomVet, const string& cedula, const string& nombreUsuario, const string& psw);
VETERINARIO* EliminarVetMedio(const string& nombre);
VETERINARIO* EliminarVetFinal();
VETERINARIO* EliminarVetInicio();
void agregarCitaFinal(VETERINARIO* veterinario);


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {

	HWND ventana = CreateDialog(hInst, MAKEINTRESOURCE(IDD_MAIN_WINDOW), NULL, VentanaPrincipal);
	if (!ventana) {
		return FALSE;
	}

	// Centrar la ventana principal (hVentana1)
	CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_MAIN_WINDOW), VentanaPrincipal);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return msg.wParam;
}

LRESULT CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		if (menuVentanas(wmId, hWnd)) {
			return FALSE;
		}
	}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaUsuario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	cargarCitasDesdeArchivo("citas.dat");
	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		if (menuVentanas(wmId, hWnd)) {
			return FALSE;
		}
	}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaRegistro(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		//Si no quieres tener un switch dentro de otro switch, puedes usar la funcion del menu y remover el switch (wmId) {}
		//if (menu(wmId, hWnd)) { 
		//	return FALSE;
		//}
	}break;
	case WM_CLOSE:
		DestroyWindow(hWnd);
		break;
	case WM_DESTROY:
		PostQuitMessage(117);
		break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaAgregar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_INITDIALOG: {

		SendMessage(GetDlgItem(hWnd, IDC_AGR_ESPECIE), CB_ADDSTRING, 0, (LPARAM)L"Perro");
		SendMessage(GetDlgItem(hWnd, IDC_AGR_ESPECIE), CB_ADDSTRING, 0, (LPARAM)L"Gato");
		SendMessage(GetDlgItem(hWnd, IDC_AGR_ESPECIE), CB_ADDSTRING, 0, (LPARAM)L"Conejo");

		SendMessage(GetDlgItem(hWnd, IDC_AGR_STATUS), CB_ADDSTRING, 0, (LPARAM)L"Activa");
		SendMessage(GetDlgItem(hWnd, IDC_AGR_STATUS), CB_ADDSTRING, 0, (LPARAM)L"Urgente");

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		switch (wmId) {
		case IDB_AGR_CITA: {

			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, IDC_AGR_NOMBRE, nombreCliente, 255);

			wchar_t nombreMascota[255];
			GetDlgItemText(hWnd, IDC_AGR_MASCOTA, nombreMascota, 255);

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, IDC_AGR_COSTO, costoCita, 255);

			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, IDC_AGR_MOTIVO, motivoCita, 255);

			wchar_t statusCita[255];
			GetDlgItemText(hWnd, IDC_AGR_STATUS, statusCita, 255);

			//wchar_t fechaCita[255];
			//GetDlgItemText(hWnd, IDC_AGR_DATETIME, fechaCita, 255);
			// Obtén la fecha seleccionada del datepicker
			wstring fechaCitaStr;  // Declarar la variable aquí

			HWND hDatePicker = GetDlgItem(hWnd, IDC_AGR_DATETIME);
			SYSTEMTIME selectedDate;

			if (hDatePicker != NULL && DateTime_GetSystemtime(hDatePicker, &selectedDate)) {
				// Convierte la fecha seleccionada a un string en formato "yyyy-MM-dd"
				fechaCitaStr = to_wstring(selectedDate.wYear) + L"-" +
					(selectedDate.wMonth < 10 ? L"0" : L"") + to_wstring(selectedDate.wMonth) + L"-" +
					(selectedDate.wDay < 10 ? L"0" : L"") + to_wstring(selectedDate.wDay);

				// Obtén la hora sin segundos
				wstring horaCitaStr = (selectedDate.wHour < 10 ? L"0" : L"") + to_wstring(selectedDate.wHour) + L":" +
					(selectedDate.wMinute < 10 ? L"0" : L"") + to_wstring(selectedDate.wMinute);

				// Combina la fecha y la hora
				wstring fechaHoraCitaStr = fechaCitaStr + L" " + horaCitaStr;
			}

			// Convertir de wchar_t a wstring
			wstring nombreClienteStr(nombreCliente);
			wstring nombreMascotaStr(nombreMascota);
			wstring motivoCitaStr(motivoCita);
			wstring statusCitaStr(statusCita);
			//wstring fechaCitaStr(fechaCitaStr);

			// Convertir de wstring a string
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());
			string nombreMascotaA(nombreMascotaStr.begin(), nombreMascotaStr.end());
			string motivoCitaA(motivoCitaStr.begin(), motivoCitaStr.end());
			string statusCitaA(statusCitaStr.begin(), statusCitaStr.end());
			string fechaCitaA(fechaCitaStr.begin(), fechaCitaStr.end());

			// Obtener el costo como float 
			float costoCitaFloat = 0.0f;
			if (costoCita[0] != L'\0') {
				costoCitaFloat = stof(costoCita);
			}

			CITA* nuevaCita = crearCita(nombreClienteA, fechaCitaA, nombreMascotaA, motivoCitaA, costoCitaFloat, statusCitaA);
			agregarCitaFinal(nuevaCita);

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se agrego exitosamente!";
			guardarCitasEnArchivo("citas.dat");
			int respuesta = MessageBox(hWnd, message.c_str(), L"Agregado", MB_OK);

			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_NOMBRE), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_MASCOTA), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_COSTO), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_MOTIVO), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_STATUS), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_AGR_ESPECIE), L"Seleccione");
				//crear una opcion para limpiar todo y no colocar todo el codigo aqui
			}
		}break;
		}
	}break;

	}
	return FALSE;
}

LRESULT CALLBACK VentanaEditar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_COMMAND: 
	{
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		case IDB_EDT_BUSCAR: {

			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, IDC_EDT_NOMBRE, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			NODOCITA* cita = buscarNombre(nombreClienteA);

			if (cita == nullptr) {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Busqueda", MB_OK);
				return FALSE;
			}

			// Obtén el control de la ListBox
			HWND hListBox = GetDlgItem(hWnd, IDC_LIST_EDITAR); 

			// Convertir datos de la cita a wstring
			wstring nombreMascotaStr = L"Mascota: " + wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
			wstring motivoStr = L"Motivo: " + wstring(cita->cita->motivo.begin(), cita->cita->motivo.end());
			wstring costoStr = L"Costo: " + to_wstring(cita->cita->costo);
			wstring fechaHoraStr = L"Fecha y Hora: " + wstring(cita->cita->fechaHora.begin(), cita->cita->fechaHora.end());
			wstring statusStr = L"Estatus: " + wstring(cita->cita->status.begin(), cita->cita->status.end());

			// Agregar las cadenas a la ListBox
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)nombreMascotaStr.c_str());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)motivoStr.c_str());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)costoStr.c_str());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)fechaHoraStr.c_str());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)statusStr.c_str());

		}break;
		case IDB_EDT_ACEPTAR: {

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, IDC_EDT_NOMBRE, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			NODOCITA* cita = buscarNombre(nombreClienteA);
			if (cita == NULL) {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Busqueda", MB_OK);
				return FALSE;
			}

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, IDC_EDT_COSTO, costoCita, 255);

			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, IDC_EDT_MOTIVO, motivoCita, 255);

			wchar_t statusCita[255];
			GetDlgItemText(hWnd, IDC_EDT_STATUS, statusCita, 255);

			// Convertir de wchar_t a wstring
			wstring motivoCitaStr(motivoCita);
			wstring statusCitaStr(statusCita);

			// Convertir de wstring a string
			string motivoCitaA(motivoCitaStr.begin(), motivoCitaStr.end());
			string statusCitaA(statusCitaStr.begin(), statusCitaStr.end());

			// Obtener el costo como float 
			float costoCitaFloat = 0.0f;
			if (costoCita[0] != L'\0') {
				costoCitaFloat = stof(costoCita);
			}

			cita->cita->costo = costoCitaFloat;
			cita->cita->motivo = motivoCitaA;
			cita->cita->status = statusCitaA;

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se edito exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Editado", MB_OK);

			guardarCitasEnArchivo("citas.dat");
			HWND hListBox = GetDlgItem(hWnd, IDC_LIST_EDITAR);

			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, IDC_EDT_NOMBRE), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_EDT_COSTO), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_EDT_MOTIVO), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_EDT_STATUS), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_LIST_EDITAR), L"");
				SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			}
		}break;
		}

	}break;
	default: 
		return FALSE;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaEliminar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {

	case WM_COMMAND: {

		int wmId = LOWORD(wParam);

		switch (wmId) {
		case IDB_DLT_BUSCAR: {

			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, IDC_DLT_NOMBRE, nombreCliente, 255);

			// Convertir de wchar_t a wstring
			wstring nombreClienteStr(nombreCliente);

			// Convertir de wstring a string
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			NODOCITA* cita = buscarNombre(nombreClienteA);

			if (cita == nullptr) {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Busqueda", MB_OK);
				return FALSE;
			}

			// Obtén el control de la ListBox
			HWND hListBox = GetDlgItem(hWnd, IDC_DLT_LIST); // Reemplaza  con el ID de tu ListBox

			// Convertir datos de la cita a wstring
			//wstring nombreClienteStr = L"Nombre del Cliente: " + wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
			wstring nombreMascotaStr = L"Nombre de la Mascota: " + wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
			wstring fechaHoraStr = L"Fecha y Hora: " + wstring(cita->cita->fechaHora.begin(), cita->cita->fechaHora.end());

			// Agregar las cadenas a la ListBox
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)nombreMascotaStr.c_str());
			SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)fechaHoraStr.c_str());

		}break;
		case IDB_DLT_DLT: {
			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, IDC_DLT_NOMBRE, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			CITA* citaEliminada = EliminarCitaMedio(nombreClienteA);


			if (citaEliminada != nullptr) {
				MessageBox(hWnd, L"Cita eliminada", L"Exito", MB_OK);
				guardarCitasEnArchivo("citas.dat");
				delete citaEliminada;
			}
			else {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Error", MB_OK);
			}
			
			HWND hListBox = GetDlgItem(hWnd, IDC_DLT_LIST);
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			SetWindowText(GetDlgItem(hWnd, IDC_DLT_NOMBRE), L"");

		}break;
		}

	}break;
	default: return FALSE;

	}
	return FALSE;
}

LRESULT CALLBACK VentanaMostrar(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {



	return FALSE;
}

NODOCITA* nuevoNodo(CITA* cita) {
	NODOCITA* nodo = new NODOCITA;
	nodo->cita = cita;
	nodo->siguiente = nullptr;
	return nodo;
}

NODOCITA* buscarNombre(const string& buscarNomCliente) {
	if (LISTACITAS.origen == nullptr)
		return nullptr;
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != nullptr) {
		if (indice->cita->nombreCliente == buscarNomCliente)
			break;
		indice = indice->siguiente;
	}
	return indice;
}

bool menuVentanas(int opcion, HWND ventana) {

	switch (opcion) {
	case ID_BTN_LOGIN: {
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_SESION), VentanaUsuario);
	} break;
	case ID_BTN_REGISTRAR: {
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_REGISTRAR), VentanaRegistro);
	} break;
	case ID_OPERACIONES_AGREGAR:
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_AGREGAR), VentanaAgregar);
		break;
	case ID_OPERACIONES_EDITAR:
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_EDITAR), VentanaEditar);
		break;
	case ID_OPERACIONES_ELIMINAR:
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_ELIMINAR), VentanaEliminar);
		break;
	case ID_OPERACIONES_MOSTRAR:
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(IDD_VENTANA_MOSTRAR), VentanaMostrar);
		break;
	default:
		return FALSE;
	}
	return FALSE;
}

bool CrearVentanaYCentrar(HWND& ventana, LPCWSTR recursoVentana, DLGPROC procedimientoVentana) {

	ventana = CreateDialog(hInst, recursoVentana, NULL, procedimientoVentana);

	if (ventana != nullptr) {
		// Obtener las dimensiones de la pantalla
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		// Obtener las dimensiones de la ventana
		RECT windowRect;
		GetWindowRect(ventana, &windowRect);
		int windowWidth = windowRect.right - windowRect.left;
		int windowHeight = windowRect.bottom - windowRect.top;

		// Calcular las coordenadas para centrar la ventana
		int x = (screenWidth - windowWidth) / 2;
		int y = (screenHeight - windowHeight) / 2;

		// Mover la ventana a las coordenadas calculadas
		MoveWindow(ventana, x, y, windowWidth, windowHeight, TRUE);

		ShowWindow(ventana, SW_SHOW);
		UpdateWindow(ventana);
		return true;
	}

	return false;
}

void guardarCitasEnArchivo(const char* nombreArchivo) {
	// Intentar abrir el archivo para escritura binaria
	std::ofstream archivo(nombreArchivo, std::ios::binary);

	if (!archivo.is_open()) {
		// Manejar errores al abrir el archivo
		MessageBox(nullptr, L"No se pudo abrir el archivo para escritura.", L"Error", MB_OK);
		return; // Salir de la función si no se pudo abrir el archivo
	}

	NODOCITA* nodo = LISTACITAS.origen;

	while (nodo != nullptr) {
		// Comprobar si el nodo o la cita son nulos antes de intentar escribirlos
		if (nodo->cita != nullptr) {
			// Intentar escribir el nodo->cita en el archivo
			archivo.write(reinterpret_cast<const char*>(nodo->cita), sizeof(CITA));
		}
		nodo = nodo->siguiente;
	}

	archivo.close(); // Cerrar el archivo después de escribir

	if (archivo.fail()) {
		// Manejar errores al cerrar el archivo
		MessageBox(nullptr, L"No se pudo cerrar el archivo correctamente.", L"Error", MB_OK);
	}
}

void cargarCitasDesdeArchivo(const char* nombreArchivo) {
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
		const CITA* citaData = reinterpret_cast<const CITA*>(buffer);
		size_t numCitas = fileSize / sizeof(CITA);

		for (size_t i = 0; i < numCitas; ++i) {
			CITA* nuevaCita = crearCita(citaData[i].nombreCliente, citaData[i].fechaHora, citaData[i].nombreMascota, citaData[i].motivo, citaData[i].costo, citaData[i].status);
			agregarCitaFinal(nuevaCita);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}

CITA* crearCita(const string& nombreCliente, const string& fechaHora, const string& nombreMascota, const string& motivo, float costo, const string& status) {
	CITA* nuevo = new CITA;
	nuevo->costo = costo;
	nuevo->fechaHora = fechaHora; // Debes asignar el valor adecuado
	nuevo->motivo = motivo;
	nuevo->nombreCliente = nombreCliente;
	nuevo->nombreMascota = nombreMascota;
	nuevo->status = status;
	return nuevo;
}

CITA* EliminarCitaMedio(const string& nombre) {
	NODOCITA* busqueda = buscarNombre(nombre);
	if (busqueda == nullptr)
		return nullptr;
	if (busqueda == LISTACITAS.origen)
		return EliminarCitaInicio();
	else if (busqueda == LISTACITAS.fin)
		return EliminarCitaFinal();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == busqueda)
			break;
		indice = indice->siguiente;
	}
	CITA* cita = busqueda->cita;
	if (indice != nullptr)
		indice->siguiente = busqueda->siguiente;
	delete busqueda;
	return cita;
}

CITA* EliminarCitaInicio() {
	if (LISTACITAS.origen == nullptr)
		return nullptr;
	if (LISTACITAS.origen == LISTACITAS.fin)
		LISTACITAS.fin = nullptr;
	NODOCITA* anterior = LISTACITAS.origen;
	LISTACITAS.origen = LISTACITAS.origen->siguiente;
	CITA* cita = anterior->cita;
	delete anterior;
	return cita;
}

CITA* EliminarCitaFinal() {
	if (LISTACITAS.origen == nullptr)
		return nullptr;
	if (LISTACITAS.origen == LISTACITAS.fin)
		return EliminarCitaInicio();
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != nullptr) {
		if (indice->siguiente == LISTACITAS.fin) {
			break;
		}
		indice = indice->siguiente;
	}
	CITA* cita = LISTACITAS.fin->cita;
	delete LISTACITAS.fin;
	LISTACITAS.fin = indice;
	if (indice != nullptr)
		indice->siguiente = nullptr;
	return cita;
}

void agregarCitaFinal(CITA* cita)
{
	NODOCITA* nodo = nuevoNodo(cita);
	if (LISTACITAS.origen == nullptr) {
		LISTACITAS.origen = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = nullptr;
	}
	else {
		LISTACITAS.fin->siguiente = nodo;
		LISTACITAS.fin = nodo;
		nodo->siguiente = nullptr;
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

