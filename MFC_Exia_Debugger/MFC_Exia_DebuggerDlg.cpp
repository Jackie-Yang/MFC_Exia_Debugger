
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
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_ICON_RED);
	m_Color_Status = RGB(255, 0, 0);
}

void CMFC_Exia_DebuggerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_COMBO_COM, m_Combox_COM);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_Static_Status);
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
		m_TimerID_Update_Data = SetTimer(ID_TIMER_UPDATE_DATA, 500, NULL);
		if (m_TimerID_Update_Data == 0)
		{
			AfxMessageBox("数据更新定时器设置失败");
		}
	}
	else
	{
		m_Static_Status.SetWindowTextA("未连接");
		m_Color_Status = RGB(255, 0, 0);
		m_Static_Status.InvalidateRect(NULL);
		GetDlgItem(IDC_BUTTON_OPEN_CLOSE)->SetWindowTextA("打开串口");
		if (m_TimerID_Update_Data)
		{
			KillTimer(m_TimerID_Update_Data);
			m_TimerID_Update_Data = 0;
		}
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
			AfxMessageBox(GetLastErrorMessage("串口关闭失败："));
		}
	}
	else
	{
		if (!m_Serial.OpenSerial(m_Serial.GetSerialInfo(m_Combox_COM.GetCurSel()), CBR_115200))
		{
			AfxMessageBox(GetLastErrorMessage("串口打开失败："));
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

	// TODO:  如果默认的不是所需画笔，则返回另一个画笔
	return hbr;
}



void CMFC_Exia_DebuggerDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	if (m_TimerID_Update_Data && nIDEvent == m_TimerID_Update_Data)
	{

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