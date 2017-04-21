// Curve.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "Curve.h"
#include "Color.h"

// CCurve

IMPLEMENT_DYNAMIC(CCurve, CWnd)

CCurve::CCurve()
{ 
	RegisterWindowClass();
	m_Brush_BG.CreateSolidBrush(BLACK);
	m_Pen_Grid.CreatePen(PS_DOT, 1, TEAL);
	m_Pen_Curve.CreatePen(PS_SOLID, 1, RED);

	m_RectGrid.SetRect(0,0,0,0);
	m_GridStepX = 0;
	m_GridStepY = 0;

	m_nOffset = 0;
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nBufSize = 0;
	m_pDataBuf = NULL;
}

CCurve::~CCurve()
{
	if (m_pDataBuf)
	{
		delete[]m_pDataBuf;
		m_pDataBuf = NULL;
		m_nBufSize = 0;
	}
}


BEGIN_MESSAGE_MAP(CCurve, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
END_MESSAGE_MAP()



// CCurve ��Ϣ�������


bool CCurve::RegisterWindowClass()
{
	HINSTANCE hInstance = AfxGetInstanceHandle();
	WNDCLASS windowclass;


	if (!(::GetClassInfo(hInstance, _T("Curve"), &windowclass)))
	{
		memset(&windowclass, 0, sizeof(WNDCLASS));
		windowclass.style = CS_DBLCLKS;
		windowclass.lpfnWndProc = ::DefWindowProc;
		windowclass.cbClsExtra = windowclass.cbWndExtra = 0;
		windowclass.hInstance = hInstance;
		windowclass.hIcon = NULL;
		windowclass.hCursor = AfxGetApp()->LoadStandardCursor(IDC_ARROW);
		windowclass.hbrBackground = (HBRUSH) ::GetStockObject(NULL);
		windowclass.lpszMenuName = NULL;
		windowclass.lpszClassName = _T("Curve");

		if (!AfxRegisterClass(&windowclass))
		{
			AfxThrowResourceException();
			return FALSE;
		}
	}

	return TRUE;
}

//Ҫresize�Ļ����԰Ѳ�����Ϊ��Ա����������
void CCurve::Init(float fScaleX, float fScaleY, int nGridX, int nGridY)
{
	CRect rect;
	GetClientRect(&rect);	//��ȡ�ؼ�����

	int nPaddingX, nPaddingY;
	nPaddingX = (int)(rect.Width() * (1 - fScaleX) * 0.5f);
	nPaddingY = (int)(rect.Height() * (1 - fScaleY) * 0.5f);

	m_RectGrid.top = nPaddingY;
	m_RectGrid.right = rect.Width() - nPaddingX;

	m_RectGrid.left = nPaddingX;
	m_RectGrid.bottom = rect.Height() - nPaddingY;

	m_GridStepX = m_RectGrid.Width() / (float)nGridX;
	m_GridStepY = m_RectGrid.Height() / (float)nGridY;

	if (m_pDataBuf)
	{
		delete[]m_pDataBuf;
		m_pDataBuf = NULL;
	}

	m_nBufSize = m_RectGrid.Width();
	m_pDataBuf = new float[m_nBufSize];
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nOffset = 0;
}



void CCurve::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()
	CRect rect;
	GetClientRect(rect);

	

	//ʹ��˫�����ͼ���Ȼ����ڴ棬�������ϲ�ʹ��OnEraseBkgnd�����������бȽ������Ļ�ͼЧ��
	CDC   MemDC;   //���ȶ���һ����ʾ�豸����   
	CBitmap   MemBitmap;//����һ��λͼ����   
	//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸   
	MemDC.CreateCompatibleDC(NULL);
	//��ʱ�����ܻ�ͼ����Ϊû�еط���  
	//����һ������Ļ��ʾ���ݵ�λͼ
	MemBitmap.CreateCompatibleBitmap(&dc, rect.Width(), rect.Height());

	//��λͼѡ�뵽�ڴ���ʾ�豸��   
	//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��   
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

	//��ͼ   
	DrawAll(&MemDC, rect);

	//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ   
	dc.BitBlt(0, 0, rect.Width(), rect.Height(), &MemDC, 0, 0, SRCCOPY);
	//��ͼ��ɺ������   
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();


}




void CCurve::DrawAll(CDC * pDC, CRect rect)
{
	int nOldMode = pDC->SetBkMode(TRANSPARENT);	//����ɫ͸��
	//���ȼ��߷ź�ߣ��Ḳ��ǰ���
	DrawBG(pDC, rect);
	DrawGrid(pDC, rect);
	DrawCurve(pDC,rect);
	DrawAxis(pDC, rect);


	pDC->SetBkMode(nOldMode);
}


void CCurve::DrawBG(CDC *pDC, CRect rect)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_Brush_BG);	//ѡ�л��ʻ���,��������ǰ�Ļ���
	pDC->Rectangle(0, rect.Height(), rect.Width(), 0);		//�ڿؼ�����һ�����ο�
	//pDC->SetBkMode(TRANSPARENT);	//���ֱ���ɫ͸��

	pDC->SelectObject(pOldBrush);		//�ָ���ǰ�Ļ���
}

void CCurve::DrawAxis(CDC *pDC, CRect rect)
{

}

void CCurve::DrawGrid(CDC *pDC, CRect rect)
{
	float nOffset = m_GridStepX - (m_nOffset - (int)(m_nOffset / m_GridStepX) * m_GridStepX);

	CPen *pOldPen = pDC->SelectObject(&m_Pen_Grid);	//ѡ�л��ʻ���,��������ǰ�Ļ���

	for (float X = (float)m_RectGrid.left + nOffset; (int)X <= m_RectGrid.right; X += m_GridStepX)
	{
		pDC->MoveTo((int)X, m_RectGrid.top);
		pDC->LineTo((int)X, m_RectGrid.bottom);
	}

	for (float Y = (float)m_RectGrid.top; (int)Y < m_RectGrid.bottom; Y += m_GridStepY)
	{
		pDC->MoveTo(m_RectGrid.left, (int)Y);
		pDC->LineTo(m_RectGrid.right, (int)Y);
	}


	pDC->SelectObject(&pOldPen);
}

void CCurve::DrawCurve(CDC *pDC, CRect rect)
{
	POINT DataPoint[1024] = { { 0 } };
	unsigned int i, nIndex;
	unsigned int nDataToDraw;
	int nZeroY = m_RectGrid.bottom - m_RectGrid.Height() / 2;
	CPen *pOldPen = pDC->SelectObject(&m_Pen_Curve);	//ѡ�л��ʻ���,��������ǰ�Ļ���

	if (m_nDataCount > m_nBufSize)
	{
		nIndex = m_nDataIndex;
	}
	else
	{
		nIndex = 0;
	}

	Graphics graphics(pDC->m_hDC);
	graphics.SetSmoothingMode(SmoothingModeAntiAlias);
	Pen newPen(Color::Red, 1);


	for (i = 0; i < m_nBufSize; i++)
	{
		if (m_nDataCount <= i)
		{
			break;
		}
		if (nIndex >= m_nBufSize)
		{
			nIndex = 0;
		}
		DataPoint[i].x = m_RectGrid.left + i;
		DataPoint[i].y = nZeroY - (int)*(m_pDataBuf + nIndex++);
	}

	nDataToDraw = i;

	for (i = 1; i < nDataToDraw; i++)
	{
		graphics.DrawLine(&newPen, DataPoint[i - 1].x, DataPoint[i - 1].y, DataPoint[i].x, DataPoint[i].y);
	}



	//pDC->MoveTo(m_RectGrid.left, nZeroY - (int)*(m_pDataBuf + nIndex++));
	//for (unsigned int i = 1; i < m_nBufSize; i++)
	//{
	//	if (nIndex >= m_nBufSize)
	//	{
	//		nIndex = 0;
	//	}
	//	if (m_nDataCount <= i)
	//	{
	//		break;
	//	}
	//	pDC->LineTo(m_RectGrid.left + i, nZeroY - (int)*(m_pDataBuf + nIndex++));
	//}

	pDC->SelectObject(&pOldPen);
}



//����ˢ����������ֱ�ӷ���TRUE
BOOL CCurve::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}



void CCurve::AddData(float fData)
{
	if (!m_pDataBuf)
	{
		return;
	}
	*(m_pDataBuf + m_nDataIndex++) = fData;
	if (m_nDataIndex >= m_nBufSize)
	{
		m_nDataIndex = 0;
	}
	if (++m_nDataCount >= m_nBufSize)
	{
		m_nOffset = m_nDataCount % m_nBufSize;
	}
	Invalidate();
	UpdateWindow();
	if (m_nOffset == 0)
	{
		fData = 1;
	}
}