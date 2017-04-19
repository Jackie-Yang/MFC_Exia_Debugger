
// MFC_Exia_DebuggerDlg.h : ͷ�ļ�
//

#pragma once
#include "Serial.h"
#include "afxwin.h"

// CMFC_Exia_DebuggerDlg �Ի���
class CMFC_Exia_DebuggerDlg : public CDialogEx
{
// ����
public:
	CMFC_Exia_DebuggerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFC_EXIA_DEBUGGER_DIALOG };

// ��ʱ��ID
	enum { ID_TIMER_UPDATE_DATA = 1 };

private:
	CSerial m_Serial;
	UINT_PTR m_TimerID_Update_Data;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	
protected:
	afx_msg LRESULT OnSerialUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialClose(WPARAM wParam, LPARAM lParam);

public:

	COLORREF m_Color_Status;

	void UpdateSerialState();

	// Combox:ѡ�񴮿�
	CComboBox m_Combox_COM;
	//��ʾ����״̬
	CStatic m_Static_Status;

	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOpenCloseBtn();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
};
