
// MFC_Exia_DebuggerDlg.h : 头文件
//

#pragma once
#include "Serial.h"
#include "afxwin.h"

// CMFC_Exia_DebuggerDlg 对话框
class CMFC_Exia_DebuggerDlg : public CDialogEx
{
// 构造
public:
	CMFC_Exia_DebuggerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFC_EXIA_DEBUGGER_DIALOG };

private:
	CSerial m_Serial;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	// Combox:选择串口
	CComboBox m_Combox_COM;
protected:
	afx_msg LRESULT OnSerialUpdate(WPARAM wParam, LPARAM lParam);
};
