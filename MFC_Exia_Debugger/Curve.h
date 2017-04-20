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
	bool RegisterWindowClass();
public:
	afx_msg void OnPaint();
};


