#pragma once


// CCurve

#include "Color.h"


//设置建议：
//LENGTH_DEFAULT,LENGTH_MAX,LENGTH_MIN应该为GRID_STEP整数倍
//而且(LENGTH_MAX - LENGTH_MIN) / GRID_STEP应该为整数
//不满足以上建议会自动调整实际效果
#define X_GRID_STEP			100		//X轴每格数据长度,建议设置为100的倍数
#define Y_GRID_STEP			200		//Y轴每格数据长度，建议设置为100的倍数

#define X_LENGTH_DEFAULT	800		//默认X轴长度
#define X_LENGTH_MAX		1000	//X轴长度最大值
#define X_LENGTH_MIN		500		//X轴长度最小值

#define Y_LENGTH_DEFAULT	1000	//默认Y轴正半轴长度（负半轴相同）
#define Y_LENGTH_MAX		1000	//Y轴正半轴长度最大值
#define Y_LENGTH_MIN		400		//Y轴正半轴长度最小值


//默认风格
//Default Style

#define STYLE_BG		CCurve::Black
#define STYLE_GRID		PS_DOT, 1, CCurve::Teal
#define STYLE_AXIS		PS_SOLID, 1, CCurve::Magenta


//添加曲线的方法，CURVE_LINE设置成响应的曲线数目，添加颜色的宏定义，进行AddData时传入的数组指针大小应该和曲线数目一致，不然也会报错
//曲线设定
#define CURVE_LINE	4
//曲线设置的颜色需要与曲线定义的数量一致
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

	//普通画笔花刷
	CBrush m_CBrush_BG;
	CPen m_CPen_Grid;
	CPen m_CPen_Axis;
	CPen m_CPen_NormalCurve[CURVE_LINE];

	//用GDI画曲线的颜色
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
	void DrawAll(CDC * pDC);		//绘制所有元素
	void DrawBG(CDC *pDC);			//绘制背景
	void DrawGrid(CDC *pDC);		//绘制网格
	void DrawAxis(CDC *pDC);		//绘制坐标
	void DrawCurve(CDC *pDC);		//绘制曲线

	float *m_pDataBuf[CURVE_LINE];
	unsigned int m_nDataIndex;
	int m_nDataCount;
	int m_nDataMAX;
	int m_nMaxX;
	int m_nMaxY;
	int m_nOffset;
	

	//GDI+相关
	bool m_bEnhance;
	GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;

	//设置参数
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

	//主要是为了兼容GDI+中的颜色名字
	enum
	{
		AliceBlue = RGB(240, 248, 255), // 爱丽丝蓝 
		AntiqueWhite = RGB(250, 235, 215), // 古董白 
		Aqua = RGB(0, 255, 255), // 水色 
		Aquamarine = RGB(127, 255, 212), // 宝石碧绿 
		Azure = RGB(240, 255, 255), // 蔚蓝色 
		Beige = RGB(245, 245, 220), // 米色(灰棕色) 
		Bisque = RGB(255, 228, 196), // 陶坯黄 
		Black = RGB(0, 0, 0), // 纯黑 
		BlanchedAlmond = RGB(255, 235, 205), // 发白的杏仁色 
		Blue = RGB(0, 0, 255), // 纯蓝 
		BlueViolet = RGB(138, 43, 226), // 蓝紫罗兰 
		Brown = RGB(165, 42, 42), // 棕色 
		BurlyWood = RGB(222, 184, 135), // 硬木色 
		CadetBlue = RGB(95, 158, 160), // 军服蓝 
		Chartreuse = RGB(127, 255, 0), // 查特酒绿 (黄绿色) 
		Chocolate = RGB(210, 105, 30), // 巧克力 
		Coral = RGB(255, 127, 80), // 珊瑚 
		CornflowerBlue = RGB(100, 149, 237), // 矢车菊蓝 
		Cornsilk = RGB(255, 248, 220), // 玉米丝色 
		Crimson = RGB(220, 20, 60), // 猩红 (深红) 
		Cyan = RGB(0, 255, 255), // 青色 
		DarkBlue = RGB(0, 0, 139), // 暗蓝色 
		DarkCyan = RGB(0, 139, 139), // 暗青色 
		DarkGoldenrod = RGB(184, 134, 11), // 暗金菊黄 
		DarkGray = RGB(169, 169, 169), // 深灰色 
		DarkGreen = RGB(0, 100, 0), // 暗绿色 
		DarkKhaki = RGB(189, 183, 107), // 深卡叽布 
		DarkMagenta = RGB(139, 0, 139), // 深洋红 
		DarkOliveGreen = RGB(85, 107, 47), // 暗橄榄绿 
		DarkOrange = RGB(255, 140, 0), // 深橙色 
		DarkOrchid = RGB(153, 50, 204), // 暗兰花紫 
		DarkRed = RGB(139, 0, 0), // 深红色 
		DarkSalmon = RGB(233, 150, 122), // 深鲜肉(鲑鱼)色 
		DarkSeaGreen = RGB(143, 188, 143), // 暗海洋绿 
		DarkSlateBlue = RGB(72, 61, 139), // 暗板岩蓝 
		DarkSlateGray = RGB(47, 79, 79), // 暗石板灰 
		DarkTurquoise = RGB(0, 206, 209), // 暗绿宝石 
		DarkViolet = RGB(148, 0, 211), // 暗紫罗兰 
		DeepPink = RGB(255, 20, 147), // 深粉红 
		DeepSkyBlue = RGB(0, 191, 255), // 深天蓝 
		DimGray = RGB(105, 105, 105), // 暗淡的灰色 
		DodgerBlue = RGB(30, 144, 255), // 道奇蓝 
		Firebrick = RGB(178, 34, 34), // 耐火砖 
		FloralWhite = RGB(255, 250, 240), // 花的白色 
		ForestGreen = RGB(34, 139, 34), // 森林绿 
		Fuchsia = RGB(255, 0, 255), // 灯笼海棠(紫红色) 
		Gainsboro = RGB(220, 220, 220), // 庚斯博罗灰色 
		GhostWhite = RGB(248, 248, 255), // 幽灵白 
		Gold = RGB(255, 215, 0), // 金色 
		Goldenrod = RGB(218, 165, 32), // 金菊黄 
		Gray = RGB(186, 183, 186), // 灰色(*) 
		Green = RGB(0, 128, 0), // 纯绿 
		GreenYellow = RGB(173, 255, 47), // 绿黄色 
		Honeydew = RGB(240, 255, 240), // 蜜瓜色 
		HotPink = RGB(255, 105, 180), // 热情的粉红 
		IndianRed = RGB(205, 92, 92), // 印度红 
		Indigo = RGB(75, 0, 130), // 靛青 (紫兰色) 
		Ivory = RGB(255, 255, 240), // 象牙 
		Khaki = RGB(240, 230, 140), // 卡叽布 
		Lavender = RGB(230, 230, 250), // 熏衣草淡紫 
		LavenderBlush = RGB(255, 240, 245), // 淡紫红 
		LawnGreen = RGB(124, 252, 0), // 草坪绿 
		LemonChiffon = RGB(255, 250, 205), // 柠檬绸 
		LightBlue = RGB(173, 216, 230), // 亮蓝 
		LightCoral = RGB(240, 128, 128), // 淡珊瑚色 
		LightCyan = RGB(224, 255, 255), // 淡青色 
		LightGoldenrodYellow = RGB(250, 250, 210), // 亮菊黄 
		LightGray = RGB(144, 238, 144), // 淡绿色 
		LightGreen = RGB(211, 211, 211), // 浅灰色 
		LightPink = RGB(255, 182, 193), // 浅粉红 
		LightSalmon = RGB(255, 160, 122), // 浅鲑鱼肉色 
		LightSeaGreen = RGB(32, 178, 170), // 浅海洋绿 
		LightSkyBlue = RGB(135, 206, 250), // 亮天蓝色 
		LightSlateGray = RGB(119, 136, 153), // 亮石板灰 
		LightSteelBlue = RGB(176, 196, 222), // 亮钢蓝 
		LightYellow = RGB(255, 255, 224), // 浅黄色 
		Lime = RGB(0, 255, 0), // 闪光绿 
		LimeGreen = RGB(50, 205, 50), // 闪光深绿 
		Linen = RGB(250, 240, 230), // 亚麻布 
		Magenta = RGB(255, 0, 255), // 洋红 (品红 玫瑰红) 
		Maroon = RGB(128, 0, 0), // 栗色 
		MediumAquamarine = RGB(102, 205, 170), // 中宝石碧绿 
		MediumBlue = RGB(0, 0, 205), // 中蓝色 
		MediumOrchid = RGB(186, 85, 211), // 中兰花紫 
		MediumPurple = RGB(147, 112, 219), // 中紫色 
		MediumSeaGreen = RGB(60, 179, 113), // 中海洋绿 
		MediumSlateBlue = RGB(123, 104, 238), // 中板岩蓝 
		MediumSpringGreen = RGB(0, 250, 154), // 中春绿色 
		MediumTurquoise = RGB(72, 209, 204), // 中绿宝石 
		MediumVioletRed = RGB(199, 21, 133), // 中紫罗兰红 
		MidnightBlue = RGB(25, 25, 112), // 午夜蓝 
		MintCream = RGB(245, 255, 250), // 薄荷奶油 
		MistyRose = RGB(255, 228, 225), // 薄雾玫瑰 
		Moccasin = RGB(255, 228, 181), // 鹿皮靴 
		NavajoWhite = RGB(255, 222, 173), // 土著白 
		Navy = RGB(0, 0, 128), // 海军蓝 
		OldLace = RGB(253, 245, 230), // 旧蕾丝 
		Olive = RGB(128, 128, 0), // 橄榄 
		OliveDrab = RGB(107, 142, 35), // 橄榄褐色 
		Orange = RGB(255, 165, 0), // 橙色 
		OrangeRed = RGB(255, 69, 0), // 橙红色 
		Orchid = RGB(218, 112, 214), // 兰花紫 
		PaleGoldenrod = RGB(238, 232, 170), // 灰菊黄 
		PaleGreen = RGB(152, 251, 152), // 弱绿色 
		PaleTurquoise = RGB(175, 238, 238), // 弱绿宝石 
		PaleVioletRed = RGB(219, 112, 147), // 弱紫罗兰红 
		PapayaWhip = RGB(255, 239, 213), // 番木瓜 
		PeachPuff = RGB(255, 218, 185), // 桃肉色 
		Peru = RGB(205, 133, 63), // 秘鲁 
		Pink = RGB(255, 192, 203), // 粉红 
		Plum = RGB(221, 160, 221), // 李子紫 
		PowderBlue = RGB(176, 224, 230), // 火药青 
		Purple = RGB(128, 0, 128), // 紫色 
		Red = RGB(255, 0, 0), // 纯红 
		RosyBrown = RGB(188, 143, 143), // 玫瑰棕色 
		RoyalBlue = RGB(65, 105, 225), // 皇家蓝 (宝蓝) 
		SaddleBrown = RGB(139, 69, 19), // 马鞍棕色 
		Salmon = RGB(250, 128, 114), // 鲜肉(鲑鱼)色 
		SandyBrown = RGB(244, 164, 96), // 沙棕色 
		SeaGreen = RGB(46, 139, 87), // 海洋绿 
		SeaShell = RGB(255, 245, 238), // 海贝壳 
		Sienna = RGB(160, 82, 45), // 黄土赭色 
		Silver = RGB(192, 192, 192), // 银灰色 
		SkyBlue = RGB(135, 206, 235), // 天蓝色 
		SlateBlue = RGB(106, 90, 205), // 板岩蓝 
		SlateGray = RGB(112, 128, 144), // 石板灰 
		Snow = RGB(255, 250, 250), // 雪 
		SpringGreen = RGB(0, 255, 127), // 春绿色 
		SteelBlue = RGB(70, 130, 180), // 钢蓝 (铁青) 
		Tan = RGB(210, 180, 140), // 茶色 
		Teal = RGB(0, 128, 128), // 水鸭色 
		Thistle = RGB(216, 191, 216), // 蓟 
		Tomato = RGB(255, 99, 71), // 番茄红 
		Turquoise = RGB(64, 224, 208), // 绿宝石 
		Violet = RGB(238, 130, 238), // 紫罗兰 
		Wheat = RGB(245, 222, 179), // 小麦色 
		White = RGB(255, 255, 255), // 纯白 
		WhiteSmoke = RGB(245, 245, 245), // 白烟 
		Yellow = RGB(255, 255, 0), // 纯黄 
		YellowGreen = RGB(154, 205, 50) // 黄绿色 
	};

	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
};


