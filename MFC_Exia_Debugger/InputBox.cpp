// InputBox.cpp : 实现文件
//

#include "stdafx.h"
#include "MFC_Exia_Debugger.h"
#include "InputBox.h"
#include "afxdialogex.h"


// CInputBox 对话框

IMPLEMENT_DYNAMIC(CInputBox, CDialogEx)

CInputBox::CInputBox(CString Title /*= _T("设定")*/, CString Tip /*= _T("请输入：")*/, CString DefaultValue /*= _T("0")*/, CWnd* pParent /*=NULL*/)
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


// CInputBox 消息处理程序

//初始化
BOOL CInputBox::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	SetWindowText(m_str_Title);
	SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);
	m_Edit_Input.SetFocus();					//设置焦点
	m_Edit_Input.SetSel(0, -1);		//全选文字
	return FALSE;

	//return TRUE;  // return TRUE unless you set the focus to a control
	// 异常:  OCX 属性页应返回 FALSE
}

//限制输入字符
BOOL CInputBox::PreTranslateMessage(MSG* pMsg)
{
	// TODO:  在此添加专用代码和/或调用基类

	if (pMsg->message == WM_CHAR && pMsg->hwnd == m_Edit_Input.m_hWnd)
	{
		if (::GetKeyState(VK_CONTROL) & 0x80)	//按下Ctrl时不属于字符输入
		{
			return CDialogEx::PreTranslateMessage(pMsg);
		}
		//只允许输入数字、小数点、退格、符号
		if (pMsg->wParam != _T('.') && pMsg->wParam != _T('\b') && pMsg->wParam != _T('-') && (pMsg->wParam > _T('9') || pMsg->wParam < _T('0')))
		{
			goto INPUT_ERROR;
		}

		CString str;
		int nSelStart = 0, nSelEnd = 0;
		m_Edit_Input.GetSel(nSelStart, nSelEnd);	//获取选中文本
		GetDlgItemText(IDC_INPUTBOX_EDIT, str);			// 获取edit中文本
		str.Delete(nSelStart, nSelEnd - nSelStart);		//删除选中的文本(因为选中文本会被输入的字符覆盖)

		//只允许输入一个小数点
		if (pMsg->wParam == _T('.'))
		{
			int nPos = 0;
			if (!nSelStart)	//第一个字符不能输入小数点
			{
				goto INPUT_ERROR;
			}
			if (nSelStart == 1 && str.GetAt(0) == _T('-'))
			{
				goto INPUT_ERROR;	//第一个字符为"-"第二个字符不能输入小数点
			}
			nPos = str.Find(_T('.')); // 查找.的位置
			if (nPos >= 0)
			{
				goto INPUT_ERROR; // 如果存在. 返回，即不再允许.输入
			}
		}
		else if (pMsg->wParam == _T('-'))	//符号只能在第一个输入
		{
			if (nSelStart)
			{
				goto INPUT_ERROR;
			}
		}
	}
	return CDialogEx::PreTranslateMessage(pMsg);

INPUT_ERROR:
	ShowBalloonTip(&m_Edit_Input, L"错误", L"输入不合法！", TTI_ERROR);
	return TRUE;
}

//上面屏蔽了键盘的不合法输入，这里主要是判断粘贴的不合法
void CInputBox::OnEnChangeInputboxEdit()
{
	// TODO:  如果该控件是 RICHEDIT 控件，它将不
	// 发送此通知，除非重写 CDialogEx::OnInitDialog()
	// 函数并调用 CRichEditCtrl().SetEventMask()，
	// 同时将 ENM_CHANGE 标志“或”运算到掩码中。
	// TODO:  在此添加控件通知处理程序代码

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

		//只允许输入一个小数点
		if (data == '.')
		{
			if (!i)	//第一个字符不能输入小数点
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
		else if (data == _T('-'))	//符号只能在第一个输入
		{
			if (i)
			{
				goto INPUT_ERROR;
			}
		}
	}
	m_str_Input = str;	//输入合法才保存当前字符串
	return;
INPUT_ERROR:
	SetDlgItemText(IDC_INPUTBOX_EDIT, m_str_Input);	//恢复旧的数据
	m_Edit_Input.SetSel(0, -1);
	ShowBalloonTip(&m_Edit_Input, L"错误", L"输入不合法！", TTI_ERROR);
	//MessageBox(_T("输入不合法！"), _T("错误"), MB_ICONERROR | MB_OK);
}

CString CInputBox::GetInput()
{
	return m_str_Input;
}

//显示气泡，传入字串需要专程Unicode的，在字符串前加上L即可
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
