
// MFC_Exia_Debugger.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CMFC_Exia_DebuggerApp: 
// �йش����ʵ�֣������ MFC_Exia_Debugger.cpp
//

class CMFC_Exia_DebuggerApp : public CWinApp
{
public:
	CMFC_Exia_DebuggerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CMFC_Exia_DebuggerApp theApp;