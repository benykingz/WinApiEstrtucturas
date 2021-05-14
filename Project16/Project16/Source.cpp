#include <windows.h>
#include "resource1.h"
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>
#include <iostream>
#include <commdlg.h>

using namespace std;

HINSTANCE hGlobalInst;

BOOL bloginExit = false;
CONST int NumLoginExit = 10;

#pragma region Nodos

struct users {
	string user;
	string name;
	string pass;
	users *suguiente;
	users *anterior;
}*originUs = NULL, *auxUs = NULL, *CurrentUs = NULL;


#pragma endregion Nodos

#pragma region stream

fstream binarioUsers;

#pragma endregion stream

char original_path[MAX_PATH];

#pragma region Funciones
void RegistroUser(string, string, string);
void GuardarUser(users *originUs);
void CargaUser();

void RegistroServicio(string, string, string);
#pragma endregion Funciones

#pragma region CALLBACKS
BOOL CALLBACK flogin(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK faltauser(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fmenu(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fservice(HWND, UINT, WPARAM, LPARAM);
#pragma endregion CALLBACKS


//Default de ventana
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, PSTR cmdline, int cShow) {
	hGlobalInst = hinst;
	HWND hlogin = CreateDialog(hinst, MAKEINTRESOURCE(ID_LOGING), NULL, flogin); //Crea la ventana de loggin
	ShowWindow(hlogin, cShow);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 1;
}

BOOL CALLBACK flogin(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		CargaUser();
		break;
	}

	case WM_COMMAND: {
		if (LOWORD(wparam) == BTN_Logging && HIWORD(wparam) == BN_CLICKED) { //boton loggin
			if (originUs == NULL) {
				MessageBox(NULL, "No existen usuarios registrados", "Error", MB_ICONERROR);
				break;
			}

			HWND hTxtUserCheck = GetDlgItem(hwnd, CT_UserCheck);
			HWND hTxtNewPassCheck = GetDlgItem(hwnd, CT_PassCheck);
			int iUserCheckLenght = GetWindowTextLength(hTxtUserCheck);
			int iPassCheckLenght = GetWindowTextLength(hTxtNewPassCheck);
			char cUserCheck[30];
			char cPassCheck[15];
			GetWindowText(hTxtUserCheck, cUserCheck, ++iUserCheckLenght);
			GetWindowText(hTxtNewPassCheck, cPassCheck, ++iPassCheckLenght);

			//validacion

			bool op = false;
			auxUs = originUs;
			while (auxUs != NULL) {
				if (auxUs->user.compare(cUserCheck) == 0) {
					if (auxUs->pass.compare(cPassCheck) == 0) {
						MessageBox(NULL, "Usuario encontrado", "Error", MB_ICONERROR);
						CurrentUs = auxUs;

						HWND hMenu = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_MENU), NULL, fmenu);
						ShowWindow(hMenu, SW_SHOW);
						DestroyWindow(hwnd);

						op = true;
						break;

					}
					else {
						MessageBox(NULL, "Contraseña incorrecta", "Error", MB_ICONERROR);
					}
				}
				else {
					auxUs = auxUs->suguiente;
				}
			}

			if (op != true) {
				MessageBox(NULL, "No se encontro usuario", "Error", MB_ICONERROR);
			}
		}
		else if (LOWORD(wparam) == BTN_NewUser && HIWORD(wparam) == BN_CLICKED) {
			DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_CreateUser), hwnd, faltauser);
		}
		break;
	}

	case WM_CLOSE:
		bloginExit = TRUE;
		DestroyWindow(hwnd);
		break;

	case WM_DESTROY:
		if (bloginExit) {
			PostQuitMessage(NumLoginExit);
		}

		break;
	

	}// fin del switch
	return false;
}

BOOL CALLBACK faltauser(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG:{
	}

	case WM_COMMAND:{
		if (LOWORD(wparam) == BTN_CheckIn && HIWORD(wparam) == BN_CLICKED) {

			HWND hTxtNewUser = GetDlgItem(hwnd, CT_IN_USER);
			HWND hTxtNewPassWord = GetDlgItem(hwnd, CT_IN_PASS);
			HWND hTxtNewName = GetDlgItem(hwnd, CT_IN_NAME);

			int iUserLenght = GetWindowTextLength(hTxtNewUser);
			int iPassWordLenght = GetWindowTextLength(hTxtNewPassWord);
			int iNameLenght = GetWindowTextLength(hTxtNewName);

			char cNewUser[30];
			char cNewPassWord[15];
			char cNewName[15];

			GetWindowText(hTxtNewUser, cNewUser, ++iUserLenght);
			GetWindowText(hTxtNewPassWord, cNewPassWord, ++iPassWordLenght);
			GetWindowText(hTxtNewName, cNewName, ++iNameLenght);

			RegistroUser(cNewUser, cNewName, cNewPassWord);
		}
	}

	}
	return false;
}



BOOL CALLBACK fmenu(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		break;
	}

	case WM_COMMAND: {

		if (LOWORD(wparam) == BTN_SERVICE && HIWORD(wparam) == BN_CLICKED) {
			
			DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaService), hwnd, fservice);
		}

		break;
	}

	}// fin del switch
	return false;
}

BOOL CALLBACK fservice(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		break;
	}

	case WM_COMMAND: {

		if (LOWORD(wparam) == BTN_AltaServ && HIWORD(wparam) == BN_CLICKED) {
			
			HWND hTxtClave = GetDlgItem(hwnd, CT_Clave);
			HWND hTxtDescripcion = GetDlgItem(hwnd, CT_Desc);
			HWND hTxtPrecio = GetDlgItem(hwnd, CT_Price);

			int iClave = GetWindowTextLength(hTxtClave);
			int iDescripcion = GetWindowTextLength(hTxtDescripcion);
			int iPrecio = GetWindowTextLength(hTxtPrecio);

			char cClave[30];
			char cDescripcion[50];
			char cPrecio[15];

			GetWindowText(hTxtClave, cClave, ++iClave);
			GetWindowText(hTxtDescripcion, cDescripcion, ++iDescripcion);
			GetWindowText(hTxtPrecio, cPrecio, ++iPrecio);

			RegistroServicio(cClave,cDescripcion,cPrecio); // rellenar

		}

		break;
	}

	}// fin del switch
	return false;
}



void RegistroServicio(string cClave, string cDescripcion, string cPrecio) {


	// rellenar
}

void RegistroUser(string rUser, string rName, string rPass) { //Registro de usuarios

	if (originUs == NULL) {
		originUs = new users;
		originUs->anterior = NULL;
		originUs->suguiente = NULL;
		originUs->name = rName;
		originUs->user = rUser;
		originUs->pass = rPass;
		auxUs = originUs;
		CurrentUs = originUs;
	}
	else{
		auxUs = originUs;
		while (auxUs->suguiente != NULL) {
			auxUs = auxUs->suguiente;
	    }
		auxUs->suguiente = new users;
		auxUs->suguiente->anterior = auxUs;
		auxUs = auxUs->suguiente;
		auxUs->suguiente = NULL;
		CurrentUs = auxUs;

		auxUs->name = rName;
		auxUs->user = rUser;
		auxUs->pass = rPass;
		CurrentUs = auxUs;
		auxUs = originUs;
	}
	GuardarUser(originUs);
}

void GuardarUser(users *originUs) { //Guardar usuarios
	const char* file_name = "Users.bin";

	binarioUsers.open(std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioUsers.is_open()) {
		while (originUs != NULL) {
			binarioUsers.write(reinterpret_cast<char*>(originUs), sizeof(users));

			users* tempUs = originUs;
			originUs = originUs->suguiente;
			delete tempUs;
		}
		MessageBox(NULL, "Guardado Usuarios", "Info", MB_ICONINFORMATION);
		binarioUsers.flush();
		binarioUsers.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "Info", MB_ICONERROR);
	}
	return;
}

void CargaUser() { //Carga de usuarios
	binarioUsers.open("Users.bin", ios::binary | ios::in | ios::ate);
	if (binarioUsers.is_open()) {
		int sizeOfFile = binarioUsers.tellg();
		if (sizeOfFile < 1) {
			binarioUsers.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(users); i++) {
			users* tempUs = new users;
			binarioUsers.seekg(i * sizeof(users));
			if (originUs == NULL) {
				originUs = new users;
				binarioUsers.read(reinterpret_cast<char*>(tempUs), sizeof(users));

				originUs->name = tempUs->name;
				originUs->pass = tempUs->pass;
				originUs->user = tempUs->user;

				originUs->suguiente = NULL;
				originUs->anterior = NULL;
				auxUs = originUs;
			}
			else {
				auxUs = originUs;
				while (auxUs->suguiente != NULL) {
					auxUs = auxUs->suguiente;
				}
				binarioUsers.read(reinterpret_cast<char*>(tempUs), sizeof(users));
				auxUs->suguiente = new users;
				auxUs->suguiente->anterior = auxUs;
				auxUs = auxUs->suguiente;
				auxUs->name = tempUs->name;
				auxUs->pass = tempUs->pass;
				auxUs->user = tempUs->user;
				
				auxUs = auxUs->suguiente = NULL;
				auxUs = originUs;
			}
			delete reinterpret_cast<char*>(tempUs);
		}
		binarioUsers.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨users¨", "Error", MB_ICONERROR);
	}
}



//atof(cPrice)