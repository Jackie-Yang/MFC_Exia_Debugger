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

	//���ڴ���ע�������߳�
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

	//�������ݽ��գ����ն�
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

	CheckSerialState();	//��鵱ǰ���ӵĴ����Ƿ���������,��������ʧЧ�Ὣ��ر�

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

	if (IsOpen())	//�ѵ�ǰ������SerialInfoָ���л�����ʱ�ṹ�壬��Ϊ�ڲ���CArray��ʱ������·����ڴ�ı���ָ��
	{
		stCurSerialInfo = *m_pSerialPort;
		m_pSerialPort = &stCurSerialInfo;
	}

//��ʼ����m_SerialList
	if (m_SerialList.GetSize() > 0)
	{
		m_SerialList.RemoveAll();
	}

	#ifdef _UNICODE
	MaxValueNameLength = MaxValueNameLength + 2;	//Unicode�½�β��Ӧ����2
	MaxValueLength = MaxValueLength + 2;
	#else
	MaxValueNameLength = MaxValueNameLength + 1;	//Unicode�½�β��Ӧ����2
	MaxValueLength = MaxValueLength + 1;
	#endif
	
	ValueName = (LPTSTR)new TCHAR[MaxValueNameLength];
	Value = (LPTSTR)new TCHAR[MaxValueLength];

	for (DWORD i = 0; i < dwValueNumber; i++)
	{
		stSerialInfo.str_Name = _T("");
		stSerialInfo.str_Port = _T("");
		stSerialInfo.h_Handle = NULL;
		//����ֱ��ʹ��MaxValueNameLength����Ϊÿ�ε��ú��ı�����ֵ
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
//m_SerialList�������

	if (IsOpen())	//������ڴ򿪣���Handle����List
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
			CloseSerial();	//���������򿪣�ע���ȴ�Ҳ�����Ӧ�Ĵ��ڣ��������Ӧ�ò�����֣��������Ӧ�ùص��ô���
		}
	}


	delete [] Value;
	delete [] ValueName;
	
	RegCloseKey(hKey);

	//���ڸ��³ɹ���������Ϣ
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
	return &m_SerialList.ElementAt(nIndex);	//ElementAt�������,GetAt���ֵ
}


//sort�ص��������������Ҫ�����򷵻�true
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
	CString PortName = _T("\\\\.\\") + m_pSerialPort->str_Port;	//����������Է�ֹ�˿ںŴ���10�޷���
	m_pSerialPort->h_Handle = CreateFile(PortName,//COM��  
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
		return FALSE;
	}

	//���û�������С
	if (!SetupComm(m_pSerialPort->h_Handle, INPUT_BUF_SIZE, OUTPUT_BUF_SIZE))
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
		return FALSE;
	}

	DCB dcb;
	GetCommState(m_pSerialPort->h_Handle, &dcb);
	dcb.BaudRate = dwBaudRate;
	dcb.ByteSize = 8; //ÿ���ֽ���8λ  
	dcb.Parity = NOPARITY; //����żУ��λ  
	dcb.StopBits = ONESTOPBIT; //һ��ֹͣλ  
	if (!SetCommState(m_pSerialPort->h_Handle, &dcb))
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
		return FALSE;
	}

	COMMTIMEOUTS TimeOuts;
	//�趨����ʱ  
	//ReadIntervalTimeout����msΪ��λָ��ͨ����·�������ַ�����֮������ʱ������
	//ReadTotalTimeout=( ReadTotalTimeoutMultiplier*bytes_to_read )+ReadTotalTimeoutConstant;
	//�������ֽڴ���������ReadIntervalTimeout���߲�����ʱ�����ReadTotalTimeout��������������û���յ���һ���ַ�����ʱ����㣩
	TimeOuts.ReadIntervalTimeout = 100;
	TimeOuts.ReadTotalTimeoutMultiplier = 0;
	TimeOuts.ReadTotalTimeoutConstant = 0;
	//�趨д��ʱ  
	TimeOuts.WriteTotalTimeoutMultiplier = 100;
	TimeOuts.WriteTotalTimeoutConstant = 0;
	if (!SetCommTimeouts(m_pSerialPort->h_Handle, &TimeOuts))
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
		return FALSE;
	}//���ó�ʱ  


	PurgeComm(m_pSerialPort->h_Handle, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);	//��ջ�����

	//���ý���֪ͨ
	//WaitCommEvent���ص�I/Oģʽ�²��������� �����������صġ���ERROR_IO_PENDING
	if (!SetCommMask(m_pSerialPort->h_Handle, EV_RXCHAR))
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
		return FALSE;
	}


	//�������ڽ����߳�
	m_pReceiveThread = AfxBeginThread(Receive_Thread, (LPVOID)this);
	if (NULL == m_pReceiveThread)
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
		return FALSE;
	}

	m_SerialRecData.hMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_SerialRecData.hMutex == NULL)
	{
		DWORD dwError = GetLastError();		//�������
		CloseSerial();						//�رմ���
		SetLastError(dwError);				//���ô�����
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

	//���ڴ򿪳ɹ���������Ϣ
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
	//PurgeComm(m_hSerialPort, PURGE_TXCLEAR | PURGE_RXCLEAR);	//��ջ�����
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
		//���ڹرճɹ���������Ϣ
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
		CString PortName = _T("\\\\.\\") + m_pSerialPort->str_Port;	//����������Է�ֹ�˿ںŴ���10�޷���
		HANDLE hPort = CreateFile(PortName,//COM��  
			GENERIC_READ | GENERIC_WRITE, //�������д  
			0, //��ռ��ʽ  
			NULL,
			OPEN_EXISTING, //�򿪶����Ǵ���  
			FILE_FLAG_OVERLAPPED, //�첽��ʽ  
			NULL);
		if (hPort == INVALID_HANDLE_VALUE)
		{
			if (GetLastError() == ERROR_ACCESS_DENIED) //��ռ�õĴ��ڣ���ʾ������������
			{     
				return TRUE;
			}
		}
		else		//�򿪳ɹ���һ�㲻�����
		{
			CloseHandle(hPort);
		}
		CloseSerial();
	}
	//�����Ѿ������ڻ��߹���������
	return FALSE;
}


#if SEND_TEST
UINT CSerial::Send_Thread(void *args)		//���Է��͹��ܣ���ʱ�����ַ���
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
	OVERLAPPED overlapped = { 0 };	//OVERLAPPED�ṹ����������I/O�첽��������Բμ�MSDN
	COMSTAT ComStat;				//����ṹ����Ҫ��������ȡ�˿���Ϣ��
	DWORD dwErrorFlags;

	UINT8 ReceiveData[BUF_SIZE] = {0};
	DWORD dwBytesRead = 0;
	DWORD dwBytesReaded = 0;

	overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);	//����CEvent����
	while (IsOpen())
	{
		ClearCommError(m_pSerialPort->h_Handle, &dwErrorFlags, &ComStat);	//����ComStat
		dwBytesRead = ComStat.cbInQue;
		if (dwBytesRead)	//�������inbuf���н��յ����ַ���ִ������Ĳ���
		{
			if (dwBytesRead > BUF_SIZE)		//���������ݴ��ڻ�������
			{
				dwBytesRead = BUF_SIZE;
			}
			dwBytesReaded = 0;
			if (!ReadFile(m_pSerialPort->h_Handle, ReceiveData, dwBytesRead, &dwBytesReaded, &overlapped))
			{
				if (GetLastError() == ERROR_IO_PENDING) //GetLastError()��������ERROR_IO_PENDING,�����������ڽ��ж����� 
				{
					//WaitForSingleObject(overlapped.hEvent, 2000);
					//ʹ��WaitForSingleObject�����ȴ���ֱ����������ɻ���ʱ�Ѵﵽ2���� 
					//�����ڶ�����������Ϻ�overlapped��hEvent�¼����Ϊ���ź�,WaitForSingleObject��ҪGetOverlappedResult��ȡ���
					GetOverlappedResult(m_pSerialPort->h_Handle, &overlapped, &dwBytesRead, TRUE);	//���޵ȴ����I/O���������
					BufWriteData(&m_SerialRecData, ReceiveData, dwBytesRead);
				}
			}
			else
			{
				BufWriteData(&m_SerialRecData, ReceiveData, dwBytesRead);
			}
			if (ComStat.cbInQue > BUF_SIZE)	//���ڻ��������ݴ��ڻ������ݣ�����ʣ������û������ջ�����
			{
				PurgeComm(m_pSerialPort->h_Handle, PURGE_RXABORT | PURGE_RXCLEAR);
			}
			#if RECEIVE_TEST
			printf("Buf:%d,Rec:%d,CycBuf:%d\n", ComStat.cbInQue, dwBytesReaded, m_SerialRecData.nByteToRead);
			#endif
		}
		Sleep(10);		//һֱ��ѯ�����CPU����
	}

	return 0;
}


DWORD CSerial::SendData(const UINT8 *pData, DWORD nDataLength)
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



bool CSerial::StartWatchingSerialList()
{
	LONG   Ret;
	// Open a key.
	Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_NOTIFY, &m_hWatchingKey);
	if (Ret != ERROR_SUCCESS)
	{
		if (Ret == ERROR_FILE_NOT_FOUND)
		{
			//�޴��ڣ�����Device
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

	//�����̻߳�����
	m_hWatchingMutex = CreateMutex(nullptr, FALSE, nullptr);
	if (m_hWatchingMutex == NULL)
	{
		goto FAIL_PROCESS;
	}

	//�������̼߳��Ӵ����б�������ʱ���������У�  
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
	//�ȴ������߳̽���
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

//��������
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
		if (WaitForSingleObject(m_hWatchingEvent, 100) == WAIT_OBJECT_0)	//ʹ���˻��������������ȴ���Ҫ���ó�ʱ��������������
		{
			if (m_bNoSerial)	//�޴��ڣ����Լ�������
			{
				HKEY   TempKey;
				Ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE, _T("HARDWARE\\DEVICEMAP\\SERIALCOMM"), 0, KEY_NOTIFY, &TempKey);
				if (Ret == ERROR_SUCCESS)
				{
					if (RegCloseKey(m_hWatchingKey) == ERROR_SUCCESS)		//�д��ڣ���ʼ��������
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
		ResetEvent(m_hWatchingEvent);	//��λ�¼�
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

	if (nSize > BUF_SIZE - pBuf->nByteToRead)	//д���޷���д
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
	//��������ˣ�д���˲�����д
	//if (pBuf->nByteToRead > BUF_SIZE)	//���
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