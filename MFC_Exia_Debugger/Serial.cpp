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
}


CSerial::~CSerial()
{
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
	

	//�������̣߳�������ʱ���������У�  
	m_pUpdateThread = AfxBeginThread(Update_thread, this);
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

UINT CSerial::Update_thread(void *args)
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
		::SendMessage(m_pOwner->m_hWnd, WM_SERIAL_UPDATE, (WPARAM)0, (LPARAM)0);
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