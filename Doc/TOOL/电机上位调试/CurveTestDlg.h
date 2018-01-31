// CurveTestDlg.h : header file
//

#if !defined(AFX_CURVETESTDLG_H__69A774E1_F790_4C0B_B915_1B3BEE6FC9DE__INCLUDED_)
#define AFX_CURVETESTDLG_H__69A774E1_F790_4C0B_B915_1B3BEE6FC9DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "pid.h"
#include "PIDDlg.h"
#include "queue.h"
#include "GetPIDDescription.h"
#include "pcomm.h"



#define DATA_NUM 6
#define VALCOUNT 20000

/////////////////////////////////////////////////////////////////////////////
// CCurveTestDlg dialog

class CCurveTestDlg : public CDialog
{
// Construction
public:
	CCurveTestDlg(CWnd* pParent = NULL);	// standard constructor
	~CCurveTestDlg();
// Dialog Data
	//{{AFX_DATA(CCurveTestDlg)
	enum { IDD = IDD_CURVETEST_DIALOG };
	CProgressCtrl	m_position_loop_progress;
	int		m_nPoints;
	int		m_nKP;
	int		m_nKPD;
	int		m_nKPP;
	int		m_nKPI;
	int		m_nMoSpeed;
	int		m_nPWMVal;
    int     m_nVoltage;
	CString	m_nMsg;
	int		m_nSetMoSP;
	int		m_usbSpeed;
	int		m_nKP2;
	int		m_nKP3;
	int		m_nKPD2;
	int		m_nKPD3;
	int		m_nKPI2;
	int		m_nKPI3;
	int		m_nKPP2;
	int		m_nKPP3;
	int		m_nMoSpeed2;
	int		m_nMoSpeed3;
	int		m_nPoints2;
	int		m_nPoints3;
	int		m_nPWMVal2;
	int		m_nPWMVal3;
	int		m_nSetMoSP2;
	int		m_nSetMoSP3;
	CString	m_dc_motor_position1;
	CString	m_dc_motor_position2;
	CString	m_dc_motor_position3;
	CString	m_SetDCMotorPosition;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveTestDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CCurveTestDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButtonStartMo();
	afx_msg void OnButtonOpenCom();
	afx_msg void OnButtonStopCap();
	afx_msg void OnButtonStopMo();
	afx_msg void OnButtonCloseCom();
	afx_msg void OnButtonStartCap();
	afx_msg void OnButtonSetSp();
	afx_msg void OnButtonSetPid();
	afx_msg void OnButtonSaveData();
	afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonReport();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnButtonStartCap2();
	afx_msg void OnButtonStopCap2();
	afx_msg void OnButtonStartCap3();
	afx_msg void OnButtonStopCap3();
	afx_msg void OnButtonSetPid2();
	afx_msg void OnButtonSetPid3();
	afx_msg void OnButtonSetSp2();
	afx_msg void OnButtonSetSp3();
	afx_msg void OnButtonStopMo2();
	afx_msg void OnButtonStartMo2();
	afx_msg void OnButtonStartMo3();
	afx_msg void OnButtonStopMo3();
	afx_msg void OnButtonSetPid4();
	afx_msg void OnChangeEditKp();
	afx_msg void OnChangeEditKp2();
	afx_msg void OnChangeEditKp3();
	afx_msg void OnChangeEditKpd();
	afx_msg void OnChangeEditKpd2();
	afx_msg void OnChangeEditKpd3();
	afx_msg void OnChangeEditKpi();
	afx_msg void OnChangeEditKpi2();
	afx_msg void OnChangeEditKpi3();
	afx_msg void OnChangeEditKpp();
	afx_msg void OnChangeEditKpp2();
	afx_msg void OnChangeEditKpp3();
	afx_msg void OnChangeEditSetMoSp();
	afx_msg void OnChangeEditSetMoSp2();
	afx_msg void OnChangeEditSetMoSp3();
	afx_msg void OnButtonPositionLoop();
	afx_msg void OnButtonGetParam();
	afx_msg void OnCheckPositionLoop();
	afx_msg void OnChangeEditPositionVal();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

	afx_msg void OnMsg(LPARAM lParam, WPARAM wParam);

private :
	CXGraph		  m_Graph;
	int           m_nSeries;
	int           m_nAxes;
	TDataPoint**  m_Values;
	TDataPoint**  m_Values2;
	TDataPoint**  m_Values3;
	CPID          m_pid;
	char          m_recv_buf[128];
	int           m_set_speed;
	// thread
	CWinThread*   m_SerialThread;
	UINT          m_data_points;
	UINT          m_data_points2;
	UINT          m_data_points3;
	BOOL          m_cap_flag;
	BOOL          m_cap_flag2;
	BOOL          m_cap_flag3;
	char          m_cConfigFilePath[MAX_PATH];
	UINT          m_port_num;
	UINT          m_data_speed;
	UINT          m_usb_data_size;
    CString       m_soft_version;
    // dc motor parameter
    UINT          m_max_pwm;
    UINT          m_encoder_number;
    UINT          m_dc_motor_reduction_ratio;

public:
	CQueue        m_queue;


private:
	void DecodeCString(CString source,  CStringArray& dest, char division);
	static UINT	DataThread(LPVOID pParam);
	void DoCMDData(UINT ch);
	void DisableButtuon(BOOL bSet);
	int  GetParametersFromInifile(char* path);
	void SavePIDParamToIniFile(char* path);
	void serial_send_cmd_data(char* buf, UINT len);
	void set_edit_text(int nID);
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVETESTDLG_H__69A774E1_F790_4C0B_B915_1B3BEE6FC9DE__INCLUDED_)
