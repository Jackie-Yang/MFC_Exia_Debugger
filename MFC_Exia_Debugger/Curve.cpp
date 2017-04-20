// Curve.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "Curve.h"


// CCurve

IMPLEMENT_DYNAMIC(CCurve, CWnd)

CCurve::CCurve()
{
	RegisterWindowClass();
}

CCurve::~CCurve()
{
}


BEGIN_MESSAGE_MAP(CCurve, CWnd)
	ON_WM_PAINT()
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
		windowclass.hbrBackground = (HBRUSH) ::GetStockObject(BLACK_BRUSH);
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

void CCurve::OnPaint()
{
	CPaintDC dc(this); // device context for painting
	// TODO:  在此处添加消息处理程序代码
	// 不为绘图消息调用 CWnd::OnPaint()
	CRect rect;

	this->GetClientRect(rect);



	dc.MoveTo(0, 0);

	dc.LineTo(rect.right, rect.bottom);
}
