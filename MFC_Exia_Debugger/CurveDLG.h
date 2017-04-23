#pragma once
#include "Curve.h"

// CurveDLG 对话框

class CurveDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CurveDLG)

public:
	CurveDLG(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CurveDLG();

	CCurve m_Curve;

// 对话框数据
	enum { IDD = IDD_MFC_EXIA_CURVE_DIALOG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

private:
	POINT				m_ptMinTrackSize;		//窗口的最小大小
	CRect				m_CurClientRect;		//窗口的当前大小
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnQueryOpen();
	void UpdateCurve();
	virtual void PostNcDestroy();
};
