#include "stdafx.h"
#include "Serial.h"
#include <algorithm>



CSerial::CSerial()
{
	m_hUpdateEvent = NULL;
	m_hUpdateEvent = NULL;
	m_pUpdateThread = NULL;
	m_pReceiveThread = NULL;
#ifdef SEND_TEST
	m_pSendThread = NULL;
#endif
	m_pOwner = NULL;
	m_bNoSerial = true;
	m_hUpdateMutex = NULL;
	m_pSerialPort = NULL;
}


CSerial::~CSerial()
{
	if (IsOpen())
	{
		CloseSerial();
	}

	m_pOwner = NULL;
	WaitForSingleObject(m_hUpdateMutex, INFINITE);
	if (m_hUpdateEvent)
	{
		if (CloseHandle(m_hUpdateEvent))
		{
			m_hUpdateEvent = NULL;
		}
	}
	if (m_hUpdateKey)
	{
		if (RegCloseKey(m_hUpdateKey) == ERROR_SUCCESS)
		{
			m_hUpdateKey = NULL;
		}
	}	
	ReleaseMutex(m_hUpdateMutex);
	//等待监听线程结束
	if (m_pUpdateThread)
	{
		WaitForSingleObject(m_pUpdateThread->m_hThread, INFINITE);
		m_pUpdateThread = NULL;
	}
	if (m_hUpdateMutex)
	{
		CloseHandle(m_hUpdateMutex);
		m_hUpdateMutex = NULL;
	}
}

bool CSerial::Init(CWnd* pOwner)
{
	LONG   Ret;
	m_pOwner = pOwner;

	// Open a key.
	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_NOTIFY, &m_hUpdateKey);
	if (Ret != ERROR_SUCCESS)
	{
		if (Ret == ERROR_FILE_NOT_FOUND)
		{
			//无串口，监视Device
			Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP", 0, KEY_NOTIFY, &m_hUpdateKey);
			if (Ret != ERROR_SUCCESS)
			{
				goto FAIL_PROCESS;
			}
		}
		else
		{
			goto FAIL_PROCESS;
		}
	}
	else
	{
		m_bNoSerial = false;
		//Get List
		UpdateSerialList();
	}

	// Create an event.
	m_hUpdateEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hUpdateEvent == NULL)
	{
		goto FAIL_PROCESS;
	}

	//创建线程互斥锁
	m_hUpdateMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_hUpdateMutex == NULL)
	{
		goto FAIL_PROCESS;
	}

	//创建子线程监视串口列表（创建的时候立即运行）  
	m_pUpdateThread = AfxBeginThread(Update_Thread, this);
	if (NULL == m_pUpdateThread)
	{
		goto FAIL_PROCESS;
	}

	//测试数据接收，打开终端
#ifdef RECEIVE_TEST
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;
#endif

	return TRUE;

FAIL_PROCESS:
	if (m_hUpdateEvent)
	{
		if (CloseHandle(m_hUpdateEvent))
		{
			m_hUpdateEvent = NULL;
		}
	}
	if (m_hUpdateKey)
	{
		if (RegCloseKey(m_hUpdateKey) == ERROR_SUCCESS)
		{
			m_hUpdateKey = NULL;
		}
	}
	return false;
}

UINT CSerial::Update_Thread(void *args)
{
	CSerial *pCSerial = (CSerial *)args;
	BOOL ret = pCSerial->UpdateSerial();
	return ret;
}

//监听串口
bool CSerial::UpdateSerial()
{
	DWORD  dwFilter = REG_NOTIFY_CHANGE_NAME |
		REG_NOTIFY_CHANGE_ATTRIBUTES |
		REG_NOTIFY_CHANGE_LAST_SET |
		REG_NOTIFY_CHANGE_SECURITY;
	LONG   Ret;
	// Watch the registry key for a change of value.
	while (true)
	{
		WaitForSingleObject(m_hUpdateMutex, INFINITE);
		if (!(m_hUpdateKey && m_hUpdateEvent))
		{
			break;
		}
		Ret = RegNotifyChangeKeyValue(m_hUpdateKey,
			TRUE,
			dwFilter,
			m_hUpdateEvent,
			TRUE);
		if (Ret != ERROR_SUCCESS)
		{
			continue;
		}
		if (WaitForSingleObject(m_hUpdateEvent, 100) == WAIT_OBJECT_0)	//使用了互斥锁，因此这里等待需要设置超时，否则会造成死锁
		{
			if (m_bNoSerial)	//无串口，尝试监听串口
			{
				HKEY   TempKey;
				Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_NOTIFY, &TempKey);
				if (Ret == ERROR_SUCCESS)
				{
					if (RegCloseKey(m_hUpdateKey) == ERROR_SUCCESS)		//有串口，开始监听串口
					{
						m_hUpdateKey = TempKey;
						m_bNoSerial = false;
						Sleep(500);
						UpdateSerialList();
					}
					else
					{
						RegCloseKey(TempKey);
						TempKey = NULL;
					}
				}
			}
			else
			{
				UpdateSerialList();
			}
		}
		ResetEvent(m_hUpdateEvent);	//复位事件
		ReleaseMutex(m_hUpdateMutex);
	}
	
	return 0;
}


LONG CSerial::UpdateSerialList()
{
	LONG Ret;
	HKEY hKey;
	DWORD  MaxValueNameLength;
	DWORD  MaxValueLength;
	DWORD  dwValueNumber;
	LPSTR  ValueName, Value;
	SerialInfo stSerialInfo;

	//Get Key Handle
	Ret = RegOpenKeyExA(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_QUERY_VALUE, &hKey);

	if (ERROR_SUCCESS != Ret)
	{
		return Ret;
	}

	//Get Value Num And Max Length
	Ret = RegQueryInfoKeyA(hKey,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&dwValueNumber,
		&MaxValueNameLength,
		&MaxValueLength,
		NULL,
		NULL);

	if (ERROR_SUCCESS != Ret) 
	{
		RegCloseKey(hKey);
		return Ret;
	}

	if (m_SerialList.GetSize() > 0)
	{
		m_SerialList.RemoveAll();
	}

	MaxValueNameLength = MaxValueNameLength + 1;
	MaxValueLength = MaxValueLength + 1;

	ValueName = (LPSTR)new char[MaxValueNameLength];
	Value = (LPSTR)new char[MaxValueLength];

	for (DWORD i = 0; i < dwValueNumber; i++)
	{
		//不能直接使用MaxValueNameLength，因为每次调用后会改变它的值
		DWORD  ValueNameLength = MaxValueNameLength;
		DWORD  ValueLength = MaxValueLength;
		memset(ValueName, 0, ValueNameLength);
		memset(Value, 0, ValueLength);
		
		Ret = RegEnumValueA(hKey,
			i,
			ValueName,
			&ValueNameLength,
			NULL,
			NULL,
			NULL,
			NULL);

		if ((Ret != ERROR_SUCCESS) && (Ret != ERROR_NO_MORE_ITEMS))
		{
			continue;
		}

		Ret = RegQueryValueExA(hKey,
			ValueName,
			NULL,
			NULL,
			(LPBYTE)Value,
			&ValueLength);

		if (Ret != ERROR_SUCCESS)
		{
			continue;
		}

		//Add to List
		stSerialInfo.str_Name = ValueName;
		stSerialInfo.str_Port = Value;
		m_SerialList.Add(stSerialInfo);
	}

	if (m_SerialList.GetSize() > 0)
	{
		std::sort(m_SerialList.GetData(), m_SerialList.GetData() + m_SerialList.GetSize(), Sort_Port);
	}

	delete [] Value;
	delete [] ValueName;
	
	RegCloseKey(hKey);

	//串口更新成功，发送消息
	if (m_pOwner)
	{
		::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_UPDATE_LIST, (WPARAM)0, (LPARAM)0);
	}	
	return ERROR_SUCCESS;
}


//sort回掉函数，如果不需要排序则返回true
bool CSerial::Sort_Port(SerialInfo &Info1, SerialInfo &Info2)
{
	return strcmp(Info1.str_Port.c_str(), Info2.str_Port.c_str()) < 0;
}

bool CSerial::Sort_Name(SerialInfo &Info1, SerialInfo &Info2)
{
	return strcmp(Info1.str_Name.c_str(), Info2.str_Name.c_str()) < 0;
}

bool CSerial::OpenSerial(p_SerialInfo pSerialInfo, DWORD dwBaudRate)
{
	m_pSerialPort = pSerialInfo;
	std::string PortName = "\\\\.\\" + m_pSerialPort->str_Port;	//加上这个可以防止端口号大于10无法打开
	m_pSerialPort->h_Handle = CreateFile(PortName.c_str(),//COM口  
		GENERIC_READ | GENERIC_WRITE, //允许读和写  
		0, //独占方式  
		NULL,
		OPEN_EXISTING, //打开而不是创建  
		FILE_FLAG_OVERLAPPED, //异步方式  
		NULL);
	;
	if (m_pSerialPort->h_Handle == INVALID_HANDLE_VALUE)
	{
		m_pSerialPort->h_Handle = NULL;
		m_pSerialPort = NULL;
		return false;
	}

	//设置缓冲区大小
	if (!SetupComm(m_pSerialPort->h_Handle, INPUT_BUF_SIZE, OUTPUT_BUF_SIZE))
	{
		CloseSerial();
		return false;
	}

	DCB dcb;
	GetCommState(m_pSerialPort->h_Handle, &dcb);
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = 8; //每个字节有8位  
	dcb.Parity = NOPARITY; //无奇偶校验位  
	dcb.StopBits = ONESTOPBIT; //一个停止位  
	if (!SetCommState(m_pSerialPort->h_Handle, &dcb))
	{
		//DWORD dw = GetLastError();
		CloseSerial();
		return false;
	}

	COMMTIMEOUTS TimeOuts;
	//设定读超时  
	//ReadIntervalTimeout：以ms为单位指定通信线路上两个字符到达之间的最大时间间隔。
	//ReadTotalTimeout=( ReadTotalTimeoutMultiplier*bytes_to_read )+ReadTotalTimeoutConstant;
	//即两个字节传输间隔超过ReadIntervalTimeout或者操作总时间大于ReadTotalTimeout则操作结束（如果没接收到第一个字符则按总时间计算）
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 100;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//在读一次输入缓冲区的内容后读操作就立即返回，  
	//而不管是否读入了要求的字符。  
	//设定写超时  
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(m_pSerialPort->h_Handle, &TimeOuts))
	{
		CloseSerial();
		return false;
	}//设置超时  


	PurgeComm(m_pSerialPort->h_Handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);	//清空缓冲区

	//设置接收通知
	if (!SetCommMask(m_pSerialPort->h_Handle, EV_RXCHAR))
	{
		CloseSerial();
		return false;
	}


	//开启串口接收线程
	m_pReceiveThread = AfxBeginThread(Receive_Thread, (LPVOID)this);
	if (NULL == m_pReceiveThread)
	{
		CloseSerial();
		return false;
	}


#ifdef SEND_TEST
	m_pSendThread = AfxBeginThread(Send_Thread, (LPVOID)this);
	if (NULL == m_pSendThread)
	{
		CloseSerial();
		return false;
	}
#endif

	return TRUE;
}

bool CSerial::IsOpen()
{
	if (m_pSerialPort)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool CSerial::CloseSerial()
{
	//PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);	//清空缓冲区
	if (CloseHandle(m_pSerialPort->h_Handle))
	{
		m_pSerialPort->h_Handle = NULL;
		m_pSerialPort = NULL;
		if (m_pReceiveThread)
		{
			WaitForSingleObject(m_pReceiveThread->m_hThread, INFINITE);
			m_pReceiveThread = NULL;
		}
#ifdef SEND_TEST
		if (m_pSendThread)
		{
			WaitForSingleObject(m_pSendThread->m_hThread, INFINITE);
			m_pSendThread = NULL;
		}
#endif
		return true;
	}
	else
	{
		return false;
	}
}

#ifdef SEND_TEST
UINT CSerial::Send_Thread(void *args)
{
	char SendData[5] = {0};
	unsigned long Count = 0;
	CSerial *pCSerial = (CSerial *)args;
	while (pCSerial->IsOpen())
	{
		sprintf(SendData, "%d\n", Count);
		pCSerial->SendData(SendData, strlen(SendData));
		Sleep(200);
		if (++Count == 1000)
		{
			Count = 0;
		}
	}
	return 0;
}
#endif


UINT CSerial::Receive_Thread(void *args)
{
	CSerial *pCSerial = (CSerial *)args;
	UINT ret = pCSerial->ReceiveData();
	return ret;
}

UINT CSerial::ReceiveData()
{
	OVERLAPPED overlapped = { 0 };	//OVERLAPPED结构体用来设置I/O异步，具体可以参见MSDN
	COMSTAT ComStat;				//这个结构体主要是用来获取端口信息的
	DWORD dwErrorFlags;

	char tempBuf[DMA_BUFF_SIZE];
	//DWORD dwBytesLength = 1;
	DWORD dwBytesRead = 0;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//创建CEvent对象
	while (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);

		if (!ReadFile(m_pSerialPort->h_Handle, tempBuf, DMA_BUFF_SIZE, &dwBytesRead, &overlapped))
		{
			if (GetLastError() == ERROR_IO_PENDING) //GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作 
			{
				//WaitForSingleObject(overlapped.hEvent, 2000);
				//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟 
				//当串口读操作进行完毕后，overlapped的hEvent事件会变为有信号 

				GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesRead, TRUE);	//无限等待这个I/O操作的完成
				if (dwBytesRead != DMA_BUFF_SIZE)
				{
					printf("Fail\n");
				}
			}
		}
		PurgeComm(m_pSerialPort->h_Handle, PURGE_RXABORT | PURGE_RXCLEAR);
	}

	return 0;
}

DWORD CSerial::SendData(const char *pData, DWORD nDataLength)
{
	OVERLAPPED overlapped = { 0 };	//OVERLAPPED结构体用来设置I/O异步，具体可以参见MSDN
	COMSTAT ComStat;				//这个结构体主要是用来获取端口信息的
	DWORD dwErrorFlags;
	DWORD dwBytesSend = 0;
	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//创建CEvent对象

	if (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);

		if (!WriteFile(m_pSerialPort->h_Handle, pData, nDataLength, &dwBytesSend, &overlapped))
		{
			if (GetLastError() == ERROR_IO_PENDING) //GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作 
			{
				//WaitForSingleObject(overlapped.hEvent, 2000);
				//使用WaitForSingleObject函数等待，直到I/O操作完成或延时已达到2秒钟 
				//当串口操作进行完毕后，overlapped的hEvent事件会变为有信号 

				GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesSend, TRUE);	//无限等待这个I/O操作的完成

				//return dwBytesRead;
			}
		}

		PurgeComm(m_pSerialPort->h_Handle, PURGE_TXABORT | PURGE_TXCLEAR);
	}
	return dwBytesSend;
}



//HANDLE hPort = ::CreateFile(sPort, GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, 0, 0);
//if (hPort == INVALID_HANDLE_VALUE)
//{
//	DWORD dwError = GetLastError();
//
//	if (dwError == ERROR_ACCESS_DENIED)
//	{
//		bSuccess = TRUE;
//		portsu.Add(i);       //已占用的串口  
//	}
//}