
// GatewayDlg.h: archivo de encabezado
//

#pragma once

class CDataThread : public CWinThread
{
private:
	bool free_rx_buf;
public:
	CDataThread();
	virtual BOOL InitInstance();
	virtual int Run();
	CString COMMport;
	char txBuf[50];
	char *rxBuf;
	int rxlen;
	bool tx_flag;
	bool rx_flag;
	bool wait_rx(int time);
	char active = 0;
};

// Cuadro de diálogo de CGatewayDlg
class CGatewayDlg : public CDHtmlDialog
{
	// Construcción
public:
	CGatewayDlg(CWnd* pParent = nullptr);	// Constructor estándar

// Datos del cuadro de diálogo
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GATEWAY_DIALOG, IDH = IDR_HTML_GATEWAY_DIALOG };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// Compatibilidad con DDX/DDV

	HRESULT OnButtonOK(IHTMLElement* pElement);
	HRESULT OnButtonCancel(IHTMLElement* pElement);

	// Implementación
protected:
	HICON m_hIcon;
	bool StatusConexion = False;
	UINT_PTR tmrRefresh;
	bool firstTime = true;

	// Funciones de asignación de mensajes generadas
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
	DECLARE_DHTML_EVENT_MAP()
	void send_led(int led, int val);
public:
	CDataThread* adapter;
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedBtnConnect();
//	afx_msg void OnBnClickedCheckLed1();
	afx_msg void OnBnClickedCheckLed1();
	afx_msg void OnBnClickedCheckLed2();
	afx_msg void OnBnClickedCheckLed3();
	afx_msg void OnBnClickedCheckLed4();
	afx_msg void OnBnClickedCheckLed5();
	afx_msg void OnBnClickedCheckLed6();
	CButton chkLed1;
	CButton chkLed2;
	CButton chkLed3;
	CButton chkLed4;
	CButton chkLed5;
	CButton chkLed6;
	afx_msg void OnBnClickedBtnSendroutes();
	afx_msg void OnBnClickedBtnReadroutes();
};
