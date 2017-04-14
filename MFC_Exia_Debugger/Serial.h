#pragma once
#include <string>
#include <afxtempl.h>

#define INPUT_BUF_SIZE	500
#define OUTPUT_BUF_SIZE	500

#define WM_SERIAL_UPDATE_LIST		WM_USER+1

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
	bool OpenSerial(std::string PortName);
	bool IsOpen();
	bool CloseSerial();
	bool ReceiveDate();

private:

	// owner window(receives message)
	CWnd*	m_pOwner;

	//Serial
	HANDLE m_hSerialPort;

	//Update
	HANDLE	m_hUpdateEvent;
	HKEY	m_hUpdateKey;
	HANDLE	m_hUpdateMutex;
	CWinThread	*m_pUpdateThread;

	bool UpdateSerial();
	//线程回调函数
	static UINT Update_Thread(void *args);
	static UINT Receive_Thread(void *args);
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

