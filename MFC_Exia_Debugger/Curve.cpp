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
	m_Pen_Axis.CreatePen(PS_SOLID, 1, MAGENTA);

	m_RectCurve.SetRect(0,0,0,0);
	m_RectBG.SetRect(0, 0, 0, 0);
	m_GridStepX = 0;
	m_GridStepY = 0;
	m_AxisStepX = 0;
	m_AxisStepY = 0;
	m_PointStepX = 0;

	m_nOffset = 0;
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nBufSize = 0;
	m_pDataBuf = NULL;
	m_bAntialias = FALSE;
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
void CCurve::Init(unsigned int nBufSize, float fScaleX, float fScaleY, int nGridX, int nGridY)
{
	GetClientRect(&m_RectBG);	//��ȡ�ؼ�����

	int nPaddingX, nPaddingY;
	nPaddingX = (int)(m_RectBG.Width() * (1 - fScaleX) * 0.5f);
	nPaddingY = (int)(m_RectBG.Height() * (1 - fScaleY) * 0.5f);

	m_RectCurve.top = nPaddingY;
	m_RectCurve.right = m_RectBG.Width() - nPaddingX;

	m_RectCurve.left = nPaddingX;
	m_RectCurve.bottom = m_RectBG.Height() - nPaddingY;

	m_GridStepX = m_RectCurve.Width() / (float)nGridX;
	m_GridStepY = m_RectCurve.Height() / (float)nGridY;

	m_AxisStepX = m_GridStepX / 5.0f;
	m_AxisStepY = m_GridStepY / 5.0f;

	if (m_pDataBuf)
	{
		delete[]m_pDataBuf;
		m_pDataBuf = NULL;
	}

	//m_nBufSize = m_RectCurve.Width();
	m_nBufSize = nBufSize;
	m_PointStepX = (float)m_RectCurve.Width() / (float)m_nBufSize;
	m_pDataBuf = new float[m_nBufSize];	//���ݻ���Ϊ����������
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nOffset = 0;
}



void CCurve::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO:  �ڴ˴������Ϣ����������
	// ��Ϊ��ͼ��Ϣ���� CWnd::OnPaint()

	//ʹ��˫�����ͼ���Ȼ����ڴ棬�������ϲ�ʹ��OnEraseBkgnd�����������бȽ������Ļ�ͼЧ��
	CDC   MemDC;   //���ȶ���һ����ʾ�豸����   
	CBitmap   MemBitmap;//����һ��λͼ����   
	//���������Ļ��ʾ���ݵ��ڴ���ʾ�豸   
	MemDC.CreateCompatibleDC(NULL);
	//��ʱ�����ܻ�ͼ����Ϊû�еط���  
	//����һ������Ļ��ʾ���ݵ�λͼ
	MemBitmap.CreateCompatibleBitmap(&dc, m_RectBG.Width(), m_RectBG.Height());

	//��λͼѡ�뵽�ڴ���ʾ�豸��   
	//ֻ��ѡ����λͼ���ڴ���ʾ�豸���еط���ͼ������ָ����λͼ��   
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

	//��ͼ   
	DrawAll(&MemDC);

	//���ڴ��е�ͼ��������Ļ�Ͻ�����ʾ   
	dc.BitBlt(0, 0, m_RectBG.Width(), m_RectBG.Height(), &MemDC, 0, 0, SRCCOPY);
	//��ͼ��ɺ������   
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();


}

void CCurve::Update()
{
	Invalidate();
	UpdateWindow();
}



void CCurve::DrawAll(CDC * pDC)
{
	int nOldMode = pDC->SetBkMode(TRANSPARENT);	//����ɫ͸��
	//���ȼ��߷ź�ߣ��Ḳ��ǰ���
	DrawBG(pDC);
	DrawGrid(pDC);
	DrawCurve(pDC);
	DrawAxis(pDC);

	pDC->SetBkMode(nOldMode);
}


void CCurve::DrawBG(CDC *pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_Brush_BG);	//ѡ�л��ʻ���,��������ǰ�Ļ���
	pDC->Rectangle(0, m_RectBG.Height(), m_RectBG.Width(), 0);		//�ڿؼ�����һ�����ο�
	pDC->SelectObject(pOldBrush);		//�ָ���ǰ�Ļ���
}

void CCurve::DrawAxis(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = -(nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//����ƫ��
	CPen *pOldPen = pDC->SelectObject(&m_Pen_Axis);	//ѡ�л��ʻ���,��������ǰ�Ļ���
	int i = 0;

	for (float X = (float)m_RectCurve.left + nOffset; (int)X <= m_RectCurve.right; X += m_AxisStepX, i++)
	{
		if ((int)X > m_RectCurve.left && i % 5)
		{
			pDC->MoveTo((int)X, m_RectCurve.bottom - 4);
			pDC->LineTo((int)X, m_RectCurve.bottom);
		}
	}

	i = 0;
	for (float Y = (float)m_RectCurve.top; (int)Y <= m_RectCurve.bottom; Y += m_AxisStepY, i++)
	{
		if (i % 5)
		{
			pDC->MoveTo(m_RectCurve.left, (int)Y);
			pDC->LineTo(m_RectCurve.left + 4, (int)Y);
		}
	}

	pDC->SelectObject(&pOldPen);
}

void CCurve::DrawGrid(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = m_GridStepX - (nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//����ƫ��
	CString str;

	CPen *pOldPen = pDC->SelectObject(&m_Pen_Grid);	//ѡ�л��ʻ���,��������ǰ�Ļ���

	pDC->MoveTo(m_RectCurve.left, m_RectCurve.top);	//��һ����Ҫ��
	pDC->LineTo(m_RectCurve.left, m_RectCurve.bottom);

	for (float X = (float)m_RectCurve.left + nOffset; (int)X <= m_RectCurve.right; X += m_GridStepX)
	{
		pDC->MoveTo((int)X, m_RectCurve.top);
		pDC->LineTo((int)X, m_RectCurve.bottom);
	}

	for (float Y = (float)m_RectCurve.top; (int)Y <= m_RectCurve.bottom; Y += m_GridStepY)
	{
		pDC->MoveTo(m_RectCurve.left, (int)Y);
		pDC->LineTo(m_RectCurve.right, (int)Y);
	}


	pDC->SelectObject(&pOldPen);
}

void CCurve::DrawCurve(CDC *pDC)
{
	unsigned int i, nIndex;
	int nZeroY = m_RectCurve.bottom - m_RectCurve.Height() / 2;		//�������м�Ϊ0

	if (m_nDataCount > m_nBufSize)
	{
		nIndex = m_nDataIndex;
	}
	else
	{
		nIndex = 0;
	}

	POINT LastPoint;
	POINT NewPoint;

	if (m_bAntialias)	//Gdi+�����
	{
		Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		Pen newPen(Color::Red, 1);
		//��һ����
		LastPoint.x = m_RectCurve.left;
		LastPoint.y = nZeroY - (int)*(m_pDataBuf + nIndex++);
		if (LastPoint.y < m_RectCurve.top)
		{
			LastPoint.y = m_RectCurve.top;
		}
		else if (LastPoint.y > m_RectCurve.bottom)	//Խ��
		{
			LastPoint.y = m_RectCurve.bottom;
		}

		for (i = 1; i < m_nBufSize; i++, nIndex++)
		{
			if (m_nDataCount <= i)
			{
				break;
			}
			if (nIndex >= m_nBufSize)
			{
				nIndex = 0;
			}
			NewPoint.x = (int)(m_RectCurve.left + i * m_PointStepX);	//����x����
			if (NewPoint.x == LastPoint.x)	//���x�������һ��һ�����򲻻�
			{
				continue;
			}
			NewPoint.y = nZeroY - (int)*(m_pDataBuf + nIndex);
			if (NewPoint.y < m_RectCurve.top)
			{
				NewPoint.y = m_RectCurve.top;
			}
			else if (NewPoint.y > m_RectCurve.bottom)	//Խ��
			{
				NewPoint.y = m_RectCurve.bottom;
			}
			graphics.DrawLine(&newPen, LastPoint.x, LastPoint.y, NewPoint.x, NewPoint.y);
			LastPoint = NewPoint;
		}
	}
	else	//��ͨ�����������
	{
		CPen *pOldPen = pDC->SelectObject(&m_Pen_Curve);	//ѡ�л��ʻ���,��������ǰ�Ļ���
		//��һ����
		LastPoint.x = m_RectCurve.left;
		LastPoint.y = nZeroY - (int)*(m_pDataBuf + nIndex++);
		if (LastPoint.y < m_RectCurve.top)
		{
			LastPoint.y = m_RectCurve.top;
		}
		else if (LastPoint.y > m_RectCurve.bottom)	//Խ��
		{
			LastPoint.y = m_RectCurve.bottom;
		}

		pDC->MoveTo(LastPoint.x, LastPoint.y);
		for (unsigned int i = 1; i < m_nBufSize; i++, nIndex++)
		{
			if (nIndex >= m_nBufSize)
			{
				nIndex = 0;
			}
			if (m_nDataCount <= i)
			{
				break;
			}
			NewPoint.x = (int)(m_RectCurve.left + i * m_PointStepX);	//����x����
			if (NewPoint.x == LastPoint.x)	//���x�������һ��һ�����򲻻�
			{
				continue;
			}
			NewPoint.y = nZeroY - (int)*(m_pDataBuf + nIndex);
			if (NewPoint.y < m_RectCurve.top)
			{
				NewPoint.y = m_RectCurve.top;
			}
			else if (NewPoint.y > m_RectCurve.bottom)	//Խ��
			{
				NewPoint.y = m_RectCurve.bottom;
			}
			pDC->LineTo(NewPoint.x, NewPoint.y);
			LastPoint = NewPoint;
		}
		pDC->SelectObject(&pOldPen);
	}
	
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
}