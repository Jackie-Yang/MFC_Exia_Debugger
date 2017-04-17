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
	//�ȴ������߳̽���
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
			//�޴��ڣ�����Device
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

	//�����̻߳�����
	m_hUpdateMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_hUpdateMutex == NULL)
	{
		goto FAIL_PROCESS;
	}

	//�������̼߳��Ӵ����б�������ʱ���������У�  
	m_pUpdateThread = AfxBeginThread(Update_Thread, this);
	if (NULL == m_pUpdateThread)
	{
		goto FAIL_PROCESS;
	}

	//�������ݽ��գ����ն�
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

//��������
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
		if (WaitForSingleObject(m_hUpdateEvent, 100) == WAIT_OBJECT_0)	//ʹ���˻��������������ȴ���Ҫ���ó�ʱ��������������
		{
			if (m_bNoSerial)	//�޴��ڣ����Լ�������
			{
				HKEY   TempKey;
				Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, "HARDWARE\\DEVICEMAP\\SERIALCOMM", 0, KEY_NOTIFY, &TempKey);
				if (Ret == ERROR_SUCCESS)
				{
					if (RegCloseKey(m_hUpdateKey) == ERROR_SUCCESS)		//�д��ڣ���ʼ��������
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
		ResetEvent(m_hUpdateEvent);	//��λ�¼�
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
		//����ֱ��ʹ��MaxValueNameLength����Ϊÿ�ε��ú��ı�����ֵ
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

	//���ڸ��³ɹ���������Ϣ
	if (m_pOwner)
	{
		::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_UPDATE_LIST, (WPARAM)0, (LPARAM)0);
	}	
	return ERROR_SUCCESS;
}


//sort�ص��������������Ҫ�����򷵻�true
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
	std::string PortName = "\\\\.\\" + m_pSerialPort->str_Port;	//����������Է�ֹ�˿ںŴ���10�޷���
	m_pSerialPort->h_Handle = CreateFile(PortName.c_str(),//COM��  
		GENERIC_READ | GENERIC_WRITE, //�������д  
		0, //��ռ��ʽ  
		NULL,
		OPEN_EXISTING, //�򿪶����Ǵ���  
		FILE_FLAG_OVERLAPPED, //�첽��ʽ  
		NULL);
	;
	if (m_pSerialPort->h_Handle == INVALID_HANDLE_VALUE)
	{
		m_pSerialPort->h_Handle = NULL;
		m_pSerialPort = NULL;
		return false;
	}

	//���û�������С
	if (!SetupComm(m_pSerialPort->h_Handle, INPUT_BUF_SIZE, OUTPUT_BUF_SIZE))
	{
		CloseSerial();
		return false;
	}

	DCB dcb;
	GetCommState(m_pSerialPort->h_Handle, &dcb);
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = 8; //ÿ���ֽ���8λ  
	dcb.Parity = NOPARITY; //����żУ��λ  
	dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ  
	if (!SetCommState(m_pSerialPort->h_Handle, &dcb))
	{
		//DWORD dw = GetLastError();
		CloseSerial();
		return false;
	}

	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ  
	//ReadIntervalTimeout����msΪ��λָ��ͨ����·�������ַ�����֮������ʱ������
	//ReadTotalTimeout=( ReadTotalTimeoutMultiplier*bytes_to_read )+ReadTotalTimeoutConstant;
	//�������ֽڴ���������ReadIntervalTimeout���߲�����ʱ�����ReadTotalTimeout��������������û���յ���һ���ַ�����ʱ����㣩
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 100;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//�ڶ�һ�����뻺���������ݺ���������������أ�  
	//�������Ƿ������Ҫ����ַ���  
	//�趨д��ʱ  
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(m_pSerialPort->h_Handle, &TimeOuts))
	{
		CloseSerial();
		return false;
	}//���ó�ʱ  


	PurgeComm(m_pSerialPort->h_Handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);	//��ջ�����

	//���ý���֪ͨ
	if (!SetCommMask(m_pSerialPort->h_Handle, EV_RXCHAR))
	{
		CloseSerial();
		return false;
	}


	//�������ڽ����߳�
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
	//PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);	//��ջ�����
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
	OVERLAPPED overlapped = { 0 };	//OVERLAPPED�ṹ����������I/O�첽��������Բμ�MSDN
	COMSTAT ComStat;				//����ṹ����Ҫ��������ȡ�˿���Ϣ��
	DWORD dwErrorFlags;

	char tempBuf[DMA_BUFF_SIZE];
	//DWORD dwBytesLength = 1;
	DWORD dwBytesRead = 0;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//����CEvent����
	while (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);

		if (!ReadFile(m_pSerialPort->h_Handle, tempBuf, DMA_BUFF_SIZE, &dwBytesRead, &overlapped))
		{
			if (GetLastError() == ERROR_IO_PENDING) //GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж����� 
			{
				//WaitForSingleObject(overlapped.hEvent, 2000);
				//ʹ��WaitForSingleObject�����ȴ���ֱ����������ɻ���ʱ�Ѵﵽ2���� 
				//�����ڶ�����������Ϻ�overlapped��hEvent�¼����Ϊ���ź� 

				GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesRead, TRUE);	//���޵ȴ����I/O���������
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
	OVERLAPPED overlapped = { 0 };	//OVERLAPPED�ṹ����������I/O�첽��������Բμ�MSDN
	COMSTAT ComStat;				//����ṹ����Ҫ��������ȡ�˿���Ϣ��
	DWORD dwErrorFlags;
	DWORD dwBytesSend = 0;
	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//����CEvent����

	if (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);

		if (!WriteFile(m_pSerialPort->h_Handle, pData, nDataLength, &dwBytesSend, &overlapped))
		{
			if (GetLastError() == ERROR_IO_PENDING) //GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж����� 
			{
				//WaitForSingleObject(overlapped.hEvent, 2000);
				//ʹ��WaitForSingleObject�����ȴ���ֱ��I/O������ɻ���ʱ�Ѵﵽ2���� 
				//�����ڲ���������Ϻ�overlapped��hEvent�¼����Ϊ���ź� 

				GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesSend, TRUE);	//���޵ȴ����I/O���������

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
//		portsu.Add(i);       //��ռ�õĴ���  
//	}
//}