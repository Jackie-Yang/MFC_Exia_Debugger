// CurveDLG.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "CurveDLG.h"
#include "afxdialogex.h"


// CurveDLG �Ի���

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


// CurveDLG ��Ϣ�������


BOOL CurveDLG::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	//��ȡ���ڵĳ�ʼ��С
	CRect rect;
	GetWindowRect(&rect);
	//����ʼֵ��Ϊ��Сֵ
	m_ptMinTrackSize.x = (LONG)rect.Width();
	m_ptMinTrackSize.y = (LONG)rect.Height();

	GetClientRect(&m_CurClientRect);	//��ǰ�ͻ�����С

	m_Curve.Init(1000, 1000, 10000, 0.8f, 0.8f, 10, 10);

	return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}


void CurveDLG::OnSize(UINT nType, int cx, int cy)
{
	CDialogEx::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	//��һ�δ���ʱ����OnSize������ʱ���ڻ�û�����ɣ����ܵ���
	if (m_Curve && nType != SIZE_MINIMIZED)	//��С��ʱ����Ҫ�ı�ؼ���С
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

//����������С��С
void CurveDLG::OnGetMinMaxInfo(MINMAXINFO* lpMMI)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	lpMMI->ptMinTrackSize = m_ptMinTrackSize;
	CDialogEx::OnGetMinMaxInfo(lpMMI);
}

//���ڴ���С����ԭʱ��������(��С��ʱ����ͼ)
BOOL CurveDLG::OnQueryOpen()
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	m_Curve.Update();
	return CDialogEx::OnQueryOpen();
}


void CurveDLG::UpdateCurve()
{
	if (IsWindowVisible() && !IsIconic())	//����С��ʱ�����л�ͼ
	{
		m_Curve.Update();
	}
}

void CurveDLG::PostNcDestroy()
{
	// TODO:  �ڴ����ר�ô����/����û���
	CDialogEx::PostNcDestroy();
	delete this;
}
