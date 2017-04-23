#pragma once
#include "Curve.h"

// CurveDLG �Ի���

class CurveDLG : public CDialogEx
{
	DECLARE_DYNAMIC(CurveDLG)

public:
	CurveDLG(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CurveDLG();

	CCurve m_Curve;

// �Ի�������
	enum { IDD = IDD_MFC_EXIA_CURVE_DIALOG };

protected:
	HICON m_hIcon;
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

private:
	POINT				m_ptMinTrackSize;		//���ڵ���С��С
	CRect				m_CurClientRect;		//���ڵĵ�ǰ��С
public:
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO* lpMMI);
	afx_msg BOOL OnQueryOpen();
	void UpdateCurve();
	virtual void PostNcDestroy();
};
