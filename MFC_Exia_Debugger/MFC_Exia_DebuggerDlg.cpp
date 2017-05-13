
// MFC_Exia_DebuggerDlg.cpp : 实现文件
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


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// CMFC_Exia_DebuggerDlg 对话框



CMFC_Exia_DebuggerDlg::CMFC_Exia_DebuggerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CMFC_Exia_DebuggerDlg::IDD, pParent)
	, m_str_THRO(_T("无信号"))
	, m_str_RUDD(_T("无信号"))
	, m_str_ELEV(_T("无信号"))
	, m_str_AILE(_T("无信号"))
	, m_str_BuffByte(_T("0 Bytes"))
	, m_str_Serial_InQue(_T("0 Bytes"))
	, m_str_Accel_Sensor_X(_T("无信号"))
	, m_str_Accel_Sensor_Y(_T("无信号"))
	, m_str_Accel_Sensor_Z(_T("无信号"))
	, m_str_Gyro_Sensor_X(_T("无信号"))
	, m_str_Gyro_Sensor_Y(_T("无信号"))
	, m_str_Gyro_Sensor_Z(_T("无信号"))
	, m_str_HMC5883L_X(_T("无信号"))
	, m_str_HMC5883L_Y(_T("无信号"))
	, m_str_HMC5883L_Z(_T("无信号"))
	, m_str_HMC5883L_Angle(_T("无信号"))
	, m_str_Roll(_T("无信号"))
	, m_str_Pitch(_T("无信号"))
	, m_str_Yaw(_T("无信号"))
	, m_str_Motor1(_T("无信号"))
	, m_str_Motor2(_T("无信号"))
	, m_str_Motor3(_T("无信号"))
	, m_str_Motor4(_T("无信号"))
	, m_str_Gyro_X(_T("无信号"))
	, m_str_Gyro_Y(_T("无信号"))
	, m_str_Gyro_Z(_T("无信号"))
	, m_str_Accel_X(_T("无信号"))
	, m_str_Accel_Y(_T("无信号"))
	, m_str_Accel_Z(_T("无信号"))
	, m_str_HIGH_KS10X(_T("无信号"))
	, m_str_HIGH_MS5611(_T("无信号"))
	, m_str_Temp_MPU6050(_T("无信号"))
	, m_str_Temp_MS5611(_T("无信号"))
	, m_str_Press_MS5611(_T("无信号"))
	, m_str_HIGH_Accel(_T("无信号"))
	, m_str_Roll_G_Kp(_T("无信号"))
	, m_str_Roll_G_Ki(_T("无信号"))
	, m_str_Roll_G_Kd(_T("无信号"))
	, m_str_Roll_A_Kp(_T("无信号"))
	, m_str_Roll_A_Ki(_T("无信号"))
	, m_str_Roll_A_Kd(_T("无信号"))
	, m_str_Pitch_G_Kp(_T("无信号"))
	, m_str_Pitch_G_Ki(_T("无信号"))
	, m_str_Pitch_G_Kd(_T("无信号"))
	, m_str_Pitch_A_Kp(_T("无信号"))
	, m_str_Pitch_A_Ki(_T("无信号"))
	, m_str_Pitch_A_Kd(_T("无信号"))
	, m_str_Yaw_G_Kp(_T("无信号"))
	, m_str_Yaw_G_Ki(_T("无信号"))
	, m_str_Yaw_G_Kd(_T("无信号"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_Color_Status = RGB(255, 0, 0);
	m_Color_BuffByte = RGB(0, 0, 0);
	m_Color_SerialInQueByte = RGB(0, 0, 0);
	m_Timer_Update_Data = 0;
	m_Timer_Show_Data = 0;
	m_Timer_Update_Curve = 0;
	memset(&m_State,0,sizeof(m_State));

	m_pCurveDLG = new CurveDLG();
	if (m_pCurveDLG)
	{
		m_pCurveDLG->Create(IDD_MFC_EXIA_CURVE_DIALOG); //创建一个非模态对话框 
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

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO:  在此添加额外的初始化代码

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
		m_ComBoxCurve[Curve].InsertString(SEL_NONE, _T("无信号"));
		m_ComBoxCurve[Curve].InsertString(SEL_ROLL, _T("侧滚(Roll)"));
		m_ComBoxCurve[Curve].InsertString(SEL_PITCH, _T("俯仰(Pitch)"));
		m_ComBoxCurve[Curve].InsertString(SEL_YAW, _T("偏摆(Yaw)"));
		m_ComBoxCurve[Curve].InsertString(SEL_COMPASS, _T("罗盘角度"));
		m_ComBoxCurve[Curve].InsertString(SEL_X_GYRO, _T("X轴角速度"));
		m_ComBoxCurve[Curve].InsertString(SEL_Y_GYRO, _T("Y轴角速度"));
		m_ComBoxCurve[Curve].InsertString(SEL_Z_GYRO, _T("Z轴角速度"));
		m_ComBoxCurve[Curve].InsertString(SEL_X_ACCEL, _T("X轴加速度"));
		m_ComBoxCurve[Curve].InsertString(SEL_Y_ACCEL, _T("Y轴加速度"));
		m_ComBoxCurve[Curve].InsertString(SEL_Z_ACCEL, _T("Z轴加速度"));
		m_ComBoxCurve[Curve].SetCurSel(0);
		m_CurveSelected[Curve] = 0;
	}

	UpdateSelected(0);

	m_CombocRefresh.InsertString(0, _T("刷新率：50Hz"));
	m_CombocRefresh.InsertString(1, _T("刷新率：25Hz"));
	m_CombocRefresh.InsertString(2, _T("刷新率：10Hz"));
	m_CombocRefresh.SetCurSel(0);
	m_nRefreshTime = 20;	//1000/50Hz = 20


	GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetFocus();
	return FALSE;

	//return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CMFC_Exia_DebuggerDlg::OnDestroy()
{
	CDialogEx::OnDestroy();
	// TODO:  在此处添加消息处理程序代码
	//关闭定时器,在析构中关闭定时器会崩溃，因为析构的时候窗口已经销毁
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
	DDX_Text(pDX, IDC_STATIC_ROLL_G_Kp, m_str_Roll_G_Kp);
	DDX_Text(pDX, IDC_STATIC_ROLL_G_Ki, m_str_Roll_G_Ki);
	DDX_Text(pDX, IDC_STATIC_ROLL_G_Kd, m_str_Roll_G_Kd);
	DDX_Text(pDX, IDC_STATIC_ROLL_A_Kp, m_str_Roll_A_Kp);
	DDX_Text(pDX, IDC_STATIC_ROLL_A_Ki, m_str_Roll_A_Ki);
	DDX_Text(pDX, IDC_STATIC_ROLL_A_Kd, m_str_Roll_A_Kd);
	DDX_Text(pDX, IDC_STATIC_PITCH_G_Kp, m_str_Pitch_G_Kp);
	DDX_Text(pDX, IDC_STATIC_PITCH_G_Ki, m_str_Pitch_G_Ki);
	DDX_Text(pDX, IDC_STATIC_PITCH_G_Kd, m_str_Pitch_G_Kd);
	DDX_Text(pDX, IDC_STATIC_PITCH_A_Kp, m_str_Pitch_A_Kp);
	DDX_Text(pDX, IDC_STATIC_PITCH_A_Ki, m_str_Pitch_A_Ki);
	DDX_Text(pDX, IDC_STATIC_PITCH_A_Kd, m_str_Pitch_A_Kd);
	DDX_Text(pDX, IDC_STATIC_YAW_G_Kp, m_str_Yaw_G_Kp);
	DDX_Text(pDX, IDC_STATIC_YAW_G_Ki, m_str_Yaw_G_Ki);
	DDX_Text(pDX, IDC_STATIC_YAW_G_Kd, m_str_Yaw_G_Kd);
	DDX_Text(pDX, IDC_SERIAL_INQUE_BYTE, m_str_Serial_InQue);
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
	ON_CONTROL_RANGE(CBN_SELCHANGE, IDC_CURVE1_GAIN, IDC_CURVE4, &CMFC_Exia_DebuggerDlg::UpdateSelected)
	ON_CBN_SELCHANGE(IDC_CURVE_REFRESH, &CMFC_Exia_DebuggerDlg::OnCbnSelchangeCurveRefresh)
	ON_WM_SETCURSOR()
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STATIC_THRO, IDC_STATIC_AILE, &CMFC_Exia_DebuggerDlg::OnStnClickedColtrol)
	ON_CONTROL_RANGE(STN_CLICKED, IDC_STATIC_ROLL_G_Kp, IDC_STATIC_YAW_G_Kd, &CMFC_Exia_DebuggerDlg::OnStnClickedSetPID)
	ON_BN_CLICKED(IDC_BUTTON_HORIZON, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonHorizon)
	ON_BN_CLICKED(IDC_BUTTON_STOP, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonStop)
	ON_BN_CLICKED(IDC_BUTTON_SELFTEST, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonSelftest)
	ON_BN_CLICKED(IDC_BUTTON_PID_SAVE, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonPidSave)
	ON_BN_CLICKED(IDC_BUTTON_PID_LOAD, &CMFC_Exia_DebuggerDlg::OnBnClickedButtonPidLoad)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CMFC_Exia_DebuggerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CMFC_Exia_DebuggerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


//UI更新串口状态
void CMFC_Exia_DebuggerDlg::UpdateSerialState()
{
//更新列表
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
//更新状态
	p_SerialInfo pCurSerial = m_Serial.GetCurSerial();
	if (m_Serial.IsOpen() && pCurSerial)
	{
		CString Info;
		Info.Format(_T("已连接 %s (%s)"), pCurSerial->str_Port, pCurSerial->str_Name);
		Info.Replace(_T("\\Device\\"), _T(""));
		m_Static_Status.SetWindowText(Info);
		m_Color_Status = RGB(0, 128, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowText(_T("关闭串口"));
	}
	else
	{
		m_Static_Status.SetWindowText(_T("未连接"));
		m_Color_Status = RGB(255, 0, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowText(_T("打开串口"));
	}
	return;
}



void CMFC_Exia_DebuggerDlg::OnBnClickedOpenCloseBtn()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_Serial.IsOpen())
	{
		if (!m_Serial.CloseSerial())
		{
			MessageBox(GetLastErrorMessage(), _T("串口关闭失败"), MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("串口关闭失败："));
		}
	}
	else
	{
		if (!m_Serial.OpenSerial(m_Serial.GetSerialInfo(m_Combox_COM.GetCurSel()), CBR_115200))
		{
			MessageBox(GetLastErrorMessage(), _T("串口打开失败"), MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("串口打开失败："));
		}
	}
}


//收到设备更新消息，刷新串口列表
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

//刷新串口列表完成，UI刷新串口状态
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialUpdateList(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	return 0;
}

//串口打开成功，UI刷新串口状态
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialOpen(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	//开启数据更新定时器
	if (m_Timer_Update_Data)	//已经开启则先关闭
	{
		KillTimer(m_Timer_Update_Data);
		m_Timer_Update_Data = 0;
	}
	m_Timer_Update_Data = SetTimer(ID_TIMER_UPDATE_DATA, 20, NULL);
	if (m_Timer_Update_Data == 0)
	{
		//AfxMessageBox("数据更新定时器设置失败");
		MessageBox(GetLastErrorMessage(), _T("数据更新定时器设置失败"), MB_ICONERROR | MB_OK);
	}

	//开启数据显示定时器
	if (m_Timer_Show_Data)	//已经开启则先关闭
	{
		KillTimer(m_Timer_Show_Data);
		m_Timer_Show_Data = 0;
	}
	m_Timer_Show_Data = SetTimer(ID_TIMER_SHOW_DATA, 500, NULL);
	if (m_Timer_Show_Data == 0)
	{
		//AfxMessageBox("数据更新定时器设置失败");
		MessageBox(GetLastErrorMessage(), _T("数据显示定时器设置失败"), MB_ICONERROR | MB_OK);
	}

	//开启曲线更新定时器
	if (m_Timer_Update_Curve)	//已经开启则先关闭
	{
		KillTimer(m_Timer_Update_Curve);
		m_Timer_Update_Curve = 0;
	}
	m_Timer_Update_Curve = SetTimer(ID_TIMER_UPDATE_CURVE, m_nRefreshTime, NULL);
	if (m_Timer_Update_Curve == 0)
	{
		//AfxMessageBox("数据更新定时器设置失败");
		MessageBox(GetLastErrorMessage(), _T("曲线更新定时器设置失败"), MB_ICONERROR | MB_OK);
	}
	return 0;
}

//串口关闭成功
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialClose(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
	//关闭定时器
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


//修改字体颜色
HBRUSH CMFC_Exia_DebuggerDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialogEx::OnCtlColor(pDC, pWnd, nCtlColor);

	// TODO:  在此更改 DC 的任何特性

	if (pWnd->GetDlgCtrlID() == IDC_STATIC_STATUS)
	{
		pDC->SetTextColor(m_Color_Status);
	}

	if (pWnd->GetDlgCtrlID() == IDC_BUFF_BYTE)
	{
		pDC->SetTextColor(m_Color_BuffByte);
	}

	if (pWnd->GetDlgCtrlID() == IDC_SERIAL_INQUE_BYTE)
	{
		pDC->SetTextColor(m_Color_SerialInQueByte);
	}

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}



//定时器处理
void CMFC_Exia_DebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值

	//从串口更新数据
	if (nIDEvent == m_Timer_Update_Data)
	{
		bool result = FALSE;
		while (GetQuadrotorState())	//拿到缓冲区最新的数据
		{
			result = TRUE;	//获取数据
		} 
		//m_Serial.ClearRecData();	//清空缓冲区
		if (result && m_pCurveDLG)	//获取成功则添加到曲线中
		{
			SetCurveData();	//根据曲线选项设置曲线数据
			m_pCurveDLG->m_Curve.AddData(&m_CurveData);
		}
	}
	else if (nIDEvent == m_Timer_Show_Data)
	{
		ShowQuadrotorState();
		m_pCurveDLG->UpdateValueStr();
	}
	else if (nIDEvent == m_Timer_Update_Curve)	//刷新曲线
	{
		if (m_pCurveDLG)
		m_pCurveDLG->UpdateCurve();
	}

	CDialogEx::OnTimer(nIDEvent);
}


//根据曲线选项设置曲线数据
void CMFC_Exia_DebuggerDlg::SetCurveData()
{
	CString str;
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ComBoxGain[Curve].GetLBText(m_GainSelected[Curve], str);
		m_CurveData[Curve].fGain = (float)_ttof(str);
		switch (m_CurveSelected[Curve])
		{
			case SEL_ROLL:
			{
				m_CurveData[Curve].fData = m_State.f_Roll;
				break;
			}
			case SEL_PITCH:
			{
				m_CurveData[Curve].fData = m_State.f_Pitch;
				break;
			}
			case SEL_YAW:
			{
				m_CurveData[Curve].fData = m_State.f_Yaw;
				break;
			}
			case SEL_COMPASS:
			{
				m_CurveData[Curve].fData = m_State.f_HMC5883L_Angle;
				break;
			}
			case SEL_X_GYRO:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro[0] / 16.4f;
				break;
			}
			case SEL_Y_GYRO:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro[1] / 16.4f;
				break;
			}
			case SEL_Z_GYRO:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Gyro[2] / 16.4f;
				break;
			}
			case SEL_X_ACCEL:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel[0] / 16384.0f;
				break;
			}
			case SEL_Y_ACCEL:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel[1] / 16384.0f;
				break;
			}
			case SEL_Z_ACCEL:
			{
				m_CurveData[Curve].fData = (float)m_State.s16_Accel[2] / 16384.0f;
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

void CMFC_Exia_DebuggerDlg::UpdateSelected(UINT uID)
{
	CString GainStr;
	CString CurveStr;
	CString LabelStr[CURVE_LINE];
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_GainSelected[Curve] = m_ComBoxGain[Curve].GetCurSel();		//更新选中项
		m_CurveSelected[Curve] = m_ComBoxCurve[Curve].GetCurSel();
		if (m_CurveSelected[Curve])
		{
			m_ComBoxGain[Curve].GetLBText(m_GainSelected[Curve], GainStr);	//获取选中项字符串
			m_ComBoxCurve[Curve].GetLBText(m_CurveSelected[Curve], CurveStr);
			LabelStr[Curve].Format(_T("%s(x%s)"), CurveStr, GainStr);
		}
		else
		{
			LabelStr[Curve] = _T("无信号");
		}
	}
	m_pCurveDLG->SetLabelStr(&LabelStr);
}


CString CMFC_Exia_DebuggerDlg::GetErrorMessage(DWORD dwError, const TCHAR* ErrorTip)
{
	CString ErrorMessage = ErrorTip;
	LPVOID lpMsgBuf = NULL;

	FormatMessage(
		FORMAT_MESSAGE_ALLOCATE_BUFFER |		//缓冲区由系统分配
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		dwError,
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),	//语言设置为本地默认
		(LPTSTR)&lpMsgBuf,
		0,		//缓冲区由系统分配,不需要指定大小
		NULL);
	ErrorMessage += (LPTSTR)lpMsgBuf;
	LocalFree(lpMsgBuf);	//释放系统分配的内存
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


//将获取到的数据显示到控件
void CMFC_Exia_DebuggerDlg::ShowQuadrotorState()
{
	UINT nBufByte = m_Serial.GetRecBufByte();
	DWORD nSerialInQueByte = m_Serial.GetHardBufByte();
	if (nBufByte > BUF_SIZE * 0.8)
	{
		m_Color_BuffByte = RGB(255, 0, 0);
	}
	else
	{
		m_Color_BuffByte = RGB(0, 0, 0);
	}
	if (nSerialInQueByte > BUF_SIZE * 0.8)
	{
		m_Color_SerialInQueByte = RGB(255, 0, 0);
	}
	else
	{
		m_Color_SerialInQueByte = RGB(0, 0, 0);
	}
	//GetDlgItem(IDC_BUFF_BYTE)->InvalidateRect(NULL);
	m_str_BuffByte.Format(_T("%u Bytes"), nBufByte);
	m_str_Serial_InQue.Format(_T("%u Bytes"), nSerialInQueByte);


	if (m_State.u16_Thro)
	{
		m_str_THRO.Format(_T("%d"), (int)(((float)m_State.u16_Thro - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_THRO = _T("无信号");
	}

	if (m_State.u16_Rudd)
	{
		m_str_RUDD.Format(_T("%d"), (int)(((float)m_State.u16_Rudd - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_RUDD = _T("无信号");
	}

	if (m_State.u16_Elev)
	{
		m_str_ELEV.Format(_T("%d"), (int)(((float)m_State.u16_Elev - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_ELEV = _T("无信号");
	}

	if (m_State.u16_Aile)
	{
		m_str_AILE.Format(_T("%d"), (int)(((float)m_State.u16_Aile - 1100.0f) / 8.0f + 0.5f) - 50);
	}
	else
	{
		m_str_AILE = _T("无信号");
	}
	
	m_str_Accel_Sensor_X.Format(_T("%d"), m_State.s16_Accel[0]);
	m_str_Accel_Sensor_Y.Format(_T("%d"), m_State.s16_Accel[1]);
	m_str_Accel_Sensor_Z.Format(_T("%d"), m_State.s16_Accel[2]);

	m_str_Gyro_Sensor_X.Format(_T("%d"), m_State.s16_Gyro[0]);
	m_str_Gyro_Sensor_Y.Format(_T("%d"), m_State.s16_Gyro[1]);
	m_str_Gyro_Sensor_Z.Format(_T("%d"), m_State.s16_Gyro[2]);

	m_str_HMC5883L_X.Format(_T("%d"), m_State.s16_HMC5883L[0]);
	m_str_HMC5883L_Y.Format(_T("%d"), m_State.s16_HMC5883L[1]);
	m_str_HMC5883L_Z.Format(_T("%d"), m_State.s16_HMC5883L[2]);
	m_str_HMC5883L_Angle.Format(_T("%.1f"), m_State.f_HMC5883L_Angle);

	m_str_Roll.Format(_T("%.3f"), m_State.f_Roll);
	m_str_Pitch.Format(_T("%.3f"), m_State.f_Pitch);
	m_str_Yaw.Format(_T("%.3f"), m_State.f_Yaw);
	
	if (m_State.u16_Motor1)
	{
		m_str_Motor1.Format(_T("%d"), (int)(((float)m_State.u16_Motor1 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor1 = _T("无信号");
	}
	if (m_State.u16_Motor2)
	{
		m_str_Motor2.Format(_T("%d"), (int)(((float)m_State.u16_Motor2 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor2 = _T("无信号");
	}
	if (m_State.u16_Motor3)
	{
		m_str_Motor3.Format(_T("%d"), (int)(((float)m_State.u16_Motor3 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor3 = _T("无信号");
	}
	if (m_State.u16_Motor4)
	{
		m_str_Motor4.Format(_T("%d"), (int)(((float)m_State.u16_Motor4 - 1100.0f) / 8.0f + 0.5f));
	}
	else
	{
		m_str_Motor4 = _T("无信号");
	}
	
	m_str_Gyro_X.Format(_T("%.3f"), (float)m_State.s16_Gyro[0] / 16.4f);
	m_str_Gyro_Y.Format(_T("%.3f"), (float)m_State.s16_Gyro[1] / 16.4f);
	m_str_Gyro_Z.Format(_T("%.3f"), (float)m_State.s16_Gyro[2] / 16.4f);

	m_str_Accel_X.Format(_T("%.3f"), (float)m_State.s16_Accel[0] / 16384.0f);
	m_str_Accel_Y.Format(_T("%.3f"), (float)m_State.s16_Accel[1] / 16384.0f);
	m_str_Accel_Z.Format(_T("%.3f"), (float)m_State.s16_Accel[2] / 16384.0f);

	m_str_HIGH_KS10X.Format(_T("%.3f"), (float)m_State.u16_KS10X_High / 1000.0f);
	
	m_str_Temp_MPU6050.Format(_T("%.2f"), m_State.f_MPU6050_Temp);
	m_str_HIGH_MS5611.Format(_T("%.3f"), m_State.f_MS5611_HIGH / 100.0f);
	m_str_Temp_MS5611.Format(_T("%.2f"), m_State.f_MS5611_Temp / 100.0f);
	m_str_Press_MS5611.Format(_T("%.2f"), m_State.f_MS5611_Press / 1000.0f);

	m_str_HIGH_Accel.Format(_T("%.3f"), m_State.f_High_Accel);

	m_str_Roll_G_Kp.Format(_T("%.2f"), (float)m_State.u16_ROLL_G_Kp / 100.0f);
	m_str_Roll_G_Ki.Format(_T("%.2f"), (float)m_State.u16_ROLL_G_Ki / 100.0f);
	m_str_Roll_G_Kd.Format(_T("%.2f"), (float)m_State.u16_ROLL_G_Kd / 100.0f);
	m_str_Roll_A_Kp.Format(_T("%.2f"), (float)m_State.u16_ROLL_Angle_Kp / 100.0f);
	m_str_Roll_A_Ki.Format(_T("%.2f"), (float)m_State.u16_ROLL_Angle_Ki / 100.0f);
	m_str_Roll_A_Kd.Format(_T("%.2f"), (float)m_State.u16_ROLL_Angle_Kd / 100.0f);
	m_str_Pitch_G_Kp.Format(_T("%.2f"), (float)m_State.u16_PITCH_G_Kp / 100.0f);
	m_str_Pitch_G_Ki.Format(_T("%.2f"), (float)m_State.u16_PITCH_G_Ki / 100.0f);
	m_str_Pitch_G_Kd.Format(_T("%.2f"), (float)m_State.u16_PITCH_G_Kd / 100.0f);
	m_str_Pitch_A_Kp.Format(_T("%.2f"), (float)m_State.u16_PITCH_Angle_Kp / 100.0f);
	m_str_Pitch_A_Ki.Format(_T("%.2f"), (float)m_State.u16_PITCH_Angle_Ki / 100.0f);
	m_str_Pitch_A_Kd.Format(_T("%.2f"), (float)m_State.u16_PITCH_Angle_Kd / 100.0f);
	m_str_Yaw_G_Kp.Format(_T("%.2f"), (float)m_State.u16_YAW_G_Kp / 100.0f);
	m_str_Yaw_G_Ki.Format(_T("%.2f"), (float)m_State.u16_YAW_G_Ki / 100.0f);
	m_str_Yaw_G_Kd.Format(_T("%.2f"), (float)m_State.u16_YAW_G_Kd / 100.0f);
	UpdateData(FALSE);
}

void CMFC_Exia_DebuggerDlg::InitQuadrotorState()
{
	memset(&m_State, 0, sizeof(m_State));
	m_Color_BuffByte = RGB(0, 0, 0);
	m_Color_SerialInQueByte = RGB(0, 0, 0);
	GetDlgItem(IDC_BUFF_BYTE)->SetWindowText(_T("0 Bytes"));
	GetDlgItem(IDC_SERIAL_INQUE_BYTE)->SetWindowText(_T("0 Bytes"));
	int nID;
	for (nID = IDC_STATIC_THRO; nID <= IDC_STATIC_YAW_G_Kd; nID++)
	{
		GetDlgItem(nID)->SetWindowText(_T("无信号"));
	}
	//直接设置了控件文字后，要把文件更新回变量中
	UpdateData(TRUE);
}

void CMFC_Exia_DebuggerDlg::OnBnClickedButtonOpenCurve()
{
	// TODO:  在此添加控件通知处理程序代码
	if (m_pCurveDLG)
	{
		m_pCurveDLG->ShowWindow(SW_SHOW);
	}
	else
	{
		MessageBox(_T("无法打开曲线监控窗口"), _T("打开失败"), MB_ICONERROR | MB_OK);
	}
}


//设置图像增强
void CMFC_Exia_DebuggerDlg::OnBnClickedCurveEnhance()
{
	// TODO:  在此添加控件通知处理程序代码
	if (((CButton*)GetDlgItem(IDC_CURVE_ENHANCE))->GetCheck())
	{
		if (!m_pCurveDLG->CurveEnhance(TRUE))
		{
			((CButton*)GetDlgItem(IDC_CURVE_ENHANCE))->SetCheck(FALSE);
			MessageBox(_T("无法打开画质增强"), _T("打开失败"), MB_ICONERROR | MB_OK);
		}
	}
	else
	{
		m_pCurveDLG->CurveEnhance(FALSE);
	}
}




//曲线截图
void CMFC_Exia_DebuggerDlg::OnBnClickedButtonScreenshot()
{
	// TODO:  在此添加控件通知处理程序代码
	CTime CurTime; 
	CurTime = CTime::GetCurrentTime();	//获取系统时间
	CString stFilePath_save = CurTime.Format(_T("ScreenShot %Y_%m_%d %H.%M.%S.bmp"));

	// 设置过滤器，过滤出Bmp文件
	CString szOpenFilter = _T("位图文件 (*.bmp;*.dib)|*.bmp;*.dib|PNG (*.png)|*.png|JPEG (*.jpg;*.jpeg;*.jpe)|*.jpg;*.jpeg;*.jpe|GIF (*.gif)|*.gif||");
	// 构造保存文件对话框 
	CFileDialog SaveDlgDst(FALSE, _T(".bmp"), stFilePath_save, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szOpenFilter);

	if (SaveDlgDst.DoModal() == IDOK)
	{
		stFilePath_save = SaveDlgDst.GetPathName();

		if (!m_pCurveDLG->m_Curve.ScreenShot(stFilePath_save))
		{
			AfxMessageBox(_T("文件保存失败,请检查文件名及后缀名"), MB_ICONINFORMATION);
		}

	}

}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonClearCurve()
{
	// TODO:  在此添加控件通知处理程序代码
	m_pCurveDLG->m_Curve.ClearData();
}


void CMFC_Exia_DebuggerDlg::OnCbnSelchangeCurveRefresh()
{
	// TODO:  在此添加控件通知处理程序代码
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
		default:
		{
			RefreshTime = 20;	//1000ms/50Hz = 20ms
			break;
		}
	}
	if (RefreshTime != m_nRefreshTime)
	{
		m_nRefreshTime = RefreshTime;
		//开启曲线更新定时器
		if (m_Timer_Update_Curve)	//已经开启则重新设置，没打开就不用打开，串口开启再开
		{
			KillTimer(m_Timer_Update_Curve);
			m_Timer_Update_Curve = 0;
			m_Timer_Update_Curve = SetTimer(ID_TIMER_UPDATE_CURVE, m_nRefreshTime, NULL);
			if (m_Timer_Update_Curve == 0)
			{
				//AfxMessageBox("数据更新定时器设置失败");
				MessageBox(GetLastErrorMessage(), _T("曲线更新定时器设置失败"), MB_ICONERROR | MB_OK);
			}
		}

	}
}





void CMFC_Exia_DebuggerDlg::OnOK()
{
	// TODO:  在此添加专用代码和/或调用基类
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


//设置鼠标移动到上方的光标
BOOL CMFC_Exia_DebuggerDlg::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int DlgCtrlID = pWnd->GetDlgCtrlID();
	if (DlgCtrlID >= IDC_STATIC_THRO && DlgCtrlID <= IDC_STATIC_AILE || DlgCtrlID >= IDC_STATIC_ROLL_G_Kp && DlgCtrlID <= IDC_STATIC_YAW_G_Kd)
	{
		SetCursor(LoadCursor(NULL, IDC_HAND));
		return TRUE;
	}
	return CDialogEx::OnSetCursor(pWnd, nHitTest, message);
}

//发送指令
void CMFC_Exia_DebuggerDlg::SendQuadrotorCommand(uint8_t command, uint8_t* pData, DWORD dataSize)
{
	uint8_t* pSendBuf = NULL;
	DWORD BufSize = sizeof(command) + dataSize;
	pSendBuf = new uint8_t[BufSize];
	if (!pSendBuf)
	{
		MessageBox(_T("内存分配失败"), _T("指令发送失败"), MB_ICONERROR | MB_OK);
		return;
	}
	pSendBuf[0] = command;
	memcpy(pSendBuf + sizeof(command), pData, dataSize);
	for (DWORD i = 0; i < BufSize; i++)
	{
		if (m_Serial.SendData(pSendBuf + i, 1) != 1)
		{
			MessageBox(_T("数据发送错误"), _T("指令发送失败"), MB_ICONERROR | MB_OK);
			break;
		}
		Sleep(1);
	}
	
	delete[] pSendBuf;
}


//输入、发送指令统一响应
void CMFC_Exia_DebuggerDlg::OnStnClickedColtrol(UINT uID)
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	CString ControlName;
	CString Min, Max;
	uint8_t command = COMMAND_SET_THRO + uID - IDC_STATIC_THRO;
	ControlName.LoadString(IDS_STRING_THRO + uID - IDC_STATIC_THRO);
	int offset = -50;
	if (uID == IDC_STATIC_THRO)
	{
		offset = 0;
	}
	Min.Format(_T("%d"), offset);
	Max.Format(_T("%d"), 100 + offset);
	CInputBox InputBox(ControlName + _T("设定"), _T("请输入") + ControlName + _T("设定值:"), _T("0"), Min, Max);
	if (IDOK == InputBox.DoModal())
	{
		int nInput = 0;
		uint8_t SendData = 0;
		nInput = _ttoi(InputBox.GetInput()) - offset;
		SendData = (uint8_t)nInput;
		SendQuadrotorCommand(command, &(SendData), sizeof(SendData));
	}
}


void CMFC_Exia_DebuggerDlg::OnStnClickedSetPID(UINT uID)
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	if (m_State.u16_Thro != 0 && m_State.u16_Thro != 1100)
	{
		MessageBox(_T("请将油门置零！"), _T("危险！"), MB_ICONWARNING | MB_OK);
		return;
	}
	CString ControlName;
	uint8_t command = COMMAND_ROLL_GYRO_KP + uID - IDC_STATIC_ROLL_G_Kp;
	ControlName.LoadString(IDS_STRING_ROLL_G_Kp + uID - IDC_STATIC_ROLL_G_Kp);
	CInputBox InputBox(_T("PID参数整定"), _T("请输入") + ControlName + _T("设定值:"), _T("0"), _T("0"), _T("10"));
	if (IDOK == InputBox.DoModal())
	{
		uint16_t SendData = 0;
		SendData = (uint16_t)(_ttof(InputBox.GetInput()) * 100);
		SendQuadrotorCommand(command, (uint8_t *)&(SendData), sizeof(SendData));
	}
}

void CMFC_Exia_DebuggerDlg::OnBnClickedButtonHorizon()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	SendQuadrotorCommand(COMMAND_HORIZON, NULL, 0);
}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonStop()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	SendQuadrotorCommand(COMMAND_STOP, NULL, 0);
}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonSelftest()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	if (MessageBox(_T("请将机体水平放置\r机体将把当前状态设置为水平状态"), _T("提示"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
	{
		SendQuadrotorCommand(COMMAND_MPU6050_SETOFFSET, NULL, 0);
	}
	if (MessageBox(_T("请将机体放置在地面\r机体将把当前高度设置为参考高度"), _T("提示"), MB_ICONWARNING | MB_OKCANCEL) == IDOK)
	{
		SendQuadrotorCommand(COMMAND_SET_HIGH_REF, NULL, 0);
	}
}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonPidSave()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}

	CFile SaveFile;
	CFileException Exception;
	CTime CurTime;
	CurTime = CTime::GetCurrentTime();	//获取系统时间
	CString stFilePath_save = CurTime.Format(_T("PID_Config %Y_%m_%d %H.%M.%S.txt"));

	// 设置过滤器，过滤出Txt文件
	CString szOpenFilter = _T("文本文件 (*.txt)|*.txt||");
	// 构造保存文件对话框 
	CFileDialog SaveDlgDst(FALSE, _T(".txt"), stFilePath_save, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szOpenFilter);

	if (SaveDlgDst.DoModal() == IDOK)
	{
		stFilePath_save = SaveDlgDst.GetPathName();
		if (!SaveFile.Open(stFilePath_save, CFile::modeCreate | CFile::modeWrite, &Exception))
		{
			MessageBox(GetErrorMessage(Exception.m_cause), _T("配置保存失败"), MB_ICONERROR | MB_OK);
			return;
		}

		char FileBuf[] = "          Kernel_Kp Kernel_Ki Kernel_Kd Shell_Kp  Shell_Ki  Shell_Kd  \r\n";
		SaveFile.Write(FileBuf, strlen(FileBuf));
		sprintf(FileBuf, "%-10s%-10.2f%-10.2f%-10.2f%-10.2f%-10.2f%-10.2f\r\n", "Roll", (float)m_State.u16_ROLL_G_Kp / 100.0f, (float)m_State.u16_ROLL_G_Ki / 100.0f, (float)m_State.u16_ROLL_G_Kd / 100.0f, (float)m_State.u16_ROLL_Angle_Kp / 100.0f, (float)m_State.u16_ROLL_Angle_Ki / 100.0f, (float)m_State.u16_ROLL_Angle_Kd / 100.0f);
		SaveFile.Write(FileBuf, strlen(FileBuf));
		sprintf(FileBuf, "%-10s%-10.2f%-10.2f%-10.2f%-10.2f%-10.2f%-10.2f\r\n", "Pitch", (float)m_State.u16_PITCH_G_Kp / 100.0f, (float)m_State.u16_PITCH_G_Ki / 100.0f, (float)m_State.u16_PITCH_G_Kd / 100.0f, (float)m_State.u16_PITCH_Angle_Kp / 100.0f, (float)m_State.u16_PITCH_Angle_Ki / 100.0f, (float)m_State.u16_PITCH_Angle_Kd / 100.0f);
		SaveFile.Write(FileBuf, strlen(FileBuf));
		sprintf(FileBuf, "%-10s%-10.2f%-10.2f%-10.2f\r\n", "Yaw", (float)m_State.u16_YAW_G_Kp / 100.0f, (float)m_State.u16_YAW_G_Ki / 100.0f, (float)m_State.u16_YAW_G_Kd / 100.0f);
		SaveFile.Write(FileBuf, strlen(FileBuf));
		SaveFile.Close();
	}
}


int FindStr(char *DataBuf,unsigned int DataLength, const char *Str)
{
	int Index = 0;
	while (1)
	{
		if (Index + strlen(Str) > DataLength)
		{
			break;
		}
		if (!strncmp(DataBuf + Index, Str, strlen(Str)))
		{
			return Index;
		}
		Index++;
	}
	return -1;
}


void CMFC_Exia_DebuggerDlg::OnBnClickedButtonPidLoad()
{
	// TODO:  在此添加控件通知处理程序代码
	if (!m_Serial.IsOpen())
	{
		MessageBox(_T("未连接飞行器"), _T("无法发送指令"), MB_ICONERROR | MB_OK);
		return;
	}
	if (m_State.u16_Thro != 0 && m_State.u16_Thro != 1100)
	{
		MessageBox(_T("请将油门置零！"), _T("危险！"), MB_ICONWARNING | MB_OK);
		return;
	}
	CFile LoadFile;
	CFileException Exception;
	CString stFilePath_Load;

	char *pFileBuf = NULL;
	unsigned int FileLength = 0;
	int ReadIndex = 0;

	float PID_Parm[15] = {0};

	int ResultRoll = 0;
	int ResultPitch = 0;
	int ResultYaw = 0;
	int Sel = 0;

	CString TipStr;
	CString TipRollStr;
	CString TipPitchStr;
	CString TipYawStr;

	CString szOpenFilter = _T("文本文件 (*.txt)|*.txt||");
	// 构造打开文件对话框 
	CFileDialog OpenFileDlg(TRUE, NULL, NULL, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szOpenFilter);
	if (OpenFileDlg.DoModal() != IDOK)
	{
		return;
	}

	stFilePath_Load = OpenFileDlg.GetPathName();

	if (!LoadFile.Open(stFilePath_Load, CFile::modeRead, &Exception))
	{
		MessageBox(GetErrorMessage(Exception.m_cause), _T("配置载入失败"), MB_ICONERROR | MB_OK);
		return;
	}
	
	if (LoadFile.GetLength() > CONFIG_FILE_MAX_SIZE)
	{
		MessageBox(_T("配置文件过大"), _T("配置载入失败"), MB_ICONERROR | MB_OK);
		LoadFile.Close();
		return;
	}
	FileLength = (unsigned int)LoadFile.GetLength();

	pFileBuf = new char[FileLength];
	if (!pFileBuf)
	{
		MessageBox(_T("内存分配失败"), _T("配置载入失败"), MB_ICONERROR | MB_OK);
		LoadFile.Close();
		return;
	}

	if (LoadFile.Read(pFileBuf, FileLength) != FileLength)
	{
		MessageBox(_T("文件读取错误"), _T("配置载入失败"), MB_ICONERROR | MB_OK);
		LoadFile.Close();
		delete[] pFileBuf;
		return;
	}


	ReadIndex = FindStr(pFileBuf, FileLength, "Roll");
	if (ReadIndex >= 0)
	{
		ResultRoll = sscanf(pFileBuf + ReadIndex + strlen("Roll"), "%f%f%f%f%f%f", &PID_Parm[0], &PID_Parm[1], &PID_Parm[2], &PID_Parm[3], &PID_Parm[4], &PID_Parm[5]);
	}

	ReadIndex = FindStr(pFileBuf, FileLength, "Pitch");
	if (ReadIndex >= 0)
	{
		ResultPitch = sscanf(pFileBuf + ReadIndex + strlen("Pitch"), "%f%f%f%f%f%f", &PID_Parm[6], &PID_Parm[7], &PID_Parm[8], &PID_Parm[9], &PID_Parm[10], &PID_Parm[11]);
	}

	ReadIndex = FindStr(pFileBuf, FileLength, "Yaw");
	if (ReadIndex >= 0)
	{
		ResultYaw = sscanf(pFileBuf + ReadIndex + strlen("Yaw"), "%f%f%f", &PID_Parm[12], &PID_Parm[13], &PID_Parm[14]);
	}

	LoadFile.Close();
	delete[] pFileBuf;

	TipRollStr.Format(_T("%-10s %-10.2f   %-10.2f   %-10.2f   %-10.2f  %-10.2f  %-10.2f\r\n"), _T("Roll"), PID_Parm[0], PID_Parm[1], PID_Parm[2], PID_Parm[3], PID_Parm[4], PID_Parm[5]);
	TipPitchStr.Format(_T("%-10s %-10.2f   %-10.2f   %-10.2f   %-10.2f  %-10.2f  %-10.2f\r\n"), _T("Pitch"), PID_Parm[6], PID_Parm[7], PID_Parm[8], PID_Parm[9], PID_Parm[10], PID_Parm[11]);
	TipYawStr.Format(_T("%-10s%-10.2f   %-10.2f   %-10.2f\r\n"), _T("Yaw"), PID_Parm[12], PID_Parm[13], PID_Parm[14]);

	if (ResultRoll < 6 || ResultPitch < 6 || ResultYaw < 3)
	{
		TipStr.Format(_T("      配置载入错误，是否仍要将以下参数写入机体？\n\n          Kernel_Kp Kernel_Ki Kernel_Kd Shell_Kp  Shell_Ki  Shell_Kd  \r\n"));
		TipStr += TipRollStr + TipPitchStr + TipYawStr;
		Sel = MessageBox(TipStr, _T("配置载入错误"), MB_ICONERROR | MB_OK | MB_OKCANCEL);
	}
	else
	{
		TipStr.Format(_T("      是否将以下参数写入机体？\n\n          Kernel_Kp Kernel_Ki Kernel_Kd Shell_Kp  Shell_Ki  Shell_Kd  \r\n"));
		TipStr += TipRollStr + TipPitchStr + TipYawStr;
		Sel = MessageBox(TipStr, _T("参数确认"), MB_ICONWARNING | MB_OKCANCEL);
	}
	
	if (IDOK == Sel)
	{
		uint8_t command = COMMAND_ROLL_GYRO_KP;
		uint16_t SendData = 0;
		for (int i = 0; i < 15; i++, command++)
		{
			SendData = (uint16_t)(PID_Parm[i] * 100);
			SendQuadrotorCommand(command, (uint8_t *)&(SendData), sizeof(SendData));
			Sleep(1);
		}
	}
}
