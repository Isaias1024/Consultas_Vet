// DemoCIOOPA.cpp : Define el punto de entrada de la aplicación.
//

#include "framework.h"
#include "DemoCIOOPA.h"
#include <string>
#include <commctrl.h>

#define MAX_LOADSTRING 100

using namespace std;

// Variables globales:
HINSTANCE hInst;                                // instancia actual
WCHAR szTitle[MAX_LOADSTRING];                  // Texto de la barra de título
WCHAR szWindowClass[MAX_LOADSTRING];            // nombre de clase de la ventana principal

// Declaraciones de funciones adelantadas incluidas en este módulo de código:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK    PrincipalCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
bool menu(int opcion, HWND ventana);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
	_In_opt_ HINSTANCE hPrevInstance,
	_In_ LPWSTR    lpCmdLine,
	_In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);


	// Inicializar cadenas globales
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_DEMOCIOOPA, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Realizar la inicialización de la aplicación:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_DEMOCIOOPA));

	MSG msg;

	// Bucle principal de mensajes:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int)msg.wParam;
}

//
//  FUNCIÓN: MyRegisterClass()
//
//  PROPÓSITO: Registra la clase de ventana.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_DEMOCIOOPA));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_DEMOCIOOPA);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassExW(&wcex);
}

//
//   FUNCIÓN: InitInstance(HINSTANCE, int)
//
//   PROPÓSITO: Guarda el identificador de instancia y crea la ventana principal
//
//   COMENTARIOS:
//
//        En esta función, se guarda el identificador de instancia en una variable común y
//        se crea y muestra la ventana principal del programa.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Almacenar el identificador de instancia en una variable global

	// Crear el diálogo principal
	HWND hWnd = CreateDialog(hInst, MAKEINTRESOURCE(VET_LOGIN), NULL, WndProc);

	if (!hWnd)
	{
		return FALSE;
	}

	// Obtener las dimensiones de la pantalla
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	// Obtener las dimensiones de la ventana
	RECT windowRect;
	GetWindowRect(hWnd, &windowRect);
	int windowWidth = windowRect.right - windowRect.left;
	int windowHeight = windowRect.bottom - windowRect.top;

	// Calcular las coordenadas para centrar la ventana
	int x = (screenWidth - windowWidth) / 2;
	int y = (screenHeight - windowHeight) / 2;

	// Mover la ventana a las coordenadas calculadas
	MoveWindow(hWnd, x, y, windowWidth, windowHeight, TRUE);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  FUNCIÓN: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PROPÓSITO: Procesa mensajes de la ventana principal.
//
//  WM_COMMAND  - procesar el menú de aplicaciones
//  WM_PAINT    - Pintar la ventana principal
//  WM_DESTROY  - publicar un mensaje de salida y volver
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {

	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		// Analizar las selecciones de menú:
		switch (wmId) {
		case LOGIN: {
			HWND ventana =
				CreateDialog(hInst,
					MAKEINTRESOURCE(VET_PRINCIPAL),
					NULL,
					PrincipalCallback);
			ShowWindow(ventana, SW_SHOW);
			EndDialog(hWnd, 0);
		}break;
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
				   break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Agregar cualquier código de dibujo que use hDC aquí...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Controlador de mensajes del cuadro Acerca de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND: {
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
	}break;
	}
	return (INT_PTR)FALSE;
}

LRESULT CALLBACK PrincipalCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		if (menu(wmId, hWnd)) {
			return FALSE;
		}
		switch (wmId) {
		case WM_DESTROY: {
			int respuesta = MessageBox(hWnd, L"NO ME CIERRES!", L"Deseas Cerrar?", MB_OKCANCEL);
			if (respuesta == MB_OKCANCEL) {
				EndDialog(hWnd, 0);
			}
		}break;
		case PRINCIPAL_MOVER: {
			RECT rect;
			if (GetWindowRect(hWnd, &rect)) {
				int width = rect.right - rect.left;
				int height = rect.bottom - rect.top;
				MoveWindow(hWnd, 300, 200, width, height, true);
			}
		}
		}
	}break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaEditCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		if (menu(wmId, hWnd)) {
			return FALSE;
		}
		switch (wmId) {
		case WM_DESTROY: {
			int respuesta = MessageBox(hWnd, L"NO ME CIERRES!", L"Deseas Cerrar?", MB_OKCANCEL);
			if (respuesta == MB_OKCANCEL) {
				EndDialog(hWnd, 0);
			}
		}break;
		case IDC_EDITAR_CITA: {
			WCHAR textow[255];
			GetDlgItemText(hWnd,
				VET_NOMBRE,
				textow,
				255);
			/*MessageBox(hWnd,
				textow,
				L"Tu nombre es:",
				0);*/
			float num = wcstof(textow, NULL);
			num = num * 5;
			wstring snum = to_wstring(num);
			SendMessage(
				GetDlgItem(hWnd, IDC_LIST1),
				LB_ADDSTRING,
				0,
				(LPARAM)snum.c_str());
		}break;
		case IDC_BUTTON2: {
			wstring nombre(L"");
			SetDlgItemText(hWnd,
				VET_NOMBRE,
				nombre.c_str());
			SYSTEMTIME fecha = { 0 };
			fecha.wYear = 2020;
			fecha.wMonth = 3;
			fecha.wDay = 17;
			fecha.wHour = 5;
			fecha.wMinute = 30;
			fecha.wSecond = 10;
			DateTime_SetSystemtime(
				GetDlgItem(hWnd, IDC_DATETIMEPICKER1),
				0, //GDT_VALID
				&fecha);
			DateTime_SetSystemtime(
				GetDlgItem(hWnd, IDC_DATETIMEPICKER2),
				0, //GDT_VALID
				&fecha);
		}break;
		case IDC_LIST1: {
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {
				int opc =
					SendMessage(
						GetDlgItem(hWnd, IDC_LIST1),
						LB_GETCURSEL,
						0, 0);
				if (opc >= 0) {
					WCHAR textow[255] = { 0 };
					SendMessage(
						GetDlgItem(hWnd, IDC_LIST1),
						LB_GETTEXT,
						opc,
						(LPARAM)textow);
					MessageBox(hWnd,
						textow,
						L"Listbox", 0);
				}
			}break;
			}
		}break;
		case VET_NOMBRE: {
			switch (HIWORD(wParam)) {
			case EN_CHANGE: {
				/*MessageBox(hWnd,
					L"He cambiado",
					L"Atte El indomable",
					0);*/
			}break;
			}
		}break;
		}
	}break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaAgregarCallback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message) {
	case WM_INITDIALOG: {
		SendMessage(
			GetDlgItem(hWnd, IDC_ESPECIES),
			CB_INSERTSTRING,
			0,
			(LPARAM)L"Perro");
		SendMessage(
			GetDlgItem(hWnd, IDC_ESPECIES),
			CB_INSERTSTRING,
			0,
			(LPARAM)TEXT("Gato"));
		SendMessage(
			GetDlgItem(hWnd, IDC_ESPECIES),
			CB_INSERTSTRING,
			0,
			(LPARAM)L"Conejo");
	}break;
	case WM_COMMAND: {
		int wmId = LOWORD(wParam);
		if (menu(wmId, hWnd)) {
			return FALSE;
		}
		switch (wmId) {
		case WM_DESTROY: {
			int respuesta = MessageBox(hWnd, L"Adios!", L"Deseas Cerrar?", MB_OKCANCEL);
			if (respuesta == MB_OKCANCEL) {
				EndDialog(hWnd, 0);
			}
		}break;
		case IDC_AGREGAR_CITA: {
			TCHAR  nombreCliente[255];
			GetDlgItemText(hWnd, VET_NOMBRE, nombreCliente, 255);
			wstring message = L"La cita de " + wstring(nombreCliente) + L" se agrego exitosamente!";
			// Mostrar el nombre agregado en un messageBox
			int respuesta = MessageBox(hWnd, message.c_str(), L"Agregado", MB_OK);

			//agregarCitaFinal(crearCita("Isaias", "10-10-23", "Thor", "Vacunas", 100, "Normal"));

			//CITA nuevaCita;
			//_tcscpy_s(nuevaCita.nombreCliente, nombreCliente);
			//_tcscpy_s(nuevaCita.fechaHora, fechaHora);            
			//_tcscpy_s(nuevaCita.nombreMascota, nombreMascota);
			//_tcscpy_s(nuevaCita.motivo, motivo);
			//nuevaCita.costo = costo;
			//_tcscpy_s(nuevaCita.status, status);


			if (respuesta == IDOK) {
				// Limpiar el contenido del cuadro de texto
				SetWindowText(GetDlgItem(hWnd, VET_NOMBRE), L"");
				SetWindowText(GetDlgItem(hWnd, VET_NOMBRE_MASCOTA), L"");
				SetWindowText(GetDlgItem(hWnd, VET_COSTO), L"");
				SetWindowText(GetDlgItem(hWnd, VET_MOTIVO), L"");
				SetWindowText(GetDlgItem(hWnd, VET_STATUS), L"");
				SetWindowText(GetDlgItem(hWnd, IDC_ESPECIES), L"Seleccione");
				//crear una opcion para limpiar todo y no colocar todo el codigo aqui
			}
		}break;
		case IDC_LIST1: {
			switch (HIWORD(wParam)) {
			case LBN_SELCHANGE: {
				int opc =
					SendMessage(
						GetDlgItem(hWnd, IDC_LIST1),
						LB_GETCURSEL,
						0, 0);
				if (opc >= 0) {
					WCHAR textow[255] = { 0 };
					SendMessage(
						GetDlgItem(hWnd, IDC_LIST1),
						LB_GETTEXT,
						opc,
						(LPARAM)textow);
					MessageBox(hWnd,
						textow,
						L"Listbox", 0);
				}
			}break;
			}
		}break;
		case VET_NOMBRE: {
			switch (HIWORD(wParam)) {
			case EN_CHANGE: {
				/*MessageBox(hWnd,
					L"He cambiado",
					L"Atte El indomable",
					0);*/
			}break;
			}
		}break;
		}
	}break;
	}
	return FALSE;
}

LRESULT CALLBACK VentanaEjemplo8Callback(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {

	case WM_INITDIALOG: {
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_INSERTSTRING, 0, (LPARAM)L"Japon");
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_INSERTSTRING, 0, (LPARAM)TEXT("Rusia"));
		SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_INSERTSTRING, 0, (LPARAM)L"Mexico");

	}break;
	case WM_COMMAND: {
		int id = LOWORD(wParam);
		if (menu(id, hWnd))
			return FALSE;
		switch (id) {
		case IDC_EDITAR_CITA: {
			if (IsDlgButtonChecked(
				hWnd, IDC_RADIO1) == BST_CHECKED) {
				MessageBox(hWnd,
					L"Japones",
					L"Nacionalidad", 0);
			}
			else if (IsDlgButtonChecked(
				hWnd, IDC_RADIO2) == BST_CHECKED) {
				MessageBox(hWnd,
					L"Rusa",
					L"Nacionalidad", 0);
			}
			else if (IsDlgButtonChecked(
				hWnd, IDC_RADIO3) == BST_CHECKED) {
				MessageBox(hWnd,
					L"Mexicana",
					L"Nacionalidad", 0);
			}
		}break;
		case IDC_COMBO1: {
			switch (HIWORD(wParam)) {
			case CBN_SELENDOK: {
				int opc = SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETCURSEL, 0, 0);

				if (opc >= 0) {
					WCHAR textow[255] = { 0 };
					SendMessage(GetDlgItem(hWnd, IDC_COMBO1), CB_GETLBTEXT, opc, (LPARAM)textow);
					if (wcscmp(textow, L"Mexico") == 0) {
						SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_INSERTSTRING, 0, (LPARAM)L"Nuevo Leon");
						SendMessage(GetDlgItem(hWnd, IDC_COMBO2), CB_SETCURSEL, 0, 0); CheckDlgButton(hWnd, IDC_RADIO1, BST_UNCHECKED);
						CheckDlgButton(hWnd, IDC_RADIO2, BST_UNCHECKED);
						CheckDlgButton(hWnd,IDC_RADIO3,BST_CHECKED);
					}
					else {
						SendMessage(GetDlgItem(hWnd, IDC_COMBO2),CB_RESETCONTENT,0, 0);
					}
				}
			}break;
			}
		}break;
		}
	}break;
	}
	return FALSE;
}

bool menu(int opcion, HWND ventana) {


	switch (opcion) {
	case IDR_ARCHIVO_AGREGAR: {
		HWND ventana = CreateDialog(
			hInst,
			MAKEINTRESOURCE(IDC_VET_AGREGAR),
			NULL,
			VentanaAgregarCallback);

		ShowWindow(ventana, SW_SHOW);
	}break;
	case IDM_ABOUT: {
		DialogBox(hInst,
			MAKEINTRESOURCE(IDD_ABOUTBOX),
			ventana, About);
	}break;
	case IDR_ARCHIVO_EDITAR: {
		HWND ventana = CreateDialog(
			hInst,
			MAKEINTRESOURCE(VET_EDITCONTROL),
			NULL,
			VentanaEditCallback);

		ShowWindow(ventana, SW_SHOW);
	}break;
	case IDR_ARCHIVO_ELIMINAR: {
		HWND ventana = CreateDialog(
			hInst,
			MAKEINTRESOURCE(IDD_DIALOG1),
			NULL,
			VentanaEjemplo8Callback);
		ShowWindow(ventana, SW_SHOW);        }break;
	default: return false;
	}
	return true;
}