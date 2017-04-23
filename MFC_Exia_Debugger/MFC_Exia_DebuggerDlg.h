
// MFC_Exia_DebuggerDlg.h : ͷ�ļ�
//

#pragma once
#include "Serial.h"
#include "afxwin.h"
#include "CurveDLG.h"

//��ԭ�����뷽ʽ����ѹջ�������µĶ��뷽ʽ����Ϊһ���ֽڶ���(������)
#pragma  pack (push,1)
typedef struct __QUADROTOR_STATE__
{
	UINT16 Rudd;
	UINT16 Thro;
	UINT16 Aile;
	UINT16 Elev;
	UINT16 Check_Data;
	UINT16 Check_State;
	UINT16 Temp1;
	UINT16 Temp2;
	UINT16 Temp3;
	UINT16 Temp4;
	INT16 Accel_X;
	INT16 Accel_Y;
	INT16 Accel_Z;
	INT16 Gyro_X;
	INT16 Gyro_Y;
	INT16 Gyro_Z;
	UINT16 MPU6050_Temp;
	INT32 MS5611_Temp;
	INT32 MS5611_Press;
	INT32 MS5611_HIGH;
	INT16 HMC5883L_X;
	INT16 HMC5883L_Y;
	INT16 HMC5883L_Z;
	INT16 HMC5883L_Angle;
	UINT16 KS10X_High;
	UINT16 High_Accel;
	INT16 Roll;
	INT16 Pitch;
	INT16 Yaw;
	UINT16 Motor1;
	UINT16 Motor2;
	UINT16 Motor3;
	UINT16 Motor4;
}Quadrotor_State, *p_Quadrotor_State;
#pragma pack(pop) 
//�ָ�����״̬

// CMFC_Exia_DebuggerDlg �Ի���
class CMFC_Exia_DebuggerDlg : public CDialogEx
{
public:
	// ����
	CMFC_Exia_DebuggerDlg(CWnd* pParent = NULL);	// ��׼���캯��
	//����
	~CMFC_Exia_DebuggerDlg();

//ö��
	// �Ի�������
	enum { IDD = IDD_MAIN_DIALOG };
	// ��ʱ��ID
	enum 
	{ 
		ID_TIMER_UPDATE_DATA = 1, 
		ID_TIMER_SHOW_DATA ,
		ID_TIMER_UPDATE_CURVE
	};

//�ؼ�
	// Combox:ѡ�񴮿�
	CComboBox m_Combox_COM;
	//��ʾ����״̬
	CStatic m_Static_Status;


	Quadrotor_State m_State;
	COLORREF m_Color_Status;
	COLORREF m_Color_BuffByte;
	
	CString GetErrorMessage(DWORD dwError, const char* ErrorTip = NULL);
	CString GetLastErrorMessage(const char* ErrorTip = NULL);
	void UpdateSerialState();
	bool GetQuadrotorState();
	void ShowQuadrotorState();

//������
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOpenCloseBtn();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


private:

	CurveDLG *m_pCurveDLG;	//����ͼ�Ի���

	CSerial m_Serial;
	UINT_PTR m_Timer_Update_Data;
	UINT_PTR m_Timer_Show_Data;
	UINT_PTR m_Timer_Update_Curve;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSerialUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialClose(WPARAM wParam, LPARAM lParam);
public:
	// ��ʾ����������
	CString m_str_BuffByte;
	// ����
	CString m_str_THRO;
	// �����
	CString m_str_RUDD;
	// ������
	CString m_str_ELEV;
	// ����
	CString m_str_AILE;
	// ���ٶȴ�����ԭʼ��ֵ
	CString m_str_Accel_Sensor_X;
	CString m_str_Accel_Sensor_Y;
	CString m_str_Accel_Sensor_Z;
	// ������ԭʼ��ֵ
	CString m_str_Gyro_Sensor_X;
	CString m_str_Gyro_Sensor_Y;
	CString m_str_Gyro_Sensor_Z;
	// ��������HMC5883L����
	CString m_str_HMC5883L_X;
	CString m_str_HMC5883L_Y;
	CString m_str_HMC5883L_Z;
	CString m_str_HMC5883L_Angle;
	// ʵʱ��̬
	CString m_str_Roll;
	CString m_str_Pitch;
	CString m_str_Yaw;
	afx_msg void OnBnClickedButtonOpenCurve();
	afx_msg void OnBnClickedCurveEnhance();
	afx_msg void OnDestroy();
};
