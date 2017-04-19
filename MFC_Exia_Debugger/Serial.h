#pragma once
#include <string>
#include "basetsd.h"
#include <afxtempl.h>

#define SEND_TEST		FALSE
#define RECEIVE_TEST	FALSE

#if RECEIVE_TEST
#include <io.h>    
#include <fcntl.h>  
#endif

#define	BUF_SIZE		1024
#define INPUT_BUF_SIZE	BUF_SIZE
#define OUTPUT_BUF_SIZE	BUF_SIZE

#define WM_SERIAL_UPDATE_LIST		WM_USER+1
#define WM_SERIAL_OPEN				WM_USER+2
#define WM_SERIAL_CLOSE				WM_USER+3

typedef struct __ST_SERIAL_INFO__
{
	std::string str_Name;
	std::string str_Port;
	HANDLE h_Handle;
}SerialInfo, * p_SerialInfo;

typedef struct __CycBuf__
{
	UINT32	nWriteIndex;
	UINT32	nReadIndex;
	UINT32	nByteToRead;
	HANDLE	hMutex;
	UINT8	pData[BUF_SIZE];
}CycBuf, *p_CycBuf;

class CSerial
{
public:

	

	CSerial();
	~CSerial();

	bool Init(CWnd* pOwner);
	bool UpdateSerialList();
	INT_PTR GetSerialNum();
	p_SerialInfo GetCurSerial();
	p_SerialInfo GetSerialInfo(INT_PTR nIndex);
	bool OpenSerial(p_SerialInfo pSerialInfo, DWORD dwBaudRate);
	bool IsOpen();
	bool IsWatching();
	bool CheckSerialState();
	bool CloseSerial();
	DWORD SendData(const char *pData, DWORD nDataLength);

	bool StartWatchingSerialList();
	void StopWatchingSerialList();

	UINT32 BufWriteData(p_CycBuf pBuf, const PUINT8 pData, UINT32 nSize);
	UINT32 BufGetData(p_CycBuf pBuf, PUINT8 pData, UINT32 nSize);
	void BufClear(p_CycBuf pBuf);

private:

	// owner window(receives message)
	CWnd*	m_pOwner;

	//Serial
	p_SerialInfo m_pSerialPort;
	CArray<SerialInfo> m_SerialList;

	CycBuf m_SerialRecData;

	CWinThread	*m_pReceiveThread;
	UINT ReceiveData();


	//Watching Thread
	HANDLE	m_hWatchingEvent;
	HKEY	m_hWatchingKey;
	HANDLE	m_hWatchingMutex;
	CWinThread	*m_pWatchingThread;
	bool WatchingSerial();
	//线程回调函数
	static UINT Watching_Thread(void *args);
	bool m_bNoSerial;
	bool m_bIsWatching;

	
#if SEND_TEST
	CWinThread	*m_pSendThread;
#endif

	

	//线程回调函数
	static UINT Receive_Thread(void *args);
#if SEND_TEST
	static UINT Send_Thread(void *args);
#endif

	//用于串口列表排序
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

