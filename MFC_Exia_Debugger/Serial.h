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

	LONG UpdateSerialList();

private:
	static bool Sort_Name(SerialInfo &Info1, SerialInfo &Info2);
	static bool Sort_Port(SerialInfo &Info1, SerialInfo &Info2);
};

