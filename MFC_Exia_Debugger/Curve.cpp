// Curve.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "Curve.h"
#include "Color.h"

// CCurve

IMPLEMENT_DYNAMIC(CCurve, CWnd)

CCurve::CCurve():	//初始化画笔花刷
m_CBrush_BG(STYLE_BG),
m_CPen_Grid(STYLE_GRID),
m_CPen_Axis(STYLE_AXIS),
m_RectCurve(0, 0, 0, 0),
m_RectBG(0, 0, 0, 0)
{ 
	RegisterWindowClass();

	m_GridStepX = 0;
	m_GridStepY = 0;
	m_AxisStepX = 0;
	m_AxisStepY = 0;
	m_PointStepX = 0;

	m_nOffset = 0;
	m_nDataIndex = 0;
	m_nDataShowIndex = 0;
	m_nDataCount = 0;
	m_nMaxX = 0;
	m_nMaxY = 0;
	m_bEnhance = FALSE;
	gdiplusToken = 0;


	//初始化曲线颜色及标签
	GDI_CURVE_INIT(m_GDI_CurveColor);
	NORMAL_CURVE_INIT(m_CPen_NormalCurve);
	LABEL_STRING_INIT(m_LabelStr);

}

CCurve::~CCurve()
{
	
	if (m_bEnhance)
	{
		// 关闭GDI+
		GdiplusShutdown(gdiplusToken);
		m_bEnhance = FALSE;
	}
}


BEGIN_MESSAGE_MAP(CCurve, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_SIZE()
	ON_WM_MOUSEWHEEL()
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

//初始化曲线图：
//nDataMAX:数据索引的最大值（X轴能显示的最大值，会自动调整为X_GRID_STEP的整数倍，但是只能存放最多1000个最新的数据）
//fScaleX,fScaleY:曲线的宽度，高度占控件的比例
void CCurve::Init(unsigned int nDataMAX, float fScaleX, float fScaleY)
{
	SetParm(X_LENGTH_DEFAULT, Y_LENGTH_DEFAULT, nDataMAX, fScaleX, fScaleY);
}


//设置绘制参数：
//nMaxX,nMaxY:X轴长度，Y轴正半轴长度（负半轴相同）
void CCurve::SetParm(int nMaxX, int nMaxY, unsigned int nDataMAX, float fScaleX, float fScaleY)
{
	if (nMaxX > X_LENGTH_MAX)
	{
		nMaxX = X_LENGTH_MAX;
	}
	else if (nMaxX < X_LENGTH_MIN)
	{
		nMaxX = X_LENGTH_MIN;
	}
	if (nMaxY > Y_LENGTH_MAX)
	{
		nMaxY = Y_LENGTH_MAX;
	}
	else if (nMaxY < Y_LENGTH_MIN)
	{
		nMaxY = Y_LENGTH_MIN;
	}

	//进行调整，调整后根据格数（整数格）及每格的大小，nMaxX，nMaxY会改变，可能会小于宏定义的设定最小值，但不会大于最大值
	m_nGridX = nMaxX / X_GRID_STEP;
	m_nGridY = nMaxY / Y_GRID_STEP * 2;		//有正负半轴，所以乘以2
	nMaxX = m_nGridX * X_GRID_STEP;	
	nMaxY = m_nGridY / 2 * Y_GRID_STEP;
	nDataMAX = nDataMAX / X_GRID_STEP * X_GRID_STEP;	//大小应该设置为X_GRID_STEP的整数倍

	

	GetClientRect(&m_RectBG);	//获取控件区域

	int nPaddingX, nPaddingY;
	nPaddingX = (int)(m_RectBG.Width() * (1 - fScaleX) * 0.5f);
	nPaddingY = (int)(m_RectBG.Height() * (1 - fScaleY) * 0.5f);

	m_RectCurve.top = nPaddingY;
	m_RectCurve.right = m_RectBG.Width() - nPaddingX;

	m_RectCurve.left = nPaddingX;
	m_RectCurve.bottom = m_RectBG.Height() - nPaddingY;

	m_GridStepX = m_RectCurve.Width() / (float)m_nGridX;
	m_GridStepY = m_RectCurve.Height() / (float)m_nGridY;

	m_AxisStepX = m_GridStepX / 5.0f;
	m_AxisStepY = m_GridStepY / 5.0f;

	//m_nBufSize = m_RectCurve.Width();
	m_PointStepX = (float)m_RectCurve.Width() / (float)nMaxX;
	m_PointStepY = (float)m_RectCurve.Height() / (float)nMaxY / 2;

	//if (nMaxX != m_nMaxX)	//没有改变nMaxX时不用修改这些数据
	//{
	//	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	//	{
	//		if (m_pDataBuf[Curve])
	//		{
	//			delete[]m_pDataBuf[Curve];
	//			m_pDataBuf[Curve] = NULL;
	//		}
	//		m_pDataBuf[Curve] = new float[nMaxX];	//数据缓存
	//	}
	//	m_nDataIndex = 0;
	//	m_nDataCount = 0;
	//	m_nOffset = 0;
	//	m_nMaxX = nMaxX;
	//}
	m_nMaxX = nMaxX;
	if (m_nDataCount >= m_nMaxX)		//数据量大于X轴当前能显示的数据量，计算X轴偏移
	{
		m_nOffset = m_nDataCount % m_nMaxX;
	}
	else
	{
		m_nOffset = 0;
	}
	m_nMaxY = nMaxY;

	m_fScaleX = fScaleX;
	m_fScaleY = fScaleY;
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


//绘制到文件
bool CCurve::ScreenShot(CString strFilePath)
{
	CPaintDC dc(this); // device context for painting

	//获取文件后缀名
	int nDot_index = strFilePath.ReverseFind(_T('.'));	//获取后缀'.'位置
	//获取后缀名
	CString szFileType = strFilePath.Right(strFilePath.GetLength() - nDot_index);
	szFileType.MakeLower();	//统一后缀小写
	HRESULT Result = E_FAIL;

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

	if (MemBitmap.m_hObject)
	{
		CImage imgTemp;
		// CImage是MFC中的类。
		imgTemp.Attach(MemBitmap.operator HBITMAP());

		//选择默认文件类型
		if (szFileType == _T(".bmp") || szFileType == _T(".dib"))
		{
			Result = imgTemp.Save(strFilePath, Gdiplus::ImageFormatBMP);
		}
		else if (szFileType == _T(".png"))
		{
			Result = imgTemp.Save(strFilePath, Gdiplus::ImageFormatPNG);
		}
		else if (szFileType == _T(".jpg") || szFileType == _T(".jpeg") || szFileType == _T(".jpe"))
		{
			Result = imgTemp.Save(strFilePath, Gdiplus::ImageFormatJPEG);
		}
		else if (szFileType == _T(".gif"))
		{
			Result = imgTemp.Save(strFilePath, Gdiplus::ImageFormatGIF);
		}
		
	}

	//绘图完成后的清理   
	MemBitmap.DeleteObject();
	MemDC.DeleteDC();
	if (Result == S_OK)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void CCurve::Update()
{
	Invalidate();
	UpdateWindow();
}



void CCurve::DrawAll(CDC * pDC)
{
	int nOldMode = pDC->SetBkMode(TRANSPARENT);	//背景色透明
	COLORREF TextOldColor = pDC->SetTextColor(CCurve::TEXT_COLOR);
	//优先级高放后边，会覆盖前面的
	DrawBG(pDC);
	DrawGrid(pDC);
	DrawCurve(pDC);
	DrawAxis(pDC);
	DrawLabel(pDC);

	pDC->SetTextColor(TextOldColor);
	pDC->SetBkMode(nOldMode);
}


void CCurve::DrawBG(CDC *pDC)
{
	CBrush *pOldBrush = pDC->SelectObject(&m_CBrush_BG);	//选中画笔绘制,并保存以前的画笔
	pDC->Rectangle(0, m_RectBG.Height(), m_RectBG.Width(), 0);		//在控件区域画一个矩形框
	pDC->SelectObject(pOldBrush);		//恢复以前的画笔
}

void CCurve::DrawAxis(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = -(nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//计算偏移
	CPen *pOldPen = pDC->SelectObject(&m_CPen_Axis);	//选中画笔绘制,并保存以前的画笔
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

//绘制网格及文字
void CCurve::DrawGrid(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = - (nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//计算偏移
	CPen *pOldPen = pDC->SelectObject(&m_CPen_Grid);	//选中画笔绘制网格,并保存以前的画笔

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

	UINT OldTextAlign = pDC->SetTextAlign(TA_CENTER);
	LOGFONT logFont;
	CString str;
	pDC->GetCurrentFont()->GetLogFont(&logFont);
	int Y_offset = logFont.lfHeight / 2;
	int X_offset = logFont.lfWidth / 2;

	pDC->MoveTo(m_RectCurve.left, m_RectCurve.top);	//第一条线要画
	pDC->LineTo(m_RectCurve.left, m_RectCurve.bottom);

	if ((int)nOffset == 0)		//横坐标0
	{
		str.Format(_T("%d"), NumX);
		pDC->TextOut(m_RectCurve.left, m_RectCurve.bottom + Y_offset, str);
	}
	NumX += NumX_Step;
	
	for (float X = (float)m_RectCurve.left + nOffset + m_GridStepX; (int)X <= m_RectCurve.right; X += m_GridStepX, NumX += NumX_Step)	//横坐标
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
	pDC->SetTextAlign(OldTextAlign);
}

//绘制曲线
void CCurve::DrawCurve(CDC *pDC)
{
	const int nZeroY = m_RectCurve.bottom - m_RectCurve.Height() / 2;		//坐标轴中间为Y轴0坐标
	//i是实际区域坐标，nIndex是数据的坐标
	int nIndex;

	if (m_bEnhance)	//Gdi+抗锯齿
	{
		Gdiplus::Point *pGdiPoint = NULL;
		pGdiPoint = new Gdiplus::Point[m_nMaxX];
		if (!pGdiPoint)
		{
			return;
		}

		Gdiplus::Point LastPoint;
		Gdiplus::Point NewPoint;
		Gdiplus::Graphics graphics(pDC->m_hDC);
		graphics.SetSmoothingMode(Gdiplus::SmoothingModeAntiAlias);
		Gdiplus::Pen Pen_Curve(Gdiplus::Color::Transparent, 1);
		
		//四条曲线
		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			int nPointCount = 0;	//需要画的点数
			//计算nIndex取数据的起始位置
			if (m_nDataCount > m_nMaxX)
			{
				//要取最新的m_nMaxX个数据
				if (m_nDataCount > X_LENGTH_MAX)
				{
					nIndex = m_nDataIndex + X_LENGTH_MAX - m_nMaxX;
					if (nIndex >= X_LENGTH_MAX)	//越界处理
					{
						nIndex = nIndex - X_LENGTH_MAX;
					}
				}
				else
				{
					nIndex = m_nDataCount - m_nMaxX;
				}
			}
			else
			{
				nIndex = 0;
			}
			Pen_Curve.SetColor(m_GDI_CurveColor[Curve]);
			//第一个点
			LastPoint.X = m_RectCurve.left;
			LastPoint.Y = nZeroY - (int)(m_pDataBuf[Curve][nIndex++].fData * m_PointStepY);
			if (LastPoint.Y < m_RectCurve.top)
			{
				LastPoint.Y = m_RectCurve.top;
			}
			else if (LastPoint.Y > m_RectCurve.bottom)	//越界
			{
				LastPoint.Y = m_RectCurve.bottom;
			}
			pGdiPoint[nPointCount++] = LastPoint;

			float X = m_PointStepX;	//X坐标
			for (int i = 1; i < m_nMaxX; i++, nIndex++, X += m_PointStepX)
			{
				if (m_nDataCount <= i)
				{
					break;
				}
				if (nIndex >= X_LENGTH_MAX)
				{
					nIndex = 0;
				}
				NewPoint.X = (int)(m_RectCurve.left + X);	//计算x实际坐标
				if (NewPoint.X == LastPoint.X)	//如果x坐标和上一点一样，则不画
				{
					continue;
				}
				NewPoint.Y = nZeroY - (int)(m_pDataBuf[Curve][nIndex].fData * m_PointStepY);	//计算y坐标
				if (NewPoint.Y < m_RectCurve.top)
				{
					NewPoint.Y = m_RectCurve.top;
				}
				else if (NewPoint.Y > m_RectCurve.bottom)	//越界
				{
					NewPoint.Y = m_RectCurve.bottom;
				}
				//graphics.DrawLine(&Pen_Curve, LastPoint, NewPoint);
				pGdiPoint[nPointCount++] = NewPoint;
				LastPoint = NewPoint;
			}

			graphics.DrawLines(&Pen_Curve, pGdiPoint, nPointCount);	//直接画出所有点，不然占用CPU资源很高
		}
		graphics.ReleaseHDC(pDC->m_hDC);
		if (pGdiPoint)
		{
			delete[] pGdiPoint;
		}
	}
	else	//普通画法不抗锯齿
	{
		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//选中画笔绘制,并保存以前的画笔
		POINT LastPoint;
		POINT NewPoint;

		//画CURVE_LINE条曲线，每次画完切换下一种画笔
		for (int Curve = 0; Curve < CURVE_LINE; Curve++, pDC->SelectObject(m_CPen_NormalCurve + Curve))
		{
			//计算nIndex取数据的起始位置
			if (m_nDataCount > m_nMaxX)
			{
				//要取最新的m_nMaxX个数据
				if (m_nDataCount > X_LENGTH_MAX)
				{
					nIndex = m_nDataIndex + X_LENGTH_MAX - m_nMaxX;	
					if (nIndex >= X_LENGTH_MAX)	//越界处理
					{
						nIndex = nIndex - X_LENGTH_MAX;
					}
				}
				else
				{
					nIndex = m_nDataCount - m_nMaxX;
				}
			}
			else
			{
				nIndex = 0;
			}
			//第一个点
			LastPoint.x = m_RectCurve.left;
			LastPoint.y = nZeroY - (int)(m_pDataBuf[Curve][nIndex++].fData * m_PointStepY);
			if (LastPoint.y < m_RectCurve.top)
			{
				LastPoint.y = m_RectCurve.top;
			}
			else if (LastPoint.y > m_RectCurve.bottom)	//越界
			{
				LastPoint.y = m_RectCurve.bottom;
			}
			//移动到第一个点，之后的点连续画
			pDC->MoveTo(LastPoint.x, LastPoint.y);
			float X = m_PointStepX;	//第一个点X坐标
			for (int i = 1; i < m_nMaxX; i++, nIndex++, X += m_PointStepX)
			{
				if (nIndex >= X_LENGTH_MAX)
				{
					nIndex = 0;
				}
				if (m_nDataCount <= i)
				{
					break;
				}
				NewPoint.x = (int)(m_RectCurve.left + X);	//计算x坐标
				if (NewPoint.x == LastPoint.x)	//如果x坐标和上一点一样，则不画
				{
					continue;
				}
				NewPoint.y = nZeroY - (int)(m_pDataBuf[Curve][nIndex].fData * m_PointStepY);	//计算y坐标
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
		}
		
		pDC->SelectObject(&pOldPen);
	}
	
}

//绘制标签
void CCurve::DrawLabel(CDC *pDC)
{
	if (m_bEnhance)	//Gdi+抗锯齿
	{
		USES_CONVERSION;	//CString转WCHAR*要用到的声明
		WCHAR * wcharLabel[CURVE_LINE] = { NULL };
		WCHAR * wcharValue[CURVE_LINE] = { NULL };
		Gdiplus::Graphics graphics(pDC->m_hDC);
		Gdiplus::Font font(L"宋体", 10);
		Gdiplus::SolidBrush drawBrush(Gdiplus::Color::TEXT_COLOR);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);//StringAlignmentFar  StringAlignmentNear

		Gdiplus::PointF origin(0.0f, (m_RectCurve.top - m_RectBG.top) / 2.0f);	//曲线上方空白区域的中间
		Gdiplus::RectF Rect[CURVE_LINE];		//每个Label的Rect位置和大小信息
		Gdiplus::RectF RectLable, RectValue;	//Label和Value两个字符串的值，取宽的那个	
		Gdiplus::REAL StrLengthSum = 0;			//总字串长度，用于计算间隔
		Gdiplus::REAL LabelSpace = 0;				//每格Label间隔

		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//选中画笔绘制,并保存以前的画笔

		//获取，测量字符串
		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			wcharLabel[Curve] = T2W(m_LabelStr[Curve].GetBuffer(m_LabelStr[Curve].GetLength()));
			graphics.MeasureString(wcharLabel[Curve], wcslen(wcharLabel[Curve]), &font, origin, &RectLable);
		
			wcharValue[Curve] = T2W(m_ValueStr[Curve].GetBuffer(m_ValueStr[Curve].GetLength()));
			graphics.MeasureString(wcharValue[Curve], wcslen(wcharValue[Curve]), &font, origin, &RectValue);

			Rect[Curve].Width = RectLable.Width > RectValue.Width ? RectLable.Width : RectValue.Width;	//Lable和Value较宽的作为这个Lable宽度
			Rect[Curve].Height = RectLable.Height > RectValue.Height ? RectLable.Height : RectValue.Height;	//Lable和Value较高的作为这个Lable高度
			StrLengthSum += Rect[Curve].Width;
		}

		LabelSpace = (m_RectBG.Width() - StrLengthSum) / (CURVE_LINE + 1);	//每个Label之间的间隔

		//绘制曲线
		origin.X = LabelSpace;
		for (int Curve = 0; Curve < CURVE_LINE; Curve++, origin.X += LabelSpace, pDC->SelectObject(m_CPen_NormalCurve + Curve))
		{
			Rect[Curve].X = origin.X;
			Rect[Curve].Y = origin.Y;
			graphics.DrawString(wcharValue[Curve], wcslen(wcharValue[Curve]), &font, Rect[Curve], &format, &drawBrush);
			Rect[Curve].Y -= Rect[Curve].Height;
			graphics.DrawString(wcharLabel[Curve], wcslen(wcharLabel[Curve]), &font, Rect[Curve], &format, &drawBrush);

			pDC->MoveTo((int)origin.X, (int)origin.Y);
			origin.X += Rect[Curve].Width;
			pDC->LineTo((int)origin.X, (int)origin.Y);
		}

		//释放申请的字符串
		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			m_LabelStr[Curve].ReleaseBuffer(m_LabelStr[Curve].GetLength());
			m_ValueStr[Curve].ReleaseBuffer(m_ValueStr[Curve].GetLength());
		}

		pDC->SelectObject(&pOldPen);
		graphics.ReleaseHDC(pDC->m_hDC);
	}
	else
	{
		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//选中画笔绘制,并保存以前的画笔
		UINT OldTextAlign = pDC->SetTextAlign(TA_BOTTOM);	//设置文字对齐方式
		CFont font;	//设置文字字体
		font.CreateFont(
			16,          //nHeight  指定逻辑单位的字符或者字符元高度
			7,           //nWidth   指定逻辑单位的字体字符的平均宽度
			0,           //nEscapement 指定每行文本输出时相对于设备x轴的角度，其单位为1/10度。
			0,           //nOrientation  指定字符基线相对于设备x轴的角度，其单位为1/10度。此值在Win9X中和lfEscapement具有相同的值，而在WinNT下有时候可能不同。
			FW_NORMAL,   //nWeight  指定字体的重量，Windows中字体重量表示字体的粗细程度，其范围在0～1000之间，正常为400，粗体为700，若此值为空，则使用默认的字体重量。
			FALSE,       //bItalic  此值为TRUE时，字体为斜体。
			FALSE,       //bUnderline  此值为TRUE时，字体带下划线。
			0,           //cStrikeOut  此值为TRUE时，字体带删除线。
			GB2312_CHARSET,                       //nCharSet  指定所使用的字符集，如GB2312_CHARSET,CHINESEBIG5_CHARSET等。
			OUT_DEFAULT_PRECIS,                 //nOutPrecision  指定输出精度，它定义了输出与所要求的字体高度、宽度、字符方向及字体类型等相接近的程度。
			CLIP_DEFAULT_PRECIS,                //nClipPrecision  指定剪辑精度，它定义了当字符的一部分超过剪辑区域时对字符的剪辑方式。
			ANTIALIASED_QUALITY,          //nQuality  指定输出质量，它定义了GDI在匹配逻辑字体属性到实际的物理字体时所使用的方式。
			DEFAULT_PITCH | FF_SWISS,			//nPitchAndFamily  指定字体的字符间距和族。
			_T("宋体"));						//lpszFaceName  指向NULL结尾的字符串的指针，此字符串即为所使用的字体名称，其长度不能超过32个字符，如果为空，则使用系统默认的字体。
		CFont *pOldFont = pDC->SelectObject(&font);

		CSize StrSize[CURVE_LINE];		//每个Label的Size信息
		CSize LableStrSize, ValueStrSize;	//Label和Value两个字符串的值，取长的
		LONG StrLengthSum = 0;			//总字串长度，用于计算间隔
		int LabelSpace = 0;				//每格Label间隔
		int X = 0;
		int Y = (m_RectCurve.top - m_RectBG.top) / 2;	//曲线上方空白区域的中间

		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			LableStrSize = pDC->GetOutputTextExtent(m_LabelStr[Curve]);
			ValueStrSize = pDC->GetOutputTextExtent(m_ValueStr[Curve]);
			StrSize[Curve] = LableStrSize.cx > ValueStrSize.cx ? LableStrSize.cx : ValueStrSize.cx;	//Lable和Value较宽的作为这个Lable宽度
			StrLengthSum += StrSize[Curve].cx;
		}
		LabelSpace = (m_RectBG.Width() - StrLengthSum) / (CURVE_LINE + 1);


		for (int Curve = 0, X = LabelSpace; Curve < CURVE_LINE; Curve++, X += LabelSpace, pDC->SelectObject(m_CPen_NormalCurve + Curve))
		{
			pDC->SetTextAlign(TA_BOTTOM);
			pDC->TextOut(X, Y - 1, m_LabelStr[Curve]);
			pDC->SetTextAlign(TA_TOP);
			pDC->TextOut(X, Y + 1, m_ValueStr[Curve]);
			pDC->MoveTo(X, Y);
			X += StrSize[Curve].cx;
			pDC->LineTo(X, Y);
		}


		pDC->SelectObject(&pOldFont);	//恢复以前的字体
		pDC->SetTextAlign(OldTextAlign);
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


//添加曲线数据：
//参数：float(*fData)[CURVE_LINE]为，指向一个与曲线数目相同的数组
void CCurve::AddData(CurveData(*pData)[CURVE_LINE])
{
	if (m_nDataCount >= m_nDataMAX)		//数据量大于最大数据，清空
	{
		ClearData();
	}
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		//if (!m_pDataBuf[Curve])
		//{
		//	return;
		//}
		(*pData)[Curve].fData = (*pData)[Curve].fData * (*pData)[Curve].fGain;
		m_pDataBuf[Curve][m_nDataIndex] = (*pData)[Curve];
	}
	m_nDataShowIndex = m_nDataIndex;		//待显示数据的数据Index为最新插入的
	if (++m_nDataIndex >= X_LENGTH_MAX)		//Index大于数组索引最大值
	{
		m_nDataIndex = 0;
	}
	if (++m_nDataCount >= m_nMaxX)		//数据量大于X轴当前能显示的数据量，开始计算X轴偏移
	{
		m_nOffset = m_nDataCount % m_nMaxX;
	}
}

//清空所有曲线数据
void CCurve::ClearData()
{
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nOffset = 0;
	m_nDataShowIndex = 0;
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)	//清空显示字串
	{
		m_ValueStr[Curve] = _T("");
	}
}

//将数据写到显示字符串中
void CCurve::UpdateValueStr()
{
	if (m_nDataCount)	//没有数据时没有数据可以显示
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ValueStr[Curve].Format(_T("%.2f"), m_pDataBuf[Curve][m_nDataShowIndex].fData / m_pDataBuf[Curve][m_nDataShowIndex].fGain);	//显示增益之前的初始值
	}
}

//设置数据标签
void CCurve::SetLabelStr(CString(*pStr)[CURVE_LINE])
{
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_LabelStr[Curve] = (*pStr)[Curve];
	}
}



//设置图像增强，对曲线进行抗锯齿处理
bool CCurve::CurveEnhance(bool bOpen)
{
	if (bOpen)
	{
		if (!m_bEnhance)
		{
			//添加的初始化GDI+  
			if (Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL) == Gdiplus::Ok)
			{
				m_bEnhance = TRUE;
				return TRUE;
			}
		}
	}
	else
	{
		if (m_bEnhance)
		{
			// 关闭GDI+
			Gdiplus::GdiplusShutdown(gdiplusToken);
			m_bEnhance = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}


//窗口大小改变时重新计算
void CCurve::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  在此处添加消息处理程序代码
	SetParm(m_nMaxX, m_nMaxY, m_nDataMAX, m_fScaleX, m_fScaleY);
	Update();
}


//滚动鼠标滚轮时更改坐标轴长度
BOOL CCurve::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	int nChengeValueX = 0, nChengeValueY = 0;
	ScreenToClient(&pt);	//转换成客户区坐标（控件中）

	if (pt.x >= m_RectCurve.left && pt.x <= m_RectCurve.right)	//在曲线图左侧到右侧范围滚动，调整X轴
	{
		nChengeValueX = X_GRID_STEP * (zDelta / 120);
	}

	if (pt.y >= m_RectCurve.top && pt.y <= m_RectCurve.bottom)	//在曲线图顶端到底端范围滚动，调整Y轴
	{
		nChengeValueY = Y_GRID_STEP * (zDelta / 120);
	}

	SetParm(m_nMaxX + nChengeValueX, m_nMaxY + nChengeValueY, m_nDataMAX, m_fScaleX, m_fScaleY);
	Update();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
