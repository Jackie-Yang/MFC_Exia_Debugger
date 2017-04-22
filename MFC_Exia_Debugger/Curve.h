#pragma once


// CCurve

class CCurve : public CWnd
{
	DECLARE_DYNAMIC(CCurve)

public:
	CCurve();
	virtual ~CCurve();

protected:
	DECLARE_MESSAGE_MAP()

private:

	CBrush m_Brush_BG;
	CPen m_Pen_Grid;
	CPen m_Pen_Curve;
	CPen m_Pen_Axis;

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
	void DrawAll(CDC * pDC);
	void DrawBG(CDC *pDC);
	void DrawGrid(CDC *pDC);
	void DrawAxis(CDC *pDC);
	void DrawCurve(CDC *pDC);

	float *m_pDataBuf;
	unsigned int m_nDataIndex;
	unsigned int m_nDataCount;
	unsigned int m_nDataMAX;
	unsigned int m_nMaxX;
	unsigned int m_nMaxY;
	int m_nOffset;

public:
	afx_msg void OnPaint();
	
	void Init(unsigned int nMaxX, unsigned int nMaxY, unsigned int nDataMAX, float fScaleX, float fScaleY, int nGridX, int nGridY);

	void AddData(float fData);
	void ClearData();
	void Update();

	bool m_bAntialias;
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	
	afx_msg void OnSize(UINT nType, int cx, int cy);
};


