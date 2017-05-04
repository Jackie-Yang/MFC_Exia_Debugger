// Curve.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "Curve.h"
#include "Color.h"

// CCurve

IMPLEMENT_DYNAMIC(CCurve, CWnd)

CCurve::CCurve():	//��ʼ�����ʻ�ˢ
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


	//��ʼ��������ɫ����ǩ
	GDI_CURVE_INIT(m_GDI_CurveColor);
	NORMAL_CURVE_INIT(m_CPen_NormalCurve);
	LABEL_STRING_INIT(m_LabelStr);

}

CCurve::~CCurve()
{
	
	if (m_bEnhance)
	{
		// �ر�GDI+
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

//��ʼ������ͼ��
//nDataMAX:�������������ֵ��X������ʾ�����ֵ�����Զ�����ΪX_GRID_STEP��������������ֻ�ܴ�����1000�����µ����ݣ�
//fScaleX,fScaleY:���ߵĿ�ȣ��߶�ռ�ؼ��ı���
void CCurve::Init(unsigned int nDataMAX, float fScaleX, float fScaleY)
{
	SetParm(X_LENGTH_DEFAULT, Y_LENGTH_DEFAULT, nDataMAX, fScaleX, fScaleY);
}


//���û��Ʋ�����
//nMaxX,nMaxY:X�᳤�ȣ�Y�������᳤�ȣ���������ͬ��
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

	//���е�������������ݸ����������񣩼�ÿ��Ĵ�С��nMaxX��nMaxY��ı䣬���ܻ�С�ں궨����趨��Сֵ��������������ֵ
	m_nGridX = nMaxX / X_GRID_STEP;
	m_nGridY = nMaxY / Y_GRID_STEP * 2;		//���������ᣬ���Գ���2
	nMaxX = m_nGridX * X_GRID_STEP;	
	nMaxY = m_nGridY / 2 * Y_GRID_STEP;
	nDataMAX = nDataMAX / X_GRID_STEP * X_GRID_STEP;	//��СӦ������ΪX_GRID_STEP��������

	

	GetClientRect(&m_RectBG);	//��ȡ�ؼ�����

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

	//if (nMaxX != m_nMaxX)	//û�иı�nMaxXʱ�����޸���Щ����
	//{
	//	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	//	{
	//		if (m_pDataBuf[Curve])
	//		{
	//			delete[]m_pDataBuf[Curve];
	//			m_pDataBuf[Curve] = NULL;
	//		}
	//		m_pDataBuf[Curve] = new float[nMaxX];	//���ݻ���
	//	}
	//	m_nDataIndex = 0;
	//	m_nDataCount = 0;
	//	m_nOffset = 0;
	//	m_nMaxX = nMaxX;
	//}
	m_nMaxX = nMaxX;
	if (m_nDataCount >= m_nMaxX)		//����������X�ᵱǰ����ʾ��������������X��ƫ��
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
	//�Ѳ�����Ϊ��Ա����������.Ҫresize�Ļ�������
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


//���Ƶ��ļ�
bool CCurve::ScreenShot(CString strFilePath)
{
	CPaintDC dc(this); // device context for painting

	//��ȡ�ļ���׺��
	int nDot_index = strFilePath.ReverseFind(_T('.'));	//��ȡ��׺'.'λ��
	//��ȡ��׺��
	CString szFileType = strFilePath.Right(strFilePath.GetLength() - nDot_index);
	szFileType.MakeLower();	//ͳһ��׺Сд
	HRESULT Result = E_FAIL;

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

	if (MemBitmap.m_hObject)
	{
		CImage imgTemp;
		// CImage��MFC�е��ࡣ
		imgTemp.Attach(MemBitmap.operator HBITMAP());

		//ѡ��Ĭ���ļ�����
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

	//��ͼ��ɺ������   
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
	int nOldMode = pDC->SetBkMode(TRANSPARENT);	//����ɫ͸��
	COLORREF TextOldColor = pDC->SetTextColor(CCurve::TEXT_COLOR);
	//���ȼ��߷ź�ߣ��Ḳ��ǰ���
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
	CBrush *pOldBrush = pDC->SelectObject(&m_CBrush_BG);	//ѡ�л��ʻ���,��������ǰ�Ļ���
	pDC->Rectangle(0, m_RectBG.Height(), m_RectBG.Width(), 0);		//�ڿؼ�����һ�����ο�
	pDC->SelectObject(pOldBrush);		//�ָ���ǰ�Ļ���
}

void CCurve::DrawAxis(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = -(nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//����ƫ��
	CPen *pOldPen = pDC->SelectObject(&m_CPen_Axis);	//ѡ�л��ʻ���,��������ǰ�Ļ���
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

//������������
void CCurve::DrawGrid(CDC *pDC)
{
	float nOffset = m_nOffset * m_PointStepX;
	nOffset = - (nOffset - (int)(nOffset / m_GridStepX) * m_GridStepX);	//����ƫ��
	CPen *pOldPen = pDC->SelectObject(&m_CPen_Grid);	//ѡ�л��ʻ�������,��������ǰ�Ļ���

	//��������
	int NumY = m_nMaxY;						//Y����������ֵ��ʼ��
	int NumY_Step = m_nMaxY * 2 / m_nGridY;	//ÿ�������ٸ�����
	int NumX = m_nDataCount - m_nMaxX;		//X��������ʼֵ
	if (NumX < 0)
	{
		NumX = 0;
	}
	int NumX_Step = (m_nMaxX / m_nGridX);	//ÿ�������ٸ�����
	NumX = NumX / NumX_Step * NumX_Step;	//X���һ������������

	UINT OldTextAlign = pDC->SetTextAlign(TA_CENTER);
	LOGFONT logFont;
	CString str;
	pDC->GetCurrentFont()->GetLogFont(&logFont);
	int Y_offset = logFont.lfHeight / 2;
	int X_offset = logFont.lfWidth / 2;

	pDC->MoveTo(m_RectCurve.left, m_RectCurve.top);	//��һ����Ҫ��
	pDC->LineTo(m_RectCurve.left, m_RectCurve.bottom);

	if ((int)nOffset == 0)		//������0
	{
		str.Format(_T("%d"), NumX);
		pDC->TextOut(m_RectCurve.left, m_RectCurve.bottom + Y_offset, str);
	}
	NumX += NumX_Step;
	
	for (float X = (float)m_RectCurve.left + nOffset + m_GridStepX; (int)X <= m_RectCurve.right; X += m_GridStepX, NumX += NumX_Step)	//������
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

//��������
void CCurve::DrawCurve(CDC *pDC)
{
	const int nZeroY = m_RectCurve.bottom - m_RectCurve.Height() / 2;		//�������м�ΪY��0����
	//i��ʵ���������꣬nIndex�����ݵ�����
	int nIndex;

	if (m_bEnhance)	//Gdi+�����
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
		
		//��������
		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			int nPointCount = 0;	//��Ҫ���ĵ���
			//����nIndexȡ���ݵ���ʼλ��
			if (m_nDataCount > m_nMaxX)
			{
				//Ҫȡ���µ�m_nMaxX������
				if (m_nDataCount > X_LENGTH_MAX)
				{
					nIndex = m_nDataIndex + X_LENGTH_MAX - m_nMaxX;
					if (nIndex >= X_LENGTH_MAX)	//Խ�紦��
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
			//��һ����
			LastPoint.X = m_RectCurve.left;
			LastPoint.Y = nZeroY - (int)(m_pDataBuf[Curve][nIndex++].fData * m_PointStepY);
			if (LastPoint.Y < m_RectCurve.top)
			{
				LastPoint.Y = m_RectCurve.top;
			}
			else if (LastPoint.Y > m_RectCurve.bottom)	//Խ��
			{
				LastPoint.Y = m_RectCurve.bottom;
			}
			pGdiPoint[nPointCount++] = LastPoint;

			float X = m_PointStepX;	//X����
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
				NewPoint.X = (int)(m_RectCurve.left + X);	//����xʵ������
				if (NewPoint.X == LastPoint.X)	//���x�������һ��һ�����򲻻�
				{
					continue;
				}
				NewPoint.Y = nZeroY - (int)(m_pDataBuf[Curve][nIndex].fData * m_PointStepY);	//����y����
				if (NewPoint.Y < m_RectCurve.top)
				{
					NewPoint.Y = m_RectCurve.top;
				}
				else if (NewPoint.Y > m_RectCurve.bottom)	//Խ��
				{
					NewPoint.Y = m_RectCurve.bottom;
				}
				//graphics.DrawLine(&Pen_Curve, LastPoint, NewPoint);
				pGdiPoint[nPointCount++] = NewPoint;
				LastPoint = NewPoint;
			}

			graphics.DrawLines(&Pen_Curve, pGdiPoint, nPointCount);	//ֱ�ӻ������е㣬��Ȼռ��CPU��Դ�ܸ�
		}
		graphics.ReleaseHDC(pDC->m_hDC);
		if (pGdiPoint)
		{
			delete[] pGdiPoint;
		}
	}
	else	//��ͨ�����������
	{
		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//ѡ�л��ʻ���,��������ǰ�Ļ���
		POINT LastPoint;
		POINT NewPoint;

		//��CURVE_LINE�����ߣ�ÿ�λ����л���һ�ֻ���
		for (int Curve = 0; Curve < CURVE_LINE; Curve++, pDC->SelectObject(m_CPen_NormalCurve + Curve))
		{
			//����nIndexȡ���ݵ���ʼλ��
			if (m_nDataCount > m_nMaxX)
			{
				//Ҫȡ���µ�m_nMaxX������
				if (m_nDataCount > X_LENGTH_MAX)
				{
					nIndex = m_nDataIndex + X_LENGTH_MAX - m_nMaxX;	
					if (nIndex >= X_LENGTH_MAX)	//Խ�紦��
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
			//��һ����
			LastPoint.x = m_RectCurve.left;
			LastPoint.y = nZeroY - (int)(m_pDataBuf[Curve][nIndex++].fData * m_PointStepY);
			if (LastPoint.y < m_RectCurve.top)
			{
				LastPoint.y = m_RectCurve.top;
			}
			else if (LastPoint.y > m_RectCurve.bottom)	//Խ��
			{
				LastPoint.y = m_RectCurve.bottom;
			}
			//�ƶ�����һ���㣬֮��ĵ�������
			pDC->MoveTo(LastPoint.x, LastPoint.y);
			float X = m_PointStepX;	//��һ����X����
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
				NewPoint.x = (int)(m_RectCurve.left + X);	//����x����
				if (NewPoint.x == LastPoint.x)	//���x�������һ��һ�����򲻻�
				{
					continue;
				}
				NewPoint.y = nZeroY - (int)(m_pDataBuf[Curve][nIndex].fData * m_PointStepY);	//����y����
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
		}
		
		pDC->SelectObject(&pOldPen);
	}
	
}

//���Ʊ�ǩ
void CCurve::DrawLabel(CDC *pDC)
{
	if (m_bEnhance)	//Gdi+�����
	{
		USES_CONVERSION;	//CStringתWCHAR*Ҫ�õ�������
		WCHAR * wcharLabel[CURVE_LINE] = { NULL };
		WCHAR * wcharValue[CURVE_LINE] = { NULL };
		Gdiplus::Graphics graphics(pDC->m_hDC);
		Gdiplus::Font font(L"����", 10);
		Gdiplus::SolidBrush drawBrush(Gdiplus::Color::TEXT_COLOR);
		Gdiplus::StringFormat format;
		format.SetAlignment(Gdiplus::StringAlignmentCenter);//StringAlignmentFar  StringAlignmentNear

		Gdiplus::PointF origin(0.0f, (m_RectCurve.top - m_RectBG.top) / 2.0f);	//�����Ϸ��հ�������м�
		Gdiplus::RectF Rect[CURVE_LINE];		//ÿ��Label��Rectλ�úʹ�С��Ϣ
		Gdiplus::RectF RectLable, RectValue;	//Label��Value�����ַ�����ֵ��ȡ����Ǹ�	
		Gdiplus::REAL StrLengthSum = 0;			//���ִ����ȣ����ڼ�����
		Gdiplus::REAL LabelSpace = 0;				//ÿ��Label���

		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//ѡ�л��ʻ���,��������ǰ�Ļ���

		//��ȡ�������ַ���
		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			wcharLabel[Curve] = T2W(m_LabelStr[Curve].GetBuffer(m_LabelStr[Curve].GetLength()));
			graphics.MeasureString(wcharLabel[Curve], wcslen(wcharLabel[Curve]), &font, origin, &RectLable);
		
			wcharValue[Curve] = T2W(m_ValueStr[Curve].GetBuffer(m_ValueStr[Curve].GetLength()));
			graphics.MeasureString(wcharValue[Curve], wcslen(wcharValue[Curve]), &font, origin, &RectValue);

			Rect[Curve].Width = RectLable.Width > RectValue.Width ? RectLable.Width : RectValue.Width;	//Lable��Value�Ͽ����Ϊ���Lable���
			Rect[Curve].Height = RectLable.Height > RectValue.Height ? RectLable.Height : RectValue.Height;	//Lable��Value�ϸߵ���Ϊ���Lable�߶�
			StrLengthSum += Rect[Curve].Width;
		}

		LabelSpace = (m_RectBG.Width() - StrLengthSum) / (CURVE_LINE + 1);	//ÿ��Label֮��ļ��

		//��������
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

		//�ͷ�������ַ���
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
		CPen *pOldPen = pDC->SelectObject(m_CPen_NormalCurve);	//ѡ�л��ʻ���,��������ǰ�Ļ���
		UINT OldTextAlign = pDC->SetTextAlign(TA_BOTTOM);	//�������ֶ��뷽ʽ
		CFont font;	//������������
		font.CreateFont(
			16,          //nHeight  ָ���߼���λ���ַ������ַ�Ԫ�߶�
			7,           //nWidth   ָ���߼���λ�������ַ���ƽ�����
			0,           //nEscapement ָ��ÿ���ı����ʱ������豸x��ĽǶȣ��䵥λΪ1/10�ȡ�
			0,           //nOrientation  ָ���ַ�����������豸x��ĽǶȣ��䵥λΪ1/10�ȡ���ֵ��Win9X�к�lfEscapement������ͬ��ֵ������WinNT����ʱ����ܲ�ͬ��
			FW_NORMAL,   //nWeight  ָ�������������Windows������������ʾ����Ĵ�ϸ�̶ȣ��䷶Χ��0��1000֮�䣬����Ϊ400������Ϊ700������ֵΪ�գ���ʹ��Ĭ�ϵ�����������
			FALSE,       //bItalic  ��ֵΪTRUEʱ������Ϊб�塣
			FALSE,       //bUnderline  ��ֵΪTRUEʱ��������»��ߡ�
			0,           //cStrikeOut  ��ֵΪTRUEʱ�������ɾ���ߡ�
			GB2312_CHARSET,                       //nCharSet  ָ����ʹ�õ��ַ�������GB2312_CHARSET,CHINESEBIG5_CHARSET�ȡ�
			OUT_DEFAULT_PRECIS,                 //nOutPrecision  ָ��������ȣ����������������Ҫ�������߶ȡ���ȡ��ַ������������͵���ӽ��ĳ̶ȡ�
			CLIP_DEFAULT_PRECIS,                //nClipPrecision  ָ���������ȣ��������˵��ַ���һ���ֳ�����������ʱ���ַ��ļ�����ʽ��
			ANTIALIASED_QUALITY,          //nQuality  ָ�������������������GDI��ƥ���߼��������Ե�ʵ�ʵ���������ʱ��ʹ�õķ�ʽ��
			DEFAULT_PITCH | FF_SWISS,			//nPitchAndFamily  ָ��������ַ������塣
			_T("����"));						//lpszFaceName  ָ��NULL��β���ַ�����ָ�룬���ַ�����Ϊ��ʹ�õ��������ƣ��䳤�Ȳ��ܳ���32���ַ������Ϊ�գ���ʹ��ϵͳĬ�ϵ����塣
		CFont *pOldFont = pDC->SelectObject(&font);

		CSize StrSize[CURVE_LINE];		//ÿ��Label��Size��Ϣ
		CSize LableStrSize, ValueStrSize;	//Label��Value�����ַ�����ֵ��ȡ����
		LONG StrLengthSum = 0;			//���ִ����ȣ����ڼ�����
		int LabelSpace = 0;				//ÿ��Label���
		int X = 0;
		int Y = (m_RectCurve.top - m_RectBG.top) / 2;	//�����Ϸ��հ�������м�

		for (int Curve = 0; Curve < CURVE_LINE; Curve++)
		{
			LableStrSize = pDC->GetOutputTextExtent(m_LabelStr[Curve]);
			ValueStrSize = pDC->GetOutputTextExtent(m_ValueStr[Curve]);
			StrSize[Curve] = LableStrSize.cx > ValueStrSize.cx ? LableStrSize.cx : ValueStrSize.cx;	//Lable��Value�Ͽ����Ϊ���Lable���
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


		pDC->SelectObject(&pOldFont);	//�ָ���ǰ������
		pDC->SetTextAlign(OldTextAlign);
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


//����������ݣ�
//������float(*fData)[CURVE_LINE]Ϊ��ָ��һ����������Ŀ��ͬ������
void CCurve::AddData(CurveData(*pData)[CURVE_LINE])
{
	if (m_nDataCount >= m_nDataMAX)		//����������������ݣ����
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
	m_nDataShowIndex = m_nDataIndex;		//����ʾ���ݵ�����IndexΪ���²����
	if (++m_nDataIndex >= X_LENGTH_MAX)		//Index���������������ֵ
	{
		m_nDataIndex = 0;
	}
	if (++m_nDataCount >= m_nMaxX)		//����������X�ᵱǰ����ʾ������������ʼ����X��ƫ��
	{
		m_nOffset = m_nDataCount % m_nMaxX;
	}
}

//���������������
void CCurve::ClearData()
{
	m_nDataIndex = 0;
	m_nDataCount = 0;
	m_nOffset = 0;
	m_nDataShowIndex = 0;
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)	//�����ʾ�ִ�
	{
		m_ValueStr[Curve] = _T("");
	}
}

//������д����ʾ�ַ�����
void CCurve::UpdateValueStr()
{
	if (m_nDataCount)	//û������ʱû�����ݿ�����ʾ
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_ValueStr[Curve].Format(_T("%.2f"), m_pDataBuf[Curve][m_nDataShowIndex].fData / m_pDataBuf[Curve][m_nDataShowIndex].fGain);	//��ʾ����֮ǰ�ĳ�ʼֵ
	}
}

//�������ݱ�ǩ
void CCurve::SetLabelStr(CString(*pStr)[CURVE_LINE])
{
	for (int Curve = 0; Curve < CURVE_LINE; Curve++)
	{
		m_LabelStr[Curve] = (*pStr)[Curve];
	}
}



//����ͼ����ǿ�������߽��п���ݴ���
bool CCurve::CurveEnhance(bool bOpen)
{
	if (bOpen)
	{
		if (!m_bEnhance)
		{
			//��ӵĳ�ʼ��GDI+  
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
			// �ر�GDI+
			Gdiplus::GdiplusShutdown(gdiplusToken);
			m_bEnhance = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}


//���ڴ�С�ı�ʱ���¼���
void CCurve::OnSize(UINT nType, int cx, int cy)
{
	CWnd::OnSize(nType, cx, cy);

	// TODO:  �ڴ˴������Ϣ����������
	SetParm(m_nMaxX, m_nMaxY, m_nDataMAX, m_fScaleX, m_fScaleY);
	Update();
}


//����������ʱ���������᳤��
BOOL CCurve::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO:  �ڴ������Ϣ�����������/�����Ĭ��ֵ
	int nChengeValueX = 0, nChengeValueY = 0;
	ScreenToClient(&pt);	//ת���ɿͻ������꣨�ؼ��У�

	if (pt.x >= m_RectCurve.left && pt.x <= m_RectCurve.right)	//������ͼ��ൽ�Ҳ෶Χ����������X��
	{
		nChengeValueX = X_GRID_STEP * (zDelta / 120);
	}

	if (pt.y >= m_RectCurve.top && pt.y <= m_RectCurve.bottom)	//������ͼ���˵��׶˷�Χ����������Y��
	{
		nChengeValueY = Y_GRID_STEP * (zDelta / 120);
	}

	SetParm(m_nMaxX + nChengeValueX, m_nMaxY + nChengeValueY, m_nDataMAX, m_fScaleX, m_fScaleY);
	Update();

	return CWnd::OnMouseWheel(nFlags, zDelta, pt);
}
