#pragma once
#include "afxwin.h"


// CInputBox �Ի���

class CInputBox : public CDialogEx
{
	DECLARE_DYNAMIC(CInputBox)

public:
	CInputBox(CString Title = "�趨", CString Tip = "�����룺", CString DefaultValue = "0", CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CInputBox();

// �Ի�������
	enum { IDD = IDD_INPUTBOX };

private:
	CString m_str_Title;
	CString m_str_Input;
	CString m_str_Tip;
	CEdit m_Edit_Input;

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	CString GetInput();
	
	afx_msg void OnEnChangeInputboxEdit();
};
