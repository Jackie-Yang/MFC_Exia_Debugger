#include "stdafx.h"
#include "Serial.h"
#include <algorithm>

CSerial::CSerial()
{
	m_hUpdateEvent = NULL;
	m_hUpdateEvent = NULL;
	m_pUpdateThread = NULL;
	m_pOwner = NULL;
	m_bNoSerial = true;
	m_hUpdateMutex = NULL;
	m_hSerialPort = NULL;
}


CSerial::~CSerial()
{
	if (m_hSerialPort)
	{
		CloseHandle(m_hSerialPort);
		m_hSerialPort = NULL;
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
	WaitForSingleObject(m_pUpdateThread->m_hThread, INFINITE);
	CloseHandle(m_hUpdateMutex);
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

bool CSerial::OpenSerial(std::string PortName)
{
	PortName = "\\\\.\\" + PortName;	//加上这个可以防止端口号大于10无法打开
	m_hSerialPort = CreateFile(PortName.c_str(),//COM口  
		GENERIC_READ | GENERIC_WRITE, //允许读和写  
		0, //独占方式  
		NULL,
		OPEN_EXISTING, //打开而不是创建  
		FILE_FLAG_OVERLAPPED, //异步方式  
		NULL);
	;
	if (m_hSerialPort == INVALID_HANDLE_VALUE)
	{
		m_hSerialPort = NULL;
		return false;
	}

	SetupComm(m_hSerialPort, INPUT_BUF_SIZE, OUTPUT_BUF_SIZE);

	COMMTIMEOUTS TimeOuts;
	//设定读超时  
	TimeOuts.ReadIntervalTimeout = MAXDWORD;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//在读一次输入缓冲区的内容后读操作就立即返回，  
	//而不管是否读入了要求的字符。  
	//设定写超时  
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 500;
	if (!SetCommTimeouts(m_hSerialPort, &TimeOuts))
	{
		return false;
	}//设置超时  

	DCB dcb;
	GetCommState(m_hSerialPort, &dcb);
	dcb.BaudRate = CBR_115200; //波特率为115200  
	dcb.ByteSize = 8; //每个字节有8位  
	dcb.Parity = NOPARITY; //无奇偶校验位  
	dcb.StopBits = ONESTOPBIT; //一个停止位  
	if (!SetCommState(m_hSerialPort, &dcb))
	{
		DWORD dw = GetLastError();
		return false;
	}
	PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);	//清空缓冲区

	//开启串口接收线程
	//AfxBeginThread(Receive_Thread, (LPVOID)this);

	return TRUE;
}

bool CSerial::IsOpen()
{
	if (m_hSerialPort)
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
	if (CloseHandle(m_hSerialPort))
	{
		m_hSerialPort = NULL;
		return true;
	}
	else
	{
		return false;
	}
}


UINT CSerial::Receive_Thread(void *args)
{
	CSerial *pCSerial = (CSerial *)args;
	BOOL ret = pCSerial->ReceiveDate();
	return ret;
}

bool CSerial::ReceiveDate()
{

}