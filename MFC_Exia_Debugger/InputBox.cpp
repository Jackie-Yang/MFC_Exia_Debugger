// InputBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "InputBox.h"
#include "afxdialogex.h"


// CInputBox �Ի���

IMPLEMENT_DYNAMIC(CInputBox, CDialogEx)

CInputBox::CInputBox(CString Title /*= _T("�趨")*/, CString Tip /*= _T("�����룺")*/, CString DefaultValue /*= _T("0")*/, CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputBox::IDD, pParent)
	, m_str_Input(DefaultValue)
	, m_str_Title(Title)
	, m_str_Tip(Tip)
{

}

CInputBox::~CInputBox()
{
}

void CInputBox::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_INPUTBOX_TIP, m_str_Tip);
	DDX_Control(pDX, IDC_INPUTBOX_EDIT, m_Edit_Input);
}


BEGIN_MESSAGE_MAP(CInputBox, CDialogEx)
	ON_EN_CHANGE(IDC_INPUTBOX_EDIT, &CInputBox::OnEnChangeInputboxEdit)
END_MESSAGE_MAP()


// CInputBox ��Ϣ�������

//��ʼ��
BOOL CInputBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	SetWindowText(m_str_Title);
	SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);
	m_Edit_Input.SetFocus();					//���ý���
	m_Edit_Input.SetSel(0, -1);		//ȫѡ����
	return FALSE;

	//return TRUE;  // return TRUE unless you set the focus to a control
	// �쳣:  OCX ����ҳӦ���� FALSE
}

//���������ַ�
BOOL CInputBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  �ڴ����ר�ô����/����û���

	if (pMsg->message == WM_CHAR && pMsg->hwnd == m_Edit_Input.m_hWnd)
	{
		if (::GetKeyState(VK_CONTROL) & 0x80)	//����Ctrlʱ�������ַ�����
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
		//ֻ�����������֡�С���㡢�˸񡢷���
		if (pMsg->wParam != _T('.') && pMsg->wParam != _T('\b') && pMsg->wParam != _T('-') && (pMsg->wParam > _T('9') || pMsg->wParam < _T('0')))
		{
			goto INPUT_ERROR;
		}

		CString str;
		int nSelStart = 0, nSelEnd = 0;
		m_Edit_Input.GetSel(nSelStart, nSelEnd);	//��ȡѡ���ı�
		GetDlgItemText(IDC_INPUTBOX_EDIT, str);			// ��ȡedit���ı�
		str.Delete(nSelStart, nSelEnd - nSelStart);		//ɾ��ѡ�е��ı�(��Ϊѡ���ı��ᱻ������ַ�����)

		//ֻ��������һ��С����
		if (pMsg->wParam == _T('.'))
		{
			int nPos = 0;
			if (!nSelStart)	//��һ���ַ���������С����
			{
				goto INPUT_ERROR;
			}
			if (nSelStart == 1 && str.GetAt(0) == _T('-'))
			{
				goto INPUT_ERROR;	//��һ���ַ�Ϊ"-"�ڶ����ַ���������С����
			}
			nPos = str.Find(_T('.')); // ����.��λ��
			if (nPos >= 0)
			{
				goto INPUT_ERROR; // �������. ���أ�����������.����
			}
		}
		else if (pMsg->wParam == _T('-'))	//����ֻ���ڵ�һ������
		{
			if (nSelStart)
			{
				goto INPUT_ERROR;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);

INPUT_ERROR:
	ShowBalloonTip(&m_Edit_Input, L"����", L"���벻�Ϸ���", TTI_ERROR);
	return TRUE;
}

//���������˼��̵Ĳ��Ϸ����룬������Ҫ���ж�ճ���Ĳ��Ϸ�
void CInputBox::OnEnChangeInputboxEdit()
{
	// TODO:  ����ÿؼ��� RICHEDIT �ؼ���������
	// ���ʹ�֪ͨ��������д CDialogEx::OnInitDialog()
	// ���������� CRichEditCtrl().SetEventMask()��
	// ͬʱ�� ENM_CHANGE ��־�������㵽�����С�
	// TODO:  �ڴ���ӿؼ�֪ͨ����������

	CString str;
	TCHAR data;
	unsigned char DotCount = 0;
	GetDlgItemText(IDC_INPUTBOX_EDIT, str);
	for (int i = 0; i < str.GetLength(); i++)
	{
		data = str.GetAt(i);
		if (data != _T('.') && data != _T('-') && (data > _T('9') || data < _T('0')))
		{
			goto INPUT_ERROR;
		}

		//ֻ��������һ��С����
		if (data == '.')
		{
			if (!i)	//��һ���ַ���������С����
			{
				goto INPUT_ERROR;
			}
			if (i == 1 && str.GetAt(0) == _T('-'))
			{
				goto INPUT_ERROR;
			}
			if (DotCount++)
			{
				goto INPUT_ERROR;
			}
		}
		else if (data == _T('-'))	//����ֻ���ڵ�һ������
		{
			if (i)
			{
				goto INPUT_ERROR;
			}
		}
	}
	m_str_Input = str;	//����Ϸ��ű��浱ǰ�ַ���
	return;
INPUT_ERROR:
	SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//�ָ��ɵ�����
	m_Edit_Input.SetSel(0, -1);
	ShowBalloonTip(&m_Edit_Input, L"����", L"���벻�Ϸ���", TTI_ERROR);
	//MessageBox(_T("���벻�Ϸ���"), _T("����"), MB_ICONERROR | MB_OK);
}

CString CInputBox::GetInput()
{
	return m_str_Input;
}

//��ʾ���ݣ������ִ���Ҫר��Unicode�ģ����ַ���ǰ����L����
AFX_INLINE BOOL CInputBox::ShowBalloonTip(CEdit * pEdit, _In_z_ LPCWSTR lpszTitle, _In_z_ LPCWSTR lpszText, _In_ INT ttiIcon)
{
	ASSERT(pEdit != NULL);
	if (pEdit == NULL)
	{
		return FALSE;
	}
	ASSERT(::IsWindow(pEdit->m_hWnd));
	EDITBALLOONTIP bt;
	::ZeroMemory(&bt, sizeof(EDITBALLOONTIP));

	bt.cbStruct = sizeof(EDITBALLOONTIP);
	bt.pszTitle = lpszTitle;
	bt.pszText = lpszText;
	bt.ttiIcon = ttiIcon;

	return Edit_ShowBalloonTip(pEdit->m_hWnd, &bt); // EM_SHOWBALLOONTIP
}
