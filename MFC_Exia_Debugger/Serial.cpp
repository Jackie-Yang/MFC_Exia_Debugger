#include "stdafx.h"
#include "Serial.h"
#include <algorithm>



CSerial::CSerial()
{
	m_pReceiveThread = NULL;
	m_SerialRecData.nByteToRead = 0;
	m_SerialRecData.nReadIndex = 0;
	m_SerialRecData.nWriteIndex = 0;

#if SEND_TEST
	m_pSendThread = NULL;
#endif
	m_pOwner = NULL;
	m_pSerialPort = NULL;

	//用于串口注册表监视线程
	m_hWatchingEvent = NULL;
	m_hWatchingKey = NULL;
	m_hWatchingMutex = NULL;
	m_pWatchingThread = NULL;
	m_bNoSerial = true;
	m_bIsWatching = FALSE;
}


CSerial::~CSerial()
{
	if (IsOpen())
	{
		CloseSerial();
	}

	if (m_bIsWatching)
	{
		StopWatchingSerialList();
	}

	m_pOwner = NULL;
	
}

bool CSerial::Init(CWnd* pOwner)
{
	m_pOwner = pOwner;

	//测试数据接收，打开终端
#if RECEIVE_TEST
	AllocConsole();
	HANDLE handle = GetStdHandle(STD_OUTPUT_HANDLE);
	int hCrt = _open_osfhandle((long)handle, _O_TEXT);
	FILE * hf = _fdopen(hCrt, "w");
	*stdout = *hf;
#endif

	return UpdateSerialList();
}



bool CSerial::UpdateSerialList()
{
	LONG Ret;
	HKEY hKey;
	DWORD  MaxValueNameLength;
	DWORD  MaxValueLength;
	DWORD  dwValueNumber;
	LPTSTR  ValueName = NULL, Value = NULL;
	SerialInfo stSerialInfo;
	static SerialInfo stCurSerialInfo;
	stCurSerialInfo.str_Name = _T("");
	stCurSerialInfo.str_Port = _T("");
	stCurSerialInfo.h_Handle = NULL;

	CheckSerialState();	//检查当前连接的串口是否连接正常,若串口已失效会将其关闭

	//Get Key Handle
	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_QUERY_VALUE, &hKey);

	if (ERROR_SUCCESS != Ret)
	{
		return FALSE;
	}

	//Get Value Num And Max Length
	Ret = RegQueryInfoKey(hKey,
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
		return FALSE;
	}

	if (IsOpen())	//把当前工作的SerialInfo指针切换到临时结构体，因为在操作CArray的时候会重新分配内存改变其指针
	{
		stCurSerialInfo = *m_pSerialPort;
		m_pSerialPort = &stCurSerialInfo;
	}

//开始更新m_SerialList
	if (m_SerialList.GetSize() > 0)
	{
		m_SerialList.RemoveAll();
	}

	#ifdef _UNICODE
	MaxValueNameLength = MaxValueNameLength + 2;	//Unicode下结尾符应该算2
	MaxValueLength = MaxValueLength + 2;
	#else
	MaxValueNameLength = MaxValueNameLength + 1;	//Unicode下结尾符应该算2
	MaxValueLength = MaxValueLength + 1;
	#endif
	
	ValueName = (LPTSTR)new TCHAR[MaxValueNameLength];
	Value = (LPTSTR)new TCHAR[MaxValueLength];

	for (DWORD i = 0; i < dwValueNumber; i++)
	{
		stSerialInfo.str_Name = _T("");
		stSerialInfo.str_Port = _T("");
		stSerialInfo.h_Handle = NULL;
		//不能直接使用MaxValueNameLength，因为每次调用后会改变它的值
		DWORD  ValueNameLength = MaxValueNameLength;
		DWORD  ValueLength = MaxValueLength;
		memset(ValueName, 0, ValueNameLength);
		memset(Value, 0, ValueLength);
		
		Ret = RegEnumValue(hKey,
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

		Ret = RegQueryValueEx(hKey,
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
//m_SerialList更新完毕

	if (IsOpen())	//如果串口打开，将Handle存入List
	{
		p_SerialInfo pSerialInfo = NULL;
		int i;
		for (i = 0; i < GetSerialNum(); i++)
		{
			pSerialInfo = GetSerialInfo(i);
			if (m_pSerialPort->str_Port == pSerialInfo->str_Port)
			{
				pSerialInfo->h_Handle = m_pSerialPort->h_Handle;
				m_pSerialPort = pSerialInfo;
				break;
			}
		}
		if (i == GetSerialNum())
		{
			CloseSerial();	//串口正常打开，注册表却找不到响应的串口，这种情况应该不会出现，保险起见应该关掉该串口
		}
	}


	delete [] Value;
	delete [] ValueName;
	
	RegCloseKey(hKey);

	//串口更新成功，发送消息
	if (m_pOwner)
	{
		::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_UPDATE_LIST, (WPARAM)0, (LPARAM)0);
	}	
	return TRUE;
}

INT_PTR CSerial::GetSerialNum()
{
	return m_SerialList.GetSize();
}

p_SerialInfo CSerial::GetCurSerial()
{
	return m_pSerialPort;
}

p_SerialInfo CSerial::GetSerialInfo(INT_PTR nIndex)
{
	if (nIndex < 0 || nIndex >= m_SerialList.GetSize())
	{
		return NULL;
	}
	return &m_SerialList.ElementAt(nIndex);	//ElementAt获得引用,GetAt获得值
}


//sort回掉函数，如果不需要排序则返回true
bool CSerial::Sort_Port(SerialInfo &Info1, SerialInfo &Info2)
{
	if (Info1.str_Port.GetLength() == Info2.str_Port.GetLength())
	{
		return _tcscmp(Info1.str_Port, Info2.str_Port) < 0;
	}
	else
	{
		return Info1.str_Port.GetLength() < Info2.str_Port.GetLength();
	}
	
}

bool CSerial::Sort_Name(SerialInfo &Info1, SerialInfo &Info2)
{
	return _tcscmp(Info1.str_Name, Info2.str_Name) < 0;
}





bool CSerial::OpenSerial(p_SerialInfo pSerialInfo, DWORD dwBaudRate)
{
	m_pSerialPort = pSerialInfo;
	if (!m_pSerialPort)
	{
		SetLastError(ERROR_FILE_NOT_FOUND);
		return FALSE;
	}
	CString PortName = _T("\\\\.\\") + m_pSerialPort->str_Port;	//加上这个可以防止端口号大于10无法打开
	m_pSerialPort->h_Handle = CreateFile(PortName,//COM口  
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
		return FALSE;
	}

	//设置缓冲区大小
	if (!SetupComm(m_pSerialPort->h_Handle, INPUT_BUF_SIZE, OUTPUT_BUF_SIZE))
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}

	DCB dcb;
	GetCommState(m_pSerialPort->h_Handle, &dcb);
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = 8; //每个字节有8位  
	dcb.Parity = NOPARITY; //无奇偶校验位  
	dcb.StopBits = ONESTOPBIT; //一个停止位  
	if (!SetCommState(m_pSerialPort->h_Handle, &dcb))
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}

	COMMTIMEOUTS TimeOuts;
	//设定读超时  
	//ReadIntervalTimeout：以ms为单位指定通信线路上两个字符到达之间的最大时间间隔。
	//ReadTotalTimeout=( ReadTotalTimeoutMultiplier*bytes_to_read )+ReadTotalTimeoutConstant;
	//即两个字节传输间隔超过ReadIntervalTimeout或者操作总时间大于ReadTotalTimeout则操作结束（如果没接收到第一个字符则按总时间计算）
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//设定写超时  
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(m_pSerialPort->h_Handle, &TimeOuts))
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}//设置超时  


	PurgeComm(m_pSerialPort->h_Handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);	//清空缓冲区

	//设置接收通知
	//WaitCommEvent在重叠I/O模式下不会阻塞， 而是立即返回的。报ERROR_IO_PENDING
	if (!SetCommMask(m_pSerialPort->h_Handle, EV_RXCHAR))
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}


	//开启串口接收线程
	m_pReceiveThread = AfxBeginThread(Receive_Thread, (LPVOID)this);
	if (NULL == m_pReceiveThread)
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}

	m_SerialRecData.hMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_SerialRecData.hMutex == NULL)
	{
		DWORD dwError = GetLastError();		//保存错误
		CloseSerial();						//关闭串口
		SetLastError(dwError);				//设置错误标记
		return FALSE;
	}
	BufClear(&m_SerialRecData);


#if SEND_TEST
	m_pSendThread = AfxBeginThread(Send_Thread, (LPVOID)this);
	if (NULL == m_pSendThread)
	{
		CloseSerial();
		return FALSE;
	}
#endif

	//串口打开成功，发送消息
	if (m_pOwner)
	{
		::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_OPEN, (WPARAM)0, (LPARAM)0);
	}

	return TRUE;
}





bool CSerial::IsOpen()
{
	if (m_pSerialPort)
	{
		if (m_pSerialPort->h_Handle)
		{
			return true;
		}
		else
		{
			return FALSE;
		}
	}
	else
	{
		return FALSE;
	}
}

bool CSerial::IsWatching()
{
	return m_bIsWatching;
}

bool CSerial::CloseSerial()
{
	//PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);	//清空缓冲区
	if (CloseHandle(m_pSerialPort->h_Handle))
	{
		m_pSerialPort->h_Handle = NULL;
		if (m_pReceiveThread)
		{
			WaitForSingleObject(m_pReceiveThread->m_hThread, INFINITE);
			m_pReceiveThread = NULL;
		}

		ClearRecData();
		if (m_SerialRecData.hMutex)
		{
			CloseHandle(m_SerialRecData.hMutex);
			m_SerialRecData.hMutex = NULL;
		}
#if SEND_TEST
		if (m_pSendThread)
		{
			WaitForSingleObject(m_pSendThread->m_hThread, INFINITE);
			m_pSendThread = NULL;
		}
#endif
		m_pSerialPort = NULL;
		//串口关闭成功，发送消息
		if (m_pOwner)
		{
			::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_CLOSE, (WPARAM)0, (LPARAM)0);
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}


bool CSerial::CheckSerialState()
{
	if (m_pSerialPort)
	{
		CString PortName = _T("\\\\.\\") + m_pSerialPort->str_Port;	//加上这个可以防止端口号大于10无法打开
		HANDLE hPort = CreateFile(PortName,//COM口  
			GENERIC_READ | GENERIC_WRITE, //允许读和写  
			0, //独占方式  
			NULL,
			OPEN_EXISTING, //打开而不是创建  
			FILE_FLAG_OVERLAPPED, //异步方式  
			NULL);
		if (hPort == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED) //已占用的串口，表示串口正常工作
			{     
				return TRUE;
			}
		}
		else		//打开成功，一般不会出现
		{
			CloseHandle(hPort);
		}
		CloseSerial();
	}
	//串口已经不存在或者工作不正常
	return FALSE;
}


#if SEND_TEST
UINT CSerial::Send_Thread(void *args)		//测试发送功能，定时发送字符串
{
	char SendData[5] = {0};
	unsigned long Count = 0;
	CSerial *pCSerial = (CSerial *)args;
	while (pCSerial->IsOpen())
	{
		sprintf(SendData, "%u\n", Count);
		pCSerial->SendData((UINT8 *)SendData, lstrlen(SendData));
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

	UINT8 ReceiveData[BUF_SIZE] = {0};
	DWORD dwBytesRead = 0;
	DWORD dwBytesReaded = 0;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//创建CEvent对象
	while (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);	//更新ComStat
		dwBytesRead = ComStat.cbInQue;
		if (dwBytesRead)	//如果串口inbuf中有接收到的字符就执行下面的操作
		{
			if (dwBytesRead > BUF_SIZE)		//缓冲区数据大于缓存数据
			{
				dwBytesRead = BUF_SIZE;
			}
			dwBytesReaded = 0;
			if (!ReadFile(m_pSerialPort->h_Handle, ReceiveData, dwBytesRead, &dwBytesReaded, &overlapped))
			{
				if (GetLastError() == ERROR_IO_PENDING) //GetLastError()函数返回ERROR_IO_PENDING,表明串口正在进行读操作 
				{
					//WaitForSingleObject(overlapped.hEvent, 2000);
					//使用WaitForSingleObject函数等待，直到读操作完成或延时已达到2秒钟 
					//当串口读操作进行完毕后，overlapped的hEvent事件会变为有信号,WaitForSingleObject仍要GetOverlappedResult获取结果
					GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesRead, TRUE);	//无限等待这个I/O操作的完成
					BufWriteData(&m_SerialRecData, ReceiveData, dwBytesRead);
				}
			}
			else
			{
				BufWriteData(&m_SerialRecData, ReceiveData, dwBytesRead);
			}
			if (ComStat.cbInQue > BUF_SIZE)	//串口缓冲区数据大于缓存数据，则有剩余数据没读，清空缓冲区
			{
				PurgeComm(m_pSerialPort->h_Handle, PURGE_RXABORT | PURGE_RXCLEAR);
			}
			#if RECEIVE_TEST
			printf("Buf:%d,Rec:%d,CycBuf:%d\n", ComStat.cbInQue, dwBytesReaded, m_SerialRecData.nByteToRead);
			#endif
		}
		Sleep(10);		//一直轮询会加重CPU负担
	}

	return 0;
}


DWORD CSerial::SendData(const UINT8 *pData, DWORD nDataLength)
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



bool CSerial::StartWatchingSerialList()
{
	LONG   Ret;
	// Open a key.
	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_NOTIFY, &m_hWatchingKey);
	if (Ret != ERROR_SUCCESS)
	{
		if (Ret == ERROR_FILE_NOT_FOUND)
		{
			//无串口，监视Device
			Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP"), 0, KEY_NOTIFY, &m_hWatchingKey);
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
		m_bNoSerial = FALSE;
		//Get List
		UpdateSerialList();
	}

	// Create an event.
	m_hWatchingEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (m_hWatchingEvent == NULL)
	{
		goto FAIL_PROCESS;
	}

	//创建线程互斥锁
	m_hWatchingMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_hWatchingMutex == NULL)
	{
		goto FAIL_PROCESS;
	}

	//创建子线程监视串口列表（创建的时候立即运行）  
	m_pWatchingThread = AfxBeginThread(Watching_Thread, this);
	if (NULL == m_pWatchingThread)
	{
		goto FAIL_PROCESS;
	}


	m_bIsWatching = TRUE;
	return TRUE;

FAIL_PROCESS:
	if (m_hWatchingEvent)
	{
		if (CloseHandle(m_hWatchingEvent))
		{
			m_hWatchingEvent = NULL;
		}
	}
	if (m_hWatchingKey)
	{
		if (RegCloseKey(m_hWatchingKey) == ERROR_SUCCESS)
		{
			m_hWatchingKey = NULL;
		}
	}
	if (m_hWatchingMutex)
	{
		CloseHandle(m_hWatchingMutex);
		m_hWatchingMutex = NULL;
	}
	return FALSE;
}

void CSerial::StopWatchingSerialList()
{
	if (m_hWatchingMutex)
	{
		WaitForSingleObject(m_hWatchingMutex, INFINITE);
	}
	if (m_hWatchingEvent)
	{
		if (CloseHandle(m_hWatchingEvent))
		{
			m_hWatchingEvent = NULL;
		}
	}
	if (m_hWatchingKey)
	{
		if (RegCloseKey(m_hWatchingKey) == ERROR_SUCCESS)
		{
			m_hWatchingKey = NULL;
		}
	}
	if (m_hWatchingMutex)
	{
		ReleaseMutex(m_hWatchingMutex);
	}
	//等待监听线程结束
	if (m_pWatchingThread)
	{
		WaitForSingleObject(m_pWatchingThread->m_hThread, INFINITE);
		m_pWatchingThread = NULL;
	}
	if (m_hWatchingMutex)
	{
		CloseHandle(m_hWatchingMutex);
		m_hWatchingMutex = NULL;
	}
	m_bIsWatching = FALSE;
}



UINT CSerial::Watching_Thread(void *args)
{
	CSerial *pCSerial = (CSerial *)args;
	BOOL ret = pCSerial->WatchingSerial();
	return ret;
}

//监听串口
bool CSerial::WatchingSerial()
{
	DWORD  dwFilter = REG_NOTIFY_CHANGE_NAME |
		REG_NOTIFY_CHANGE_ATTRIBUTES |
		REG_NOTIFY_CHANGE_LAST_SET |
		REG_NOTIFY_CHANGE_SECURITY;
	LONG   Ret;
	// Watch the registry key for a change of value.
	while (true)
	{
		WaitForSingleObject(m_hWatchingMutex, INFINITE);
		if (!(m_hWatchingKey && m_hWatchingEvent))
		{
			break;
		}
		Ret = RegNotifyChangeKeyValue(m_hWatchingKey,
			TRUE,
			dwFilter,
			m_hWatchingEvent,
			TRUE);
		if (Ret != ERROR_SUCCESS)
		{
			continue;
		}
		if (WaitForSingleObject(m_hWatchingEvent, 100) == WAIT_OBJECT_0)	//使用了互斥锁，因此这里等待需要设置超时，否则会造成死锁
		{
			if (m_bNoSerial)	//无串口，尝试监听串口
			{
				HKEY   TempKey;
				Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_NOTIFY, &TempKey);
				if (Ret == ERROR_SUCCESS)
				{
					if (RegCloseKey(m_hWatchingKey) == ERROR_SUCCESS)		//有串口，开始监听串口
					{
						m_hWatchingKey = TempKey;
						m_bNoSerial = FALSE;
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
		ResetEvent(m_hWatchingEvent);	//复位事件
		ReleaseMutex(m_hWatchingMutex);
	}

	return 0;
}




UINT32 CSerial::BufWriteData(p_CycBuf pBuf, const PUINT8 pData, UINT32 nSize)
{
	if (pBuf->hMutex)
	{
		if (WaitForSingleObject(pBuf->hMutex, INFINITE))	//!= WAIT_OBJECT_0
		{
			return 0;
		}
	}

	if (nSize > BUF_SIZE - pBuf->nByteToRead)	//写满无法再写
	{
		nSize = BUF_SIZE - pBuf->nByteToRead;
	}

	if (nSize == 0)
	{
		if (pBuf->hMutex)
		{
			ReleaseMutex(pBuf->hMutex);
		}
		return nSize;
	}

	if (BUF_SIZE - pBuf->nWriteIndex >= nSize)
	{
		memcpy(pBuf->pData + pBuf->nWriteIndex, pData, nSize);
	}
	else
	{
		memcpy(pBuf->pData + pBuf->nWriteIndex, pData, BUF_SIZE - pBuf->nWriteIndex);
		memcpy(pBuf->pData, pData + (BUF_SIZE - pBuf->nWriteIndex), nSize - (BUF_SIZE - pBuf->nWriteIndex));
	}

	pBuf->nWriteIndex += nSize;
	if (pBuf->nWriteIndex >= BUF_SIZE)
	{
		pBuf->nWriteIndex = pBuf->nWriteIndex - BUF_SIZE;
	}
	pBuf->nByteToRead += nSize;
	//不会溢出了，写满了不会再写
	//if (pBuf->nByteToRead > BUF_SIZE)	//溢出
	//{
	//	pBuf->nReadIndex += pBuf->nByteToRead - BUF_SIZE;
	//	if (pBuf->nReadIndex >= BUF_SIZE)
	//	{
	//		pBuf->nReadIndex = pBuf->nReadIndex - BUF_SIZE;
	//	}
	//	pBuf->nByteToRead = BUF_SIZE;
	//}
	if (pBuf->hMutex)
	{
		ReleaseMutex(pBuf->hMutex);
	}
	return nSize;
}

UINT32 CSerial::BufGetData(p_CycBuf pBuf, PUINT8 pData, UINT32 nSize)
{
	if (pBuf->hMutex)
	{
		if (WaitForSingleObject(pBuf->hMutex, INFINITE))	//!= WAIT_OBJECT_0
		{
			return 0;
		}
	}
	if (nSize > pBuf->nByteToRead)
	{
		nSize = pBuf->nByteToRead;
	}
	if (nSize == 0)
	{
		if (pBuf->hMutex)
		{
			ReleaseMutex(pBuf->hMutex);
		}
		return nSize;
	}

	if (BUF_SIZE - pBuf->nReadIndex >= nSize)
	{
		memcpy(pData, pBuf->pData + pBuf->nReadIndex, nSize);
	}
	else
	{
		memcpy(pData, pBuf->pData + pBuf->nReadIndex, BUF_SIZE - pBuf->nReadIndex);
		memcpy(pData + (BUF_SIZE - pBuf->nReadIndex), pBuf->pData, nSize - (BUF_SIZE - pBuf->nReadIndex));
	}

	pBuf->nReadIndex += nSize;
	if (pBuf->nReadIndex >= BUF_SIZE)
	{
		pBuf->nReadIndex = pBuf->nReadIndex - BUF_SIZE;
	}
	pBuf->nByteToRead -= nSize;
	if (pBuf->hMutex)
	{
		ReleaseMutex(pBuf->hMutex);
	}
	return nSize;
}

void CSerial::BufClear(p_CycBuf pBuf)
{
	if (pBuf->hMutex)
	{
		if (WaitForSingleObject(pBuf->hMutex, INFINITE))	//!= WAIT_OBJECT_0
		{
			return;
		}
	}
	pBuf->nByteToRead = 0;
	pBuf->nReadIndex = 0;
	pBuf->nWriteIndex = 0;
	if (pBuf->hMutex)
	{
		ReleaseMutex(pBuf->hMutex);
	}
}

UINT32 CSerial::GetRecBufByte()
{
	return m_SerialRecData.nByteToRead;
}

UINT32 CSerial::GetRecData(PUINT8 pData, UINT32 nSize)
{
	return BufGetData(&m_SerialRecData, pData, nSize);
}

void CSerial::ClearRecData()
{
	BufClear(&m_SerialRecData);
}