
// MFC_Exia_DebuggerDlg.h : ͷ�ļ�
//

#pragma once


// CMFC_Exia_DebuggerDlg �Ի���
class CMFC_Exia_DebuggerDlg : public CDialogEx
{
// ����
public:
	CMFC_Exia_DebuggerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFC_EXIA_DEBUGGER_DIALOG };

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
};
