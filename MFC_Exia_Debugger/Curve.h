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


	CRect m_RectGrid;
	float m_GridStepX;
	float m_GridStepY;

	bool RegisterWindowClass();
	void DrawAll(CDC * pDC, CRect rect);
	void DrawBG(CDC *pDC, CRect rect);
	void DrawGrid(CDC *pDC, CRect rect);
	void DrawAxis(CDC *pDC, CRect rect);
	void DrawCurve(CDC *pDC, CRect rect);

	float *m_pDataBuf;
	unsigned int m_nDataIndex;
	unsigned int m_nDataCount;
	unsigned int m_nBufSize;
	int m_nOffset;

public:
	afx_msg void OnPaint();
	
	void Init(float fScaleX, float fScaleY, int nGridX, int nGridY);

	void AddData(float fData);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);

	
	
};


