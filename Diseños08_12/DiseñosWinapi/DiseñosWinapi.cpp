#include <iostream>
#include <windows.h>
#include <string>
#include <stdio.h>
#include <commctrl.h>
#include <fstream>
#include <map>
#include "DiseñoWinapi.h"
#include <gdiplus.h>
#pragma comment (lib, "gdiplus.lib")
#include <codecvt>
#include <locale>
#include <algorithm>
#include <string>
#include <filesystem>

using namespace std;
using namespace Gdiplus;
HINSTANCE hInst;
ULONG_PTR tokenGdiplus;


struct USUARIO {
	std::string cedula;
};

map<string, USUARIO> sesionesActivas;

struct VETERINARIO
{
	string nombreVeterinario;
	string cedula;
	string foto;
	string usuario;
	string contrasena;
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

struct CITA {
	string nombreCliente;
	string fechaCita;
	string horaCita;
	string telefonoCliente;
	string nombreMascota;
	string motivo;
	float costo;
	string especie;
	string statusCita;
	string cedulaVeterinario;
};

struct NODOCITA {
	CITA* cita;
	NODOCITA* anterior;
	NODOCITA* siguiente;
};

struct CITAS {
	NODOCITA* origen;
	NODOCITA* fin;
}LISTACITAS;


CITA* crearCita(const string& nombreCliente, const string& fechaCita, const string& horaCita, const string telefonoCita, const string& nombreMascota, const string& motivoCita, float costoCita, const string& especie, const string& statusCita, const string& cedula);
void agregarCitaFinal(CITA* cita);
NODOCITA* nuevoNodoCita(CITA* cita);
NODOCITA* buscarCita(const string& buscarNomCliente, const string& nombreMascota, const string& cedulaUsuario);
bool buscarFechaDisponible(const string& fecha, const string& hora, const string& cedulaUsuario);

CITA* EliminarCitaMedio(const string& nombre, const string& nombreMascota, const string& cedulaUsuario);
CITA* EliminarCitaFinal();
CITA* EliminarCitaInicio();

bool validarUsuarioExistente(const string& nombreUsuario, const string& cedulaUsuario);
VETERINARIO* crearUsuario(const string& nombreVeterinario, const string& cedula, const string& usuario, const string& contrasena, const string& foto);
NODOVET* verificarUsuario(const string& usuario, const string& contrasena);
void agregarVeterinarioFinal(VETERINARIO* veterinario);

void guardarUsuariosEnArchivo(const string& nombreArchivo);
void cargarUsuariosDesdeArchivo(const string& nombreArchivo);
void EliminarListaCita();

void guardarCitasEnArchivo(const char* nombreArchivo);
void cargarCitasDesdeArchivo(const char* nombreArchivo);
void EliminarListaVeterinarios();

string cedulaUsuarioActivo();
LRESULT CALLBACK VentanaUsuario(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

bool MenuUsuario(int opcion, HWND ventana);
bool CrearVentanaYCentrar(HWND& ventana, LPCWSTR recursoVentana, DLGPROC procedimientoVentana);

bool validarCaracteres(const string& textoValidar);
bool esFechaHoraValida(const string& fechaCita, const string& horaCita);

bool citaEsParaHoy(const string& fechaCita);
bool esFechaValida(const string& fechaCita);
bool AbrirCuadroDialogo(HWND hWnd, wchar_t* rutaArchivo);

void FinalizarAplicacion();
void InicializarAplicacion();

NODOVET* rutaImagenUsuario(const string& cedula);
std::wstring stringToWString(const std::string& str);
std::string reemplazarBarrasInvertidas(const std::string& ruta);
std::string obtenerNombreArchivo(const std::string& ruta);

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, PSTR cmdLine, int cShow) {

	InicializarAplicacion(); // Inicializar GDI+ y otras configuraciones

	cargarCitasDesdeArchivo("CitasMiCat.bin");
	cargarUsuariosDesdeArchivo("UsuariosMiCat.bin");

	HWND ventana = CreateDialog(hInst, MAKEINTRESOURCE(ID_DIA_Login), NULL, VentanaPrincipal);

	if (!ventana) {
		return FALSE;
	}

	// Centrar la ventana principal (hVentana1)
	CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	FinalizarAplicacion(); // Limpiar GDI+ y otras configuraciones

	return msg.wParam;
}

LRESULT CALLBACK VentanaRegistroUsuario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	static wchar_t rutaArchivo[MAX_PATH] = { 0 };
	HBITMAP hBitmapGlobal = NULL;
	static HBITMAP hBitmap = nullptr;

	switch (msg) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_BTN_RegExaminar: {

			OPENFILENAME ofn;
			ZeroMemory(&ofn, sizeof(OPENFILENAME));
			char cDirFile[MAX_PATH] = "";

			ofn.hwndOwner = hWnd;
			ofn.lpstrFile = rutaArchivo;
			ofn.lStructSize = sizeof(OPENFILENAME);
			ofn.nMaxFile = MAX_PATH;
			ofn.lpstrDefExt = L"png";
			ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY | OFN_NOCHANGEDIR;
			ofn.lpstrFilter = L"Imagenes PNG\0*.png\0Todos los Archivos\0*.*\0";

			if (GetOpenFileName(&ofn)) {
				HWND hwndButton = GetDlgItem(hWnd, IDC_FILE_NAME);
				SetWindowText(hwndButton, rutaArchivo);
			}

			Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromFile(rutaArchivo);
			if (pOriginalBitmap)
			{
				// Crear un nuevo bitmap con el tamaño deseado
				Gdiplus::Bitmap* pResizedBitmap = new Gdiplus::Bitmap(100, 100, PixelFormat32bppARGB);

				// Crear un objeto Graphics para dibujar en el nuevo bitmap
				Gdiplus::Graphics graphics(pResizedBitmap);
				graphics.DrawImage(pOriginalBitmap, 0, 0, 100, 100);

				// Convertir el bitmap redimensionado a HBITMAP
				HBITMAP hBitmap;
				pResizedBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);


				if (hBitmap) {

					if (hBitmapGlobal) {
						DeleteObject(hBitmapGlobal); // Liberar la versión anterior si existe
					}
					hBitmapGlobal = hBitmap;

					// Liberar el bitmap antiguo
					HBITMAP hOldBitmap = (HBITMAP)SendDlgItemMessage(hWnd, ID_FOTO_RegUss, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
					if (hOldBitmap) {
						DeleteObject(hOldBitmap);
					}
					// Actualizar el control
					HWND hPictureControl = GetDlgItem(hWnd, ID_FOTO_RegUss);
					InvalidateRect(hPictureControl, NULL, TRUE);
					UpdateWindow(hPictureControl);
				}
				delete pOriginalBitmap;
				delete pResizedBitmap;
			}

		} break;

		case ID_BTN_RegUssOK: {

			// Leemos los datos desde el UI
			wchar_t nombreVeterinario[255];
			GetDlgItemText(hWnd, ID_EC_RegNombreDoctor, nombreVeterinario, 255);

			wchar_t cedula[255];
			GetDlgItemText(hWnd, ID_EC_RegCedula, cedula, 255);

			wchar_t usuario[255];
			GetDlgItemText(hWnd, ID_EC_RegClaveUss, usuario, 255);

			wchar_t contrasena[255];
			GetDlgItemText(hWnd, ID_EC_RegContrasena, contrasena, 255);

			//wchar_t -> wstring
			wstring nombreVeterinarioStr(nombreVeterinario);
			wstring cedulaStr(cedula);
			wstring usuarioStr(usuario);
			wstring contrasenaStr(contrasena);
			wstring rutaArchivoStr(rutaArchivo);

			if (nombreVeterinarioStr == L"" || cedulaStr == L"" || usuarioStr == L"" || contrasenaStr == L"" || rutaArchivoStr == L"") {
				MessageBox(hWnd, L"Ningun campo puede estar vacio!", L"ERROR!", MB_OKCANCEL);
				break;
			}
			//wstring -> string
			string nombreVeterinarioA(nombreVeterinarioStr.begin(), nombreVeterinarioStr.end());
			string cedulaA(cedulaStr.begin(), cedulaStr.end());
			string usuarioA(usuarioStr.begin(), usuarioStr.end());
			string contrasenaA(contrasenaStr.begin(), contrasenaStr.end());
			string rutaArchivoA(rutaArchivoStr.begin(), rutaArchivoStr.end());

			//VALIDAR USUARIO EXISTENTE
			bool datosValidos = validarUsuarioExistente(usuarioA, cedulaA);
			if (!datosValidos) {
				MessageBox(hWnd, L"Datos ya existen!", L"VETERINARIA", MB_OKCANCEL);
				break;
			}

			if (!validarCaracteres(nombreVeterinarioA))
			{
				MessageBox(hWnd, L"Nombre de Veterinario no valido!", L"ERROR!", MB_OKCANCEL);
				break;
			}
			
			//crar una funcion que tome copiarArchivoRuta(rutaArchivoA)
			std::string nombreArchivo = obtenerNombreArchivo(rutaArchivoA);

			VETERINARIO* nuevo = crearUsuario(nombreVeterinarioA, cedulaA, usuarioA, contrasenaA, nombreArchivo);
			if (nuevo == nullptr)
				break;

			agregarVeterinarioFinal(nuevo);
			guardarUsuariosEnArchivo("UsuariosMiCat.bin");

			// Mostrar el nombre del Vet agregado en un messageBox
			wstring message = L"El usuario: " + wstring(nombreVeterinario) + L" se registró exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Registrado", MB_OK);


			// Limpiar el contenido del cuadro de texto
			SetWindowText(GetDlgItem(hWnd, ID_EC_RegNombreDoctor), L"");
			SetWindowText(GetDlgItem(hWnd, ID_EC_RegCedula), L"");
			SetWindowText(GetDlgItem(hWnd, ID_EC_RegClaveUss), L"");
			SetWindowText(GetDlgItem(hWnd, ID_EC_RegContrasena), L"");
			SetWindowText(GetDlgItem(hWnd, IDC_FILE_NAME), L""); \
				//QUITAR IMAGEN DE LA PANTALLA

		}break;
		case ID_OPCIONES_REGRESAR: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal);
		} break;
		}
	}break;
	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);

		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}

	}break;
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_INITDIALOG: {
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_BTN_Acceder: {

			wchar_t nombreUsuario[255];
			wchar_t nombrePsw[255];

			GetDlgItemText(hWnd, ID_EC_LoginUss, nombreUsuario, 255);
			GetDlgItemText(hWnd, ID_EC_LoginPassword, nombrePsw, 255);

			wstring nombreUsuarioStr(nombreUsuario);
			wstring nombrePswStr(nombrePsw);

			string nombreUsuarioA(nombreUsuarioStr.begin(), nombreUsuarioStr.end());
			string nombrePswA(nombrePswStr.begin(), nombrePswStr.end());

			NODOVET* usuarioEncontrado = verificarUsuario(nombreUsuarioA, nombrePswA);

			if (usuarioEncontrado == nullptr) {
				MessageBox(hWnd, L"Usuario o Password incorrecto", L"VETERINARIA", MB_OK);
				break;
			}

			USUARIO nuevoUsuario;
			nuevoUsuario.cedula = usuarioEncontrado->veterinario->cedula;
			sesionesActivas["sesionUsuario"] = nuevoUsuario;


			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Menu), VentanaUsuario);


		} break;
		case ID_BTN_RegistrarUss: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_RegistroUss), VentanaRegistroUsuario);
		} break;
		case ID_MEN_CerrarSesion: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_CerrarSesion), VentanaPrincipal); //REVISAR
		} break;
		}
	}break;
	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	}break;
	default:
		break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaUsuario(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	HBITMAP hBitmapGlobal = NULL;

	switch (msg) {
	case WM_INITDIALOG: {

		string cedulaUsuario = cedulaUsuarioActivo();
		NODOVET* veterrinatioActual = rutaImagenUsuario(cedulaUsuario); //esto lo quitarias

		//copiar y pegar para el nombre
								//c:Dise; oWinapi / Imagenes / + cedulaUsuario
		string rutaCompleta = "C:\\Users\\isaia\\OneDrive\\Escritorio\\" + veterrinatioActual->veterinario->foto; 
		wstring nombreVeterinario(veterrinatioActual->veterinario->nombreVeterinario.begin(), veterrinatioActual->veterinario->nombreVeterinario.end());
		HWND hwndNombreMascota = GetDlgItem(hWnd, IDC_NOMBRE_VET_INICIO);
		SetWindowText(hwndNombreMascota, nombreVeterinario.c_str());

		std::wstring rutaImagenW = stringToWString(rutaCompleta);
		Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromFile(rutaImagenW.c_str());

		if (pOriginalBitmap)
		{
			// Crear un nuevo bitmap con el tamaño deseado
			Gdiplus::Bitmap* pResizedBitmap = new Gdiplus::Bitmap(100, 100, PixelFormat32bppARGB);

			// Crear un objeto Graphics para dibujar en el nuevo bitmap
			Gdiplus::Graphics graphics(pResizedBitmap);
			graphics.DrawImage(pOriginalBitmap, 0, 0, 100, 100);

			// Convertir el bitmap redimensionado a HBITMAP
			HBITMAP hBitmap;
			pResizedBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);


			if (hBitmap) {

				if (hBitmapGlobal) {
					DeleteObject(hBitmapGlobal); // Liberar la versión anterior si existe
				}
				hBitmapGlobal = hBitmap;

				// Liberar el bitmap antiguo
				HBITMAP hOldBitmap = (HBITMAP)SendDlgItemMessage(hWnd, IDC_PROFILE_PIC, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
				if (hOldBitmap) {
					DeleteObject(hOldBitmap);
				}
				// Actualizar el control
				HWND hPictureControl = GetDlgItem(hWnd, IDC_PROFILE_PIC);
				InvalidateRect(hPictureControl, NULL, TRUE);
				UpdateWindow(hPictureControl);
			}
			delete pOriginalBitmap;
			delete pResizedBitmap;
		}

		CheckDlgButton(hWnd, IDC_CHECK_PROXIMAS, BST_CHECKED);//Selecciona el checkbox de citas proximas
		NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
		if (cita == nullptr) { //si no existen ps no pasa nada
			return FALSE;
		}

		HWND hListBox = GetDlgItem(hWnd, ID_LBOX_AgendaMenu); //obter el ID del lisBox
		while (cita != nullptr)
		{ //Mientras la lista tenga datos
			if (cita->cita->statusCita != "Eliminada" && cita->cita->cedulaVeterinario == cedulaUsuario && citaEsParaHoy(cita->cita->fechaCita))
			{
				wstring nombreCliente = wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
				wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
				wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
				wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
				wstring listaCitas = nombreCliente + L" Mascota: " + nombreMacota + L" " + fechaStr + L" " + horaStr;
				SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
			}
			cita = cita->siguiente; //avanzamos 1 lugar en la fila
		}
	}break;

	case WM_COMMAND: {
		int wmId = LOWORD(wParam);

		switch (wmId)
		{
		case IDC_CHECK_PROXIMAS: {

			LRESULT checkState = SendMessage(GetDlgItem(hWnd, IDC_CHECK_PROXIMAS), BM_GETCHECK, 0, 0);

			// Verificar si se está intentando desmarcar y si los otros dos están desmarcados
			if (checkState == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_TODAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_CANCELADAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED) {

				// No permitir que se desmarque
				CheckDlgButton(hWnd, IDC_CHECK_PROXIMAS, BST_CHECKED);
				return TRUE;
			}

			CheckDlgButton(hWnd, IDC_CHECK_TODAS, BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK_CANCELADAS, BST_UNCHECKED);

			string cedulaUsuario = cedulaUsuarioActivo();
			NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
			if (cita == nullptr) { //si no existen ps no pasa nada
				return FALSE;
			}

			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_AgendaMenu); //obter el ID del lisBox
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			while (cita != nullptr) { //Mientras la lista tenga datos				
				if (cita->cita->statusCita != "Cancelada" && cita->cita->cedulaVeterinario == cedulaUsuario && citaEsParaHoy(cita->cita->fechaCita)) { //&& comparar la fecha
					wstring nombreCliente = wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
					wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
					wstring listaCitas = nombreCliente + L" Mascota: " + nombreMacota + L" " + fechaStr + L" " + horaStr;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
				}
				cita = cita->siguiente; //avanzamos 1 lugar en la fila
			}
		}break;
		case IDC_CHECK_TODAS: {

			LRESULT checkState = SendMessage(GetDlgItem(hWnd, IDC_CHECK_TODAS), BM_GETCHECK, 0, 0);

			// Verificar si se está intentando desmarcar y si los otros dos están desmarcados
			if (checkState == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_PROXIMAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_CANCELADAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED) {

				// No permitir que se desmarque
				CheckDlgButton(hWnd, IDC_CHECK_TODAS, BST_CHECKED);
				return TRUE;
			}

			CheckDlgButton(hWnd, IDC_CHECK_PROXIMAS, BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK_CANCELADAS, BST_UNCHECKED);

			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
			if (cita == nullptr) { //si no existen ps no pasa nada
				return FALSE;
			}
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_AgendaMenu); //obter el ID del lisBox
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			while (cita != nullptr) { //Mientras la lista tenga datos

				//codigo para mostrar las citas que queremos ?
				//validar las activas y la cedula
				if (cita->cita->statusCita != "Eliminada" && cita->cita->cedulaVeterinario == cedulaUsuario) { //&& comparar la fecha
					wstring nombreCliente = wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
					wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
					wstring listaCitas = nombreCliente + L" Mascota: " + nombreMacota + L" " + fechaStr + L" " + horaStr;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
				}
				cita = cita->siguiente; //avanzamos 1 lugar en la fila
			}
		}break;
		case IDC_CHECK_CANCELADAS: {

			LRESULT checkState = SendMessage(GetDlgItem(hWnd, IDC_CHECK_CANCELADAS), BM_GETCHECK, 0, 0);

			// Verificar si se está intentando desmarcar y si los otros dos están desmarcados
			if (checkState == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_PROXIMAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED &&
				SendMessage(GetDlgItem(hWnd, IDC_CHECK_TODAS), BM_GETCHECK, 0, 0) == BST_UNCHECKED) {

				// No permitir que se desmarque
				CheckDlgButton(hWnd, IDC_CHECK_CANCELADAS, BST_CHECKED);
				return TRUE;
			}
			CheckDlgButton(hWnd, IDC_CHECK_PROXIMAS, BST_UNCHECKED);
			CheckDlgButton(hWnd, IDC_CHECK_TODAS, BST_UNCHECKED);

			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
			if (cita == nullptr) { //si no existen ps no pasa nada
				return FALSE;
			}
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_AgendaMenu); //obter el ID del lisBox
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			while (cita != nullptr) { //Mientras la lista tenga datos

				//codigo para mostrar las citas que queremos ?
				//validar las activas y la cedula
				if (cita->cita->statusCita == "Cancelada" && cita->cita->cedulaVeterinario == cedulaUsuario) { //&& comparar la fecha
					wstring nombreCliente = wstring(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
					wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
					wstring listaCitas = nombreCliente + L" Mascota: " + nombreMacota + L" " + fechaStr + L" " + horaStr;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
				}
				cita = cita->siguiente; //avanzamos 1 lugar en la fila
			}
		}break;
		default:break;
		}

		if (MenuUsuario(wmId, hWnd)) {
			return FALSE;
		}

	}break;

	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	}break;
	default:
		break;
	}

	return FALSE;
}

LRESULT CALLBACK VentanaRegistrarCita(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	//Aqui va el codigo de la ventana
	static bool mensajeMostrado = false; // Variable estática para rastrear el mensaje

	switch (msg)
	{
	case WM_INITDIALOG: {

		//Lineas de la imaggen
		string cedulaUsuario = cedulaUsuarioActivo();
		NODOVET* veterrinatioActual = rutaImagenUsuario(cedulaUsuario);
		string rutaCompleta = "C:\\Users\\isaia\\OneDrive\\Escritorio\\" + veterrinatioActual->veterinario->foto;

		wstring nombreVeterinario(veterrinatioActual->veterinario->nombreVeterinario.begin(), veterrinatioActual->veterinario->nombreVeterinario.end());
		HWND hwndNombreMascota = GetDlgItem(hWnd, IDC_NOMBRE_VET_INICIO);
		SetWindowText(hwndNombreMascota, nombreVeterinario.c_str());
		std::wstring rutaImagenW = stringToWString(rutaCompleta);
		Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromFile(rutaImagenW.c_str());

		if (pOriginalBitmap)
		{
			// Crear un nuevo bitmap con el tamaño deseado
			Gdiplus::Bitmap* pResizedBitmap = new Gdiplus::Bitmap(100, 100, PixelFormat32bppARGB);

			// Crear un objeto Graphics para dibujar en el nuevo bitmap
			Gdiplus::Graphics graphics(pResizedBitmap);
			graphics.DrawImage(pOriginalBitmap, 0, 0, 100, 100);

			// Convertir el bitmap redimensionado a HBITMAP
			HBITMAP hBitmap;
			pResizedBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);


			if (hBitmap) {
				// Liberar el bitmap antiguo
				HBITMAP hOldBitmap = (HBITMAP)SendDlgItemMessage(hWnd, IDC_PIC_REG, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
				if (hOldBitmap) {
					DeleteObject(hOldBitmap);
				}
				// Actualizar el control
				HWND hPictureControl = GetDlgItem(hWnd, IDC_PIC_REG);
				InvalidateRect(hPictureControl, NULL, TRUE);
				UpdateWindow(hPictureControl);
			}
			delete pOriginalBitmap;
			delete pResizedBitmap;
		}

		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Perro");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Gato");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Ave");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), CB_ADDSTRING, 0, (LPARAM)L"Conejo");


		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), CB_ADDSTRING, 0, (LPARAM)L"Activa");
		SendMessage(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), CB_ADDSTRING, 0, (LPARAM)L"Urgente");

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId)
		{
		case ID_BTN_RegCitaOK: {
			string cedulaUsuario = cedulaUsuarioActivo();
			// Leemos los datos desde el UI
			wchar_t fechaCita[255];
			GetDlgItemText(hWnd, ID_DATE_RegistroFecha, fechaCita, 255);

			wchar_t horaCita[255];
			GetDlgItemText(hWnd, ID_DATE_RegistroHora, horaCita, 255);

			SYSTEMTIME fecha;
			HWND hCal = GetDlgItem(hWnd, ID_DATE_RegistroFecha);
			MonthCal_GetCurSel(hCal, &fecha);
			wchar_t fechaCita2[255];
			wsprintf(fechaCita2, L"%02d/%02d/%04d", fecha.wDay, fecha.wMonth, fecha.wYear);

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaNombre, nombreCliente, 255);

			wchar_t telefonoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaTel, telefonoCita, 255);

			wchar_t nombreMascota[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaMascota, nombreMascota, 255);

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaCosto, costoCita, 255);

			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, ID_EC_RegCitaMotivo, motivoCita, 255);

			wchar_t especie[255];
			GetDlgItemText(hWnd, ID_COMB_RegCitaEspecie, especie, 255);

			wchar_t statusCita[255];
			GetDlgItemText(hWnd, ID_COMB_RegCitaStatus, statusCita, 255);

			// Convertir de wchar_t a wstring
			wstring fechaStr(fechaCita);
			wstring fechaStr2(fechaCita2);
			wstring horaStr(horaCita);
			wstring nombreClienteStr(nombreCliente);
			wstring telefonoCitaStr(telefonoCita);
			wstring nombreMascotaStr(nombreMascota);
			wstring costoCitaeStr(costoCita);
			wstring motivoCitaStr(motivoCita);
			wstring especieStr(especie);
			wstring statusCitaStr(statusCita);

			// Convertir de wstring a string
			string fechaA(fechaStr.begin(), fechaStr.end());
			string fechaA2(fechaStr2.begin(), fechaStr2.end());
			string horaA(horaStr.begin(), horaStr.end());
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());
			string telefonoCitaA(telefonoCitaStr.begin(), telefonoCitaStr.end());
			string nombreMascotaA(nombreMascotaStr.begin(), nombreMascotaStr.end());
			string costoCitaeA(costoCitaeStr.begin(), costoCitaeStr.end());
			string motivoCitaA(motivoCitaStr.begin(), motivoCitaStr.end());
			string especieA(especieStr.begin(), especieStr.end());
			string statusCitaA(statusCitaStr.begin(), statusCitaStr.end());


			if (!validarCaracteres(nombreClienteA))
			{
				MessageBox(hWnd, L"Nombre del cliente no valido!", L"ERROR!", MB_OK);
				break;
			}

			if (!validarCaracteres(nombreMascotaA))
			{
				MessageBox(hWnd, L"Nombre de la mascota no valido!", L"ERROR!", MB_OK);
				break;
			}

			if (telefonoCitaA.length() < 8 || telefonoCitaA.length() > 10) {
				MessageBox(hWnd, L"El telefono debe estar entre 8 y 10 digitos", L"Agregado", MB_OK);
				break;
			}

			if (!buscarFechaDisponible(fechaA2, horaA, cedulaUsuario)) {
				MessageBox(hWnd, L"Fecha u hora no válidas!", L"ERROR!", MB_OK);
				break;
			}

			if (!esFechaHoraValida(fechaA2, horaA)) {
				MessageBox(hWnd, L"Fecha u hora no válidas!", L"ERROR!", MB_OK);
				break;
			}



			if (fechaA2 == "" || horaA == "" || nombreClienteA == "" || telefonoCitaA == "" || nombreMascotaA == "" || costoCitaeA == "" || motivoCitaA == "" || especieA == "" || statusCitaA == "") {
				MessageBox(hWnd, L"No pueden existir campos vacios!", L"ERROR!", MB_OK);
				break;
			}

			// Obtener el costo como float 
			float costoCitaFloat = 0.0f;
			if (costoCita[0] != L'\0') {
				costoCitaFloat = stof(costoCita);
			}

			// Crear la estructura CITA

			CITA* nuevo = crearCita(nombreClienteA, fechaA2, horaA, telefonoCitaA, nombreMascotaA, motivoCitaA, costoCitaFloat, especieA, statusCitaA, cedulaUsuario);

			agregarCitaFinal(nuevo);

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se agrego exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Agregado", MB_OK);

			guardarCitasEnArchivo("CitasMiCat.bin");

			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, ID_DATE_RegistroFecha), L"");
				SetWindowText(GetDlgItem(hWnd, ID_DATE_RegistroHora), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaNombre), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaTel), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaMascota), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaCosto), L"");
				SetWindowText(GetDlgItem(hWnd, ID_EC_RegCitaMotivo), L"");
				SetWindowText(GetDlgItem(hWnd, ID_COMB_RegCitaStatus), L"");
				SetWindowText(GetDlgItem(hWnd, ID_COMB_RegCitaEspecie), L"");
				//crear una opcion para limpiar todo y no colocar todo el codigo aqui

			}

		}break;
		case ID_OPCIONES_CERRARSESION: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal); //REVISAR
		} break;
		case ID_OPCIONES_REGRESAR: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Menu), VentanaUsuario);
		} break;
		}
	}break;
	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	}break;
	default:
		break;
	}

	return FALSE;
}

LRESULT CALLBACK VentanaEditarCita(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg)
	{

	case WM_INITDIALOG: {

		//Lineas de la imaggen
		string cedulaUsuario = cedulaUsuarioActivo();
		NODOVET* veterrinatioActual = rutaImagenUsuario(cedulaUsuario);

		string rutaCompleta = "C:\\Users\\isaia\\OneDrive\\Escritorio\\" + veterrinatioActual->veterinario->foto;

		wstring nombreVeterinario(veterrinatioActual->veterinario->nombreVeterinario.begin(), veterrinatioActual->veterinario->nombreVeterinario.end());
		HWND hwndNombreMascota = GetDlgItem(hWnd, IDC_NOMBRE_VET_EDITAR);
		SetWindowText(hwndNombreMascota, nombreVeterinario.c_str());
		std::wstring rutaImagenW = stringToWString(rutaCompleta);
		Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromFile(rutaImagenW.c_str());

		if (pOriginalBitmap)
		{
			// Crear un nuevo bitmap con el tamaño deseado
			Gdiplus::Bitmap* pResizedBitmap = new Gdiplus::Bitmap(100, 100, PixelFormat32bppARGB);

			// Crear un objeto Graphics para dibujar en el nuevo bitmap
			Gdiplus::Graphics graphics(pResizedBitmap);
			graphics.DrawImage(pOriginalBitmap, 0, 0, 100, 100);

			// Convertir el bitmap redimensionado a HBITMAP
			HBITMAP hBitmap;
			pResizedBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);


			if (hBitmap) {
				// Liberar el bitmap antiguo
				HBITMAP hOldBitmap = (HBITMAP)SendDlgItemMessage(hWnd, IDC_FOTO_EDITAR, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
				if (hOldBitmap) {
					DeleteObject(hOldBitmap);
				}
				// Actualizar el control
				HWND hPictureControl = GetDlgItem(hWnd, IDC_FOTO_EDITAR);
				InvalidateRect(hPictureControl, NULL, TRUE);
				UpdateWindow(hPictureControl);
			}
			delete pOriginalBitmap;
			delete pResizedBitmap;
		}
		SendMessage(GetDlgItem(hWnd, ID_COMB_EDIT_Status), CB_ADDSTRING, 0, (LPARAM)L"Completada");
		SendMessage(GetDlgItem(hWnd, ID_COMB_EDIT_Status), CB_ADDSTRING, 0, (LPARAM)L"Cancelada");

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_BTN_Edit_BuscarCita: {

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_Edit_IngCita, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();
			NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
			if (cita == nullptr) { //si no existen ps no pasa nada
				MessageBox(hWnd, L"No existen citas", L"ERROR", MB_OK);
			}

			bool clienteEncontrado = false;
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_Editar); //obter el ID del lisBox
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			while (cita != nullptr) { //Mientras la lista tenga datos
				if (cita->cita->statusCita != "Cancelada" && cita->cita->cedulaVeterinario == cedulaUsuario && cita->cita->nombreCliente == nombreClienteA) {
					wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
					wstring listaCitas = nombreMacota + L": " + fechaStr + L" " + horaStr;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
					clienteEncontrado = true;
				}cita = cita->siguiente; //avanzamos 1 lugar en la fila
			}

			if (!clienteEncontrado) {
				wstring message = L"La cita de " + wstring(nombreCliente) + L" no se encontró";
				int respuesta = MessageBox(hWnd, message.c_str(), L"ERROR", MB_OK);
				return FALSE;
			}

		}break;
		case ID_BTN_EditarOK: {

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_Edit_IngCita, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			//REPETIR EN ELIMINAR
			wchar_t nombreMascota[255];
			GetDlgItemText(hWnd, IDC_EDIT_MASCOTA, nombreMascota, 255);
			wstring nombreMascotaStr(nombreMascota);
			string nombreMascotaA(nombreMascotaStr.begin(), nombreMascotaStr.end());


			string cedulaUsuario = cedulaUsuarioActivo();

			NODOCITA* cita = buscarCita(nombreClienteA, nombreMascotaA, cedulaUsuario);

			if (cita == NULL) {
				wstring message = L"La cita de " + wstring(nombreCliente) + L" no se encontró";
				int respuesta = MessageBox(hWnd, message.c_str(), L"ERROR", MB_OK);
				return FALSE;

			}

			if (!esFechaValida(cita->cita->fechaCita)) {
				MessageBox(hWnd, L"No se puede modificar una cita pasada!", L"ERROR!", MB_OK);
				break;
			}

			wchar_t costoCita[255];
			GetDlgItemText(hWnd, ID_EC_EDIT_Costo, costoCita, 255);
			wchar_t motivoCita[255];
			GetDlgItemText(hWnd, ID_EC_EDIT_Motivo, motivoCita, 255);
			wchar_t statusCita[255];
			GetDlgItemText(hWnd, ID_COMB_EDIT_Status, statusCita, 255);


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

			if (motivoCitaA == "" || statusCitaA == "" || costoCita == L"") {
				MessageBox(hWnd, L"No pueden existir valores vacios", L"ERROR!", MB_OK);
				break;
			}

			cita->cita->costo = costoCitaFloat;
			cita->cita->motivo = motivoCitaA;
			cita->cita->statusCita = statusCitaA;

			guardarCitasEnArchivo("CitasMiCat.bin");

			// Mostrar el nombre agregado en un messageBox
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se edito exitosamente!";
			int respuesta = MessageBox(hWnd, message.c_str(), L"Editado", MB_OK);
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_Editar);

			if (respuesta == IDOK) {
				SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			}

		}break;
		case ID_BTN_EditarCANCEL: {
		}break;
			//REPETIR EN ELIMINAR
		case ID_LBOX_Editar: {
			switch (HIWORD(wParam)) {
			case LBN_DBLCLK: {
				//Manejar doble clic en el ListBox

				int index = SendMessage(GetDlgItem(hWnd, ID_LBOX_Editar), LB_GETCURSEL, 0, 0);
				if (index != LB_ERR) {

					wchar_t selectedItem[256];
					SendMessage(GetDlgItem(hWnd, ID_LBOX_Editar), LB_GETTEXT, index, (LPARAM)selectedItem);
					wstring selectedItemW(selectedItem);
					string selectedItemStr(selectedItemW.begin(), selectedItemW.end());

					// Parsear la nombre
					string nombreMascotaStr = "";
					size_t pos = selectedItemStr.find(':');
					if (pos != std::string::npos) {
						nombreMascotaStr = selectedItemStr.substr(0, pos);
						// Ahora 'nombre' contiene de la mascota
					}

					wchar_t nombreCliente[255];
					GetDlgItemText(hWnd, ID_EC_Edit_IngCita, nombreCliente, 255);
					wstring nombreClienteStr(nombreCliente);
					string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

					string cedulaUsuario = cedulaUsuarioActivo();

					NODOCITA* cita = buscarCita(nombreClienteA, nombreMascotaStr, cedulaUsuario);

					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());

					wstring costoCita = to_wstring(cita->cita->costo);
					HWND hwndTextBox1 = GetDlgItem(hWnd, ID_EC_EDIT_Costo);
					SetWindowText(hwndTextBox1, costoCita.c_str());

					wstring nombreCita(cita->cita->nombreCliente.begin(), cita->cita->nombreCliente.end());
					HWND hwndTextBox2 = GetDlgItem(hWnd, ID_LBOX_Editar);
					SetWindowText(hwndTextBox2, nombreCita.c_str());

					wstring motivoCita(cita->cita->motivo.begin(), cita->cita->motivo.end());
					HWND hwndTextBox3 = GetDlgItem(hWnd, ID_EC_EDIT_Motivo);
					SetWindowText(hwndTextBox3, motivoCita.c_str());


					wstring nombreMascota(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					HWND hwndTextBox4 = GetDlgItem(hWnd, IDC_EDIT_MASCOTA);
					SetWindowText(hwndTextBox4, nombreMascota.c_str());

					HWND hwndButton = GetDlgItem(hWnd, ID_BTN_EditarOK);
					if (hwndButton != nullptr) {
						EnableWindow(hwndButton, TRUE); // Habilitar el botón
					}
				}

			}break;
			}
		}break;
		case ID_OPCIONES_CERRARSESION: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal); //REVISAR
		} break;
		case ID_OPCIONES_REGRESAR: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Menu), VentanaUsuario);
		} break;
		}
	}break;
	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	}break;
	default:
		break;
	}

	return FALSE;

}

LRESULT CALLBACK VentanaEliminarCita(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {

	switch (msg) {
	case WM_INITDIALOG: {

		//Lineas de la imaggen
		string cedulaUsuario = cedulaUsuarioActivo();
		NODOVET* veterrinatioActual = rutaImagenUsuario(cedulaUsuario);
		string rutaCompleta = "C:\\Users\\isaia\\OneDrive\\Escritorio\\" + veterrinatioActual->veterinario->foto;

		wstring nombreVeterinario(veterrinatioActual->veterinario->nombreVeterinario.begin(), veterrinatioActual->veterinario->nombreVeterinario.end());
		HWND hwndNombreMascota = GetDlgItem(hWnd, IDC_NOMBRE_VET_ELIMINAR);
		SetWindowText(hwndNombreMascota, nombreVeterinario.c_str());

		std::wstring rutaImagenW = stringToWString(rutaCompleta);
		Gdiplus::Bitmap* pOriginalBitmap = Gdiplus::Bitmap::FromFile(rutaImagenW.c_str());

		if (pOriginalBitmap)
		{
			// Crear un nuevo bitmap con el tamaño deseado
			Gdiplus::Bitmap* pResizedBitmap = new Gdiplus::Bitmap(100, 100, PixelFormat32bppARGB);

			// Crear un objeto Graphics para dibujar en el nuevo bitmap
			Gdiplus::Graphics graphics(pResizedBitmap);
			graphics.DrawImage(pOriginalBitmap, 0, 0, 100, 100);

			// Convertir el bitmap redimensionado a HBITMAP
			HBITMAP hBitmap;
			pResizedBitmap->GetHBITMAP(Gdiplus::Color(255, 255, 255), &hBitmap);


			if (hBitmap) {
				// Liberar el bitmap antiguo
				HBITMAP hOldBitmap = (HBITMAP)SendDlgItemMessage(hWnd, IDC_FOTO_ELIMINAR, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hBitmap);
				if (hOldBitmap) {
					DeleteObject(hOldBitmap);
				}
				// Actualizar el control
				HWND hPictureControl = GetDlgItem(hWnd, IDC_FOTO_ELIMINAR);
				InvalidateRect(hPictureControl, NULL, TRUE);
				UpdateWindow(hPictureControl);
			}
			delete pOriginalBitmap;
			delete pResizedBitmap;
		}

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		switch (wmId) {
		case ID_BTN_BuscarCitaEliminar: {

			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_SelCitaAEliminar, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

			string cedulaUsuario = cedulaUsuarioActivo();
			NODOCITA* cita = LISTACITAS.origen; //Obtener la primera cita de todas las citas
			if (cita == nullptr) { //si no existen ps no pasa nada
				MessageBox(hWnd, L"No existen citas", L"ERROR", MB_OK);
			}

			bool clienteEncontrado = false;
			HWND hListBox = GetDlgItem(hWnd, ID_LBOX_CitaEliminar); //obter el ID del lisBox
			SendMessage(hListBox, LB_RESETCONTENT, 0, 0);
			while (cita != nullptr) { //Mientras la lista tenga datos
				if (cita->cita->statusCita != "Cancelada" && cita->cita->cedulaVeterinario == cedulaUsuario && cita->cita->nombreCliente == nombreClienteA) {
					wstring nombreMacota = wstring(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					wstring fechaStr = wstring(cita->cita->fechaCita.begin(), cita->cita->fechaCita.end());
					wstring horaStr = wstring(cita->cita->horaCita.begin(), cita->cita->horaCita.end());
					wstring listaCitas = nombreMacota + L": " + fechaStr + L" " + horaStr;
					SendMessage(hListBox, LB_ADDSTRING, 0, (LPARAM)listaCitas.c_str());
					clienteEncontrado = true;
				}cita = cita->siguiente; //avanzamos 1 lugar en la fila
			}

			if (!clienteEncontrado) {
				wstring message = L"La cita de " + wstring(nombreCliente) + L" no se encontró";
				int respuesta = MessageBox(hWnd, message.c_str(), L"ERROR", MB_OK);
				return FALSE;
			}

		}break;
		case ID_BTN_EliminarOK: {
			// Leemos los datos desde el UI
			wchar_t nombreCliente[255];
			GetDlgItemText(hWnd, ID_EC_SelCitaAEliminar_oculto, nombreCliente, 255);
			wstring nombreClienteStr(nombreCliente);

			// Convertir de wstring a string
			string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());
			
			// Leemos los datos desde el UI
			wchar_t nombreMascota[255];
			GetDlgItemText(hWnd, IDC_DELETE_MASCOTA, nombreMascota, 255);
			wstring nombreMascotaStr(nombreMascota);

			// Convertir de wstring a string
			string nombreMascotaA(nombreMascotaStr.begin(), nombreMascotaStr.end());
			string cedulaUsuario = cedulaUsuarioActivo();
			NODOCITA* cita = buscarCita(nombreClienteA, nombreMascotaA, cedulaUsuario);//REVISAR
			if (!esFechaValida(cita->cita->fechaCita)) {
				MessageBox(hWnd, L"No se puede eliminar una cita pasada!", L"ERROR!", MB_OK);
				break;
			}

			CITA* citaEliminada = EliminarCitaMedio(nombreClienteA, nombreMascotaA, cedulaUsuario);//REVISAR

			if (citaEliminada != nullptr) {
				MessageBox(hWnd, L"Cita eliminada", L"Exito", MB_OK);
				delete citaEliminada;
			}
			else {
				MessageBox(hWnd, L"No se encontro ninguna cita con ese nombre", L"Error", MB_OK);
			}

			guardarCitasEnArchivo("CitasMiCat.bin");

		}break;
		case ID_LBOX_CitaEliminar: {
			switch (HIWORD(wParam)) {
			case LBN_DBLCLK: {
				//Manejar doble clic en el ListBox

				int index = SendMessage(GetDlgItem(hWnd, ID_LBOX_CitaEliminar), LB_GETCURSEL, 0, 0);
				if (index != LB_ERR) {

					wchar_t selectedItem[256];
					SendMessage(GetDlgItem(hWnd, ID_LBOX_CitaEliminar), LB_GETTEXT, index, (LPARAM)selectedItem);
					wstring selectedItemW(selectedItem);
					string selectedItemStr(selectedItemW.begin(), selectedItemW.end());


					// Parsear la nombre
					string nombreMascotaStr = "";
					size_t pos = selectedItemStr.find(':');
					if (pos != std::string::npos) {
						nombreMascotaStr = selectedItemStr.substr(0, pos);
						// Ahora 'nombre' contiene de la mascota
					}

					wchar_t nombreCliente[255];
					GetDlgItemText(hWnd, ID_EC_SelCitaAEliminar, nombreCliente, 255);
					wstring nombreClienteStr(nombreCliente);
					string nombreClienteA(nombreClienteStr.begin(), nombreClienteStr.end());

					string cedulaUsuario = cedulaUsuarioActivo();

					NODOCITA* cita = buscarCita(nombreClienteA, nombreMascotaStr, cedulaUsuario);

					HWND hwndNombreOculto = GetDlgItem(hWnd, ID_EC_SelCitaAEliminar_oculto);
					SetWindowText(hwndNombreOculto, nombreCliente);					


					wstring nombreMascota(cita->cita->nombreMascota.begin(), cita->cita->nombreMascota.end());
					HWND hwndNombreMascota = GetDlgItem(hWnd, IDC_DELETE_MASCOTA);
					SetWindowText(hwndNombreMascota, nombreMascota.c_str());

					HWND hwndButton = GetDlgItem(hWnd, ID_BTN_EliminarOK);
					if (hwndButton != nullptr) {
						EnableWindow(hwndButton, TRUE); // Habilitar el botón
					}
				}

			}break;
			}
		}break;

		case ID_OPCIONES_CERRARSESION: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal);
		} break;
		case ID_OPCIONES_REGRESAR: {
			EndDialog(hWnd, 0);
			return CrearVentanaYCentrar(hWnd, MAKEINTRESOURCE(ID_DIA_Menu), VentanaUsuario);
		} break;

		}
	}break;
	case WM_CLOSE: {
		int respuesta = MessageBox(hWnd, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	}break;
	default:
		break;
	}
	return FALSE;
}

CITA* crearCita(const string& nombreCliente, const string& fechaCita, const string& horaCita, const string telefonoCliente, const string& nombreMascota, const string& motivoCita, float costoCita, const string& especie, const string& statusCita, const string& cedula) {
	CITA* nuevo = new CITA;
	nuevo->nombreCliente = nombreCliente;
	nuevo->fechaCita = fechaCita;
	nuevo->horaCita = horaCita;
	nuevo->telefonoCliente = telefonoCliente;
	nuevo->nombreMascota = nombreMascota;
	nuevo->motivo = motivoCita;
	nuevo->costo = costoCita;
	nuevo->especie = especie;
	nuevo->statusCita = statusCita;
	nuevo->cedulaVeterinario = cedula;
	return nuevo;
}

void agregarCitaFinal(CITA* cita) {
	NODOCITA* nodo = nuevoNodoCita(cita);
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

bool buscarFechaDisponible(const string& fecha, const string& hora, const string& cedulaUsuario) {
	if (LISTACITAS.origen == NULL)
		return true;
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (indice->cita->fechaCita == fecha && indice->cita->horaCita == hora && indice->cita->cedulaVeterinario == cedulaUsuario)
			return false;
		indice = indice->siguiente;
	}
	return true;
}

NODOCITA* buscarCita(const string& buscarNomCliente, const string& nombreMascota, const string& cedulaUsuario) {
	if (LISTACITAS.origen == NULL)
		return NULL;
	NODOCITA* indice = LISTACITAS.origen;
	while (indice != NULL) {
		if (indice->cita->nombreCliente == buscarNomCliente && indice->cita->nombreMascota == nombreMascota && indice->cita->cedulaVeterinario == cedulaUsuario)
			break;
		indice = indice->siguiente;
	}
	return indice;
}

NODOCITA* nuevoNodoCita(CITA* cita) {
	NODOCITA* nodo = new NODOCITA;
	nodo->cita = cita;
	nodo->siguiente = NULL;
	return nodo;
}

CITA* EliminarCitaMedio(const string& nombre, const string& nombreMascota, const string& cedulaUsuario) {
	NODOCITA* busqueda = buscarCita(nombre, nombreMascota, cedulaUsuario);
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

NODOVET* nuevoNodoVeterinario(VETERINARIO* veterinario) {
	NODOVET* nodo = new NODOVET;
	nodo->veterinario = veterinario;
	nodo->siguiente = nullptr;
	return nodo;
}

bool validarUsuarioExistente(const string& nombreUsuario, const string& cedulaUsuario) {
	if (LISTAVETERINARIOS.origen == NULL)
		return TRUE;
	NODOVET* usuario = LISTAVETERINARIOS.origen;
	while (usuario != NULL) {
		if (usuario->veterinario->usuario == nombreUsuario)
			return FALSE;
		else if (usuario->veterinario->cedula == cedulaUsuario)
			return FALSE;
		usuario = usuario->siguiente;
	}
	return TRUE;
}

VETERINARIO* crearUsuario(const string& nombreVeterinario, const string& cedula, const string& usuario, const string& contrasena, const string& foto) {
	VETERINARIO* nuevo = new VETERINARIO;
	nuevo->nombreVeterinario = nombreVeterinario;
	nuevo->cedula = cedula;
	nuevo->usuario = usuario;
	nuevo->contrasena = contrasena;
	nuevo->foto = foto;
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

NODOVET* verificarUsuario(const string& usuario, const string& contrasena) {

	if (LISTAVETERINARIOS.origen == nullptr)
		return nullptr;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != nullptr) {
		if ((indice->veterinario->usuario == usuario) && (indice->veterinario->contrasena == contrasena)) {
			return indice;
		}
		indice = indice->siguiente;
	}
	return nullptr;
}

void EliminarListaVeterinarios() {
	while (LISTAVETERINARIOS.origen != NULL) {
		NODOVET* temporal = LISTAVETERINARIOS.origen;
		LISTAVETERINARIOS.origen = LISTAVETERINARIOS.origen->siguiente;
		if (temporal->veterinario != NULL)
			delete temporal->veterinario;
		delete temporal;
	}
}

//Archivo de Veterinarios

void guardarUsuariosEnArchivo(const string& nombreArchivo) {
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

void cargarUsuariosDesdeArchivo(const string& nombreArchivo) {
	std::ifstream archivo(nombreArchivo, std::ios::binary);

	if (archivo.is_open()) {
		// Primero, elimina todas las citas existentes en la lista para evitar duplicados
		EliminarListaVeterinarios();

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
		const VETERINARIO* vetData = reinterpret_cast<const VETERINARIO*>(buffer);
		size_t numVets = fileSize / sizeof(VETERINARIO);

		for (size_t i = 0; i < numVets; ++i) {
			VETERINARIO* nuevoVet = crearUsuario(vetData[i].nombreVeterinario, vetData[i].cedula, vetData[i].usuario, vetData[i].contrasena, vetData[i].foto);
			agregarVeterinarioFinal(nuevoVet);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}

string cedulaUsuarioActivo() {

	string cedulaUsuario;
	auto it = sesionesActivas.find("sesionUsuario");
	if (it == sesionesActivas.end())
		cedulaUsuario = "";

	USUARIO& usuario = it->second;
	cedulaUsuario = usuario.cedula;
	return cedulaUsuario;

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
			CITA* nuevaCita = crearCita(citaData[i].nombreCliente, citaData[i].fechaCita, citaData[i].horaCita, citaData[i].telefonoCliente, citaData[i].nombreMascota,
				citaData[i].motivo, citaData[i].costo, citaData[i].especie, citaData[i].statusCita, citaData[i].cedulaVeterinario);
			agregarCitaFinal(nuevaCita);
		}
		// Liberar la memoria del buffer
		delete[] buffer;
	}
	else {
		MessageBox(nullptr, L"No se pudo abrir el archivo para lectura.", L"Error", MB_OK);
	}
}

bool MenuUsuario(int opcion, HWND ventana) {
	switch (opcion)
	{
	case ID_MEN_Anadir: {
		EndDialog(ventana, 0);
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(ID_DIAL_RegCitas), VentanaRegistrarCita);
	} break;

	case ID_MEN_Editar: {
		EndDialog(ventana, 0);
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(ID_DIA_EditarCita), VentanaEditarCita);
	} break;

	case ID_MEN_Eliminar: {
		EndDialog(ventana, 0);
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(ID_DIA_EliminarCita), VentanaEliminarCita);
	} break;

	case ID_MEN_CerrarSesion: {
		EndDialog(ventana, 0);
		return CrearVentanaYCentrar(ventana, MAKEINTRESOURCE(ID_DIA_Login), VentanaPrincipal); //REVISAR
	} break;

	case ID_MEN_Salir: {
		EndDialog(ventana, 0);
		int respuesta = MessageBox(ventana, L"Seguro que desea cerrar el programa!", L"VETERINARIA", MB_OKCANCEL);
		if (respuesta == IDOK) {
			PostQuitMessage(117);
		}
	} break;

	default:
		return FALSE;
		break;
	}
	return true;
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

bool validarCaracteres(const string& textoValidar) {
	for (char c : textoValidar) {
		if (!((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))) {
			return false;
		}
	}
	return true;
}

bool esFechaHoraValida(const string& fechaCita, const string& horaCita) {

	// Parsear la fecha
	int dia, mes, year;
	sscanf_s(fechaCita.c_str(), "%d/%d/%d/%d", &dia, &mes, &year);

	// Configurar la estructura SYSTEMTIME con la fecha parseada
	SYSTEMTIME fechaComparar = { 0 };
	fechaComparar.wDay = dia;
	fechaComparar.wMonth = mes;
	fechaComparar.wYear = year;

	// Obtener la fecha actual
	SYSTEMTIME fechaActual;
	GetLocalTime(&fechaActual);

	//que sea el dia valido
	if (fechaComparar.wYear < fechaActual.wYear) {
		return false;
	}
	else if (fechaComparar.wYear == fechaActual.wYear) { //REVISAR
		if (fechaComparar.wMonth < fechaActual.wMonth) {
			return false;
		}
		else if (fechaComparar.wMonth == fechaActual.wMonth) {
			if (fechaComparar.wDay < fechaActual.wDay)
				return false;
		}
	}

	// Parsear la hora
	int hora, min, seg;
	char ampm[5] = {}; // Aumentar el tamaño para "a. m." o "p. m."
	sscanf_s(horaCita.c_str(), "%d:%d:%d %4s", &hora, &min, &seg, ampm, (unsigned)_countof(ampm));

	if (strcmp(ampm, "p.") == 0 && hora < 12) {
		hora += 12;
	}
	else if (strcmp(ampm, "a.") == 0 && hora == 12) {
		hora = 0;
	}
	// Configurar la estructura SYSTEMTIME con la fecha parseada
	SYSTEMTIME horaToCompare = { 0 };
	horaToCompare.wHour = hora;
	horaToCompare.wMinute = min;

	// Obtener la fecha actual
	SYSTEMTIME horaActual;
	GetLocalTime(&horaActual);

	//si es hoy que la hora sea valida
	if ((fechaComparar.wYear == fechaActual.wYear) && (fechaComparar.wMonth == fechaActual.wMonth) && (fechaComparar.wDay == fechaActual.wDay)) {
		if (horaToCompare.wHour < horaActual.wHour) {
			return false;
		}
		else if (horaToCompare.wHour == horaActual.wHour) // mi hora actual la da en 24hrs
		{
			if (horaToCompare.wMinute <= horaActual.wMinute) //SE LE PUEDE PONER UN LIMITE 10MIN DE LA HORA ACTUAL PARA ARRIBA
				return false;
		}
	}
	//Horario laboral
	if ((horaToCompare.wHour < 8) || (horaToCompare.wHour > 18))//REVISAR AM / PM 8am a 6pm
		return false;


	return true;
}

bool citaEsParaHoy(const string& fechaCita)
{
	// Parsear la fecha
	int day, month, year;
	sscanf_s(fechaCita.c_str(), "%d/%d/%d", &day, &month, &year);

	// Configurar la estructura SYSTEMTIME con la fecha parseada
	SYSTEMTIME fechaComparar = { 0 };
	fechaComparar.wDay = day;
	fechaComparar.wMonth = month;
	fechaComparar.wYear = year;

	// Obtener la fecha actual
	SYSTEMTIME fechaActual;
	GetLocalTime(&fechaActual);

	if ((fechaComparar.wYear != fechaActual.wYear) || (fechaComparar.wMonth != fechaActual.wMonth) || (fechaComparar.wDay != fechaActual.wDay)) {
		return false;
	}

	return true;
}

bool esFechaValida(const string& fechaCita)
{
	// Parsear la fecha
	int day, month, year;
	sscanf_s(fechaCita.c_str(), "%d/%d/%d", &day, &month, &year);

	// Configurar la estructura SYSTEMTIME con la fecha parseada
	SYSTEMTIME fechaComparar = { 0 };
	fechaComparar.wDay = day;
	fechaComparar.wMonth = month;
	fechaComparar.wYear = year;

	// Obtener la fecha actual
	SYSTEMTIME fechaActual;
	GetLocalTime(&fechaActual);

	if (fechaComparar.wYear < fechaActual.wYear)
	{
		return false;
	}
	else if (fechaComparar.wYear == fechaActual.wYear)
	{
		if (fechaComparar.wMonth < fechaActual.wMonth)
		{
			return false;
		}
		else if (fechaComparar.wMonth == fechaActual.wMonth)
		{
			return fechaComparar.wDay >= fechaActual.wDay;
		}
	}

	return true;
}

bool AbrirCuadroDialogo(HWND hWnd, wchar_t* rutaArchivo) {
	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFile = rutaArchivo;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrFilter = L"Todos los archivos\0*.*\0Archivos de imagen\0*.bmp;*.jpg;*.jpeg;*.png\0";
	ofn.nFilterIndex = 2;
	ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

	return GetOpenFileNameW(&ofn);
}

void InicializarAplicacion() {
	// Inicializar GDI+
	GdiplusStartupInput gdiplusStartupInput;
	GdiplusStartup(&tokenGdiplus, &gdiplusStartupInput, NULL);

	// Otras inicializaciones si son necesarias
}

void FinalizarAplicacion() {
	// Limpiar GDI+
	GdiplusShutdown(tokenGdiplus);
}

NODOVET* rutaImagenUsuario(const string& cedula) {
	if (LISTAVETERINARIOS.origen == NULL)
		return NULL;
	NODOVET* indice = LISTAVETERINARIOS.origen;
	while (indice != NULL) {
		if (indice->veterinario->cedula == cedula)
			return indice;
		indice = indice->siguiente;
	}
	return nullptr;

}

std::wstring stringToWString(const std::string& str) {
	std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
	return converter.from_bytes(str);
}

std::string reemplazarBarrasInvertidas(const std::string& ruta) {
	std::string rutaModificada = ruta;
	std::replace(rutaModificada.begin(), rutaModificada.end(), '\\', '/');
	return rutaModificada;
}

std::string obtenerNombreArchivo(const std::string& ruta) {
	size_t pos = ruta.find_last_of("/\\");
	return (pos != std::string::npos) ? ruta.substr(pos + 1) : ruta;
}

/*void copiarArchivoRuta(const std::string& rutaArchivoA, const std::string& cedula){

rutaArchivoA creas una carpeta dentro el proyecto pegas la imagen aqui, con el nombre de la cedula
c:Dise;oWinapi/Imagenes/cedula.png
}



*/