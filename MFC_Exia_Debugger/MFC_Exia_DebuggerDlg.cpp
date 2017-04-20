
// MFC_Exia_DebuggerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "MFC_Exia_DebuggerDlg.h"
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



	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
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
//更新状态
	p_SerialInfo pCurSerial = m_Serial.GetCurSerial();
	if (m_Serial.IsOpen() && pCurSerial)
	{
		CString Info;
		Info.Format("已连接 %s (%s)", pCurSerial->str_Port.c_str(), pCurSerial->str_Name.c_str());
		Info.Replace("\\Device\\", NULL);
		m_Static_Status.SetWindowTextA(Info);
		m_Color_Status = RGB(0, 128, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowTextA("关闭串口");
	}
	else
	{
		m_Static_Status.SetWindowTextA("未连接");
		m_Color_Status = RGB(255, 0, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowTextA("打开串口");
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
			MessageBoxA(GetLastErrorMessage(), "串口关闭失败", MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("串口关闭失败："));
		}
		else
		{
			//关闭数据定时器
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
			MessageBoxA(GetLastErrorMessage(), "串口打开失败", MB_ICONERROR | MB_OK);
			//AfxMessageBox(GetLastErrorMessage("串口打开失败："));
		}
		else
		{
			//开启数据更新定时器
			if (m_Timer_Update_Data)	//已经开启则先关闭
			{
				KillTimer(m_Timer_Update_Data);
				m_Timer_Update_Data = 0;
			}
			m_Timer_Update_Data = SetTimer(ID_TIMER_UPDATE_DATA, 50, NULL);
			if (m_Timer_Update_Data == 0)
			{
				//AfxMessageBox("数据更新定时器设置失败");
				MessageBoxA(GetLastErrorMessage(), "数据更新定时器设置失败", MB_ICONERROR | MB_OK);
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
				MessageBoxA(GetLastErrorMessage(), "数据显示定时器设置失败", MB_ICONERROR | MB_OK);
			}
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
	return 0;
}

//串口关闭成功，UI刷新串口状态
afx_msg LRESULT CMFC_Exia_DebuggerDlg::OnSerialClose(WPARAM wParam, LPARAM lParam)
{
	UpdateSerialState();
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

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}



void CMFC_Exia_DebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
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
		m_str_THRO = "无信号";
	}

	if (m_State.Rudd)
	{
		m_str_RUDD.Format("%u", m_State.Rudd);
	}
	else
	{
		m_str_RUDD = "无信号";
	}

	if (m_State.Elev)
	{
		m_str_ELEV.Format("%u", m_State.Elev);
	}
	else
	{
		m_str_ELEV = "无信号";
	}

	if (m_State.Aile)
	{
		m_str_AILE.Format("%u", m_State.Aile);
	}
	else
	{
		m_str_AILE = "无信号";
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