#pragma once
#include <string>
#include <afxtempl.h>

typedef struct __ST_SERIAL_INFO__
{
	std::string str_Name;
	std::string str_Port;
}SerialInfo, * p_SerialInfo;


class CSerial
{
public:

	CArray<SerialInfo> m_SerialList;


	CSerial();
	~CSerial();

	bool Init();
	LONG UpdateSerialList();

private:

	HANDLE m_hUpdateEvent;
	HKEY   m_hUpdateKey;
	CWinThread *m_pUpdateThread;

	bool UpdateSerial();
	static UINT Update_thread(void *args);
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

