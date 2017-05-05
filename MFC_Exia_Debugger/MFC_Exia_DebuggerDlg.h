
// MFC_Exia_DebuggerDlg.h : 头文件
//

#pragma once
#include "Serial.h"
#include "afxwin.h"
#include "CurveDLG.h"
#include "Quadrotor_State.h"


// CMFC_Exia_DebuggerDlg 对话框
class CMFC_Exia_DebuggerDlg : public CDialogEx
{
public:
	// 构造
	CMFC_Exia_DebuggerDlg(CWnd* pParent = NULL);	// 标准构造函数
	//析构
	~CMFC_Exia_DebuggerDlg();

//枚举
	// 对话框数据
	enum { IDD = IDD_MAIN_DIALOG };
	// 定时器ID
	enum 
	{ 
		ID_TIMER_UPDATE_DATA = 1, 
		ID_TIMER_SHOW_DATA ,
		ID_TIMER_UPDATE_CURVE
	};

//控件
	// Combox:选择串口
	CComboBox m_Combox_COM;
	//显示串口状态
	CStatic m_Static_Status;


	Quadrotor_State m_State;
	COLORREF m_Color_Status;
	COLORREF m_Color_BuffByte;
	
	CString GetErrorMessage(DWORD dwError, const TCHAR* ErrorTip = NULL);
	CString GetLastErrorMessage(const TCHAR* ErrorTip = NULL);
	void UpdateSerialState();
	bool GetQuadrotorState();
	void InitQuadrotorState();
	void ShowQuadrotorState();

//处理函数
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnBnClickedOpenCloseBtn();
	afx_msg BOOL OnDeviceChange(UINT nEventType, DWORD_PTR dwData);
	afx_msg void OnTimer(UINT_PTR nIDEvent);


private:

	CurveDLG *m_pCurveDLG;	//曲线图对话框

	CSerial m_Serial;
	UINT_PTR m_Timer_Update_Data;
	UINT_PTR m_Timer_Show_Data;
	UINT_PTR m_Timer_Update_Curve;

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

	afx_msg LRESULT OnSerialUpdateList(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialOpen(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnSerialClose(WPARAM wParam, LPARAM lParam);
public:
	// 显示缓冲区数据
	CString m_str_BuffByte;
	// 油门
	CString m_str_THRO;
	// 方向舵
	CString m_str_RUDD;
	// 升降舵
	CString m_str_ELEV;
	// 副翼
	CString m_str_AILE;
	// 加速度传感器原始数值
	CString m_str_Accel_Sensor_X;
	CString m_str_Accel_Sensor_Y;
	CString m_str_Accel_Sensor_Z;
	// 陀螺仪原始数值
	CString m_str_Gyro_Sensor_X;
	CString m_str_Gyro_Sensor_Y;
	CString m_str_Gyro_Sensor_Z;
	// 电子罗盘HMC5883L数据
	CString m_str_HMC5883L_X;
	CString m_str_HMC5883L_Y;
	CString m_str_HMC5883L_Z;
	CString m_str_HMC5883L_Angle;
	// 实时姿态
	CString m_str_Roll;
	CString m_str_Pitch;
	CString m_str_Yaw;
	afx_msg void OnBnClickedButtonOpenCurve();
	afx_msg void OnBnClickedCurveEnhance();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedButtonScreenshot();
	afx_msg void OnBnClickedButtonClearCurve();

	CComboBox m_ComBoxCurve[CURVE_LINE];			//CURVE_LINE有多少曲线就应该放置多少控件
	CComboBox m_ComBoxGain[CURVE_LINE];			//CURVE_LINE有多少曲线就应该放置多少控件
	int m_CurveSelected[CURVE_LINE];
	int m_GainSelected[CURVE_LINE];
	CurveData m_CurveData[CURVE_LINE];	//待添加到曲线的数据
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
	afx_msg void OnStnClickedStaticThro();
	afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
	afx_msg void OnStnClickedStaticRudd();
};
