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

#pragma region bools
BOOL bloginExit = false;
BOOL X_EXIT = false;
CONST int NumLoginExit = 10;
CONST int NumUserExit = 11;
CONST int NumServiceExit = 12;
CONST int NumMecExit = 13;
CONST int NumPrincipalExit = 14;
CONST int NumClientExit = 15;
CONST int NumCitaExit = 16;
#pragma endregion bools

#pragma region Nodos

struct users {
	string user;
	string name;
	string pass;
	users *suguiente;
	users *anterior;
}*originUs = NULL, *auxUs = NULL, *CurrentUs = NULL;

struct Services {
	string clave;
	string description;
	float price;
	Services* siguiente;
	Services* anterior;
}*originServ = NULL, *auxServ = NULL;

struct Nbre {
	string nombre;
	string apellido1;
	string apellido2;
};

struct Mecanicos {
	int NumEmpl;
	Nbre mnbre;
	string phone;
	string Especial;
	string Turn;
	bool Dias[5];
	char bitmap[MAX_PATH];
	Mecanicos* siguiente;
	Mecanicos* anterior;
}*originMec = NULL, *auxMec, *CurrentMec = NULL;

struct car {
	string placa;
	string marca;
	string modelo;
	int Año;
	int kilometraje;
};

struct Clientes {
	int id;
	Nbre cnbre;
	car carc;
	string phone;
	string calle;
	string numcalle;
	Clientes* siguiente;
	Clientes* anterior;
}*origincl = NULL, *auxcl = NULL, *Currentcl = NULL;

int iContIdClient = 1;

struct fecha {
	int dia;
	int mes;
	int año;
	int hora;
};

struct citas {
	Nbre clname;
	Nbre mecname;
	char bitmap[MAX_PATH];
	fecha date;
	string service;
	string phone;
	string status;
	string cmotiv;
	int id;
	int idcl;
	int idmec;
	citas* siguiente;
	citas* anterior;
}*origincit = NULL, *auxcit = NULL, *Currentcit = NULL;

int iContIdCitas = 1;

#pragma endregion Nodos

#pragma region stream

fstream binarioUsers; 
fstream binarioServices;
fstream binarioMec;
fstream binarioClientes;
fstream binarioCitas;

#pragma endregion stream

char original_path[MAX_PATH];

BOOL bImagenSelecPet1;

#pragma region Funciones
void RegistroUser(string, string, string);
void GuardarUser(users *originUs);
void CargaUser();

bool RegistroServicio(string, string, string);
void GuardarServices(Services *originServ);
void CargaServicio();

bool RegistroMec(string, string, string, string, string, string, string, bool arrSem[5], char bit[MAX_PATH]);
void GuardarMec(Mecanicos *originMec);
void CargaMec();

bool RegistroCliente(string, string, string, string, string, string, string, string, string, string, string);
void guardarCl(Clientes *origincl);
void CargaCl();

string Stringmec(Mecanicos *punteroMec);
bool ValidacionFecha(string, string, string);
bool ValidacionDia(string, string, string, int);
bool RegistroCitas(int, int, string, string, string, string, string);
void GuardarCitas(citas *origincit);
void EliminarCitas(HWND hwnd);
bool ModificarCita(int, int, string, string, string, string, string);
void CargaCitas();

void setInfoCita(HWND hwnd, bool flag);

void actualizar(HWND hwnd);
void mostrarinfo(HWND hwnd);
void VaciarContenido(HWND hwnd);

void quickSort();
void _quickSort(Mecanicos *Prim, Mecanicos *Ultimo);
Mecanicos* particion(Mecanicos *l, Mecanicos *h);
void swap(Mecanicos* a, Mecanicos* b);

void quickSort2();
void _quickSort2(Clientes* l, Clientes *h);
Clientes* particion2(Clientes *l, Clientes *h);
void swap2(Clientes *a, Clientes *b);

void Reporte();
bool OnlyNumber(string aux);
bool OnlyPrice(string aux);
#pragma endregion Funciones

#pragma region CALLBACKS
BOOL CALLBACK flogin(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK faltauser(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fmenu(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fservice(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fmeca(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fcliente(HWND, UINT, WPARAM, LPARAM);
BOOL CALLBACK fcitas(HWND, UINT, WPARAM, LPARAM);
#pragma endregion CALLBACKS

//Default de ventana
int WINAPI WinMain(HINSTANCE hinst, HINSTANCE hprev, PSTR cmdline, int cShow) {

	GetCurrentDirectory(MAX_PATH, original_path);
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
		CargaServicio();
		CargaMec();
		CargaCl();
		CargaCitas();
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
						CurrentUs = auxUs;

						HWND hMenu = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_MENU), NULL, fmenu);
						ShowWindow(hMenu, SW_SHOW);
						DestroyWindow(hwnd);

						op = true;
						break;
					}
					else {
						MessageBox(NULL, "Contraseña incorrecta", "Error", MB_ICONERROR);
						op = true;
						break;
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
			HWND hMenu = CreateDialog(hGlobalInst, MAKEINTRESOURCE(ID_MENU), NULL, fmenu);
			ShowWindow(hMenu, SW_SHOW);
			DestroyWindow(hwnd);
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
			X_EXIT = FALSE;
			EndDialog(hwnd, NumUserExit);
		}
	    break;
	}
	case WM_CLOSE:
		X_EXIT = TRUE;
		EndDialog(hwnd, NumUserExit);
		break;
	}
	return false;
}

char auxImgBit[MAX_PATH];
bool bitcheck = false;
bool CitaFlag;

BOOL CALLBACK fmenu(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {

	switch (msg) {

	case WM_INITDIALOG: {
		//ID_LIST
		actualizar(hwnd); 
		VaciarContenido(hwnd);
		break;
	}
	case WM_COMMAND: {

		if (LOWORD(wparam) == ID_AltaServices){
			DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaService), hwnd, fservice);
		}
		else if (LOWORD(wparam) == ID_RegistroMecanicos) {
			if (originServ == NULL) {
				MessageBox(NULL, "Registro de mecanicos necesita servicios registrados", "Error", MB_ICONERROR);
			}
			else {
				DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaMecanico), hwnd, fmeca);
			}
		}
		else if (LOWORD(wparam) == ID_RegistroCliente) {
			DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaCliente), hwnd, fcliente);
		}
		else if (LOWORD(wparam) == ID_RegistroCitas) {
			if ((origincl == NULL)||(originServ == NULL)||(originMec == NULL)) {
				MessageBox(NULL, "Registro de citas necesita mecanicos, clientes y servicios registrados", "Error", MB_ICONERROR);
			}
			else {
				CitaFlag = false;
				DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaCitas), hwnd, fcitas);
				actualizar(hwnd);
				VaciarContenido(hwnd);
			}
		}
		else if (LOWORD(wparam) == ID_Salir) {

			Reporte();

			GuardarServices(originServ);
			GuardarMec(originMec);
			guardarCl(origincl);
			GuardarCitas(origincit);

			//	quickSort();
			//  quickSort2();

			DestroyWindow(hwnd);
		}
		else if (LOWORD(wparam) == ID_LIST && HIWORD(wparam) == LBN_SELCHANGE) {
			mostrarinfo(hwnd);
		}
		else if (LOWORD(wparam) == BTN_ERASE) {
		    EliminarCitas(hwnd);
			actualizar(hwnd);
			VaciarContenido(hwnd);
		}
		else if (LOWORD(wparam) == BTN_MOD) {

			HWND hLbCitas = GetDlgItem(hwnd, ID_LIST);
			int iSelectionIndex = SendMessage(hLbCitas, LB_GETCURSEL, NULL, NULL);
			int idSelected = SendMessage(hLbCitas, LB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);
			if (iSelectionIndex == -1) {
				break;
			}
			if (origincit == NULL) {
				break;
			}
			auxcit = origincit;
			while (auxcit->id != idSelected) {
				auxcit = auxcit->siguiente;
			}
			Currentcit = auxcit;
			auxcit = origincit;
			CitaFlag = true;
			DialogBox(hGlobalInst, MAKEINTRESOURCE(ID_AltaCitas), hwnd, fcitas);
			actualizar(hwnd);
			VaciarContenido(hwnd);

		}
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(NumPrincipalExit);
		break;

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

			if (RegistroServicio(cClave, cDescripcion, cPrecio) == true) {
				X_EXIT = FALSE;
				EndDialog(hwnd, NumServiceExit);
			}
		}
		break;
	}
	case WM_CLOSE:{
		X_EXIT = TRUE;
		EndDialog(hwnd, NumUserExit);
		break;
	    }

	}// fin del switch
	return false;
}

BOOL CALLBACK fmeca(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {

	case WM_INITDIALOG: {
		HWND cbComboTipo;
		cbComboTipo = GetDlgItem(hwnd, CMB_Turn);

		SendMessage(cbComboTipo, CB_ADDSTRING, 0, (LPARAM) "Mañana ");
		SendMessage(cbComboTipo, CB_ADDSTRING, 1, (LPARAM) "Tarde ");

		SendMessage(cbComboTipo, CB_SETCURSEL, 0, 0);

		HWND cbComboEspecial;
		cbComboEspecial = GetDlgItem(hwnd, CMB_claveesp);
		if (originServ == NULL) {
			SendMessage(cbComboEspecial, CB_ADDSTRING, 0, (LPARAM) "N/A ");
		}
		else {
			auxServ = originServ;
			while (auxServ != NULL) {
				SendMessage(cbComboEspecial, CB_ADDSTRING, 0, (LPARAM) auxServ->description.c_str());
				auxServ = auxServ->siguiente;
			}
		}
		SendMessage(cbComboEspecial, CB_SETCURSEL, 0, 0);

		break;
	}

	case WM_COMMAND: {
		if (LOWORD(wparam) == BTN_RegMec && HIWORD(wparam) == BN_CLICKED) {
			HWND hTxtNumEmpl = GetDlgItem(hwnd, CT_NumEmpl);
			HWND hTxtNombreMec = GetDlgItem(hwnd, CT_NombreMec);
			HWND hTxtAPE1 = GetDlgItem(hwnd, CT_APE1);
			HWND hTxtAPE2 = GetDlgItem(hwnd, CT_APE2);
			HWND hTxtPhone = GetDlgItem(hwnd, ID_Phone);
			HWND hTxtEspecial = GetDlgItem(hwnd, CMB_claveesp);
			HWND hTxtTurn = GetDlgItem(hwnd, CMB_Turn);

			int iNumEmpl = GetWindowTextLength(hTxtNumEmpl);
			int iNombreMec = GetWindowTextLength(hTxtNombreMec);
			int iAPE1 = GetWindowTextLength(hTxtAPE1);
			int iAPE2 = GetWindowTextLength(hTxtAPE2);
			int iPhone = GetWindowTextLength(hTxtPhone);
			int iEspecial = GetWindowTextLength(hTxtEspecial);
			int iTurn = GetWindowTextLength(hTxtTurn);

			char cNumEmpl[30];
			char cNombreMec[30];
			char cAPE1[30];
			char cAPE2[30];
			char cPhone[30];
			char cEspecial[30];
			char cTurn[30];

			GetWindowText(hTxtNumEmpl, cNumEmpl, ++iNumEmpl);
			GetWindowText(hTxtNombreMec, cNombreMec, ++iNombreMec);
			GetWindowText(hTxtAPE1, cAPE1, ++iAPE1);
			GetWindowText(hTxtAPE2, cAPE2, ++iAPE2);
			GetWindowText(hTxtPhone, cPhone, ++iPhone);
			GetWindowText(hTxtEspecial, cEspecial, ++iEspecial);
			GetWindowText(hTxtTurn, cTurn, ++iTurn);

			bool Sem[5] = {0,0,0,0,0};

			Sem[0] = IsDlgButtonChecked(hwnd, ID_CB_Lun);
			Sem[1] = IsDlgButtonChecked(hwnd, ID_CB_Mar);
			Sem[2] = IsDlgButtonChecked(hwnd, ID_CB_mier);
			Sem[3] = IsDlgButtonChecked(hwnd, ID_CB_Jue);
			Sem[4] = IsDlgButtonChecked(hwnd, ID_CB_Vier);

			if (bitcheck == false) {
				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
			}
			else {
				if (RegistroMec(cNumEmpl, cNombreMec, cAPE1, cAPE2, cPhone, cEspecial, cTurn, Sem, auxImgBit) == true) {
					X_EXIT = TRUE;
					EndDialog(hwnd, NumMecExit);
					bitcheck = false;
				}
			}		
		
		}
		else if (LOWORD(wparam) == BTN_BitLoad && HIWORD(wparam) == BN_CLICKED) { //BIT_image
			
			OPENFILENAME ImagenBusquedaPet1;
			ZeroMemory(&ImagenBusquedaPet1, sizeof(ImagenBusquedaPet1));
			char cDireccionImagen[MAX_PATH] = " ";

			ImagenBusquedaPet1.hwndOwner = hwnd;
			ImagenBusquedaPet1.nMaxFile = MAX_PATH;
			ImagenBusquedaPet1.lpstrDefExt = ".bmp";
			ImagenBusquedaPet1.lStructSize = sizeof(OPENFILENAME);
			ImagenBusquedaPet1.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
			ImagenBusquedaPet1.lpstrFile = cDireccionImagen;
			ImagenBusquedaPet1.lpstrFilter = "Mostrar imagenes\0*.bmp";

			if (GetOpenFileName(&ImagenBusquedaPet1) == TRUE) {
				bImagenSelecPet1 = TRUE;

				strcpy_s(auxImgBit, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, BIT_image);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);
				bitcheck = true;
			}
			else {
				bImagenSelecPet1 = FALSE;

				strcpy_s(auxImgBit, cDireccionImagen);
				HBITMAP hImagenDoc = NULL;
				hImagenDoc = (HBITMAP)LoadImage(NULL, cDireccionImagen, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControl = GetDlgItem(hwnd, BIT_image);
				SendMessage(hPictureControl, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenDoc);
				bitcheck = false;

				MessageBox(NULL, "No selecciono una imagen", "Alerta", MB_ICONWARNING);
			}
		}
		break;
	}

	case WM_CLOSE: {
		bitcheck = false;
		X_EXIT = TRUE;
		EndDialog(hwnd, NumMecExit);
		break;
	}

	}// fin del switch
	return false;
}

BOOL CALLBACK fcliente(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {

	case WM_INITDIALOG: {
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wparam) == BTN_ALTACL && HIWORD(wparam) == BN_CLICKED) {

			HWND hTxtNombreCL = GetDlgItem(hwnd, CT_NameClient);
			HWND hTxApellido1 = GetDlgItem(hwnd, CT_Apellido1);
			HWND hTxApellido2 = GetDlgItem(hwnd, CT_Apellido2);
			HWND hTxplaca = GetDlgItem(hwnd, CT_PLACAS);
			HWND hTxmarca = GetDlgItem(hwnd, CT_MARCA);
			HWND hTxmodel = GetDlgItem(hwnd, CT_MODEL);
			HWND hTxcy = GetDlgItem(hwnd, CT_CARYEAR);
			HWND hTxkilo = GetDlgItem(hwnd, CT_KILO);
			HWND hTxPHONECL = GetDlgItem(hwnd, CT_PHONECL);
			HWND hTxCALLE = GetDlgItem(hwnd, CT_CALLE);
			HWND hTxCNUM = GetDlgItem(hwnd, CT_CNUM);

			int iNombreCL = GetWindowTextLength(hTxtNombreCL);
			int iApeliido1 = GetWindowTextLength(hTxApellido1);
			int iApeliido2 = GetWindowTextLength(hTxApellido2);
			int iplaca = GetWindowTextLength(hTxplaca);
			int imarca = GetWindowTextLength(hTxmarca);
			int imodel = GetWindowTextLength(hTxmodel);
			int icaryear = GetWindowTextLength(hTxcy);
			int ikilo = GetWindowTextLength(hTxkilo);
			int iPHONECL = GetWindowTextLength(hTxPHONECL);
			int iCALLE = GetWindowTextLength(hTxCALLE);
			int iCNUM = GetWindowTextLength(hTxCNUM);

			char cNombreCL[30];
			char cApeliido1[30];
			char cApeliido2[30];
			char cplaca[30];
			char cmarca[30];
			char cmodel[30];
			char ccaryear[30];
			char ckilo[30];
			char cPHONECL[30];
			char cCALLE[30];
			char cCNUM[30];

			GetWindowText(hTxtNombreCL, cNombreCL, ++iNombreCL);
			GetWindowText(hTxApellido1, cApeliido1, ++iApeliido1);
			GetWindowText(hTxApellido2, cApeliido2, ++iApeliido2);
			GetWindowText(hTxplaca, cplaca, ++iplaca);
			GetWindowText(hTxmarca, cmarca, ++imarca);
			GetWindowText(hTxmodel, cmodel, ++imodel);
			GetWindowText(hTxcy, ccaryear, ++icaryear);
			GetWindowText(hTxkilo, ckilo, ++ikilo);
			GetWindowText(hTxPHONECL, cPHONECL, ++iPHONECL);
			GetWindowText(hTxCALLE, cCALLE, ++iCALLE);
			GetWindowText(hTxCNUM, cCNUM, ++iCNUM);

			if (RegistroCliente(cNombreCL, cApeliido1, cApeliido2, cplaca, cmarca, cmodel, ccaryear, ckilo, cPHONECL, cCALLE, cCNUM) == true) {
				X_EXIT = TRUE;
				EndDialog(hwnd, NumClientExit);
			}
		}
		break;
	}
	case WM_CLOSE: {
		X_EXIT = TRUE;
		EndDialog(hwnd, NumClientExit);
		break;
	}

	}
	return false;
}

BOOL CALLBACK fcitas(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	switch (msg) {

	case WM_INITDIALOG: {

		setInfoCita(hwnd, CitaFlag);
		break;
	}
	case WM_COMMAND: {
		if (LOWORD(wparam) == BTN_RegistrarCita && HIWORD(wparam) == BN_CLICKED) {
			HWND cbComboMec;
			cbComboMec = GetDlgItem(hwnd, CMB_Select_Mec);
			int iSelectionIndex = SendMessage(cbComboMec, CB_GETCURSEL, NULL, NULL);
			int idSelected = SendMessage(cbComboMec, CB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);

			HWND cbComboClient;
			cbComboClient = GetDlgItem(hwnd, CMB_Select_Client);
			int iSelectionIndex2 = SendMessage(cbComboClient, CB_GETCURSEL, NULL, NULL);
			int idSelected2 = SendMessage(cbComboClient, CB_GETITEMDATA, (WPARAM)iSelectionIndex2, NULL);

			HWND hTxttype = GetDlgItem(hwnd, CMB_Type);
			HWND hTxtHour = GetDlgItem(hwnd, CT_HOURC);
			HWND hTxtday = GetDlgItem(hwnd, CT_DAY);
			HWND hTxtmonth = GetDlgItem(hwnd, CT_MONTH);
			HWND hTxtyear = GetDlgItem(hwnd, ID_YEAR);

			int itype = GetWindowTextLength(hTxttype);
			int iHour = GetWindowTextLength(hTxtHour);
			int iday = GetWindowTextLength(hTxtday);
			int imonth = GetWindowTextLength(hTxtmonth);
			int iyear = GetWindowTextLength(hTxtyear);

			char ctype[30];
			char cHour[30];
			char cday[30];
			char cmonth[30];
			char cyear[30];

			GetWindowText(hTxttype, ctype, ++itype);
			GetWindowText(hTxtHour, cHour, ++iHour);
			GetWindowText(hTxtday, cday, ++iday);
			GetWindowText(hTxtmonth, cmonth, ++imonth);
			GetWindowText(hTxtyear, cyear, ++iyear);

			if (CitaFlag == false) {
				if (RegistroCitas(idSelected2, idSelected, cHour, cday, cmonth, cyear, ctype) == true) {
					X_EXIT = TRUE;
					EndDialog(hwnd, NumCitaExit);
				}
				
			}
			else if(CitaFlag == true) {
				ModificarCita(idSelected2, idSelected, cHour, cday, cmonth, cyear, ctype);
			}
		}
		else if (LOWORD(wparam) == BTN_LEFT && HIWORD(wparam) == BN_CLICKED) {
			HWND hTxtHour = GetDlgItem(hwnd, CT_HOURC);
			int iHour = GetWindowTextLength(hTxtHour);
			char cHour[6];
			GetWindowText(hTxtHour, cHour, ++iHour);
			int auxhour = atoi(cHour);
			if (auxhour > 7) {
				auxhour--;
				string numStr = std::to_string(auxhour);
				SetWindowText(hTxtHour, numStr.c_str());
			}
		}
		else if (LOWORD(wparam) == BTN_RIGHT && HIWORD(wparam) == BN_CLICKED) {
			HWND hTxtHour = GetDlgItem(hwnd, CT_HOURC);
			int iHour = GetWindowTextLength(hTxtHour);
			char cHour[6];
			GetWindowText(hTxtHour, cHour, ++iHour);
			int auxhour = atoi(cHour);
			if (auxhour < 23) {
				auxhour++;
				string numStr = std::to_string(auxhour);
				SetWindowText(hTxtHour, numStr.c_str());
			}
		}
		else if (LOWORD(wparam) == CMB_Type && HIWORD(wparam) == CBN_SELCHANGE) {
			HWND cbComboMec;
			cbComboMec = GetDlgItem(hwnd, CMB_Select_Mec);
			SendMessage(cbComboMec, CB_RESETCONTENT, NULL, NULL);

			HWND StaticMec;
			StaticMec = GetDlgItem(hwnd, ST_MecInfo);
			SetWindowText(StaticMec, "");

			HWND cbComboEspecial;
			cbComboEspecial = GetDlgItem(hwnd, CMB_Type);
			int iSelectionIndex = SendMessage(cbComboEspecial, CB_GETCURSEL, NULL, NULL);
			SendMessage(cbComboEspecial, CB_SETCURSEL, iSelectionIndex, NULL);
			int itype = GetWindowTextLength(cbComboEspecial);
			char ctype[30];
			GetWindowText(cbComboEspecial, ctype, ++itype);

			HBITMAP hImagenPet1 = NULL;
			hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
			HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWCIT);
			SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);


			auxMec = originMec;
			int index3 = 0;
			SendMessage(cbComboMec, CB_ADDSTRING, 0, (LPARAM) "N/A ");
			SendMessage(cbComboMec, CB_SETITEMDATA, (WPARAM)index3, (LPARAM)0);
			index3++;
			while (auxMec != NULL) {
				if ( (ctype == auxMec->Especial) || (iSelectionIndex == 0)) {
					string numStr = std::to_string(auxMec->NumEmpl);
					SendMessage(cbComboMec, CB_ADDSTRING, 0, (LPARAM)(auxMec->mnbre.nombre + " " + auxMec->mnbre.apellido1 + " " + auxMec->mnbre.apellido2).c_str());
					SendMessage(cbComboMec, CB_SETITEMDATA, (WPARAM)index3, (LPARAM)auxMec->NumEmpl);
					index3++;
				}
				auxMec = auxMec->siguiente;
			}
			SendMessage(cbComboMec, CB_SETCURSEL, 0, 0);
		}
		else if (LOWORD(wparam) == CMB_Select_Mec && HIWORD(wparam) == CBN_SELCHANGE) {

			HWND cbComboMec;
			cbComboMec = GetDlgItem(hwnd, CMB_Select_Mec);
			int iSelectionIndex = SendMessage(cbComboMec, CB_GETCURSEL, NULL, NULL);
			int idSelected = SendMessage(cbComboMec, CB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);

			HWND StaticMec;
			StaticMec = GetDlgItem(hwnd, ST_MecInfo);

			if (iSelectionIndex == 0) {
				SetWindowText(StaticMec, "");

				HBITMAP hImagenPet1 = NULL;
				hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
				HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWCIT);
				SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);
			}

			auxMec = originMec;
			while (auxMec != NULL) {
				if (auxMec->NumEmpl == idSelected) {
					string sInfoMec = Stringmec(auxMec);
					SetWindowText(StaticMec, (sInfoMec).c_str());

					HBITMAP hImagenPet1 = NULL;
					hImagenPet1 = (HBITMAP)LoadImage(NULL, auxMec->bitmap, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
					HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWCIT);
					SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

					break;
				}
				else {
					auxMec = auxMec->siguiente;
				}
			}
		}
		break;
	}
	case WM_CLOSE: {
		X_EXIT = TRUE;
		EndDialog(hwnd, NumCitaExit);
		break;
	}

	}// fin del switch
	return false;
}

#pragma region mecanicos

bool RegistroMec(string cNum, string cNombr, string ap1, string ap2, string cPhone, string cEsp, string cTurn, bool arrSem[5], char bit[MAX_PATH]) {

	if ((cNum.length() < 1) || (cNombr.length() < 1) || (ap1.length() < 1) || (ap2.length() < 1) || (cPhone.length() < 1) || (cEsp.length() < 1) || (cTurn.length() < 1)) {
		MessageBox(NULL, "debe de rellenar todos los datos", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(cNum) == true) {
		MessageBox(NULL, "El numero de empleado solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (atoi(cNum.c_str()) == 0) {
		MessageBox(NULL, "Numero de empleado no puede ser cero", "Error", MB_ICONERROR);
		return false;
	}

	auxMec = originMec;
	while (auxMec != NULL) {
		if (atoi(cNum.c_str()) == auxMec->NumEmpl) {
			MessageBox(NULL, "Numero de empleado coincide con uno existente", "Error", MB_ICONERROR);
			return false;
		}
		else {
			auxMec = auxMec->siguiente;
		}
	}

	if (OnlyNumber(cPhone) == true) {
		MessageBox(NULL, "El telefono de empleado solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if ( (cPhone.length() != 8) && (cPhone.length() != 11) && (cPhone.length() != 13)) {
		MessageBox(NULL, "Telefono debe de tener solo 8, 11 o 13 digitos", "Error", MB_ICONERROR);
		return false;
	}

	if ( (arrSem[0] == 0) && (arrSem[1] == 0) && (arrSem[2] == 0) && (arrSem[3] == 0) && (arrSem[4] == 0)) {
		MessageBox(NULL, "El mecanico debe de trabajar almenos un dia a la semana", "Error", MB_ICONERROR);
		return false;
	}

	if (originMec == NULL) {
		originMec = new Mecanicos;
		originMec->anterior = NULL;
		originMec->siguiente = NULL;

		for (size_t i = 0; i < 6; i++)
		{
			originMec->Dias[i] = arrSem[i];
		}		
		originMec->Especial = cEsp;
		originMec->mnbre.nombre = cNombr;
		originMec->mnbre.apellido1 = ap1;
		originMec->mnbre.apellido2 = ap2;
		originMec->NumEmpl = atoi(cNum.c_str());
		originMec->phone = cPhone;
		originMec->Turn = cTurn;
		strcpy_s(originMec->bitmap, bit);	
	}
	else {
		auxMec = originMec;
		while (auxMec->siguiente != NULL) {
			auxMec = auxMec->siguiente;
		}
		auxMec->siguiente = new Mecanicos;
		auxMec->siguiente->anterior = auxMec;
		auxMec = auxMec->siguiente;
		auxMec->siguiente = NULL;

		for (size_t i = 0; i < 6; i++)
		{
			auxMec->Dias[i] = arrSem[i];
		}
		auxMec->Especial = cEsp;
		auxMec->mnbre.nombre = cNombr;
		auxMec->mnbre.apellido1 = ap1;
		auxMec->mnbre.apellido2 = ap2;
		auxMec->NumEmpl = atoi(cNum.c_str());
		auxMec->phone = cPhone;
		auxMec->Turn = cTurn;
		strcpy_s(auxMec->bitmap, bit);

	}
	auxMec = originMec;
	return true;
}

void GuardarMec(Mecanicos *originMec) {
	const char* file_name = "//Mecanicos.bin";
	binarioMec.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioMec.is_open()) {
		while (originMec != NULL) {
			binarioMec.write(reinterpret_cast<char*>(originMec), sizeof(Mecanicos));

			Mecanicos* tempMec = originMec;
			originMec = originMec->siguiente;
			delete tempMec;
		}
		MessageBox(NULL, "Guardado Mecanicos", "Info", MB_ICONINFORMATION);
		binarioMec.flush();
		binarioMec.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "Info", MB_ICONERROR);
	}
	return;
}

void CargaMec() { //Carga de usuarios
	binarioMec.open("Mecanicos.bin", ios::binary | ios::in | ios::ate);
	if (binarioMec.is_open()) {
		int sizeOfFile = binarioMec.tellg();
		if (sizeOfFile < 1) {
			binarioMec.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(Mecanicos); i++) {
			Mecanicos* tempMec = new Mecanicos;
			binarioMec.seekg(i * sizeof(Mecanicos));
			if (originMec == NULL) {
				originMec = new Mecanicos;
				binarioMec.read(reinterpret_cast<char*>(tempMec), sizeof(Mecanicos));

				strcpy_s(originMec->bitmap, tempMec->bitmap);
				
				for (size_t i = 0; i < 5; i++)
				{
					originMec->Dias[i] = tempMec->Dias[i];
				}

				originMec->Especial = tempMec->Especial;
				originMec->NumEmpl = tempMec->NumEmpl;
				originMec->phone = tempMec->phone;
				originMec->Turn = tempMec->Turn;
				originMec->mnbre = tempMec->mnbre;
				originMec->siguiente = NULL;
				originMec->anterior = NULL;
				auxMec = originMec;
			}
			else {
				auxMec = originMec;
				while (auxMec->siguiente != NULL) {
					auxMec = auxMec->siguiente;
				}
				binarioMec.read(reinterpret_cast<char*>(tempMec), sizeof(Mecanicos));
				auxMec->siguiente = new Mecanicos;
				auxMec->siguiente->anterior = auxMec;
				auxMec = auxMec->siguiente;

				strcpy_s(auxMec->bitmap, tempMec->bitmap);

				for (size_t i = 0; i < 5; i++)
				{
					auxMec->Dias[i] = tempMec->Dias[i];
				}

				auxMec->Especial = tempMec->Especial;
				auxMec->mnbre = tempMec->mnbre;
				auxMec->NumEmpl = tempMec->NumEmpl;
				auxMec->phone = tempMec->phone;
				auxMec->Turn = tempMec->Turn;

				auxMec = auxMec->siguiente = NULL;
			}
			delete reinterpret_cast<char*>(tempMec);
		}
		binarioMec.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨Mecanicos¨", "Error", MB_ICONERROR);
	}
}

#pragma endregion mecanicos

#pragma region servicios

bool RegistroServicio(string cClave, string cDescripcion, string cPrecio) {

	if ((cClave.length() < 1) || (cDescripcion.length() < 1) || (cPrecio.length() < 1)) {
		MessageBox(NULL, "debe de rellenar todos los datos", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyPrice(cPrecio) == true) {
		MessageBox(NULL, "Precio no admite palabras o mas de un punto decimal", "Error", MB_ICONERROR);
		return false;
	}

	if (originServ == NULL) {
		originServ = new Services;
		originServ->anterior = NULL;
		originServ->siguiente = NULL;

		originServ->clave = cClave;
		originServ->description = cDescripcion;
		originServ->price = stof(cPrecio);

		auxServ = originServ;
	}
	else {
		auxServ = originServ;
		while (auxServ->siguiente != NULL) {
			auxServ = auxServ->siguiente;
		}
		auxServ->siguiente = new Services;
		auxServ->siguiente->anterior = auxServ;
		auxServ = auxServ->siguiente;
		auxServ->siguiente = NULL;

		auxServ->clave = cClave;
		auxServ->description = cDescripcion;
		auxServ->price = stof(cPrecio);

		auxServ = originServ;
	}
	return true;
}

void GuardarServices(Services *originServ) { //Guardar usuarios
	const char* file_name = "//Services.bin";

	binarioServices.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioServices.is_open()) {
		while (originServ != NULL) {
			binarioServices.write(reinterpret_cast<char*>(originServ), sizeof(Services));

			Services* tempServ = originServ;
			originServ = originServ->siguiente;
			delete tempServ;
		}
		MessageBox(NULL, "Guardado Servicios", "Info", MB_ICONINFORMATION);
		binarioServices.flush();
		binarioServices.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "Info", MB_ICONERROR);
	}
	return;
}

void CargaServicio() { //Carga de usuarios
	binarioServices.open("Services.bin", ios::binary | ios::in | ios::ate);
	if (binarioServices.is_open()) {
		int sizeOfFile = binarioServices.tellg();
		if (sizeOfFile < 1) {
			binarioServices.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(Services); i++) {
			Services* tempServ = new Services;
			binarioServices.seekg(i * sizeof(Services));
			if (originServ == NULL) {
				originServ = new Services;
				binarioServices.read(reinterpret_cast<char*>(tempServ), sizeof(Services));

				originServ->clave = tempServ->clave;
				originServ->description = tempServ->description;
				originServ->price = tempServ->price;

				originServ->siguiente = NULL;
				originServ->anterior = NULL;
				auxServ = originServ;
			}
			else {
				auxServ = originServ;
				while (auxServ->siguiente != NULL) {
					auxServ = auxServ->siguiente;
				}
				binarioServices.read(reinterpret_cast<char*>(tempServ), sizeof(Services));
				auxServ->siguiente = new Services;
				auxServ->siguiente->anterior = auxServ;
				auxServ = auxServ->siguiente;

				auxServ->clave = tempServ->clave;
				auxServ->description = tempServ->description;
				auxServ->price = tempServ->price;

				auxServ = auxServ->siguiente = NULL;
				auxServ = originServ;
			}
			delete reinterpret_cast<char*>(tempServ);
		}
		binarioServices.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨Services¨", "Error", MB_ICONERROR);
	}
}

#pragma endregion servicios

#pragma region usuarios

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
	const char* file_name = "//Users.txt";

	binarioUsers.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

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
	binarioUsers.open("Users.txt", ios::binary | ios::in | ios::ate);
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

#pragma endregion usuarios

#pragma region clientes

bool RegistroCliente(string ncl, string ap1, string ap2, string placa, string marca, string modelo, string cyear, string kilo, string phone, string calle, string numc){

	if ((ncl.length() < 1)  || (ap1.length() < 1) || (ap2.length() < 1) || (placa.length() < 1) || (marca.length() < 1) || (modelo.length() < 1)
		|| (cyear.length() < 1) || (kilo.length() < 1) || (phone.length() < 1) || (calle.length() < 1) || (numc.length() < 1)) {
		MessageBox(NULL, "debe de rellenar todos los datos", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(cyear) == true) {
		MessageBox(NULL, "Año solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyPrice(kilo) == true) {
		MessageBox(NULL, "kilometraje solo admite numeros o mas de un punto decimal", "Error", MB_ICONERROR);
		return false;
	}

	if (origincl == NULL) {
		origincl = new Clientes;
		origincl->anterior = NULL;
		origincl->siguiente = NULL;

		origincl->id = iContIdClient;
		iContIdClient++;
		origincl->cnbre.nombre = ncl;
		origincl->cnbre.apellido1 = ap1;
		origincl->cnbre.apellido2 = ap2;
		origincl->carc.placa = placa;
		origincl->carc.marca = marca;
		origincl->carc.modelo = modelo;
		origincl->carc.Año = stof(cyear);
		origincl->carc.kilometraje = stof(kilo);
		origincl->phone = phone;
		origincl->calle = calle;
		origincl->numcalle = numc;

		auxcl= origincl;
	}
	else {
		auxcl= origincl;
		while (auxcl->siguiente != NULL) {
			auxcl = auxcl->siguiente;
		}
		auxcl->siguiente = new Clientes;
		auxcl->siguiente->anterior = auxcl;
		auxcl = auxcl->siguiente;
		auxcl->siguiente = NULL;

		auxcl->id = iContIdClient;
		iContIdClient++;
		auxcl->cnbre.nombre = ncl;
		auxcl->cnbre.apellido1 = ap1;
		auxcl->cnbre.apellido2 = ap2;
		auxcl->carc.placa = placa;
		auxcl->carc.marca = marca;
		auxcl->carc.modelo = modelo;
		auxcl->carc.Año = stof(cyear);
		auxcl->carc.kilometraje = stof(kilo);
		auxcl->phone = phone;
		auxcl->calle = calle;
		auxcl->numcalle = numc;

		auxcl = origincl;
	}
	return true;
}

void guardarCl(Clientes *origincl) {
	const char* file_name = "//Clientes.bin";

	binarioClientes.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioClientes.is_open()) {
		while (origincl != NULL) {
			binarioClientes.write(reinterpret_cast<char*>(origincl), sizeof(Clientes));

			Clientes* tempUs = origincl;
			origincl = origincl->siguiente;
			delete tempUs;
		}
		MessageBox(NULL, "Guardado Clintes", "Info", MB_ICONINFORMATION);
		binarioClientes.flush();
		binarioClientes.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "Info", MB_ICONERROR);
	}
	return;
}

void CargaCl() {
	binarioClientes.open("Clientes.bin", ios::binary | ios::in | ios::ate);
	if (binarioClientes.is_open()) {
		int sizeOfFile = binarioClientes.tellg();
		if (sizeOfFile < 1) {
			binarioClientes.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(Clientes); i++) {
			Clientes* tempcl= new Clientes;
			binarioClientes.seekg(i * sizeof(Clientes));
			if (origincl == NULL) {
				origincl = new Clientes;
				binarioClientes.read(reinterpret_cast<char*>(tempcl), sizeof(Clientes));

				origincl->calle = tempcl->calle;
				origincl->carc = tempcl->carc;
				origincl->cnbre = tempcl->cnbre;
				origincl->id = tempcl->id;
				iContIdClient = origincl->id;
				origincl->numcalle = tempcl->numcalle;
				origincl->phone = tempcl->phone;

				origincl->siguiente = NULL;
				origincl->anterior = NULL;
				auxcl = origincl;
			}
			else {
				auxcl = origincl;
				while (auxcl->siguiente != NULL) {
					auxcl = auxcl->siguiente;
				}
				binarioClientes.read(reinterpret_cast<char*>(tempcl), sizeof(Clientes));
				auxcl->siguiente = new Clientes;
				auxcl->siguiente->anterior = auxcl;
				auxcl = auxcl->siguiente;

				auxcl->calle = tempcl->calle;
				auxcl->carc = tempcl->carc;
				auxcl->cnbre = tempcl->cnbre;
				auxcl->id = tempcl->id;
				iContIdClient = auxcl->id;
				auxcl->numcalle = tempcl->numcalle;
				auxcl->phone = tempcl->phone;

				auxcl = auxcl->siguiente = NULL;
				auxcl = origincl;
			}
			delete reinterpret_cast<char*>(tempcl);
		}
		iContIdClient++;
		binarioClientes.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨Cientes¨", "Error", MB_ICONERROR);
	}
}

#pragma endregion clientes

#pragma region Citas

void setInfoCita(HWND hwnd, bool flag) {

	int auxidnum = 0;

	HWND cbComboClient;
	cbComboClient = GetDlgItem(hwnd, CMB_Select_Client);
	int indexcl = 0;
	SendMessage(cbComboClient, CB_ADDSTRING, 0, (LPARAM) "N/A ");
	SendMessage(cbComboClient, CB_SETITEMDATA, (WPARAM)indexcl, (LPARAM)0);
	indexcl++;

	auxcl = origincl;
	while (auxcl != NULL) {
		if (Currentcit != NULL) {
			if (Currentcit->idcl == auxcl->id) {
				 auxidnum = indexcl;
			}
		}
		SendMessage(cbComboClient, CB_ADDSTRING, 0, (LPARAM)(auxcl->cnbre.nombre + " " + auxcl->cnbre.apellido1 + " " + auxcl->cnbre.apellido2).c_str());
		SendMessage(cbComboClient, CB_SETITEMDATA, (WPARAM)indexcl, (LPARAM)auxcl->id);
		indexcl++;
		auxcl = auxcl->siguiente;
	}
	SendMessage(cbComboClient, CB_SETCURSEL, 0, 0);

	HWND cbComboEspecial;
	cbComboEspecial = GetDlgItem(hwnd, CMB_Type);
	int index2 = 0;
	SendMessage(cbComboEspecial, CB_ADDSTRING, 0, (LPARAM) "Todos");

	auxServ = originServ;
	while (auxServ != NULL) {
		SendMessage(cbComboEspecial, CB_ADDSTRING, 0, (LPARAM)auxServ->description.c_str());
		auxServ = auxServ->siguiente;
	}
	SendMessage(cbComboEspecial, CB_SETCURSEL, 0, 0);

	HWND cbComboMec;
	cbComboMec = GetDlgItem(hwnd, CMB_Select_Mec);
	int index3 = 0;
	SendMessage(cbComboMec, CB_ADDSTRING, 0, (LPARAM) "N/A ");
	SendMessage(cbComboMec, CB_SETITEMDATA, (WPARAM)index3, (LPARAM)0);
	index3++;

	int auxidnum2 = 0;

	auxMec = originMec;
	while (auxMec != NULL) {
		if (Currentcit != NULL) {
			if (Currentcit->idmec == auxMec->NumEmpl) {
				auxidnum2 = index3;
			}
		}
		string numStr = std::to_string(auxMec->NumEmpl);
		SendMessage(cbComboMec, CB_ADDSTRING, 0, (LPARAM)(auxMec->mnbre.nombre + " " + auxMec->mnbre.apellido1 + " " + auxMec->mnbre.apellido2).c_str());
		SendMessage(cbComboMec, CB_SETITEMDATA, (WPARAM)index3, (LPARAM)auxMec->NumEmpl);
		index3++;
		auxMec = auxMec->siguiente;
	}
	SendMessage(cbComboMec, CB_SETCURSEL, 0, 0);

	HWND StaticMec;
	StaticMec = GetDlgItem(hwnd, ST_MecInfo);
	SetWindowText(StaticMec, "");

	HWND hTxtHour = GetDlgItem(hwnd, CT_HOURC);
	SetWindowText(hTxtHour, "7");
	HWND hTxtHour2 = GetDlgItem(hwnd, CT_HRS);
	SetWindowText(hTxtHour2, ": 00");

	// mostrar datos de la cita si la cita de modificara

	if (flag == true) {
		HBITMAP hImagenPet1 = NULL;
		hImagenPet1 = (HBITMAP)LoadImage(NULL, Currentcit->bitmap, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
		HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWCIT);
		SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);

		HWND hTxtHour = GetDlgItem(hwnd, CT_HOURC);
		HWND hTxtday = GetDlgItem(hwnd, CT_DAY);
		HWND hTxtmonth = GetDlgItem(hwnd, CT_MONTH);
		HWND hTxtyear = GetDlgItem(hwnd, ID_YEAR);

		SetWindowText(hTxtHour, (to_string(Currentcit->date.hora)).c_str());
		SetWindowText(hTxtday, (to_string(Currentcit->date.dia)).c_str());
		SetWindowText(hTxtmonth, (to_string(Currentcit->date.mes)).c_str());
		SetWindowText(hTxtyear, (to_string(Currentcit->date.año)).c_str());

		SendMessage(cbComboClient, CB_SETCURSEL, auxidnum, 0);
		SendMessage(cbComboMec, CB_SETCURSEL, auxidnum2, 0);
	}
}

string Stringmec(Mecanicos *punteroMec) {
	string a = "";
	if (punteroMec->Dias[0] == true) {
		a = a + "lunes";
	}
	if (punteroMec->Dias[1] == true) {
		a = a + " martes";
	}
	if (punteroMec->Dias[2] == true) {
		a = a + " miercoles";
	}
	if (punteroMec->Dias[3] == true) {
		a = a + " jueves";
	}
	if (punteroMec->Dias[4] == true) {
		a = a + " viernes";
	}

	return a;
}

bool ValidacionFecha(string dia, string mes, string año) {
	int aDia = atoi(dia.c_str());
	int aMes = atoi(mes.c_str());
	int aAño = atoi(año.c_str());

	if (aMes < 1 || aMes > 12) {
		return false;
	}

	if (aMes == 1 || aMes == 3 || aMes == 5 ||
		aMes == 7 || aMes == 8 || aMes == 10 || aMes == 12) {
		if (aDia >= 1 && aDia <= 31) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (aMes == 4 || aMes == 6 || aMes == 9 || aMes == 11) {
		if (aDia >= 1 && aDia <= 30) {
			return true;
		}
		else {
			return false;
		}
	}
	else if (aMes == 2) {
		if (aAño % 4 == 0 && aAño % 100 != 0 || aAño % 400 == 0) {
			if (aDia >= 1 && aDia <= 29) {
				return true;
			}
			else {
				return false;
			}
		}
		else {
			if (aDia >= 1 && aDia <= 28) {
				return true;
			}
			else {
				return false;
			}
		}
	}
}

bool ValidacionDia(string dia, string mes, string año, int idmec) {
	int aDia = atoi(dia.c_str());
	int aMes = atoi(mes.c_str());
	int aAño = atoi(año.c_str());

	static int t[] = { 0, 3, 2, 5, 0, 3, 5, 1, 4, 6, 2, 4 };

	aAño -= aMes < 3;
	int day = (aAño + aAño / 4 - aAño / 100 + aAño / 400 + t[aMes - 1] + aDia) % 7;

	auxMec = originMec;
	while (auxMec != NULL) {
		if (auxMec->NumEmpl == idmec) {
			break;
		}
		else {
			auxMec = auxMec->siguiente;
		}
	}

	// lunes 1, martes 2, miercoles 3, jueves 4, viernes 5, sabado 6, domingo 0

	if (day == 6 || day == 0) {
		MessageBox(NULL, "No se pueden agendar citas los fines de semana", "Error", MB_ICONERROR);
		return false;
	}
	if ( (day == 1 && auxMec->Dias[0] == 1) || (day == 2 && auxMec->Dias[1] == 1) || (day == 3 && auxMec->Dias[2] == 1)
	  || (day == 4 && auxMec->Dias[3] == 1) || (day == 5 && auxMec->Dias[4] == 1)) {
		return true;
	}
	else {
		MessageBox(NULL, "La fecha debe ser en un dia en el que el mecanico trabaje", "Error", MB_ICONERROR);
		return false;
	}
	return false;
}

bool RegistroCitas(int idcl, int idmec, string hour, string day, string month, string year, string type) {

	if ((hour.length() < 1) || (day.length() < 1) || (month.length() < 1) || (year.length() < 1) || (type.length() < 1) ) {
		MessageBox(NULL, "debe de rellenar todos los datos", "Error", MB_ICONERROR);
		return false;
	}

	if((idcl == NULL) || (idmec == NULL)) {
		MessageBox(NULL, "Debe de rellenar todos los campos", "Error", MB_ICONERROR);
		return false;
	}

	if (ValidacionFecha(day, month, year) == false) {
		MessageBox(NULL, "fecha invalida", "Error", MB_ICONERROR);
		return false;
	}

	if (ValidacionDia(day, month, year, idmec) == false){
	    return false;
    }

	if (OnlyNumber(hour) == true) {
		MessageBox(NULL, "Hora solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(day) == true) {
		MessageBox(NULL, "Dia solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(month) == true) {
		MessageBox(NULL, "Mes solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(year) == true) {
		MessageBox(NULL, "Año solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	auxMec = originMec;
	while (auxMec != NULL) {
		if (auxMec->NumEmpl == idmec) {
			CurrentMec = auxMec;
			break;
		}
		else {
			auxMec = auxMec->siguiente;
		}
	}

	auxcl = origincl;
	while (auxcl != NULL) {
		if (auxcl->id == idcl) {
			Currentcl = auxcl;
			break;
		}
		else {
			auxcl = auxcl->siguiente;
		}
	}
	
	if (origincit == NULL) {
		origincit = new citas;
		origincit->anterior = NULL;
		origincit->siguiente = NULL;

		origincit->clname = Currentcl->cnbre;
		origincit->idcl = Currentcl->id;
		origincit->mecname = CurrentMec->mnbre;
		origincit->idmec = CurrentMec->NumEmpl;
		strcpy_s(origincit->bitmap, CurrentMec->bitmap);
		origincit->status = "pendiente";
		origincit->cmotiv = "N/A";
		origincit->phone = Currentcl->phone;
		origincit->service = CurrentMec->Especial;
		origincit->date.año = atoi(year.c_str());
		origincit->date.dia = atoi(day.c_str());
		origincit->date.hora = atoi(hour.c_str());
		origincit->date.mes = atoi(month.c_str());
		origincit->id = iContIdCitas;
		iContIdCitas++;
	}
	else {
		auxcit = origincit;
		while (auxcit->siguiente != NULL) {
			auxcit = auxcit->siguiente;
		}
		auxcit->siguiente = new citas;
		auxcit->siguiente->anterior = auxcit;
		auxcit = auxcit->siguiente;
		auxcit ->siguiente = NULL;

		auxcit->clname = Currentcl->cnbre;
		auxcit->idcl = Currentcl->id;
		auxcit->mecname = CurrentMec->mnbre;
		auxcit->idmec = CurrentMec->NumEmpl;
		strcpy_s(auxcit->bitmap, CurrentMec->bitmap);
		auxcit->status = "pendiente";
		auxcit->cmotiv = "N/A";
		auxcit->phone = Currentcl->phone;
		auxcit->service = CurrentMec->Especial;
		auxcit->date.año = atoi(year.c_str());
		auxcit->date.dia = atoi(day.c_str());
		auxcit->date.hora = atoi(hour.c_str());
		auxcit->date.mes = atoi(month.c_str());
		auxcit->id = iContIdCitas;
		iContIdCitas++;

	}
	auxcit = origincit;

	return true;
}

void GuardarCitas(citas *origincit) {
	const char* file_name = "//Citas.bin";

	binarioCitas.open(original_path + std::string(file_name), ios::binary | ios::out | ios::trunc);

	if (binarioCitas.is_open()) {
		while (origincit != NULL) {
			binarioCitas.write(reinterpret_cast<char*>(origincit), sizeof(citas));

			citas* tempUs = origincit;
			origincit = origincit->siguiente;
			delete tempUs;
		}
		MessageBox(NULL, "Guardado Citas", "Info", MB_ICONINFORMATION);
		binarioCitas.flush();
		binarioCitas.close();
	}
	else {
		MessageBox(NULL, "Error al guardar", "Info", MB_ICONERROR);
	}
	return;
}

void EliminarCitas(HWND hwnd) {
	HWND hLbCitas = GetDlgItem(hwnd, ID_LIST);
	int iSelectionIndex = SendMessage(hLbCitas, LB_GETCURSEL, NULL, NULL);
	int idSelected = SendMessage(hLbCitas, LB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);

	if (origincit == NULL) {
		return;
	}

	if (iSelectionIndex == -1) {
		return;
	}

	auxcit = origincit;
	while (auxcit->id != idSelected) {
		auxcit = auxcit->siguiente;
	}

	if (auxcit->siguiente == NULL && auxcit->anterior == NULL) {
		delete auxcit;
		origincit = auxcit = NULL;
	}
	else if (auxcit->anterior == NULL) { //PRIMERO
		origincit = auxcit->siguiente;
		delete auxcit;
		origincit->anterior = NULL;
		auxcit = origincit;
	}
	else if (auxcit->siguiente == NULL) { //ULTIMO
		auxcit->anterior->siguiente = NULL;
		delete auxcit;
		auxcit = origincit;
	}
	else {
		auxcit->anterior->siguiente = auxcit->siguiente;
		auxcit->siguiente->anterior = auxcit->anterior;
		delete auxcit;
		auxcit = origincit;
	}


}

bool ModificarCita(int idcl, int idmec, string hour, string day, string month, string year, string type) {

	if ((hour.length() < 1) || (day.length() < 1) || (month.length() < 1) || (year.length() < 1) || (type.length() < 1)) {
		MessageBox(NULL, "debe de rellenar todos los datos", "Error", MB_ICONERROR);
		return false;
	}

	if ((idcl == NULL) || (idmec == NULL)) {
		MessageBox(NULL, "Debe de rellenar todos los campos", "Error", MB_ICONERROR);
		return false;
	}

	if (ValidacionFecha(day, month, year) == false) {
		MessageBox(NULL, "fecha invalida", "Error", MB_ICONERROR);
		return false;
	}

	if (ValidacionDia(day, month, year, idmec) == false) {
		return false;
	}

	if (OnlyNumber(hour) == true) {
		MessageBox(NULL, "Hora solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(day) == true) {
		MessageBox(NULL, "Dia solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(month) == true) {
		MessageBox(NULL, "Mes solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	if (OnlyNumber(year) == true) {
		MessageBox(NULL, "Año solo admite numeros", "Error", MB_ICONERROR);
		return false;
	}

	auxMec = originMec;
	while (auxMec != NULL) {
		if (auxMec->NumEmpl == idmec) {
			CurrentMec = auxMec;
			break;
		}
		else {
			auxMec = auxMec->siguiente;
		}
	}

	auxcl = origincl;
	while (auxcl != NULL) {
		if (auxcl->id == idcl) {
			Currentcl = auxcl;
			break;
		}
		else {
			auxcl = auxcl->siguiente;
		}
	}

	Currentcit->clname = Currentcl->cnbre;
	Currentcit->idcl = Currentcl->id;
	Currentcit->mecname = CurrentMec->mnbre;
	Currentcit->idmec = CurrentMec->NumEmpl;
	strcpy_s(Currentcit->bitmap, CurrentMec->bitmap);
	Currentcit->status = "pendiente";
	Currentcit->cmotiv = "N/A";
	Currentcit->phone = Currentcl->phone;
	Currentcit->service = CurrentMec->Especial;
	Currentcit->date.año = atoi(year.c_str());
	Currentcit->date.dia = atoi(day.c_str());
	Currentcit->date.hora = atoi(hour.c_str());
	Currentcit->date.mes = atoi(month.c_str());
	Currentcit->id = iContIdCitas;
	return true;
}

void CargaCitas() {
	binarioCitas.open("Citas.bin", ios::binary | ios::in | ios::ate);
	if (binarioCitas.is_open()) {
		int sizeOfFile = binarioCitas.tellg();
		if (sizeOfFile < 1) {
			binarioCitas.close();
			return;
		}
		for (int i = 0; i < sizeOfFile / sizeof(citas); i++) {
			citas* tempcit = new citas;
			binarioCitas.seekg(i * sizeof(citas));
			if (origincit == NULL) {
				origincit = new citas;
				binarioCitas.read(reinterpret_cast<char*>(tempcit), sizeof(citas));

				strcpy_s(origincit->bitmap, tempcit->bitmap);
				origincit->clname = tempcit->clname;
				origincit->idcl = tempcit->idcl;
				origincit->cmotiv = tempcit->cmotiv;
				origincit->date = tempcit->date;
				origincit->id = tempcit->id;
				iContIdCitas = origincit->id;
				origincit->mecname = tempcit->mecname;
				origincit->idmec = tempcit->idmec;
				origincit->phone = tempcit->phone;
				origincit->service = tempcit->service;
				origincit->status = tempcit->status;

				origincit->siguiente = NULL;
				origincit->anterior = NULL;
				auxcit = origincit;
			}
			else {
				auxcit = origincit;
				while (auxcit->siguiente != NULL) {
					auxcit = auxcit->siguiente;
				}
				binarioCitas.read(reinterpret_cast<char*>(tempcit), sizeof(citas));
				auxcit->siguiente = new citas;
				auxcit->siguiente->anterior = auxcit;
				auxcit = auxcit->siguiente;

				strcpy_s(auxcit->bitmap, tempcit->bitmap);
				auxcit->clname = tempcit->clname;
				auxcit->idcl = tempcit->idcl;
				auxcit->cmotiv = tempcit->cmotiv;
				auxcit->date = tempcit->date;
				auxcit->id = tempcit->id;
				iContIdCitas = auxcit->id;
				auxcit->mecname = tempcit->mecname;
				auxcit->idmec = tempcit->idmec;
				auxcit->phone = tempcit->phone;
				auxcit->service = tempcit->service;
				auxcit->status = tempcit->status;

				auxcit = auxcit->siguiente = NULL;
				auxcit = origincit;
			}
			delete reinterpret_cast<char*>(tempcit);
		}
		iContIdCitas++;
		binarioCitas.close();
	}
	else {
		MessageBox(NULL, "No se pudo cargar archivo ¨Citas¨", "Error", MB_ICONERROR);
	}
}

#pragma endregion Citas

void actualizar(HWND hwnd) {
	int index = 0;
	HWND hLbCitas = GetDlgItem(hwnd, ID_LIST);
	SendMessage(hLbCitas, LB_RESETCONTENT, NULL, NULL);
	auxcit = origincit;
	while (auxcit != NULL) {
		SendMessage(hLbCitas, LB_ADDSTRING, 0, (LPARAM)(auxcit->clname.nombre + " " + auxcit->clname.apellido1 + " " + auxcit->clname.apellido2).c_str());
		SendMessage(hLbCitas, LB_SETITEMDATA, (WPARAM)index, (LPARAM)auxcit->id);
		index++;
		auxcit = auxcit->siguiente;
	}
}

void mostrarinfo(HWND hwnd) {
	HWND hLbCitas = GetDlgItem(hwnd, ID_LIST);
	int iSelectionIndex = SendMessage(hLbCitas, LB_GETCURSEL, NULL, NULL);
	int idSelected = SendMessage(hLbCitas, LB_GETITEMDATA, (WPARAM)iSelectionIndex, NULL);

	if (origincit == NULL) {
		return;
	}

	auxcit = origincit;
	while (auxcit->id != idSelected) {
		auxcit = auxcit->siguiente;
	}

	HWND CLname = GetDlgItem(hwnd, ID_SHOWCL);
	SetWindowText(CLname, (auxcit->clname.nombre + " " + auxcit->clname.apellido1 + " " + auxcit->clname.apellido2 + ", " + auxcit->phone).c_str());

	HWND Mecname = GetDlgItem(hwnd, ID_SHOWMEC);
	SetWindowText(Mecname, (auxcit->mecname.nombre + " " + auxcit->mecname.apellido1 + " " + auxcit->mecname.apellido2 + ", "  + auxcit->service).c_str());

	string numStr = std::to_string(auxcit->date.dia) +  " " + std::to_string(auxcit->date.mes) + " " + std::to_string(auxcit->date.año) + ", " + std::to_string(auxcit->date.hora) + ":00 - " + std::to_string(auxcit->date.hora+4) + ":00";
	HWND Date = GetDlgItem(hwnd, ID_SHOWDATE);
	SetWindowText(Date, (numStr).c_str());

	HWND motive = GetDlgItem(hwnd, ID_MOTIVE);
	SetWindowText(motive, (auxcit->status).c_str());

	HBITMAP hImagenPet1 = NULL;
	hImagenPet1 = (HBITMAP)LoadImage(NULL, auxcit->bitmap, IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
	HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWBITMAIN);
	SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);
}

void VaciarContenido(HWND hwnd) {
	HWND CLname = GetDlgItem(hwnd, ID_SHOWCL);
	SetWindowText(CLname, "");

	HWND Mecname = GetDlgItem(hwnd, ID_SHOWMEC);
	SetWindowText(Mecname, "");

	HWND Date = GetDlgItem(hwnd, ID_SHOWDATE);
	SetWindowText(Date, "");

	HWND motive = GetDlgItem(hwnd, ID_MOTIVE);
	SetWindowText(motive, "");

	HBITMAP hImagenPet1 = NULL;
	hImagenPet1 = (HBITMAP)LoadImage(NULL, "", IMAGE_BITMAP, 110, 110, LR_LOADFROMFILE);
	HWND hPictureControlP1 = GetDlgItem(hwnd, BIT_SHOWBITMAIN);
	SendMessage(hPictureControlP1, STM_SETIMAGE, IMAGE_BITMAP, (LPARAM)hImagenPet1);
}

#pragma region quicksort

void swap(Mecanicos* a, Mecanicos* b)
{
	Mecanicos t;

	t.NumEmpl = a->NumEmpl; a->NumEmpl = b->NumEmpl; b->NumEmpl = t.NumEmpl; 

	t.mnbre = a->mnbre; a->mnbre = b->mnbre; b->mnbre = t.mnbre;

	t.Especial = a->Especial; a->Especial = b->Especial; b->Especial = t.Especial;

	t.phone = a->phone; a->phone = b->phone; b->phone = t.phone;

	t.phone = a->phone; a->phone = b->phone; b->phone = t.phone;

	strcpy_s(t.bitmap, a->bitmap);  strcpy_s(a->bitmap, b->bitmap); strcpy_s(b->bitmap, t.bitmap);

	for (size_t i = 0; i < 5; i++){
		t.Dias[i] = a->Dias[i];
	    a->Dias[i] = b->Dias[i];
		a->Dias[i] = t.Dias[i];
	}
}

Mecanicos* particion(Mecanicos *l, Mecanicos *h)
{
	// set pivot as h element
	int x = h->NumEmpl;

	// similar to i = l-1 for array implementation
	Mecanicos *i = l->anterior;

	// Similar to "for (int j = l; j <= h- 1; j++)"
	for (Mecanicos *j = l; j != h; j = j->siguiente)
	{
		if (j->NumEmpl <= x)
		{
			// Similar to i++ for array
			i = (i == NULL) ? l : i->siguiente;

			swap(i,j);
		}
	}
	i = (i == NULL) ? l : i->siguiente; // Similar to i++
	swap(i,h);
	return i;
}

void _quickSort(Mecanicos* l, Mecanicos *h)
{
	if (h != NULL && l != h && l != h->siguiente)
	{
		Mecanicos *p = particion(l, h);
		_quickSort(l, p->anterior);
		_quickSort(p->siguiente, h);
	}
}

void quickSort()
{
	auxMec = originMec;
	if (auxMec == NULL) {
		return;
	}
	while (auxMec->siguiente != NULL) {
		auxMec = auxMec->siguiente;
	}
	Mecanicos *h = auxMec;

	auxMec = originMec;
	_quickSort(auxMec, h);
}

#pragma endregion quicksort

#pragma region quicksort2

void swap2(Clientes *a, Clientes *b) {
	Clientes t;

	t.cnbre = a->cnbre; a->cnbre = b->cnbre; b->cnbre = t.cnbre;

	t.calle = a->calle; a->calle = b->calle; b->calle = t.calle;

	t.carc = a->carc; a->carc = b->carc; b->carc = t.carc;

	t.cnbre = a->cnbre; a->cnbre = b->cnbre; b->cnbre = t.cnbre;

	t.id = a->id; a->id = b->id; b->id = t.id;

	t.numcalle = a->numcalle; a->numcalle = b->numcalle; b->numcalle = t.numcalle;

	t.phone = a->phone; a->phone = b->phone; b->phone = t.phone;
}

Clientes* particion2(Clientes *l, Clientes *h)
{
	// set pivot as h element

	string auxs = (h->cnbre.nombre + " " + h->cnbre.apellido1 + " " + h->cnbre.apellido2);

	// similar to i = l-1 for array implementation
	Clientes *i = l->anterior;

	// Similar to "for (int j = l; j <= h- 1; j++)"
	for (Clientes *j = l; j != h; j = j->siguiente)
	{
		string auxs2 = (j->cnbre.nombre + " " + j->cnbre.apellido1 + " " + j->cnbre.apellido2);

		if(  _strcmpi( auxs2.c_str(), auxs.c_str()) <= 0  )
		{
			// Similar to i++ for array
			i = (i == NULL) ? l : i->siguiente;

			swap2(i, j);
		}
	}
	i = (i == NULL) ? l : i->siguiente; // Similar to i++
	swap2(i, h);
	return i;
}

void _quickSort2(Clientes* l, Clientes *h)
{
	if (h != NULL && l != h && l != h->siguiente)
	{
		Clientes *p = particion2(l, h);
		_quickSort2(l, p->anterior);
		_quickSort2(p->siguiente, h);
	}
}

void quickSort2()
{
	auxcl = origincl;
	if (auxcl == NULL) {
		return;
	}
	while (auxcl->siguiente != NULL) {
		auxcl = auxcl->siguiente;
	}
	Clientes *h = auxcl;

	auxcl = origincl;
	_quickSort2(auxcl, h);
}

#pragma endregion quicksort2

void Reporte() {
	quickSort();
    quickSort2();

	auxMec = originMec;
	ofstream outfile("Reportes//Reporte_Mecanicos.txt");
	while (auxMec != NULL) {
		outfile << "Nombre: " << auxMec->mnbre.nombre << " " << auxMec->mnbre.apellido1 << " " << auxMec->mnbre.apellido2 << "\n" << endl;
		outfile << "Especialidad: " << auxMec->Especial << "\n" << endl;
		outfile << "Numero de empleado: " << auxMec->NumEmpl << "\n" << endl;
		outfile << "Telefono: " << auxMec->phone << "\n" << endl;
		outfile << "Turno: " << auxMec->Turn << "\n" << endl;
		string semana = Stringmec(auxMec);
		outfile << "Dias de la semana que trabaja: " << semana << "\n" << endl;
		outfile << "- - - - - - - - - - - - - - \n\n" << endl;
		auxMec = auxMec->siguiente;
	}
	outfile.close();

	auxcl = origincl;
	ofstream outfile1("Reportes//Reporte_Clientes.txt");
	while (auxcl != NULL) {
		outfile1 << "Nombre: " << auxcl->cnbre.nombre << " " << auxcl->cnbre.apellido1 << " " << auxcl->cnbre.apellido2 << "\n" << endl;
		outfile1 << "Calle: " << auxcl->calle << "\n" << endl;
		outfile1 << "Numero de calle: " << auxcl->numcalle << "\n" << endl;
		outfile1 << "Telefono: " << auxcl->phone << "\n\n" << endl;
		outfile1 << "Modelo del auto: " << auxcl->carc.modelo << "\n" << endl;
		outfile1 << "Marca del auto: " << auxcl->carc.marca << "\n" << endl;
		outfile1 << "Placa del auto: " << auxcl->carc.placa << "\n" << endl;
		outfile1 << "kilometraje del auto: " << auxcl->carc.kilometraje << "\n" << endl;
		outfile1 << "Año del auto: " << auxcl->carc.Año << "\n" << endl;
		outfile1 << "- - - - - - - - - - - - - - \n\n" << endl;
		auxcl = auxcl->siguiente;
	}
	outfile.close();






}

bool OnlyNumber(string aux) { 
	int auxlenght = aux.length();
	for (int i = 0; i < auxlenght; i++) {
		if (isdigit(aux.c_str()[i]) == 0) {
			return true;
		}
	}
	return false;
}

bool OnlyPrice(string aux) {
	int auxlenght = aux.length();
	char cAux[MAX_PATH];
	int contp = 0;
	for (int i = 0; i < auxlenght; i++) {
		if (((isdigit(aux.c_str()[i]) == 0) && aux.c_str()[i] != '.') || (contp > 1) ) {
			return true;
		}
		else if ( (isdigit(aux.c_str()[i]) == 0) && (aux.c_str()[i] == '.') ) {
			contp++;
		}
	}
	return false;
}