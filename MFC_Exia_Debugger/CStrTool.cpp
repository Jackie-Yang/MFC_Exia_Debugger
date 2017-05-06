#include "stdafx.h"
#include "CStrTool.h"

CStringA CStrT2CStrA(const CString &CStrT)
{
#ifdef _UNICODE  
	return CStrW2CStrA(CStrT);
#else  
	return CStrT;
#endif  
}

CStringW CStrT2CStrW(const CString &CStrT)
{
#ifdef _UNICODE  
	return CStrT;
#else  
	return CStrA2CStrW(CStrT);
#endif  
}

CString CStrA2CStrT(const CStringA &CStrA)
{
#ifdef _UNICODE  
	return CStrA2CStrW(CStrA);
#else  
	return CStrA;
#endif  
}

CString CStrW2CStrT(const CStringW &CStrW)
{
#ifdef _UNICODE  
	return CStrW;
#else  
	return CStrW2CStrA(CStrW);
#endif  
}


CStringA CStrW2CStrA(const CStringW &CStrW)
{
	int len = WideCharToMultiByte(CP_ACP, 0, CStrW, -1, NULL, 0, NULL, NULL);
	char *StrA = new char[len];
	memset(StrA, 0, len * sizeof(char));
	WideCharToMultiByte(CP_ACP, 0, CStrW, -1, StrA, len, NULL, NULL);
	CStringA CStrA = StrA;
	delete[] StrA;
	return CStrA;
}

CStringW CStrA2CStrW(const CStringA &CStrA)
{
	int len = MultiByteToWideChar(CP_ACP, 0, CStrA, -1, NULL, 0);
	wchar_t *StrW = new wchar_t[len];
	memset(StrW, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, CStrA, -1, StrW, len);
	CStringW CStrW = StrW;
	delete[] StrW;
	return CStrW;
}