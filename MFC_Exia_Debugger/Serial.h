#pragma once
#include <string>
#include <afxtempl.h>

#define WM_SERIAL_UPDATE		WM_USER+1

typedef struct __ST_SERIAL_INFO__
{
	std::string str_Name;
	std::string str_Port;
}SerialInfo, * p_SerialInfo;


class CSerial
{
public:

	CArray<SerialInfo> m_SerialList;
	bool m_bNoSerial;

	CSerial();
	~CSerial();

	bool Init(CWnd* pOwner);
	LONG UpdateSerialList();

private:

	// owner window(receives message)
	CWnd*	m_pOwner;

	HANDLE	m_hUpdateEvent;
	HKEY	m_hUpdateKey;
	HANDLE	m_hUpdateMutex;
	CWinThread	*m_pUpdateThread;

	bool UpdateSerial();
	static UINT Update_thread(void *args);
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

