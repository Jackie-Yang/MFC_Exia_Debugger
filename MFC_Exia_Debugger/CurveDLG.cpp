// CurveDLG.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "CurveDLG.h"
#include "afxdialogex.h"


// CurveDLG 对话框

IMPLEMENT_DYNAMIC(CurveDLG, CDialogEx)

CurveDLG::CurveDLG(CWnd* pParent /*=NULL*/)
	: CDialogEx(CurveDLG::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDI_MONITOR);
}

CurveDLG::~CurveDLG()
{
}

void CurveDLG::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CURVE, m_Curve);
}


BEGIN_MESSAGE_MAP(CurveDLG, CDialogEx)
	ON_WM_SIZE()
	ON_WM_GETMINMAXINFO()
	ON_WM_QUERYOPEN()
END_MESSAGE_MAP()


// CurveDLG 消息处理程序


BOOL CurveDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	//获取窗口的初始大小
	CRect rect;
	GetWindowRect(&rect);
	//将初始值设为最小值
	m_ptMinTrackSize.x = (LONG)rect.Width();
	m_ptMinTrackSize.y = (LONG)rect.Height();

	GetClientRect(&m_CurClientRect);	//当前客户区大小

	m_Curve.Init(1000, 1000, 10000, 0.8f, 0.8f, 10, 10);

	return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}


void CurveDLG::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	//第一次创建时调用OnSize，但这时窗口还没有生成，不能调用
	if (m_Curve && nType != SIZE_MINIMIZED)	//最小化时不需要改变控件大小
	{
		CRect rect;
		GetClientRect(&rect);
		if (m_CurClientRect != rect)
		{
			m_Curve.MoveWindow(rect);
			m_CurClientRect = rect;
		}
	}
	
}

//锁定窗口最小大小
void CurveDLG::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	lpMMI->ptMinTrackSize = m_ptMinTrackSize;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

//窗口从最小化还原时更新曲线(最小化时不绘图)
BOOL CurveDLG::OnQueryOpen()
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	m_Curve.Update();
	return CDialogEx::OnQueryOpen();
}


void CurveDLG::UpdateCurve()
{
	if (IsWindowVisible() && !IsIconic())	//在最小化时不进行绘图
	{
		m_Curve.Update();
	}
}

void CurveDLG::PostNcDestroy()
{
	// TODO:  在此添加专用代码和/或调用基类
	CDialogEx::PostNcDestroy();
	delete this;
}
