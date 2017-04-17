#pragma once
#include <string>
#include <afxtempl.h>

#define SEND_TEST
#define RECEIVE_TEST

#ifdef RECEIVE_TEST
#include <io.h>    
#include <fcntl.h>  
#endif

#define DMA_BUFF_SIZE	36
#define INPUT_BUF_SIZE	500
#define OUTPUT_BUF_SIZE	500

#define WM_SERIAL_UPDATE_LIST		WM_USER+1

typedef struct __ST_SERIAL_INFO__
{
	std::string str_Name;
	std::string str_Port;
	HANDLE h_Handle;
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
	bool OpenSerial(p_SerialInfo pSerialInfo, DWORD dwBaudRate);
	bool IsOpen();
	bool CloseSerial();
	DWORD SendData(const char *pData, DWORD nDataLength);

private:

	// owner window(receives message)
	CWnd*	m_pOwner;

	//Serial
	p_SerialInfo m_pSerialPort;

	//Update
	HANDLE	m_hUpdateEvent;
	HKEY	m_hUpdateKey;
	HANDLE	m_hUpdateMutex;
	CWinThread	*m_pUpdateThread;
	CWinThread	*m_pReceiveThread;
	UINT ReceiveData();
#ifdef SEND_TEST
	CWinThread	*m_pSendThread;
#endif

	bool UpdateSerial();
	//线程回调函数
	static UINT Update_Thread(void *args);
	static UINT Receive_Thread(void *args);
#ifdef SEND_TEST
	static UINT Send_Thread(void *args);
#endif
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

