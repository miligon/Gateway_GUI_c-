
// GatewayDlg.cpp: archivo de implementación
//

#include "pch.h"
#include "framework.h"
#include "Gateway.h"
#include "GatewayDlg.h"
#include "afxdialogex.h"
#include "Serial.hpp"
#include <stdlib.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

using namespace System;
using namespace System::Runtime::InteropServices;

struct GatewayData {
	double Sensor1;
	double Sensor2;
	double Sensor3;
	double Sensor4;
	double Sensor5;
	char LedStatus;
} gateway_data;

void DoEvents()
{
	MSG msg;
	while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
}

CDataThread::CDataThread() {}
BOOL CDataThread::InitInstance(){
	rx_flag = false;
	tx_flag = false;
	free_rx_buf = 0;
	binarySend_flag = false;
	binaryRecieve_flag = false;
	active = 0; 
	return TRUE;
}

bool CDataThread::wait_rx(int time = 5) {
	int count = 0;
	while (!rx_flag) {
		Thread::Sleep(1);
		count++;
		DoEvents();
		if (count >= (time*1000)) {
			return false;
		}
	}
	return true;
}

int CDataThread::Run(){
	Serial puerto;

	TRACE(_T("Configurando puerto . . ."));
	active = 1;
	puerto.SetPortBaudRate(115200);
	puerto.SetPortDataBits(8);
	puerto.SetPortStopBits(IO::Ports::StopBits::One);
	puerto.SetPortParity(IO::Ports::Parity::None);
	puerto.SetPortHandshake(IO::Ports::Handshake::None);
	puerto.SetPortName(String(COMMport.GetString()).ToString());
	puerto._serialPort->NewLine = "\n";
	
	if (puerto.OpenPort())
	{
		active = 2;
		TRACE(_T("Puerto abierto correctamente :D"));
	}
	else
	{
		active = 3;
		TRACE(_T("No se pudo abrir el puerto D:"));
	}	

	TRACE(_T("Serial Thread started!"));
	while (active == 2) {
		if (puerto.Available() > 0 && rx_flag == false) {
			if (free_rx_buf==1) {
				Marshal::FreeHGlobal((IntPtr)rxBuf);
			}
			if (free_rx_buf == 2) {
				free(rxBuf);
			}
			if (binaryRecieve_flag) {
				rxBuf = (char *)calloc(1, rxlen);
				puerto.ReadChars((unsigned char *)rxBuf, rxlen);
				binaryRecieve_flag = false;
				rx_flag = true;
				free_rx_buf = 2;

			}
			else {
				String^ data_leida = puerto.ReadLine();
				rxBuf = (char*)Marshal::StringToHGlobalAnsi(data_leida).ToPointer();
				rxlen = data_leida->Length;
				rx_flag = true;
				free_rx_buf = 1;
			}
		}
		if (this->tx_flag) {
			if (binarySend_flag) {
				puerto.Write((unsigned char *)txBuf, txLen);
				binarySend_flag = false;
			}
			else
			{
				String^ buf = gcnew String(txBuf);
				puerto.WriteLine(buf);
			}

			tx_flag = false;
		}
		Thread::Sleep(10);
	}
	puerto._serialPort->Close();
	TRACE(_T("Serial Thread terminated!"));
	return (0);
}

// Cuadro de diálogo CAboutDlg utilizado para el comando Acerca de

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // Compatibilidad con DDX/DDV

// Implementación
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// Cuadro de diálogo de CGatewayDlg

BEGIN_DHTML_EVENT_MAP(CGatewayDlg)
	DHTML_EVENT_ONCLICK(_T("ButtonOK"), OnButtonOK)
	DHTML_EVENT_ONCLICK(_T("ButtonCancel"), OnButtonCancel)
END_DHTML_EVENT_MAP()


CGatewayDlg::CGatewayDlg(CWnd* pParent /*= nullptr*/)
	: CDHtmlDialog(IDD_GATEWAY_DIALOG, IDR_HTML_GATEWAY_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGatewayDlg::DoDataExchange(CDataExchange* pDX)
{
	CDHtmlDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CHECK_LED1, chkLed1);
	DDX_Control(pDX, IDC_CHECK_LED2, chkLed2);
	DDX_Control(pDX, IDC_CHECK_LED3, chkLed3);
	DDX_Control(pDX, IDC_CHECK_LED4, chkLed4);
	DDX_Control(pDX, IDC_CHECK_LED5, chkLed5);
	DDX_Control(pDX, IDC_CHECK_LED6, chkLed6);
	DDX_Control(pDX, IDC_LST_ROUTES, lstRoutes);
}

BEGIN_MESSAGE_MAP(CGatewayDlg, CDHtmlDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CONNECT, &CGatewayDlg::OnBnClickedBtnConnect)
//	ON_BN_CLICKED(IDC_CHECK_LED1, &CGatewayDlg::OnBnClickedCheckLed1)
ON_BN_CLICKED(IDC_CHECK_LED1, &CGatewayDlg::OnBnClickedCheckLed1)
ON_BN_CLICKED(IDC_CHECK_LED2, &CGatewayDlg::OnBnClickedCheckLed2)
ON_BN_CLICKED(IDC_CHECK_LED3, &CGatewayDlg::OnBnClickedCheckLed3)
ON_BN_CLICKED(IDC_CHECK_LED4, &CGatewayDlg::OnBnClickedCheckLed4)
ON_BN_CLICKED(IDC_CHECK_LED5, &CGatewayDlg::OnBnClickedCheckLed5)
ON_BN_CLICKED(IDC_CHECK_LED6, &CGatewayDlg::OnBnClickedCheckLed6)
ON_BN_CLICKED(IDC_BTN_SENDROUTES, &CGatewayDlg::OnBnClickedBtnSendroutes)
ON_BN_CLICKED(IDC_BTN_READROUTES, &CGatewayDlg::OnBnClickedBtnReadroutes)
ON_BN_CLICKED(IDC_BTN_ADDROUTE, &CGatewayDlg::OnBnClickedBtnAddroute)
ON_BN_CLICKED(IDC_BTN_DELROUTE, &CGatewayDlg::OnBnClickedBtnDelroute)
ON_BN_CLICKED(IDC_BTN_LOAD, &CGatewayDlg::OnBnClickedBtnOpen)
ON_BN_CLICKED(IDC_BTN_SAVE, &CGatewayDlg::OnBnClickedBtnSave)
END_MESSAGE_MAP()


// Controladores de mensajes de CGatewayDlg

BOOL CGatewayDlg::OnInitDialog()
{
	CDHtmlDialog::OnInitDialog();

	// Agregar el elemento de menú "Acerca de..." al menú del sistema.

	// IDM_ABOUTBOX debe estar en el intervalo de comandos del sistema.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != nullptr)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Establecer el icono para este cuadro de diálogo.  El marco de trabajo realiza esta operación
	//  automáticamente cuando la ventana principal de la aplicación no es un cuadro de diálogo
	SetIcon(m_hIcon, TRUE);			// Establecer icono grande
	SetIcon(m_hIcon, FALSE);		// Establecer icono pequeño

	// TODO: agregar aquí inicialización adicional
	Serial serialPort;
	CComboBox* cmbPorts = (CComboBox*)GetDlgItem(IDC_COMBO_PORTS);

	pin_ptr<const wchar_t> wch;
	for (int i = 0; i < serialPort._serialPort->GetPortNames()->Length; i++)
	{
		String^ puerto = (String^)serialPort._serialPort->GetPortNames()->GetValue(i);
		wch = PtrToStringChars(puerto);
		cmbPorts->AddString(wch);
	}	
	tablaDeRuteo.numDeRutas = 0;

	lstRoutes.AddString(_T("DEST MASK GATEWAY INTERFAZ"));

	return TRUE;  // Devuelve TRUE  a menos que establezca el foco en un control
}

bool Decode(char *data, GatewayData *output) {

	try {
		int len = (int)data[1];
		if (data[0] == 'A' && data[len + 2] == 'Z') {
			memcpy(output, data + 2, len);
			//fread(&data, len, 1, data.Right(len + 1).Left(len));
			return true;
		}
		else
		{
			return false;
		}
	}
	catch (Exception^ e)
	{
		printf("Error");
	}
}

void CGatewayDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Agregue aquí su código de controlador de mensajes o llame al valor predeterminado
	//static char buf[5];
	//wchar_t wcmd[40];
	//swprintf_s(wcmd, L"%i", adapter->i);
	if (StatusConexion) {
		SetDlgItemText(IDC_LBL_ESTADO, L"Enviando . . .");
		if (adapter->tx_flag == false) {
			char frame[4] = { 'A', 1, 1, 'Z' };
			memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
			memcpy(adapter->txBuf, frame,4);
			adapter->tx_flag = true;

			if (adapter->wait_rx())
			{
				SetDlgItemText(IDC_LBL_ESTADO, L"Recibiendo . . .");
				if (adapter->rxlen > sizeof(GatewayData)) {
					Decode(adapter->rxBuf, &gateway_data);
				}
				else
				{
					CStringW data = _T("\nRx(STR) -> ");

					for (int i = 0; i < adapter->rxlen; i++) {
						CStringW buf;
						buf.Format(_T("%c"), adapter->rxBuf[i]);
						data += buf;
					}
					data += _T("\nRx(HEX) -> ");

					for (int i = 0; i < adapter->rxlen; i++) {
						CStringW buf;
						buf.Format(_T("%X "), adapter->rxBuf[i]);
						data += buf;
					}
					data += ("\n");
					CStringW anterior;
					GetDlgItemText(IDC_EDIT1, anterior);
					SetDlgItemText(IDC_EDIT1, (anterior + data));
				}
				CStringW t1;
				CStringW t2;
				CStringW t3;
				CStringW t4;
				CStringW t5;

				t1.Format(_T("T1: %2.3f"), gateway_data.Sensor1);
				t2.Format(_T("T2: %2.3f"), gateway_data.Sensor2);
				t3.Format(_T("T3: %2.3f"), gateway_data.Sensor3);
				t4.Format(_T("T4: %2.3f"), gateway_data.Sensor4);
				t5.Format(_T("T5: %2.3f"), gateway_data.Sensor5);

				SetDlgItemText(IDC_LBL_T1, CStringW(t1.GetString()));
				SetDlgItemText(IDC_LBL_T2, CStringW(t2.GetString()));
				SetDlgItemText(IDC_LBL_T3, CStringW(t3.GetString()));
				SetDlgItemText(IDC_LBL_T4, CStringW(t4.GetString()));
				SetDlgItemText(IDC_LBL_T5, CStringW(t5.GetString()));

				if (firstTime) {
					bool led1 = (bool)((gateway_data.LedStatus - 0x30) & 0x01);
					bool led2 = (bool)(((gateway_data.LedStatus - 0x30) & 0x02) >> 1);
					bool led3 = (bool)(((gateway_data.LedStatus - 0x30) & 0x04) >> 2);
					bool led4 = (bool)(((gateway_data.LedStatus - 0x30) & 0x08) >> 3);
					bool led5 = (bool)(((gateway_data.LedStatus - 0x30) & 0x10) >> 4);
					bool led6 = (bool)(((gateway_data.LedStatus - 0x30) & 0x20) >> 5);
					chkLed1.SetCheck(led1 ? BST_CHECKED : BST_UNCHECKED);
					chkLed2.SetCheck(led2 ? BST_CHECKED : BST_UNCHECKED);
					chkLed3.SetCheck(led3 ? BST_CHECKED : BST_UNCHECKED);
					chkLed4.SetCheck(led4 ? BST_CHECKED : BST_UNCHECKED);
					chkLed5.SetCheck(led5 ? BST_CHECKED : BST_UNCHECKED);
					chkLed6.SetCheck(led6 ? BST_CHECKED : BST_UNCHECKED);
					firstTime = false;
				}

				adapter->rx_flag = false;
			}
		}
		SetDlgItemText(IDC_LBL_ESTADO, L"En espera . . .");
	}

	CDHtmlDialog::OnTimer(nIDEvent);
}

void CGatewayDlg::OnBnClickedBtnConnect()
{
	if (StatusConexion == False)
	{
		CString puerto;
		GetDlgItemText(IDC_COMBO_PORTS, puerto);
		if (puerto.GetLength() < 3) {
			SetDlgItemText(IDC_LBL_ESTADO, L"Seleccione un puerto!");
			return;
		}

		adapter = new CDataThread();
		adapter->CreateThread(CREATE_SUSPENDED);
		adapter->m_bAutoDelete = false; // Let me delete it.
		adapter->COMMport = puerto;
		adapter->ResumeThread();

		SetDlgItemText(IDC_LBL_ESTADO, L"Abriendo puerto ...");

		while (adapter->active == 1);
		if (adapter->active == 2) {
			SetDlgItemText(IDC_LBL_ESTADO, L"Conectado!");
			SetDlgItemText(IDC_BTN_CONNECT, L"DESCONECTAR");
			StatusConexion = true;
			tmrRefresh = SetTimer(1, 500, NULL); // one event every 1000 ms = 1 s
		}
		else
		{
			SetDlgItemText(IDC_LBL_ESTADO, L"Error al conectar");
		}
	}
	else
	{
		KillTimer(tmrRefresh);
		if (adapter->active == 2) {
			adapter->active = false;
			WaitForSingleObject(adapter->m_hThread, INFINITE);
			delete(adapter);
		}
		firstTime = true;
		SetDlgItemText(IDC_LBL_ESTADO, L"Desconectado");
		SetDlgItemText(IDC_BTN_CONNECT, L"CONECTAR");
		StatusConexion = false;
	}
	// TODO: Agregue aquí su código de controlador de notificación de control
}

void CGatewayDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDHtmlDialog::OnSysCommand(nID, lParam);
	}
}

// Si agrega un botón Minimizar al cuadro de diálogo, necesitará el siguiente código
//  para dibujar el icono.  Para aplicaciones MFC que utilicen el modelo de documentos y vistas,
//  esta operación la realiza automáticamente el marco de trabajo.

void CGatewayDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // Contexto de dispositivo para dibujo

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Centrar icono en el rectángulo de cliente
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Dibujar el icono
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDHtmlDialog::OnPaint();
	}
}

// El sistema llama a esta función para obtener el cursor que se muestra mientras el usuario arrastra
//  la ventana minimizada.
HCURSOR CGatewayDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

HRESULT CGatewayDlg::OnButtonOK(IHTMLElement* /*pElement*/)
{
	OnOK();
	return S_OK;
}

HRESULT CGatewayDlg::OnButtonCancel(IHTMLElement* /*pElement*/)
{
	OnCancel();
	return S_OK;
}

void CGatewayDlg::send_led(int led, int val) {
	if (StatusConexion) {
		SetDlgItemText(IDC_LBL_ESTADO, L"Enviando comando de Leds ...");
		unsigned char valor = ((val << 4) & 0xF0) + (led & 0x0F);
		unsigned char frame[4] = { 'L', 1, valor, 'Z' };
		memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
		memcpy(adapter->txBuf, frame, 4);
		adapter->tx_flag = true;
	}
}

void CGatewayDlg::OnBnClickedCheckLed1()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed1.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(0, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(0, 1);
	}
}


void CGatewayDlg::OnBnClickedCheckLed2()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed2.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(1, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(1, 1);
	}
}


void CGatewayDlg::OnBnClickedCheckLed3()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed3.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(2, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(2, 1);
	}
}


void CGatewayDlg::OnBnClickedCheckLed4()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed4.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(3, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(3, 1);
	}
}


void CGatewayDlg::OnBnClickedCheckLed5()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed5.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(4, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(4, 1);
	}
}


void CGatewayDlg::OnBnClickedCheckLed6()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int ChkBox = chkLed6.GetCheck();

	if (ChkBox == BST_UNCHECKED) {
		send_led(5, 0);
	}
	else if (ChkBox == BST_CHECKED) {
		send_led(5, 1);
	}
}


void CGatewayDlg::OnBnClickedBtnSendroutes()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	if (StatusConexion) {
		//while (adapter->rx_flag);
		KillTimer(tmrRefresh);
		adapter->rx_flag = false;
		SetDlgItemText(IDC_LBL_ESTADO, L"Preparando tabla de ruteo . . .");
		memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
		int size = (sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas);
		adapter->txBuf[0] = 'W';
		adapter->txBuf[1] = 1;
		adapter->txBuf[2] = size;
		adapter->txBuf[3] = 'Z';
		adapter->tx_flag = true;
		if (adapter->wait_rx())
		{
			if (adapter->rxlen >= 2) {
				if (adapter->rxBuf[0] == 'O' && adapter->rxBuf[1] == 'K') {
					SetDlgItemText(IDC_LBL_ESTADO, L"Enviando tabla de ruteo . . .");
					memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
					memcpy(adapter->txBuf, tablaDeRuteo.rutas, size);
					adapter->txLen = size;
					adapter->binarySend_flag = true;
					adapter->rx_flag = false;
					adapter->tx_flag = true;
					memset(adapter->rxBuf, 0, adapter->rxlen);
					if (adapter->wait_rx())
					{
						if (adapter->rxlen >= 2) {
							if (adapter->rxBuf[0] == 'O' && adapter->rxBuf[1] == 'K') {
								SetDlgItemText(IDC_LBL_ESTADO, L"Enviado!");
								Thread::Sleep(50);
							}
						}
						adapter->rx_flag = false;
					}
				}
			}
			adapter->rx_flag = false;
		}
		tmrRefresh = SetTimer(1, 500, NULL); // one event every 1000 ms = 1 s
	}
}


void CGatewayDlg::OnBnClickedBtnReadroutes()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	if (StatusConexion) {
		//while (adapter->rx_flag);
		KillTimer(tmrRefresh);
		adapter->rx_flag = false;
		SetDlgItemText(IDC_LBL_ESTADO, L"Pidiendo tabla de ruteo . . .");
		memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
		int size = (sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas);
		adapter->txBuf[0] = 'R';
		adapter->txBuf[1] = 1;
		adapter->txBuf[2] = 1;
		adapter->txBuf[3] = 'Z';
		adapter->tx_flag = true;
		if (adapter->wait_rx())
		{
			if (adapter->rxlen >= 4) {
				if (adapter->rxBuf[0] == 'R' && adapter->rxBuf[3] == 'Z') {
					int size_struct = adapter->rxBuf[2];
					SetDlgItemText(IDC_LBL_ESTADO, L"Recibiendo tabla de ruteo . . .");
					memset(adapter->txBuf, 0, sizeof(adapter->txBuf));
					adapter->txBuf[0] = 'O';
					adapter->txBuf[1] = 'K';
					adapter->rxlen = (int)size_struct;
					adapter->binaryRecieve_flag = true;
					adapter->rx_flag = false;
					adapter->tx_flag = true;
					if (adapter->wait_rx())
					{
						if (adapter->rxlen >= size_struct) {
							tablaDeRuteo.numDeRutas = size_struct / sizeof(GatewayRoute);
							tablaDeRuteo.rutas = (GatewayRoute*)malloc(size_struct);
							memcpy(tablaDeRuteo.rutas, adapter->rxBuf, size_struct);
							SetDlgItemText(IDC_LBL_ESTADO, L"Recibido!");
							Thread::Sleep(50);
							adapter->binaryRecieve_flag = false;
							adapter->rx_flag = false;
						}
					}
				}
			}
			adapter->rx_flag = false;
		}
		renderRutas();
		tmrRefresh = SetTimer(1, 500, NULL); // one event every 1000 ms = 1 s
	}
}


void CGatewayDlg::OnBnClickedBtnAddroute()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	CStringW destino, netMask, gateway, interfaz;

	GetDlgItemText(IDC_TXT_DST, destino);
	GetDlgItemText(IDC_TXT_MASK, netMask);
	GetDlgItemText(IDC_TXT_GATEWAY, gateway);
	GetDlgItemText(IDC_TXT_INTERFACE, interfaz);

	tablaDeRuteo.numDeRutas++;
	//Se define un buffer
	GatewayRoute *rutas = (GatewayRoute*)malloc(sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas);

	//Se realiza un backup de las tablas de ruteo existentes
	memcpy(rutas, tablaDeRuteo.rutas, (sizeof(GatewayRoute) * (tablaDeRuteo.numDeRutas-1)));

	//Se agrega la nueva ruta
	rutas[tablaDeRuteo.numDeRutas - 1].destino = _wtoi(destino);
	rutas[tablaDeRuteo.numDeRutas - 1].mascara = _wtoi(netMask);
	rutas[tablaDeRuteo.numDeRutas - 1].puerta_de_enlace = _wtoi(gateway);

	char* cstr = new char[6];
	WideCharToMultiByte(CP_ACP, 0, interfaz, -1, rutas[tablaDeRuteo.numDeRutas - 1].interfaz, 6, NULL, NULL);

	tablaDeRuteo.rutas = rutas;

	renderRutas();
}

void CGatewayDlg::renderRutas() {
	CStringW destino;
	CStringW netMask;
	CStringW gateway;
	CStringW interfaz;

	lstRoutes.ResetContent();
	lstRoutes.AddString(_T("DEST MASK GATEWAY INTERFAZ"));

	for (int i = 0; i < tablaDeRuteo.numDeRutas; i++) {
		destino.Format(L"%d", tablaDeRuteo.rutas[i].destino);
		netMask.Format(L"%d", tablaDeRuteo.rutas[i].mascara);
		gateway.Format(L"%d", tablaDeRuteo.rutas[i].puerta_de_enlace);
		interfaz = CStringW(tablaDeRuteo.rutas[i].interfaz,6);
		
		lstRoutes.InsertString(lstRoutes.GetCount(), (destino + "\t" + netMask + "\t" + gateway + "\t" + interfaz));
	}
}


void CGatewayDlg::OnBnClickedBtnDelroute()
{
	// TODO: Agregue aquí su código de controlador de notificación de control
	int seleccion = lstRoutes.GetCurSel();
	if (seleccion > 0) {
		// TODO: Agregue aquí su código de controlador de notificación de control
		CStringW destino, netMask, gateway, interfaz;

		GetDlgItemText(IDC_TXT_DST, destino);
		GetDlgItemText(IDC_TXT_MASK, netMask);
		GetDlgItemText(IDC_TXT_GATEWAY, gateway);
		GetDlgItemText(IDC_TXT_INTERFACE, interfaz);

		tablaDeRuteo.numDeRutas--;
		//Se define un buffer
		GatewayRoute* rutas = (GatewayRoute*)malloc(sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas);

		int indx = 0;
		//Se realiza un backup de las tablas de ruteo existentes
		for (int i = 0; i < tablaDeRuteo.numDeRutas + 1; i++)
		{
			if (i != seleccion - 1) {
				rutas[indx] = tablaDeRuteo.rutas[i];
				indx++;
			}
		}

		tablaDeRuteo.rutas = rutas;

		renderRutas();
	}
}

void CGatewayDlg::OnBnClickedBtnOpen()
{

	CFileDialog x(TRUE, NULL,	NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Tabla de ruteo (*.ruta)|*.ruta||"),NULL, 0,TRUE);

	x.DoModal();
	CStringW path = x.GetPathName();

	CFile fd;

	if (fd.Open(path, CFile::modeRead))
	{
		fd.Read(&tablaDeRuteo.numDeRutas, sizeof(int));
		
		free(tablaDeRuteo.rutas);
		tablaDeRuteo.rutas = (GatewayRoute*)malloc(sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas);
		
		fd.Seek(sizeof(int) + 1,0);
		fd.Read(tablaDeRuteo.rutas, (sizeof(GatewayRoute) * tablaDeRuteo.numDeRutas));
		fd.Close();
		
		renderRutas();
	}
	else {
		AfxMessageBox(_T("No se pudo abrir el archivo"), MB_ICONERROR | MB_YESNO);
	}
}

void CGatewayDlg::OnBnClickedBtnSave()
{
	CFileDialog x(FALSE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Tabla de ruteo (*.ruta)|*.ruta||"), NULL, 0, TRUE);

	x.DoModal();
	CStringW path = x.GetPathName();

	CFile fd;

	if (fd.Open(path, CFile::modeCreate | CFile::modeWrite))
	{
		fd.Write(&tablaDeRuteo.numDeRutas, sizeof(int));
		fd.Seek(sizeof(int) + 1, 0);
		fd.Write(tablaDeRuteo.rutas, sizeof(GatewayRoute)*tablaDeRuteo.numDeRutas);
		fd.Close();
	}
	else {
		AfxMessageBox(_T("No se pudo abrir el archivo"), MB_ICONERROR | MB_YESNO);
	}
}