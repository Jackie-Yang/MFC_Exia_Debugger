// Curve.cpp : 实现文件
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
	m_nMaxX = 0;
	m_nMaxY = 0;
	m_pDataBuf = NULL;
	m_bAntialias = FALSE;
}

CCurve::~CCurve()
{
	if (m_pDataBuf)
	{
		delete[]m_pDataBuf;
		m_pDataBuf = NULL;
	}
}


BEGIN_MESSAGE_MAP(CCurve, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
END_MESSAGE_MAP()



// CCurve 消息处理程序


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


void CCurve::Init(unsigned int nMaxX, unsigned int nMaxY, unsigned int nDataMAX, float fScaleX, float fScaleY, int nGridX, int nGridY)
{
	nMaxX = nMaxX / 100 * 100;	//大小应该设置为100的整数倍
	nMaxY = nMaxY / 100 * 100;
	nDataMAX = nDataMAX / 100 * 100;

	GetClientRect(&m_RectBG);	//获取控件区域

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

	//m_nBufSize = m_RectCurve.Width();
	m_PointStepX = (float)m_RectCurve.Width() / (float)nMaxX;
	m_PointStepY = (float)m_RectCurve.Height() / (float)nMaxY / 2;

	if (nMaxX != m_nMaxX)	//没有改变nMaxX时不用修改这些数据
	{
		if (m_pDataBuf)
		{
			delete[]m_pDataBuf;
			m_pDataBuf = NULL;
		}
		m_pDataBuf = new float[nMaxX];	//数据缓存
		m_nDataIndex = 0;
		m_nDataCount = 0;
		m_nOffset = 0;
		m_nMaxX = nMaxX;
	}
	
	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;
	m_nGridX = nGridX;
	m_nGridY = nGridY;
	m_nMaxY = nMaxY;
	m_nDataMAX = nDataMAX;
	//把参数作为成员变量存下来.要resize的话可以用
}



void CCurve::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()

	//使用双缓存绘图，先画到内存，这样加上不使用OnEraseBkgnd擦除背景会有比较流畅的绘图效果
	CDC   MemDC;   //首先定义一个显示设备对象   
	CBitmap   MemBitmap;//定义一个位图对象   
	//随后建立与屏幕显示兼容的内存显示设备   
	MemDC.CreateCompatibleDC(NULL);
	//这时还不能绘图，因为没有地方画  
	//建立一个与屏幕显示兼容的位图
	MemBitmap.CreateCompatibleBitmap(&dc, m_RectBG.Width(), m_RectBG.Height());

	//将位图选入到内存显示设备中   
	//只有选入了位图的内存显示设备才有地方绘图，画到指定的位图上   
	CBitmap *pOldBit = MemDC.SelectObject(&MemBitmap);

	//绘图   
	DrawAll(&MemDC);

	//将内存中的图拷贝到屏幕上进行显示   
	dc.BitBlt(0, 0, m_RectBG.Width(), m_RectBG.Height(), &MemDC, 0, 0, SRCCOPY);
	//绘图完成后的清理   
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
	int nOldMode = pDC->SetBkMode(TRANSPARENT);	//背景色透明
	//优先级高放后边，会覆盖前面的
	DrawBG(pDC);
	DrawGrid(pDC);
	DrawCurve(pDC);
	DrawAxis(pDC);

	pDC->SetBkMode(nOldMode);
}


void CCurve::DrawBG(CDC *pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_Brush_BG);	//选中画笔绘制,并保存以前的画笔
	pDC->Rectangle(0, m_RectBG.Height(), m_RectBG.Width(), 0);		//在控件区域画一个矩形框
	pDC->SelectObject(pOldBrush);		//恢复以前的画笔
}

void CCurve::DrawAxis(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = -(nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//计算偏移
	CPen *pOldPen = pDC->SelectObject(&m_Pen_Axis);	//选中画笔绘制,并保存以前的画笔
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
	nOffset = - (nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//计算偏移
	CPen *pOldPen = pDC->SelectObject(&m_Pen_Grid);	//选中画笔绘制网格,并保存以前的画笔

	//绘制文字
	int NumY = m_nMaxY;						//Y轴坐标从最大值开始画
	int NumY_Step = m_nMaxY * 2 / m_nGridY;	//每格代表多少个数据
	int NumX = m_nDataCount - m_nMaxX;		//X轴坐标起始值
	if (NumX < 0)
	{
		NumX = 0;
	}
	int NumX_Step = (m_nMaxX / m_nGridX);	//每格代表多少个数据
	NumX = NumX / NumX_Step * NumX_Step;	//X轴第一个网格线坐标

	COLORREF TextOldColor = pDC->SetTextColor(CYAN);
	UINT OldTextAlign = pDC->SetTextAlign(TA_CENTER);
	LOGFONT logFont;
	CString str;
	pDC->GetCurrentFont()->GetLogFont(&logFont);
	int Y_offset = logFont.lfHeight / 2;
	int X_offset = logFont.lfWidth / 2;

	pDC->MoveTo(m_RectCurve.left, m_RectCurve.top);	//第一条线要画
	pDC->LineTo(m_RectCurve.left, m_RectCurve.bottom);

	if ((int)nOffset == 0)
	{
		str.Format(_T("%d"), NumX);
		pDC->TextOut(m_RectCurve.left, m_RectCurve.bottom + Y_offset, str);
	}
	NumX += NumX_Step;
	
	for (float X = (float)m_RectCurve.left + nOffset + m_GridStepX; (int)X <= m_RectCurve.right; X += m_GridStepX, NumX += NumX_Step)
	{
		str.Format(_T("%d"), NumX);
		pDC->TextOut((int)X, m_RectCurve.bottom + Y_offset, str);
		pDC->MoveTo((int)X, m_RectCurve.top);
		pDC->LineTo((int)X, m_RectCurve.bottom);
	}

	pDC->SetTextAlign(TA_RIGHT);
	for (float Y = (float)m_RectCurve.top; (int)Y <= m_RectCurve.bottom; Y += m_GridStepY, NumY -= NumY_Step)
	{
		str.Format(_T("%d"), NumY);
		pDC->TextOut(m_RectCurve.left - X_offset, (int)Y - Y_offset, str);
		pDC->MoveTo(m_RectCurve.left, (int)Y);
		pDC->LineTo(m_RectCurve.right, (int)Y);
	}


	pDC->SelectObject(&pOldPen);
	pDC->SetTextColor(TextOldColor);
	pDC->SetTextAlign(OldTextAlign);
}

void CCurve::DrawCurve(CDC *pDC)
{
	unsigned int i, nIndex;
	int nZeroY = m_RectCurve.bottom - m_RectCurve.Height() / 2;		//坐标轴中间为0

	if (m_nDataCount > m_nMaxX)
	{
		nIndex = m_nDataIndex;
	}
	else
	{
		nIndex = 0;
	}

	POINT LastPoint;
	POINT NewPoint;

	if (m_bAntialias)	//Gdi+抗锯齿
	{
		Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(SmoothingModeAntiAlias);
		Pen newPen(Color::Red, 1);
		//第一个点
		LastPoint.x = m_RectCurve.left;
		LastPoint.y = nZeroY - (int)(*(m_pDataBuf + nIndex++) * m_PointStepY);
		if (LastPoint.y < m_RectCurve.top)
		{
			LastPoint.y = m_RectCurve.top;
		}
		else if (LastPoint.y > m_RectCurve.bottom)	//越界
		{
			LastPoint.y = m_RectCurve.bottom;
		}

		for (i = 1; i < m_nMaxX; i++, nIndex++)
		{
			if (m_nDataCount <= i)
			{
				break;
			}
			if (nIndex >= m_nMaxX)
			{
				nIndex = 0;
			}
			NewPoint.x = (int)(m_RectCurve.left + i * m_PointStepX);	//计算x坐标
			if (NewPoint.x == LastPoint.x)	//如果x坐标和上一点一样，则不画
			{
				continue;
			}
			NewPoint.y = nZeroY - (int)(*(m_pDataBuf + nIndex) * m_PointStepY);	//计算y坐标
			if (NewPoint.y < m_RectCurve.top)
			{
				NewPoint.y = m_RectCurve.top;
			}
			else if (NewPoint.y > m_RectCurve.bottom)	//越界
			{
				NewPoint.y = m_RectCurve.bottom;
			}
			graphics.DrawLine(&newPen, LastPoint.x, LastPoint.y, NewPoint.x, NewPoint.y);
			LastPoint = NewPoint;
		}
	}
	else	//普通画法不抗锯齿
	{
		CPen *pOldPen = pDC->SelectObject(&m_Pen_Curve);	//选中画笔绘制,并保存以前的画笔
		//第一个点
		LastPoint.x = m_RectCurve.left;
		LastPoint.y = nZeroY - (int)(*(m_pDataBuf + nIndex++) * m_PointStepY);
		if (LastPoint.y < m_RectCurve.top)
		{
			LastPoint.y = m_RectCurve.top;
		}
		else if (LastPoint.y > m_RectCurve.bottom)	//越界
		{
			LastPoint.y = m_RectCurve.bottom;
		}

		pDC->MoveTo(LastPoint.x, LastPoint.y);
		for (unsigned int i = 1; i < m_nMaxX; i++, nIndex++)
		{
			if (nIndex >= m_nMaxX)
			{
				nIndex = 0;
			}
			if (m_nDataCount <= i)
			{
				break;
			}
			NewPoint.x = (int)(m_RectCurve.left + i * m_PointStepX);	//计算x坐标
			if (NewPoint.x == LastPoint.x)	//如果x坐标和上一点一样，则不画
			{
				continue;
			}
			NewPoint.y = nZeroY - (int)(*(m_pDataBuf + nIndex) * m_PointStepY);	//计算y坐标
			if (NewPoint.y < m_RectCurve.top)
			{
				NewPoint.y = m_RectCurve.top;
			}
			else if (NewPoint.y > m_RectCurve.bottom)	//越界
			{
				NewPoint.y = m_RectCurve.bottom;
			}
			pDC->LineTo(NewPoint.x, NewPoint.y);
			LastPoint = NewPoint;
		}
		pDC->SelectObject(&pOldPen);
	}
	
}



//重载刷背景函数，直接返回TRUE
BOOL CCurve::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;
	//return CWnd::OnEraseBkgnd(pDC);
}



void CCurve::AddData(float fData)
{
	if (m_nDataCount >= m_nDataMAX)
	{
		ClearData();
	}
	if (!m_pDataBuf)
	{
		return;
	}
	*(m_pDataBuf + m_nDataIndex++) = fData;
	if (m_nDataIndex >= m_nMaxX)
	{
		m_nDataIndex = 0;
	}
	if (++m_nDataCount >= m_nMaxX)
	{
		m_nOffset = m_nDataCount % m_nMaxX;
	}
}

void CCurve::ClearData()
{
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nOffset = 0;
}

void CCurve::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	Init(m_nMaxX, m_nMaxY, m_nDataMAX, m_fScaleX, m_fScaleY, m_nGridX, m_nGridY);
	Update();
}
