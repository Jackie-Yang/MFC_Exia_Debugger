#pragma once
#include "afxwin.h"


// CInputBox 对话框

class CInputBox : public CDialogEx
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CString Title = _T("设定"), CString Tip = _T("请输入："), CString DefaultValue = _T("0"), CString MinValue = _T(""), CString MaxValue = _T(""), CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CInputBox();

// 对话框数据
	enum { IDD = IDD_INPUTBOX };

private:
	CString m_str_Title;
	CString m_str_Input;
	CString m_str_Min;
	CString m_str_Max;
	CString m_str_Tip;
	CEdit m_Edit_Input;
	AFX_INLINE BOOL CInputBox::ShowBalloonTip(CEdit * pEdit, _In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString GetInput();
	
	afx_msg void OnEnChangeInputboxEdit();
	virtual void OnOK();
};
