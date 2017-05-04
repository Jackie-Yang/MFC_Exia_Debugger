
// MFC_Exia_DebuggerDlg.h : ͷ�ļ�
//

#pragma once
#include "Serial.h"
#include "afxwin.h"
#include "CurveDLG.h"
#include "Quadrotor_State.h"


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
	void InitQuadrotorState();
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
	afx_msg void OnBnClickedButtonScreenshot();
	afx_msg void OnBnClickedButtonClearCurve();

	CComboBox m_ComBoxCurve[CURVE_LINE];			//CURVE_LINE�ж������߾�Ӧ�÷��ö��ٿؼ�
	CComboBox m_ComBoxGain[CURVE_LINE];			//CURVE_LINE�ж������߾�Ӧ�÷��ö��ٿؼ�
	int m_CurveSelected[CURVE_LINE];
	int m_GainSelected[CURVE_LINE];
	CurveData m_CurveData[CURVE_LINE];	//����ӵ����ߵ�����
	void SetCurveData();
	afx_msg void UpdateSelected();

	CComboBox m_CombocRefresh;
	UINT_PTR m_nRefreshTime;
	afx_msg void OnCbnSelchangeCurveRefresh();
	CString m_str_Motor1;
	CString m_str_Motor2;
	CString m_str_Motor3;
	CString m_str_Motor4;
	CString m_str_Gyro_X;
	CString m_str_Gyro_Y;
	CString m_str_Gyro_Z;
	CString m_str_Accel_X;
	CString m_str_Accel_Y;
	CString m_str_Accel_Z;
	CString m_str_HIGH_KS10X;
	CString m_str_HIGH_MS5611;
	CString m_str_Temp_MPU6050;
	CString m_str_Temp_MS5611;
	CString m_str_Press_MS5611;
	CString m_str_HIGH_Accel;
	float m_fRoll_G_Kp;
	virtual void OnOK();
};
