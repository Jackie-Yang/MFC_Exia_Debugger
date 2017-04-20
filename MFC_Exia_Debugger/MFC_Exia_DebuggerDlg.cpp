
// MFC_Exia_DebuggerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "MFC_Exia_DebuggerDlg.h"
#include "afxdialogex.h"
#include "Dbt.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CMFC_Exia_DebuggerDlg �Ի���



CMFC_Exia_DebuggerDlg::CMFC_Exia_DebuggerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_Exia_DebuggerDlg::IDD, pParent)
	, m_str_THRO(_T("���ź�"))
	, m_str_RUDD(_T("���ź�"))
	, m_str_ELEV(_T("���ź�"))
	, m_str_AILE(_T("���ź�"))
	, m_str_BuffByte(_T("0 Bytes"))
	, m_str_Accel_Sensor_X(_T("���ź�"))
	, m_str_Accel_Sensor_Y(_T("���ź�"))
	, m_str_Accel_Sensor_Z(_T("���ź�"))
	, m_str_Gyro_Sensor_X(_T("���ź�"))
	, m_str_Gyro_Sensor_Y(_T("���ź�"))
	, m_str_Gyro_Sensor_Z(_T("���ź�"))
	, m_str_HMC5883L_X(_T("���ź�"))
	, m_str_HMC5883L_Y(_T("���ź�"))
	, m_str_HMC5883L_Z(_T("���ź�"))
	, m_str_HMC5883L_Angle(_T("���ź�"))
	, m_str_Roll(_T("���ź�"))
	, m_str_Pitch(_T("���ź�"))
	, m_str_Yaw(_T("���ź�"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_Color_Status = RGB(255, 0, 0);
	m_Color_BuffByte = RGB(0, 0, 0);
	m_Timer_Update_Data = 0;
	m_Timer_Show_Data = 0;
	memset(&m_State,0,sizeof(m_State));
}

void CMFC_Exia_DebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_Combox_COM);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_Static_Status);
	DDX_Control(pDX, IDC_CURVE, m_Curve);
	DDX_Text(pDX, IDC_STATIC_THRO, m_str_THRO);
	DDX_Text(pDX, IDC_STATIC_RUDD, m_str_RUDD);
	DDX_Text(pDX, IDC_STATIC_ELEV, m_str_ELEV);
	DDX_Text(pDX, IDC_STATIC_AILE, m_str_AILE);
	DDX_Text(pDX, IDC_BUFF_BYTE, m_str_BuffByte);
	DDX_Text(pDX, IDC_ACCEL_SENSOR_X, m_str_Accel_Sensor_X);
	DDX_Text(pDX, IDC_ACCEL_SENSOR_Y, m_str_Accel_Sensor_Y);
	DDX_Text(pDX, IDC_ACCEL_SENSOR_Z, m_str_Accel_Sensor_Z);
	DDX_Text(pDX, IDC_GYRO_SENSOR_X, m_str_Gyro_Sensor_X);
	DDX_Text(pDX, IDC_GYRO_SENSOR_Y, m_str_Gyro_Sensor_Y);
	DDX_Text(pDX, IDC_GYRO_SENSOR_Z, m_str_Gyro_Sensor_Z);
	DDX_Text(pDX, IDC_HMC5883L_X, m_str_HMC5883L_X);
	DDX_Text(pDX, IDC_HMC5883L_Y, m_str_HMC5883L_Y);
	DDX_Text(pDX, IDC_HMC5883L_Z, m_str_HMC5883L_Z);
	DDX_Text(pDX, IDC_HMC5883L_ANGLE, m_str_HMC5883L_Angle);
	DDX_Text(pDX, IDC_ROLL, m_str_Roll);
	DDX_Text(pDX, IDC_PITCH, m_str_Pitch);
	DDX_Text(pDX, IDC_YAW, m_str_Yaw);
}

BEGIN_MESSAGE_MAP(CMFC_Exia_DebuggerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_MESSAGE(WM_SERIAL_UPDATE_LIST, &CMFC_Exia_DebuggerDlg::OnSerialUpdateList)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CLOSE, &CMFC_Exia_DebuggerDlg::OnBnClickedOpenCloseBtn)
	ON_WM_DEVICECHANGE()
	ON_MESSAGE(WM_SERIAL_OPEN, &CMFC_Exia_DebuggerDlg::OnSerialOpen)
	ON_MESSAGE(WM_SERIAL_CLOSE, &CMFC_Exia_DebuggerDlg::OnSerialClose)
	ON_WM_CTLCOLOR()
	ON_WM_TIMER()
END_MESSAGE_MAP()


BOOL CMFC_Exia_DebuggerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������

	m_Serial.Init(this);



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFC_Exia_DebuggerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CMFC_Exia_DebuggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CMFC_Exia_DebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//UI���´���״̬
void CMFC_Exia_DebuggerDlg::UpdateSerialState()
{
//�����б�
	CString LastSelStr;
	CString CurSelStr;
	bool bMatchingSel = FALSE;
	int CueSelIndex = m_Combox_COM.GetCurSel();
	if (CueSelIndex >= 0)
	{
		m_Combox_COM.GetLBText(CueSelIndex, LastSelStr);
	}

	m_Combox_COM.ResetContent();
	for (int i = 0; i < m_Serial.GetSerialNum(); i++)
	{
		CurSelStr = m_Serial.GetSerialInfo(i)->str_Port.c_str();
		if (m_Serial.GetSerialInfo(i)->h_Handle)
		{
			CurSelStr += " *";
		}
		m_Combox_COM.InsertString(i, CurSelStr);
		m_Combox_COM.GetLBText(i, CurSelStr);
		if (CurSelStr == LastSelStr || CurSelStr + " *" == LastSelStr || CurSelStr == LastSelStr + " *")
		{
			m_Combox_COM.SetCurSel(i);
			bMatchingSel = TRUE;
		}
	}
	if (!bMatchingSel)
	{
		m_Combox_COM.SetCurSel(0);
	}
//����״̬
	p_SerialInfo pCurSerial = m_Serial.GetCurSerial();
	if (m_Serial.IsOpen() && pCurSerial)
	{
		CString Info;
		Info.Format("������ %s (%s)", pCurSerial->str_Port.c_str(), pCurSerial->str_Name.c_str());
		Info.Replace("\\Device\\", NULL);
		m_Static_Status.SetWindowTextA(Info);
		m_Color_Status = RGB(0, 128, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowTextA("�رմ���");
	}
	else
	{
		m_Static_Status.SetWindowTextA("δ����");
		m_Color_Status = RGB(255, 0, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowTextA("�򿪴���");
	}
	return;
}



void CMFC_Exia_DebuggerDlg::OnBnClickedOpenCloseBtn()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_Serial.IsOpen())
	{
		if (!m_Serial.CloseSerial())
		{
			MessageBoxA(GetLastErrorMessage(), "���ڹر�ʧ��", MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("���ڹر�ʧ�ܣ�"));
		}
		else
		{
			//�ر����ݶ�ʱ��
			if (m_Timer_Update_Data)
			{
				KillTimer(m_Timer_Update_Data);
				m_Timer_Update_Data = 0;
			}
			if (m_Timer_Show_Data)
			{
				KillTimer(m_Timer_Show_Data);
				m_Timer_Show_Data = 0;
			}
		}
	}
	else
	{
		if (!m_Serial.OpenSerial(m_Serial.GetSerialInfo(m_Combox_COM.GetCurSel()), CBR_115200))
		{
			MessageBoxA(GetLastErrorMessage(), "���ڴ�ʧ��", MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("���ڴ�ʧ�ܣ�"));
		}
		else
		{
			//�������ݸ��¶�ʱ��
			if (m_Timer_Update_Data)	//�Ѿ��������ȹر�
			{
				KillTimer(m_Timer_Update_Data);
				m_Timer_Update_Data = 0;
			}
			m_Timer_Update_Data = SetTimer(ID_TIMER_UPDATE_DATA, 50, NULL);
			if (m_Timer_Update_Data == 0)
			{
				//AfxMessageBox("���ݸ��¶�ʱ������ʧ��");
				MessageBoxA(GetLastErrorMessage(), "���ݸ��¶�ʱ������ʧ��", MB_ICONERROR | MB_OK);
			}

			//����������ʾ��ʱ��
			if (m_Timer_Show_Data)	//�Ѿ��������ȹر�
			{
				KillTimer(m_Timer_Show_Data);
				m_Timer_Show_Data = 0;
			}
			m_Timer_Show_Data = SetTimer(ID_TIMER_SHOW_DATA, 500, NULL);
			if (m_Timer_Show_Data == 0)
			{
				//AfxMessageBox("���ݸ��¶�ʱ������ʧ��");
				MessageBoxA(GetLastErrorMessage(), "������ʾ��ʱ������ʧ��", MB_ICONERROR | MB_OK);
			}
		}
	}
}


//�յ��豸������Ϣ��ˢ�´����б�
BOOL CMFC_Exia_DebuggerDlg::OnDeviceChange(UINT nEventType, DWORD_PTR dwData)
{
	switch (nEventType)
	{
		case DBT_DEVNODES_CHANGED:
		{
			m_Serial.UpdateSerialList();
			break;
		}
		default:
		{
			break;
		}
	}
	return TRUE;
}

//ˢ�´����б���ɣ�UIˢ�´���״̬
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialUpdateList(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	return 0;
}

//���ڴ򿪳ɹ���UIˢ�´���״̬
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialOpen(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	return 0;
}

//���ڹرճɹ���UIˢ�´���״̬
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialClose(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	return 0;
}


//�޸�������ɫ
HBRUSH CMFC_Exia_DebuggerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  �ڴ˸��� DC ���κ�����

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATUS)
	{
		pDC->SetTextColor(m_Color_Status);
	}

	if (pWnd->GetDlgCtrlID() == IDC_BUFF_BYTE)
	{
		pDC->SetTextColor(m_Color_BuffByte);
	}

	// TODO:  ���Ĭ�ϵĲ������軭�ʣ��򷵻���һ������
	return hbr;
}



void CMFC_Exia_DebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (nIDEvent == m_Timer_Update_Data)
	{
		GetQuadrotorState();
		m_Serial.ClearRecData();
	}
	else if (nIDEvent == m_Timer_Show_Data)
	{
		ShowQuadrotorState();
	}

	CDialogEx::OnTimer(nIDEvent);
}


CString CMFC_Exia_DebuggerDlg::GetErrorMessage(DWORD dwError, const char* ErrorTip)
{
	CString ErrorMessage = ErrorTip;
	LPVOID lpMsgBuf;

	FormatMessageA(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |		//��������ϵͳ����
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	//��������Ϊ����Ĭ��
		(LPTSTR)&lpMsgBuf,
		0,		//��������ϵͳ����,����Ҫָ����С
		NULL);
	ErrorMessage += (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);	//�ͷ�ϵͳ������ڴ�
	return ErrorMessage;
}

CString CMFC_Exia_DebuggerDlg::GetLastErrorMessage(const char* ErrorTip)
{
	return GetErrorMessage(GetLastError(),ErrorTip);
}




#define CHECK_DATA(DATA,VALUE)	if (m_Serial.GetRecData((PUINT8)&DATA, sizeof(DATA)) < sizeof(DATA))\
								{\
									return FALSE;\
								}\
								if (DATA != VALUE)\
								{\
									continue;\
								}					
bool CMFC_Exia_DebuggerDlg::GetQuadrotorState()
{
	UINT8 u8CheckData = 0;
	UINT16 u16CheckData = 0;
	Quadrotor_State TempState;
	while (TRUE)
	{
		CHECK_DATA(u8CheckData, 0x7F);

		CHECK_DATA(u8CheckData, 0xFF);

		CHECK_DATA(u16CheckData, sizeof(Quadrotor_State));

		if (m_Serial.GetRecData((PUINT8)&TempState, sizeof(Quadrotor_State)) < sizeof(Quadrotor_State))
		{
			return FALSE;
		}

		CHECK_DATA(u16CheckData, 123);

		CHECK_DATA(u16CheckData, 0xFEFF);

		m_State = TempState;
		return TRUE;
	}

	return TRUE;
}

void CMFC_Exia_DebuggerDlg::ShowQuadrotorState()
{
	UINT nBufByte = m_Serial.GetRecBufByte();
	if (nBufByte > BUF_SIZE * 0.8)
	{
		m_Color_BuffByte = RGB(255, 0, 0);
	}
	else
	{
		m_Color_BuffByte = RGB(0, 0, 0);
	}
	//GetDlgItem(IDC_BUFF_BYTE)->InvalidateRect(NULL);
	m_str_BuffByte.Format("%u Bytes", nBufByte);


	if (m_State.Thro)
	{
		m_str_THRO.Format("%u", m_State.Thro);
	}
	else
	{
		m_str_THRO = "���ź�";
	}

	if (m_State.Rudd)
	{
		m_str_RUDD.Format("%u", m_State.Rudd);
	}
	else
	{
		m_str_RUDD = "���ź�";
	}

	if (m_State.Elev)
	{
		m_str_ELEV.Format("%u", m_State.Elev);
	}
	else
	{
		m_str_ELEV = "���ź�";
	}

	if (m_State.Aile)
	{
		m_str_AILE.Format("%u", m_State.Aile);
	}
	else
	{
		m_str_AILE = "���ź�";
	}
	
	m_str_Accel_Sensor_X.Format("%d", m_State.Accel_X);
	m_str_Accel_Sensor_Y.Format("%d", m_State.Accel_Y);
	m_str_Accel_Sensor_Z.Format("%d", m_State.Accel_Z);

	m_str_Gyro_Sensor_X.Format("%d", m_State.Gyro_X);
	m_str_Gyro_Sensor_Y.Format("%d", m_State.Gyro_Y);
	m_str_Gyro_Sensor_Z.Format("%d", m_State.Gyro_Z);

	m_str_HMC5883L_X.Format("%d", m_State.HMC5883L_X);
	m_str_HMC5883L_Y.Format("%d", m_State.HMC5883L_Y);
	m_str_HMC5883L_Z.Format("%d", m_State.HMC5883L_Z);
	m_str_HMC5883L_Angle.Format("%.1f", m_State.HMC5883L_Angle / 10.0f);

	m_str_Roll.Format("%.1f", m_State.Roll / 10.0f);
	m_str_Pitch.Format("%.1f", m_State.Pitch / 10.0f);
	m_str_Yaw.Format("%.1f", m_State.Yaw / 10.0f);
	

	UpdateData(FALSE);
}