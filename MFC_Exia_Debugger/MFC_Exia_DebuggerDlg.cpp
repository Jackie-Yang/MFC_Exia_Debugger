
// MFC_Exia_DebuggerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "MFC_Exia_DebuggerDlg.h"
#include "InputBox.h"
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
	, m_str_Motor1(_T("���ź�"))
	, m_str_Motor2(_T("���ź�"))
	, m_str_Motor3(_T("���ź�"))
	, m_str_Motor4(_T("���ź�"))
	, m_str_Gyro_X(_T("���ź�"))
	, m_str_Gyro_Y(_T("���ź�"))
	, m_str_Gyro_Z(_T("���ź�"))
	, m_str_Accel_X(_T("���ź�"))
	, m_str_Accel_Y(_T("���ź�"))
	, m_str_Accel_Z(_T("���ź�"))
	, m_str_HIGH_KS10X(_T("���ź�"))
	, m_str_HIGH_MS5611(_T("���ź�"))
	, m_str_Temp_MPU6050(_T("���ź�"))
	, m_str_Temp_MS5611(_T("���ź�"))
	, m_str_Press_MS5611(_T("���ź�"))
	, m_str_HIGH_Accel(_T("���ź�"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_Color_Status = RGB(255, 0, 0);
	m_Color_BuffByte = RGB(0, 0, 0);
	m_Timer_Update_Data = 0;
	m_Timer_Show_Data = 0;
	m_Timer_Update_Curve = 0;
	memset(&m_State,0,sizeof(m_State));

	m_pCurveDLG = new CurveDLG();
	if (m_pCurveDLG)
	{
		m_pCurveDLG->Create(IDD_MFC_EXIA_CURVE_DIALOG); //����һ����ģ̬�Ի��� 
	}
}

CMFC_Exia_DebuggerDlg::~CMFC_Exia_DebuggerDlg()
{
	if (m_pCurveDLG)
	{
		m_pCurveDLG->DestroyWindow();
		//delete m_pCurveDLG;
	}
}

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


	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ComBoxGain[Curve].InsertString(0, _T("0.1"));
		m_ComBoxGain[Curve].InsertString(1, _T("0.2"));
		m_ComBoxGain[Curve].InsertString(2, _T("0.5"));
		m_ComBoxGain[Curve].InsertString(3, _T("1"));
		m_ComBoxGain[Curve].InsertString(4, _T("2"));
		m_ComBoxGain[Curve].InsertString(5, _T("5"));
		m_ComBoxGain[Curve].InsertString(6, _T("10"));
		m_ComBoxGain[Curve].InsertString(7, _T("20"));
		m_ComBoxGain[Curve].InsertString(8, _T("50"));
		m_ComBoxGain[Curve].InsertString(9, _T("100"));
		m_ComBoxGain[Curve].InsertString(10, _T("200"));
		m_ComBoxGain[Curve].InsertString(11, _T("500"));
		m_ComBoxGain[Curve].InsertString(12, _T("1000"));
		m_ComBoxGain[Curve].InsertString(13, _T("2000"));
		m_ComBoxGain[Curve].InsertString(14, _T("5000"));
		m_ComBoxGain[Curve].InsertString(15, _T("10000"));
		m_ComBoxGain[Curve].SetCurSel(3);
		m_GainSelected[Curve] = 3;
	}

	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ComBoxCurve[Curve].InsertString(0, _T("���ź�"));
		m_ComBoxCurve[Curve].InsertString(1, _T("���(Roll)"));
		m_ComBoxCurve[Curve].InsertString(2, _T("����(Pitch)"));
		m_ComBoxCurve[Curve].InsertString(3, _T("ƫ��(Yaw)"));
		m_ComBoxCurve[Curve].InsertString(4, _T("X����ٶ�"));
		m_ComBoxCurve[Curve].InsertString(5, _T("Y����ٶ�"));
		m_ComBoxCurve[Curve].InsertString(6, _T("Z����ٶ�"));
		m_ComBoxCurve[Curve].InsertString(7, _T("X����ٶ�"));
		m_ComBoxCurve[Curve].InsertString(8, _T("Y����ٶ�"));
		m_ComBoxCurve[Curve].InsertString(9, _T("Z����ٶ�"));
		m_ComBoxCurve[Curve].SetCurSel(0);
		m_CurveSelected[Curve] = 0;
	}

	UpdateSelected();

	m_CombocRefresh.InsertString(0, _T("ˢ���ʣ�50Hz"));
	m_CombocRefresh.InsertString(1, _T("ˢ���ʣ�25Hz"));
	m_CombocRefresh.InsertString(2, _T("ˢ���ʣ�10Hz"));
	m_CombocRefresh.SetCurSel(0);
	m_nRefreshTime = 20;	//1000/50Hz = 20


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void CMFC_Exia_DebuggerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO:  �ڴ˴������Ϣ����������
	//�رն�ʱ��,�������йرն�ʱ�����������Ϊ������ʱ�򴰿��Ѿ�����
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
	if (m_Timer_Update_Curve)
	{
		KillTimer(m_Timer_Update_Curve);
		m_Timer_Update_Curve = 0;
	}
}

void CMFC_Exia_DebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_Combox_COM);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_Static_Status);
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
	DDX_Control(pDX, IDC_CURVE1_GAIN, m_ComBoxGain[0]);
	DDX_Control(pDX, IDC_CURVE2_GAIN, m_ComBoxGain[1]);
	DDX_Control(pDX, IDC_CURVE3_GAIN, m_ComBoxGain[2]);
	DDX_Control(pDX, IDC_CURVE4_GAIN, m_ComBoxGain[3]);
	DDX_Control(pDX, IDC_CURVE1, m_ComBoxCurve[0]);
	DDX_Control(pDX, IDC_CURVE2, m_ComBoxCurve[1]);
	DDX_Control(pDX, IDC_CURVE3, m_ComBoxCurve[2]);
	DDX_Control(pDX, IDC_CURVE4, m_ComBoxCurve[3]);
	DDX_Control(pDX, IDC_CURVE_REFRESH, m_CombocRefresh);
	DDX_Text(pDX, IDC_MOTOR1, m_str_Motor1);
	DDX_Text(pDX, IDC_MOTOR2, m_str_Motor2);
	DDX_Text(pDX, IDC_MOTOR3, m_str_Motor3);
	DDX_Text(pDX, IDC_MOTOR4, m_str_Motor4);
	DDX_Text(pDX, IDC_GYRO_X, m_str_Gyro_X);
	DDX_Text(pDX, IDC_GYRO_Y, m_str_Gyro_Y);
	DDX_Text(pDX, IDC_GYRO_Z, m_str_Gyro_Z);
	DDX_Text(pDX, IDC_ACCEL_X, m_str_Accel_X);
	DDX_Text(pDX, IDC_ACCEL_Y, m_str_Accel_Y);
	DDX_Text(pDX, IDC_ACCEL_Z, m_str_Accel_Z);
	DDX_Text(pDX, IDC_HIGH_KS10X, m_str_HIGH_KS10X);
	DDX_Text(pDX, IDC_HIGH_MS5611, m_str_HIGH_MS5611);
	DDX_Text(pDX, IDC_TEMP_MPU6050, m_str_Temp_MPU6050);
	DDX_Text(pDX, IDC_TEMP_MS5611, m_str_Temp_MS5611);
	DDX_Text(pDX, IDC_PRESS_MS5611, m_str_Press_MS5611);
	DDX_Text(pDX, IDC_HIGH_ACCEL, m_str_HIGH_Accel);
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
	ON_BN_CLICKED(IDC_BUTTON_OPEN_CURVE, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonOpenCurve)
	ON_BN_CLICKED(IDC_CURVE_ENHANCE, &CMFC_Exia_DebuggerDlg::OnBnClickedCurveEnhance)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BUTTON_SCREENSHOT, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonScreenshot)
	ON_BN_CLICKED(IDC_BUTTON_CLEAR_CURVE, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonClearCurve)
	ON_CBN_SELCHANGE(IDC_CURVE1, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE2, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE3, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE4, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE1_GAIN, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE2_GAIN, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE3_GAIN, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE4_GAIN, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE_REFRESH, &CMFC_Exia_DebuggerDlg::OnCbnSelchangeCurveRefresh)
	ON_STN_CLICKED(IDC_STATIC_THRO, &CMFC_Exia_DebuggerDlg::OnStnClickedStaticThro)
	ON_WM_SETCURSOR()
	ON_STN_CLICKED(IDC_STATIC_RUDD, &CMFC_Exia_DebuggerDlg::OnStnClickedStaticRudd)
END_MESSAGE_MAP()




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
		CurSelStr = m_Serial.GetSerialInfo(i)->str_Port;
		if (m_Serial.GetSerialInfo(i)->h_Handle)
		{
			CurSelStr += _T(" *");
		}
		m_Combox_COM.InsertString(i, CurSelStr);
		m_Combox_COM.GetLBText(i, CurSelStr);
		if (CurSelStr == LastSelStr || CurSelStr + _T(" *") == LastSelStr || CurSelStr == LastSelStr + _T(" *"))
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
		Info.Format(_T("������ %s (%s)"), pCurSerial->str_Port, pCurSerial->str_Name);
		Info.Replace(_T("\\Device\\"), _T(""));
		m_Static_Status.SetWindowText(Info);
		m_Color_Status = RGB(0, 128, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowText(_T("�رմ���"));
	}
	else
	{
		m_Static_Status.SetWindowText(_T("δ����"));
		m_Color_Status = RGB(255, 0, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowText(_T("�򿪴���"));
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
			MessageBox(GetLastErrorMessage(), _T("���ڹر�ʧ��"), MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("���ڹر�ʧ�ܣ�"));
		}
	}
	else
	{
		if (!m_Serial.OpenSerial(m_Serial.GetSerialInfo(m_Combox_COM.GetCurSel()), CBR_115200))
		{
			MessageBox(GetLastErrorMessage(), _T("���ڴ�ʧ��"), MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("���ڴ�ʧ�ܣ�"));
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
	//�������ݸ��¶�ʱ��
	if (m_Timer_Update_Data)	//�Ѿ��������ȹر�
	{
		KillTimer(m_Timer_Update_Data);
		m_Timer_Update_Data = 0;
	}
	m_Timer_Update_Data = SetTimer(ID_TIMER_UPDATE_DATA, 20, NULL);
	if (m_Timer_Update_Data == 0)
	{
		//AfxMessageBox("���ݸ��¶�ʱ������ʧ��");
		MessageBox(GetLastErrorMessage(), _T("���ݸ��¶�ʱ������ʧ��"), MB_ICONERROR | MB_OK);
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
		MessageBox(GetLastErrorMessage(), _T("������ʾ��ʱ������ʧ��"), MB_ICONERROR | MB_OK);
	}

	//�������߸��¶�ʱ��
	if (m_Timer_Update_Curve)	//�Ѿ��������ȹر�
	{
		KillTimer(m_Timer_Update_Curve);
		m_Timer_Update_Curve = 0;
	}
	m_Timer_Update_Curve = SetTimer(ID_TIMER_UPDATE_CURVE, m_nRefreshTime, NULL);
	if (m_Timer_Update_Curve == 0)
	{
		//AfxMessageBox("���ݸ��¶�ʱ������ʧ��");
		MessageBox(GetLastErrorMessage(), _T("���߸��¶�ʱ������ʧ��"), MB_ICONERROR | MB_OK);
	}
	return 0;
}

//���ڹرճɹ�
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialClose(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	//�رն�ʱ��
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
	if (m_Timer_Update_Curve)
	{
		KillTimer(m_Timer_Update_Curve);
		m_Timer_Update_Curve = 0;
	}
	InitQuadrotorState();
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



//��ʱ������
void CMFC_Exia_DebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ

	//�Ӵ��ڸ�������
	if (nIDEvent == m_Timer_Update_Data)
	{
		bool result = FALSE;
		result = GetQuadrotorState();	//��ȡ����
		m_Serial.ClearRecData();	//��ջ�����

		if (result && m_pCurveDLG)	//��ȡ�ɹ�����ӵ�������
		{
			SetCurveData();	//��������ѡ��������������
			m_pCurveDLG->m_Curve.AddData(&m_CurveData);
		}
	}
	else if (nIDEvent == m_Timer_Show_Data)
	{
		ShowQuadrotorState();
		m_pCurveDLG->UpdateValueStr();
	}
	else if (nIDEvent == m_Timer_Update_Curve)	//ˢ������
	{
		if (m_pCurveDLG)
		m_pCurveDLG->UpdateCurve();
	}

	CDialogEx::OnTimer(nIDEvent);
}


//��������ѡ��������������
void CMFC_Exia_DebuggerDlg::SetCurveData()
{
	CString str;
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ComBoxGain[Curve].GetLBText(m_GainSelected[Curve], str);
		m_CurveData[Curve].fGain = (float)_ttof(str);
		switch (m_CurveSelected[Curve])
		{
			case 1:
			{
				m_CurveData[Curve].fData = m_State.f_Roll;
				break;
			}
			case 2:
			{
				m_CurveData[Curve].fData = m_State.f_Pitch;
				break;
			}
			case 3:
			{
				m_CurveData[Curve].fData = m_State.f_Yaw;
				break;
			}
			case 4:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro_X / 16.4f;
				break;
			}
			case 5:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro_Y / 16.4f;
				break;
			}
			case 6:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro_Z / 16.4f;
				break;
			}
			case 7:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel_X / 16384.0f;
				break;
			}
			case 8:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel_Y / 16384.0f;
				break;
			}
			case 9:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel_Z / 16384.0f;
				break;
			}
			default:
			{
				m_CurveData[Curve].fData = 0.0f;;
				break;
			}
		}
	}

}

void CMFC_Exia_DebuggerDlg::UpdateSelected()
{
	CString GainStr;
	CString CurveStr;
	CString LabelStr[CURVE_LINE];
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_GainSelected[Curve] = m_ComBoxGain[Curve].GetCurSel();		//����ѡ����
		m_CurveSelected[Curve] = m_ComBoxCurve[Curve].GetCurSel();
		if (m_CurveSelected[Curve])
		{
			m_ComBoxGain[Curve].GetLBText(m_GainSelected[Curve], GainStr);	//��ȡѡ�����ַ���
			m_ComBoxCurve[Curve].GetLBText(m_CurveSelected[Curve], CurveStr);
			LabelStr[Curve].Format(_T("%s(x%s)"), CurveStr, GainStr);
		}
		else
		{
			LabelStr[Curve] = _T("���ź�");
		}
	}
	m_pCurveDLG->SetLabelStr(&LabelStr);
}


CString CMFC_Exia_DebuggerDlg::GetErrorMessage(DWORD dwError, const TCHAR* ErrorTip)
{
	CString ErrorMessage = ErrorTip;
	LPVOID lpMsgBuf = NULL;

	FormatMessage(
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

CString CMFC_Exia_DebuggerDlg::GetLastErrorMessage(const TCHAR* ErrorTip)
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
	Quadrotor_State TempState;
	while (TRUE)
	{
		CHECK_DATA(TempState.u8_DataHead1, DATA_HEAD1);

		CHECK_DATA(TempState.u8_DataHead2, DATA_HEAD2);

		CHECK_DATA(TempState.u16_DataSize, sizeof(Quadrotor_State));

		UINT16 ReceiveDataSize = TempState.u16_DataSize - sizeof(TempState.u8_DataHead1) - sizeof(TempState.u8_DataHead2) - sizeof(TempState.u16_DataSize) - sizeof(TempState.u16_DataCheckValue) - sizeof(TempState.u16_DataEnd);

		if (m_Serial.GetRecData((PUINT8)&TempState.u16_DataSize + sizeof(TempState.u16_DataSize), ReceiveDataSize) < ReceiveDataSize)
		{
			return FALSE;
		}

		CHECK_DATA(TempState.u16_DataCheckValue, 123);

		CHECK_DATA(TempState.u16_DataEnd, 0xFEFF);

		m_State = TempState;
		return TRUE;
	}

	return TRUE;
}


//����ȡ����������ʾ���ؼ�
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
	m_str_BuffByte.Format(_T("%u Bytes"), nBufByte);


	if (m_State.u16_Thro)
	{
		m_str_THRO.Format(_T("%d"), (int)(((float)m_State.u16_Thro - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_THRO = _T("���ź�");
	}

	if (m_State.u16_Rudd)
	{
		m_str_RUDD.Format(_T("%d"), (int)(((float)m_State.u16_Rudd - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_RUDD = _T("���ź�");
	}

	if (m_State.u16_Elev)
	{
		m_str_ELEV.Format(_T("%d"), (int)(((float)m_State.u16_Elev - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_ELEV = _T("���ź�");
	}

	if (m_State.u16_Aile)
	{
		m_str_AILE.Format(_T("%d"), (int)(((float)m_State.u16_Aile - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_AILE = _T("���ź�");
	}
	
	m_str_Accel_Sensor_X.Format(_T("%d"), m_State.s16_Accel_X);
	m_str_Accel_Sensor_Y.Format(_T("%d"), m_State.s16_Accel_Y);
	m_str_Accel_Sensor_Z.Format(_T("%d"), m_State.s16_Accel_Z);

	m_str_Gyro_Sensor_X.Format(_T("%d"), m_State.s16_Gyro_X);
	m_str_Gyro_Sensor_Y.Format(_T("%d"), m_State.s16_Gyro_Y);
	m_str_Gyro_Sensor_Z.Format(_T("%d"), m_State.s16_Gyro_Z);

	m_str_HMC5883L_X.Format(_T("%d"), m_State.s16_HMC5883L_X);
	m_str_HMC5883L_Y.Format(_T("%d"), m_State.s16_HMC5883L_Y);
	m_str_HMC5883L_Z.Format(_T("%d"), m_State.s16_HMC5883L_Z);
	m_str_HMC5883L_Angle.Format(_T("%.1f"), m_State.f_HMC5883L_Angle);

	m_str_Roll.Format(_T("%.1f"), m_State.f_Roll);
	m_str_Pitch.Format(_T("%.1f"), m_State.f_Pitch);
	m_str_Yaw.Format(_T("%.1f"), m_State.f_Yaw);
	
	if (m_State.u16_Motor1)
	{
		m_str_Motor1.Format(_T("%d"), (int)(((float)m_State.u16_Motor1 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor1 = _T("���ź�");
	}
	if (m_State.u16_Motor2)
	{
		m_str_Motor2.Format(_T("%d"), (int)(((float)m_State.u16_Motor2 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor2 = _T("���ź�");
	}
	if (m_State.u16_Motor3)
	{
		m_str_Motor3.Format(_T("%d"), (int)(((float)m_State.u16_Motor3 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor3 = _T("���ź�");
	}
	if (m_State.u16_Motor4)
	{
		m_str_Motor4.Format(_T("%d"), (int)(((float)m_State.u16_Motor4 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor4 = _T("���ź�");
	}
	
	m_str_Gyro_X.Format(_T("%.1f"), (float)m_State.s16_Gyro_X / 16.4f);
	m_str_Gyro_Y.Format(_T("%.1f"), (float)m_State.s16_Gyro_Y / 16.4f);
	m_str_Gyro_Z.Format(_T("%.1f"), (float)m_State.s16_Gyro_Z / 16.4f);

	m_str_Accel_X.Format(_T("%.1f"), (float)m_State.s16_Accel_X / 16384.0f);
	m_str_Accel_Y.Format(_T("%.1f"), (float)m_State.s16_Accel_Y / 16384.0f);
	m_str_Accel_Z.Format(_T("%.1f"), (float)m_State.s16_Accel_Z / 16384.0f);

	m_str_HIGH_KS10X.Format(_T("%.3f"), (float)m_State.u16_KS10X_High / 1000.0f);
	m_str_HIGH_MS5611.Format(_T("%.3f"), (float)m_State.s32_MS5611_HIGH / 1000.0f);
	m_str_Temp_MPU6050.Format(_T("%.3f"), (float)m_State.s16_MPU6050_Temp / 340.0f + 36.53);
	m_str_Temp_MS5611.Format(_T("%.2f"), (float)m_State.s32_MS5611_Temp / 100.0f);
	m_str_Press_MS5611.Format(_T("%.3f"), (float)m_State.s32_MS5611_Press / 1000.0f);
	//m_str_HIGH_Accel
	UpdateData(FALSE);
}

void CMFC_Exia_DebuggerDlg::OnBnClickedButtonOpenCurve()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (m_pCurveDLG)
	{
		m_pCurveDLG->ShowWindow(SW_SHOW);
	}
	else
	{
		MessageBox(_T("�޷������߼�ش���"), _T("��ʧ��"), MB_ICONERROR | MB_OK);
	}
}


//����ͼ����ǿ
void CMFC_Exia_DebuggerDlg::OnBnClickedCurveEnhance()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	if (((CButton*)GetDlgItem(IDC_CURVE_ENHANCE))->GetCheck())
	{
		if (!m_pCurveDLG->CurveEnhance(TRUE))
		{
			((CButton*)GetDlgItem(IDC_CURVE_ENHANCE))->SetCheck(FALSE);
			MessageBox(_T("�޷��򿪻�����ǿ"), _T("��ʧ��"), MB_ICONERROR | MB_OK);
		}
	}
	else
	{
		m_pCurveDLG->CurveEnhance(FALSE);
	}
}




//���߽�ͼ
void CMFC_Exia_DebuggerDlg::OnBnClickedButtonScreenshot()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CTime CurTime; 
	CurTime = CTime::GetCurrentTime();	//��ȡϵͳʱ��
	CString stFilePath_save = CurTime.Format(_T("ScreenShot %Y_%m_%d %H.%M.%S.bmp"));

	int nDot_index = stFilePath_save.ReverseFind(_T('.'));	//��ȡ��׺'.'λ��
	////��ȡ��׺��
	CString szFileType = stFilePath_save.Right(stFilePath_save.GetLength() - nDot_index);
	szFileType.MakeLower();	//ͳһ��׺Сд


	// ���ù����������˳�Bmp�ļ�
	CString szOpenFilter = _T("λͼ�ļ� (*.bmp;*.dib)|*.bmp;*.dib|PNG (*.png)|*.png|JPEG (*.jpg;*.jpeg;*.jpe)|*.jpg;*.jpeg;*.jpe|GIF (*.gif)|*.gif||");
	// ���챣���ļ��Ի��� 
	CFileDialog SaveDlgDst(FALSE, szFileType, stFilePath_save, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szOpenFilter);

	if (SaveDlgDst.DoModal() == IDOK)
	{
		stFilePath_save = SaveDlgDst.GetPathName();

		if (!m_pCurveDLG->m_Curve.ScreenShot(stFilePath_save))
		{
			AfxMessageBox(_T("�ļ�����ʧ��,�����ļ�������׺��"), MB_ICONINFORMATION);
		}

	}

}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonClearCurve()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	m_pCurveDLG->m_Curve.ClearData();
}


void CMFC_Exia_DebuggerDlg::OnCbnSelchangeCurveRefresh()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CString RefreshStr;
	INT_PTR RefreshTime;
	switch (m_CombocRefresh.GetCurSel())
	{
		case 0:
		{
			RefreshTime = 20;	//1000ms/50Hz = 20ms
			break;
		}
		case 1:
		{
			RefreshTime = 40;	//1000ms/25Hz = 40ms
			break;
		}
		case 2:
		{
			RefreshTime = 100;	//1000ms/10Hz = 100ms
			break;
		}
	}
	if (RefreshTime != m_nRefreshTime)
	{
		m_nRefreshTime = RefreshTime;
		//�������߸��¶�ʱ��
		if (m_Timer_Update_Curve)	//�Ѿ��������������ã�û�򿪾Ͳ��ô򿪣����ڿ����ٿ�
		{
			KillTimer(m_Timer_Update_Curve);
			m_Timer_Update_Curve = 0;
			m_Timer_Update_Curve = SetTimer(ID_TIMER_UPDATE_CURVE, m_nRefreshTime, NULL);
			if (m_Timer_Update_Curve == 0)
			{
				//AfxMessageBox("���ݸ��¶�ʱ������ʧ��");
				MessageBox(GetLastErrorMessage(), _T("���߸��¶�ʱ������ʧ��"), MB_ICONERROR | MB_OK);
			}
		}

	}
}



void CMFC_Exia_DebuggerDlg::InitQuadrotorState()
{
	memset(&m_State, 0, sizeof(m_State));
	m_str_THRO = _T("���ź�");
	m_str_RUDD = _T("���ź�");
	m_str_ELEV = _T("���ź�");
	m_str_AILE = _T("���ź�");
	m_str_BuffByte = _T("0 Bytes");
	m_str_Accel_Sensor_X = _T("���ź�");
	m_str_Accel_Sensor_Y = _T("���ź�");
	m_str_Accel_Sensor_Z = _T("���ź�");
	m_str_Gyro_Sensor_X = _T("���ź�");
	m_str_Gyro_Sensor_Y = _T("���ź�");
	m_str_Gyro_Sensor_Z = _T("���ź�");
	m_str_HMC5883L_X = _T("���ź�");
	m_str_HMC5883L_Y = _T("���ź�");
	m_str_HMC5883L_Z = _T("���ź�");
	m_str_HMC5883L_Angle = _T("���ź�");
	m_str_Roll = _T("���ź�");
	m_str_Pitch = _T("���ź�");
	m_str_Yaw = _T("���ź�");
	m_str_Motor1 = _T("���ź�");
	m_str_Motor2 = _T("���ź�");
	m_str_Motor3 = _T("���ź�");
	m_str_Motor4 = _T("���ź�");
	m_str_Gyro_X = _T("���ź�");
	m_str_Gyro_Y = _T("���ź�");
	m_str_Gyro_Z = _T("���ź�");
	m_str_Accel_X = _T("���ź�");
	m_str_Accel_Y = _T("���ź�");
	m_str_Accel_Z = _T("���ź�");

	m_str_HIGH_KS10X = _T("���ź�");
	m_str_HIGH_MS5611 = _T("���ź�");
	m_str_Temp_MPU6050 = _T("���ź�");
	m_str_Temp_MS5611 = _T("���ź�");
	m_str_Press_MS5611 = _T("���ź�");
	m_str_HIGH_Accel = _T("���ź�");
}




void CMFC_Exia_DebuggerDlg::OnOK()
{
	// TODO:  �ڴ����ר�ô����/����û���
	//int nFocusID = GetFocus()->GetDlgCtrlID();
	//switch (nFocusID)
	//{
	//	default:
	//	{
	//		break;
	//	}
	//}

	//CDialogEx::OnOK();
}


void CMFC_Exia_DebuggerDlg::OnStnClickedStaticThro()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CInputBox InputBox(_T("�����趨"), _T("�����������趨ֵ:"), _T("0"));
	if (IDOK == InputBox.DoModal())
	{
		int nInput;
		nInput = _ttoi(InputBox.GetInput());
		if (nInput > 100)
		{
			nInput = 100;
		}
		else if (nInput < 0)
		{
			nInput = 0;
		}
	}
}


//��������ƶ����Ϸ��Ĺ��
BOOL CMFC_Exia_DebuggerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int DlgCtrlID = pWnd->GetDlgCtrlID();
	switch (DlgCtrlID)
	{
		case IDC_STATIC_THRO:
		case IDC_STATIC_RUDD:
		case IDC_STATIC_ELEV:
		case IDC_STATIC_AILE:
		{
			SetCursor(LoadCursor(NULL, IDC_HAND));
			return TRUE;
		}
		default:
		{
			break;
		}
	}

	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}


void CMFC_Exia_DebuggerDlg::OnStnClickedStaticRudd()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
}
