#pragma once


// CCurve

#include "Color.h"


//���ý��飺
//LENGTH_DEFAULT,LENGTH_MAX,LENGTH_MINӦ��ΪGRID_STEP������
//����(LENGTH_MAX - LENGTH_MIN) / GRID_STEPӦ��Ϊ����
//���������Ͻ�����Զ�����ʵ��Ч��
#define X_GRID_STEP			100		//X��ÿ�����ݳ���,��������Ϊ100�ı���
#define Y_GRID_STEP			200		//Y��ÿ�����ݳ��ȣ���������Ϊ100�ı���

#define X_LENGTH_DEFAULT	800		//Ĭ��X�᳤��
#define X_LENGTH_MAX		1000	//X�᳤�����ֵ
#define X_LENGTH_MIN		500		//X�᳤����Сֵ

#define Y_LENGTH_DEFAULT	1000	//Ĭ��Y�������᳤�ȣ���������ͬ��
#define Y_LENGTH_MAX		1000	//Y�������᳤�����ֵ
#define Y_LENGTH_MIN		400		//Y�������᳤����Сֵ


//Ĭ�Ϸ��
//Default Style

#define STYLE_BG		CCurve::Black
#define STYLE_GRID		PS_DOT, 1, CCurve::Teal
#define STYLE_AXIS		PS_SOLID, 1, CCurve::Magenta


//������ߵķ�����CURVE_LINE���ó���Ӧ��������Ŀ�������ɫ�ĺ궨�壬����AddDataʱ���������ָ���СӦ�ú�������Ŀһ�£���ȻҲ�ᱨ��
//�����趨
#define CURVE_LINE	4
//�������õ���ɫ��Ҫ�����߶��������һ��
#define COLOR_CURVE0	Red
#define COLOR_CURVE1	RoyalBlue
#define COLOR_CURVE2	Cyan
#define COLOR_CURVE3	Yellow
#define GDI_CURVE_INIT(COLOR_ARRAY)	do{\
										COLOR_ARRAY[0] = Gdiplus::Color::COLOR_CURVE0; \
										COLOR_ARRAY[1] = Gdiplus::Color::COLOR_CURVE1; \
										COLOR_ARRAY[2] = Gdiplus::Color::COLOR_CURVE2; \
										COLOR_ARRAY[3] = Gdiplus::Color::COLOR_CURVE3;\
									}while(0)

#define NORMAL_CURVE_INIT(CPEN_ARRAY) do{ \
										CPEN_ARRAY[0].CreatePen(PS_SOLID, 1, CCurve::COLOR_CURVE0);\
										CPEN_ARRAY[1].CreatePen(PS_SOLID, 1, CCurve::COLOR_CURVE1);\
										CPEN_ARRAY[2].CreatePen(PS_SOLID, 1, CCurve::COLOR_CURVE2);\
										CPEN_ARRAY[3].CreatePen(PS_SOLID, 1, CCurve::COLOR_CURVE3); \
									}while(0)

class CCurve : public CWnd
{
	DECLARE_DYNAMIC(CCurve)

public:
	CCurve();
	virtual ~CCurve();

protected:
	DECLARE_MESSAGE_MAP()

private:

	//��ͨ���ʻ�ˢ
	CBrush m_CBrush_BG;
	CPen m_CPen_Grid;
	CPen m_CPen_Axis;
	CPen m_CPen_NormalCurve[CURVE_LINE];

	//��GDI�����ߵ���ɫ
	Gdiplus::Color m_GDI_CurveColor[CURVE_LINE];


	CRect m_RectBG;
	CRect m_RectCurve;

	float m_fScaleX;
	float m_fScaleY;
	int m_nGridX;
	int m_nGridY;

	float m_GridStepX;
	float m_GridStepY;
	float m_AxisStepX;
	float m_AxisStepY;

	float m_PointStepX;
	float m_PointStepY;

	bool RegisterWindowClass();
	void DrawAll(CDC * pDC);		//��������Ԫ��
	void DrawBG(CDC *pDC);			//���Ʊ���
	void DrawGrid(CDC *pDC);		//��������
	void DrawAxis(CDC *pDC);		//��������
	void DrawCurve(CDC *pDC);		//��������

	float *m_pDataBuf[CURVE_LINE];
	unsigned int m_nDataIndex;
	int m_nDataCount;
	int m_nDataMAX;
	int m_nMaxX;
	int m_nMaxY;
	int m_nOffset;
	

	//GDI+���
	bool m_bEnhance;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	//���ò���
	void SetParm(int nMaxX, int nMaxY, unsigned int nDataMAX, float fScaleX, float fScaleY);

public:
	afx_msg void OnPaint();
	void Init(unsigned int nDataMAX, float fScaleX, float fScaleY);

	void AddData(float(*fData)[CURVE_LINE]);
	void ClearData();
	void Update();

	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	void CurveEnhance(bool bOpen);
	
	afx_msg void OnSize(UINT nType, int cx, int cy);

	//��Ҫ��Ϊ�˼���GDI+�е���ɫ����
	enum
	{
		AliceBlue = RGB(240, 248, 255), // ����˿�� 
		AntiqueWhite = RGB(250, 235, 215), // �Ŷ��� 
		Aqua = RGB(0, 255, 255), // ˮɫ 
		Aquamarine = RGB(127, 255, 212), // ��ʯ���� 
		Azure = RGB(240, 255, 255), // ε��ɫ 
		Beige = RGB(245, 245, 220), // ��ɫ(����ɫ) 
		Bisque = RGB(255, 228, 196), // ������ 
		Black = RGB(0, 0, 0), // ���� 
		BlanchedAlmond = RGB(255, 235, 205), // ���׵�����ɫ 
		Blue = RGB(0, 0, 255), // ���� 
		BlueViolet = RGB(138, 43, 226), // �������� 
		Brown = RGB(165, 42, 42), // ��ɫ 
		BurlyWood = RGB(222, 184, 135), // Ӳľɫ 
		CadetBlue = RGB(95, 158, 160), // ������ 
		Chartreuse = RGB(127, 255, 0), // ���ؾ��� (����ɫ) 
		Chocolate = RGB(210, 105, 30), // �ɿ��� 
		Coral = RGB(255, 127, 80), // ɺ�� 
		CornflowerBlue = RGB(100, 149, 237), // ʸ������ 
		Cornsilk = RGB(255, 248, 220), // ����˿ɫ 
		Crimson = RGB(220, 20, 60), // �ɺ� (���) 
		Cyan = RGB(0, 255, 255), // ��ɫ 
		DarkBlue = RGB(0, 0, 139), // ����ɫ 
		DarkCyan = RGB(0, 139, 139), // ����ɫ 
		DarkGoldenrod = RGB(184, 134, 11), // ����ջ� 
		DarkGray = RGB(169, 169, 169), // ���ɫ 
		DarkGreen = RGB(0, 100, 0), // ����ɫ 
		DarkKhaki = RGB(189, 183, 107), // �ߴ�� 
		DarkMagenta = RGB(139, 0, 139), // ����� 
		DarkOliveGreen = RGB(85, 107, 47), // ������� 
		DarkOrange = RGB(255, 140, 0), // ���ɫ 
		DarkOrchid = RGB(153, 50, 204), // �������� 
		DarkRed = RGB(139, 0, 0), // ���ɫ 
		DarkSalmon = RGB(233, 150, 122), // ������(����)ɫ 
		DarkSeaGreen = RGB(143, 188, 143), // �������� 
		DarkSlateBlue = RGB(72, 61, 139), // �������� 
		DarkSlateGray = RGB(47, 79, 79), // ��ʯ��� 
		DarkTurquoise = RGB(0, 206, 209), // ���̱�ʯ 
		DarkViolet = RGB(148, 0, 211), // �������� 
		DeepPink = RGB(255, 20, 147), // ��ۺ� 
		DeepSkyBlue = RGB(0, 191, 255), // ������ 
		DimGray = RGB(105, 105, 105), // �����Ļ�ɫ 
		DodgerBlue = RGB(30, 144, 255), // ������ 
		Firebrick = RGB(178, 34, 34), // �ͻ�ש 
		FloralWhite = RGB(255, 250, 240), // ���İ�ɫ 
		ForestGreen = RGB(34, 139, 34), // ɭ���� 
		Fuchsia = RGB(255, 0, 255), // ��������(�Ϻ�ɫ) 
		Gainsboro = RGB(220, 220, 220), // ��˹���޻�ɫ 
		GhostWhite = RGB(248, 248, 255), // ����� 
		Gold = RGB(255, 215, 0), // ��ɫ 
		Goldenrod = RGB(218, 165, 32), // ��ջ� 
		Gray = RGB(186, 183, 186), // ��ɫ(*) 
		Green = RGB(0, 128, 0), // ���� 
		GreenYellow = RGB(173, 255, 47), // �̻�ɫ 
		Honeydew = RGB(240, 255, 240), // �۹�ɫ 
		HotPink = RGB(255, 105, 180), // ����ķۺ� 
		IndianRed = RGB(205, 92, 92), // ӡ�Ⱥ� 
		Indigo = RGB(75, 0, 130), // ���� (����ɫ) 
		Ivory = RGB(255, 255, 240), // ���� 
		Khaki = RGB(240, 230, 140), // ��ߴ�� 
		Lavender = RGB(230, 230, 250), // Ѭ�²ݵ��� 
		LavenderBlush = RGB(255, 240, 245), // ���Ϻ� 
		LawnGreen = RGB(124, 252, 0), // ��ƺ�� 
		LemonChiffon = RGB(255, 250, 205), // ���ʳ� 
		LightBlue = RGB(173, 216, 230), // ���� 
		LightCoral = RGB(240, 128, 128), // ��ɺ��ɫ 
		LightCyan = RGB(224, 255, 255), // ����ɫ 
		LightGoldenrodYellow = RGB(250, 250, 210), // ���ջ� 
		LightGray = RGB(144, 238, 144), // ����ɫ 
		LightGreen = RGB(211, 211, 211), // ǳ��ɫ 
		LightPink = RGB(255, 182, 193), // ǳ�ۺ� 
		LightSalmon = RGB(255, 160, 122), // ǳ������ɫ 
		LightSeaGreen = RGB(32, 178, 170), // ǳ������ 
		LightSkyBlue = RGB(135, 206, 250), // ������ɫ 
		LightSlateGray = RGB(119, 136, 153), // ��ʯ��� 
		LightSteelBlue = RGB(176, 196, 222), // ������ 
		LightYellow = RGB(255, 255, 224), // ǳ��ɫ 
		Lime = RGB(0, 255, 0), // ������ 
		LimeGreen = RGB(50, 205, 50), // �������� 
		Linen = RGB(250, 240, 230), // ���鲼 
		Magenta = RGB(255, 0, 255), // ��� (Ʒ�� õ���) 
		Maroon = RGB(128, 0, 0), // ��ɫ 
		MediumAquamarine = RGB(102, 205, 170), // �б�ʯ���� 
		MediumBlue = RGB(0, 0, 205), // ����ɫ 
		MediumOrchid = RGB(186, 85, 211), // �������� 
		MediumPurple = RGB(147, 112, 219), // ����ɫ 
		MediumSeaGreen = RGB(60, 179, 113), // �к����� 
		MediumSlateBlue = RGB(123, 104, 238), // �а����� 
		MediumSpringGreen = RGB(0, 250, 154), // �д���ɫ 
		MediumTurquoise = RGB(72, 209, 204), // ���̱�ʯ 
		MediumVioletRed = RGB(199, 21, 133), // ���������� 
		MidnightBlue = RGB(25, 25, 112), // ��ҹ�� 
		MintCream = RGB(245, 255, 250), // �������� 
		MistyRose = RGB(255, 228, 225), // ����õ�� 
		Moccasin = RGB(255, 228, 181), // ¹Ƥѥ 
		NavajoWhite = RGB(255, 222, 173), // ������ 
		Navy = RGB(0, 0, 128), // ������ 
		OldLace = RGB(253, 245, 230), // ����˿ 
		Olive = RGB(128, 128, 0), // ��� 
		OliveDrab = RGB(107, 142, 35), // ��魺�ɫ 
		Orange = RGB(255, 165, 0), // ��ɫ 
		OrangeRed = RGB(255, 69, 0), // �Ⱥ�ɫ 
		Orchid = RGB(218, 112, 214), // ������ 
		PaleGoldenrod = RGB(238, 232, 170), // �Ҿջ� 
		PaleGreen = RGB(152, 251, 152), // ����ɫ 
		PaleTurquoise = RGB(175, 238, 238), // ���̱�ʯ 
		PaleVioletRed = RGB(219, 112, 147), // ���������� 
		PapayaWhip = RGB(255, 239, 213), // ��ľ�� 
		PeachPuff = RGB(255, 218, 185), // ����ɫ 
		Peru = RGB(205, 133, 63), // ��³ 
		Pink = RGB(255, 192, 203), // �ۺ� 
		Plum = RGB(221, 160, 221), // ������ 
		PowderBlue = RGB(176, 224, 230), // ��ҩ�� 
		Purple = RGB(128, 0, 128), // ��ɫ 
		Red = RGB(255, 0, 0), // ���� 
		RosyBrown = RGB(188, 143, 143), // õ����ɫ 
		RoyalBlue = RGB(65, 105, 225), // �ʼ��� (����) 
		SaddleBrown = RGB(139, 69, 19), // ����ɫ 
		Salmon = RGB(250, 128, 114), // ����(����)ɫ 
		SandyBrown = RGB(244, 164, 96), // ɳ��ɫ 
		SeaGreen = RGB(46, 139, 87), // ������ 
		SeaShell = RGB(255, 245, 238), // ������ 
		Sienna = RGB(160, 82, 45), // ������ɫ 
		Silver = RGB(192, 192, 192), // ����ɫ 
		SkyBlue = RGB(135, 206, 235), // ����ɫ 
		SlateBlue = RGB(106, 90, 205), // ������ 
		SlateGray = RGB(112, 128, 144), // ʯ��� 
		Snow = RGB(255, 250, 250), // ѩ 
		SpringGreen = RGB(0, 255, 127), // ����ɫ 
		SteelBlue = RGB(70, 130, 180), // ���� (����) 
		Tan = RGB(210, 180, 140), // ��ɫ 
		Teal = RGB(0, 128, 128), // ˮѼɫ 
		Thistle = RGB(216, 191, 216), // �� 
		Tomato = RGB(255, 99, 71), // ���Ѻ� 
		Turquoise = RGB(64, 224, 208), // �̱�ʯ 
		Violet = RGB(238, 130, 238), // ������ 
		Wheat = RGB(245, 222, 179), // С��ɫ 
		White = RGB(255, 255, 255), // ���� 
		WhiteSmoke = RGB(245, 245, 245), // ���� 
		Yellow = RGB(255, 255, 0), // ���� 
		YellowGreen = RGB(154, 205, 50) // ����ɫ 
	};

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


