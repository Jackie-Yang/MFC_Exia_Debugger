// InputBox.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "InputBox.h"
#include "afxdialogex.h"


// CInputBox �Ի���

IMPLEMENT_DYNAMIC(CInputBox, CDialogEx)

CInputBox::CInputBox(CString Title /*= "�趨"*/, CString Tip /*= "�����룺"*/, CString DefaultValue /*= "0"*/, CWnd* pParent /*=NULL*/)
	: CDialogEx(CInputBox::IDD, pParent)
	, m_str_Input(_T(DefaultValue))
	, m_str_Title(_T(Title))
	, m_str_Tip(_T(Tip))
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
		if (pMsg->wParam != '.' && pMsg->wParam != '\b' && pMsg->wParam != '-' && (pMsg->wParam>'9' || pMsg->wParam<'0'))
		{
			return TRUE;
		}

		CString str;
		int nSelStart = 0, nSelEnd = 0;
		m_Edit_Input.GetSel(nSelStart, nSelEnd);	//��ȡѡ���ı�
		GetDlgItemText(IDC_INPUTBOX_EDIT, str);			// ��ȡedit���ı�
		str.Delete(nSelStart, nSelEnd - nSelStart);		//ɾ��ѡ�е��ı�(��Ϊѡ���ı��ᱻ������ַ�����)

		//ֻ��������һ��С����
		if (pMsg->wParam == '.')
		{
			int nPos = 0;
			if (!nSelStart)	//��һ���ַ���������С����
			{
				return TRUE;
			}
			nPos = str.Find('.'); // ����.��λ��
			if (nPos >= 0)
			{
				return TRUE; // �������. ���أ�����������.����
			}
		}
		else if (pMsg->wParam == '-')	//����ֻ���ڵ�һ������
		{
			if (nSelStart)
			{
				return TRUE;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);
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
	char data;
	unsigned char DotCount = 0;
	GetDlgItemText(IDC_INPUTBOX_EDIT, str);
	for (int i = 0; i < str.GetLength(); i++)
	{
		data = str.GetAt(i);
		if (data != '.' && data != '-' && (data>'9' || data<'0'))
		{
			SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//�ָ��ɵ�����
			m_Edit_Input.SetSel(0, -1);
			MessageBoxA("���벻�Ϸ���", "����", MB_ICONERROR | MB_OK);
			return;
		}

		//ֻ��������һ��С����
		if (data == '.')
		{
			if (!i)	//��һ���ַ���������С����
			{
				SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//�ָ��ɵ�����
				m_Edit_Input.SetSel(0, -1);
				MessageBoxA("���벻�Ϸ���", "����", MB_ICONERROR | MB_OK);
				return;
			}
			if (DotCount++)
			{
				SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//�ָ��ɵ�����
				m_Edit_Input.SetSel(0, -1);
				MessageBoxA("���벻�Ϸ���", "����", MB_ICONERROR | MB_OK);
				return;
			}
		}
		else if (data == '-')	//����ֻ���ڵ�һ������
		{
			if (i)
			{
				SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//�ָ��ɵ�����
				m_Edit_Input.SetSel(0, -1);
				MessageBoxA("���벻�Ϸ���", "����", MB_ICONERROR | MB_OK);
				return;
			}
		}
	}
	m_str_Input = str;	//����Ϸ��ű��浱ǰ�ַ���
}

CString CInputBox::GetInput()
{
	return m_str_Input;
}


