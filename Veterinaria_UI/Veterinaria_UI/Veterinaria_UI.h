#pragma once
#ifndef VETERINARIA_UI_H
#define VETERINARIA_UI_H

#include "resource.h"

//Descomentar y removerlos de .cpp 
LRESULT CALLBACK VentanaUsuario(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VentanaRegistro(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
LRESULT CALLBACK VentanaPrincipal(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool menuVentanas(int opcion, HWND ventana);
bool CrearVentanaYCentrar(HWND& ventana, LPCWSTR recursoVentana, DLGPROC procedimientoVentana);
void EliminarListaCita();
void guardarCitasEnArchivo(const char* nombreArchivo);
void cargarCitasDesdeArchivo(const char* nombreArchivo);

#endif