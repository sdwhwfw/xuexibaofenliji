// CurveTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CurveTest.h"
#include "CurveTestDlg.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#pragma comment(lib, "PCOMM.lib")
#pragma message("*** Linking with PCOMM.dll...")

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveTestDlg dialog

CCurveTestDlg::CCurveTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CCurveTestDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CCurveTestDlg)
	m_nPoints = 0;
	m_nKP = 0;
	m_nKPD = 0;
	m_nKPP = 0;
	m_nKPI = 0;
	m_nMoSpeed = 0;
	m_nPWMVal = 0;
	m_nSetMoSP = 0;
	m_usbSpeed = 0;
	m_nKP2 = 0;
	m_nKP3 = 0;
	m_nKPD2 = 0;
	m_nKPD3 = 0;
	m_nKPI2 = 0;
	m_nKPI3 = 0;
	m_nKPP2 = 0;
	m_nKPP3 = 0;
	m_nMoSpeed2 = 0;
	m_nMoSpeed3 = 0;
	m_nPoints2 = 0;
	m_nPoints3 = 0;
	m_nPWMVal2 = 0;
	m_nPWMVal3 = 0;
	m_nSetMoSP2 = 0;
	m_nSetMoSP3 = 0;
	m_dc_motor_position1 = _T("0");
	m_dc_motor_position2 = _T("0");
	m_dc_motor_position3 = _T("0");
	m_SetDCMotorPosition = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_nSeries = 0;
	m_nAxes = 0;
	m_Values = NULL;
	m_Values2 = NULL;
	m_Values3 = NULL;
	m_cap_flag = FALSE;
	m_cap_flag2 = FALSE;
	m_cap_flag3 = FALSE;
	m_data_points = 0;
	m_data_points2 = 0;
	m_data_points3 = 0;
	m_data_speed = 0;
	m_usb_data_size = 0;
}

CCurveTestDlg::~CCurveTestDlg()
{
	if (m_Values)
	{
		for(int i = 0; i < DATA_NUM; i++)
			delete []m_Values[i];

		delete []m_Values;
	}

	if (m_Values2)
	{
		for(int i = 0; i < DATA_NUM; i++)
			delete []m_Values2[i];

		delete []m_Values2;
	}

	if (m_Values3)
	{
		for(int i = 0; i < DATA_NUM; i++)
			delete []m_Values3[i];

		delete []m_Values3;
	}

}




void CCurveTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CCurveTestDlg)
	DDX_Control(pDX, IDC_PROGRESS_PWM_DUTY_CYCLE, m_position_loop_progress);
	DDX_Text(pDX, IDC_EDIT_POINTS, m_nPoints);
	DDV_MinMaxInt(pDX, m_nPoints, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KP, m_nKP);
	DDV_MinMaxInt(pDX, m_nKP, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPD, m_nKPD);
	DDV_MinMaxInt(pDX, m_nKPD, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPP, m_nKPP);
	DDV_MinMaxInt(pDX, m_nKPP, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPI, m_nKPI);
	DDV_MinMaxInt(pDX, m_nKPI, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_MO_SPEED, m_nMoSpeed);
	DDV_MinMaxInt(pDX, m_nMoSpeed, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_PWM, m_nPWMVal);
	DDX_Text(pDX, IDC_EDIT_SET_MO_SP, m_nSetMoSP);
	DDV_MinMaxInt(pDX, m_nSetMoSP, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_USB_SPEED, m_usbSpeed);
	DDV_MinMaxInt(pDX, m_usbSpeed, 0, 1000000000);
	DDX_Text(pDX, IDC_EDIT_KP2, m_nKP2);
	DDV_MinMaxInt(pDX, m_nKP2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KP3, m_nKP3);
	DDV_MinMaxInt(pDX, m_nKP3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPD2, m_nKPD2);
	DDV_MinMaxInt(pDX, m_nKPD2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPD3, m_nKPD3);
	DDV_MinMaxInt(pDX, m_nKPD3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPI2, m_nKPI2);
	DDV_MinMaxInt(pDX, m_nKPI2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPI3, m_nKPI3);
	DDV_MinMaxInt(pDX, m_nKPI3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPP2, m_nKPP2);
	DDV_MinMaxInt(pDX, m_nKPP2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_KPP3, m_nKPP3);
	DDV_MinMaxInt(pDX, m_nKPP3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_MO_SPEED2, m_nMoSpeed2);
	DDV_MinMaxInt(pDX, m_nMoSpeed2, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_MO_SPEED3, m_nMoSpeed3);
	DDV_MinMaxInt(pDX, m_nMoSpeed3, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_POINTS2, m_nPoints2);
	DDV_MinMaxInt(pDX, m_nPoints2, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_POINTS3, m_nPoints3);
	DDV_MinMaxInt(pDX, m_nPoints3, 0, 2147483647);
	DDX_Text(pDX, IDC_EDIT_PWM2, m_nPWMVal2);
	DDV_MinMaxInt(pDX, m_nPWMVal2, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_PWM3, m_nPWMVal3);
	DDV_MinMaxInt(pDX, m_nPWMVal3, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_SET_MO_SP2, m_nSetMoSP2);
	DDV_MinMaxInt(pDX, m_nSetMoSP2, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_SET_MO_SP3, m_nSetMoSP3);
	DDV_MinMaxInt(pDX, m_nSetMoSP3, -100000000, 100000000);
	DDX_Text(pDX, IDC_EDIT_POSITION_1, m_dc_motor_position1);
	DDX_Text(pDX, IDC_EDIT_POSITION_2, m_dc_motor_position2);
	DDX_Text(pDX, IDC_EDIT_POSITION_3, m_dc_motor_position3);
	DDX_Text(pDX, IDC_EDIT_POSITION_VAL, m_SetDCMotorPosition);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CCurveTestDlg, CDialog)
	//{{AFX_MSG_MAP(CCurveTestDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON_START_MO, OnButtonStartMo)
	ON_BN_CLICKED(IDC_BUTTON_OPEN_COM, OnButtonOpenCom)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP, OnButtonStopCap)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MO, OnButtonStopMo)
	ON_BN_CLICKED(IDC_BUTTON_CLOSE_COM, OnButtonCloseCom)
	ON_BN_CLICKED(IDC_BUTTON_START_CAP, OnButtonStartCap)
	ON_BN_CLICKED(IDC_BUTTON_SET_SP, OnButtonSetSp)
	ON_BN_CLICKED(IDC_BUTTON_SET_PID, OnButtonSetPid)
	ON_BN_CLICKED(IDC_BUTTON_SAVE_DATA, OnButtonSaveData)
	ON_WM_CTLCOLOR()
	ON_BN_CLICKED(IDC_BUTTON_REPORT, OnButtonReport)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BUTTON_START_CAP2, OnButtonStartCap2)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP2, OnButtonStopCap2)
	ON_BN_CLICKED(IDC_BUTTON_START_CAP3, OnButtonStartCap3)
	ON_BN_CLICKED(IDC_BUTTON_STOP_CAP3, OnButtonStopCap3)
	ON_BN_CLICKED(IDC_BUTTON_SET_PID2, OnButtonSetPid2)
	ON_BN_CLICKED(IDC_BUTTON_SET_PID3, OnButtonSetPid3)
	ON_BN_CLICKED(IDC_BUTTON_SET_SP2, OnButtonSetSp2)
	ON_BN_CLICKED(IDC_BUTTON_SET_SP3, OnButtonSetSp3)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MO2, OnButtonStopMo2)
	ON_BN_CLICKED(IDC_BUTTON_START_MO2, OnButtonStartMo2)
	ON_BN_CLICKED(IDC_BUTTON_START_MO3, OnButtonStartMo3)
	ON_BN_CLICKED(IDC_BUTTON_STOP_MO3, OnButtonStopMo3)
	ON_BN_CLICKED(IDC_BUTTON_SET_PID4, OnButtonSetPid4)
	ON_EN_CHANGE(IDC_EDIT_KP, OnChangeEditKp)
	ON_EN_CHANGE(IDC_EDIT_KP2, OnChangeEditKp2)
	ON_EN_CHANGE(IDC_EDIT_KP3, OnChangeEditKp3)
	ON_EN_CHANGE(IDC_EDIT_KPD, OnChangeEditKpd)
	ON_EN_CHANGE(IDC_EDIT_KPD2, OnChangeEditKpd2)
	ON_EN_CHANGE(IDC_EDIT_KPD3, OnChangeEditKpd3)
	ON_EN_CHANGE(IDC_EDIT_KPI, OnChangeEditKpi)
	ON_EN_CHANGE(IDC_EDIT_KPI2, OnChangeEditKpi2)
	ON_EN_CHANGE(IDC_EDIT_KPI3, OnChangeEditKpi3)
	ON_EN_CHANGE(IDC_EDIT_KPP, OnChangeEditKpp)
	ON_EN_CHANGE(IDC_EDIT_KPP2, OnChangeEditKpp2)
	ON_EN_CHANGE(IDC_EDIT_KPP3, OnChangeEditKpp3)
	ON_EN_CHANGE(IDC_EDIT_SET_MO_SP, OnChangeEditSetMoSp)
	ON_EN_CHANGE(IDC_EDIT_SET_MO_SP2, OnChangeEditSetMoSp2)
	ON_EN_CHANGE(IDC_EDIT_SET_MO_SP3, OnChangeEditSetMoSp3)
	ON_BN_CLICKED(IDC_BUTTON_POSITION_LOOP, OnButtonPositionLoop)
	ON_BN_CLICKED(IDC_BUTTON_GET_PARAM, OnButtonGetParam)
	ON_BN_CLICKED(IDC_CHECK_POSITION_LOOP, OnCheckPositionLoop)
	ON_EN_CHANGE(IDC_EDIT_POSITION_VAL, OnChangeEditPositionVal)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCurveTestDlg message handlers

BOOL CCurveTestDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	CString str;

	m_Values = new TDataPoint*[DATA_NUM];
	m_Values2 = new TDataPoint*[DATA_NUM];
	m_Values3 = new TDataPoint*[DATA_NUM];

	for(int i = 0; i < DATA_NUM; ++i)
	{
		m_Values[i] = new TDataPoint[VALCOUNT];
		m_Values2[i] = new TDataPoint[VALCOUNT];
		m_Values3[i] = new TDataPoint[VALCOUNT];
	}

	// start thread for data process
	if (!(m_SerialThread = AfxBeginThread(DataThread, this)))
		return FALSE;

	m_queue.init_queue();

	DisableButtuon(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP2))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP3))->EnableWindow(FALSE);

	// 从配置文件中读取设定值
	GetCurrentDirectory (MAX_PATH, m_cConfigFilePath);
	// config.ini放在程序目录下
	strcat(m_cConfigFilePath, "\\config.ini");
    int nRet = GetParametersFromInifile(m_cConfigFilePath);
    if(nRet != 0)
    {
        MessageBox("Get Parameter From ini file error!");
        return FALSE;
    }

	str.Format("PID graph display - %s[%s]", m_soft_version, __DATE__);
	this->SetWindowText(str);

	// set timer
	SetTimer(1, 100, NULL);

	return TRUE;  // return TRUE  unless you set the focus to a control
}

//
// 将当前对话框中的数据保存到配置文件中
//
void CCurveTestDlg::SavePIDParamToIniFile(char* path)
{
	CString str;

	UpdateData(TRUE);
	// 保存COM口号
	m_port_num = ((CComboBox*)GetDlgItem(IDC_COMBO_SERIAL))->GetCurSel();
	str.Format("%d", m_port_num + 1);
	WritePrivateProfileString(  "COM_NUM",
								"COM",
								str,
								path);

	//-----------------------MOTOR1-------------------------
	// PID参数KP
	str.Format("%d", m_nKP);
	WritePrivateProfileString(
								"PID_PARAM",
								"KP",
								str,
								path);

	// PID参数KPP
	str.Format("%d", m_nKPP);
	WritePrivateProfileString(
								"PID_PARAM",
								"KPP",
								str,
								path);

	// PID参数KPI
	str.Format("%d", m_nKPI);
	WritePrivateProfileString(
								"PID_PARAM",
								"KPI",
								str,
								path);

	// PID参数KPD
	str.Format("%d", m_nKPD);
	WritePrivateProfileString(
								"PID_PARAM",
								"KPD",
								str,
								path);

	// 电机转速RPM
	str.Format("%d", m_nSetMoSP);
	WritePrivateProfileString(
								"PID_PARAM",
								"MORPM",
								str,
								path);

	//-----------------------MOTOR2-------------------------
	// PID参数KP
	str.Format("%d", m_nKP2);
	WritePrivateProfileString(
								"PID2_PARAM",
								"KP",
								str,
								path);

	// PID参数KPP
	str.Format("%d", m_nKPP2);
	WritePrivateProfileString(
								"PID2_PARAM",
								"KPP",
								str,
								path);

	// PID参数KPI
	str.Format("%d", m_nKPI2);
	WritePrivateProfileString(
								"PID2_PARAM",
								"KPI",
								str,
								path);

	// PID参数KPD
	str.Format("%d", m_nKPD2);
	WritePrivateProfileString(
								"PID2_PARAM",
								"KPD",
								str,
								path);

	// 电机转速RPM
	str.Format("%d", m_nSetMoSP2);
	WritePrivateProfileString(
								"PID3_PARAM",
								"MORPM",
								str,
								path);

	//-----------------------MOTOR3-------------------------
	// PID参数KP
	str.Format("%d", m_nKP3);
	WritePrivateProfileString(
								"PID3_PARAM",
								"KP",
								str,
								path);

	// PID参数KPP
	str.Format("%d", m_nKPP3);
	WritePrivateProfileString(
								"PID3_PARAM",
								"KPP",
								str,
								path);

	// PID参数KPI
	str.Format("%d", m_nKPI3);
	WritePrivateProfileString(
								"PID3_PARAM",
								"KPI",
								str,
								path);

	// PID参数KPD
	str.Format("%d", m_nKPD3);
	WritePrivateProfileString(
								"PID3_PARAM",
								"KPD",
								str,
								path);

	// 电机转速RPM
	str.Format("%d", m_nSetMoSP3);
	WritePrivateProfileString(
								"PID3_PARAM",
								"MORPM",
								str,
								path);

    /* save DC motor param */
	str.Format("%d", m_max_pwm);
	WritePrivateProfileString(
								"DC_MOTOR_PARAM",
								"max_pwm",
								str,
								path);

	str.Format("%d", m_encoder_number);
	WritePrivateProfileString(
								"DC_MOTOR_PARAM",
								"encoder_number",
								str,
								path);

	str.Format("%d", m_dc_motor_reduction_ratio);
	WritePrivateProfileString(
								"DC_MOTOR_PARAM",
								"reduction_ratio",
								str,
								path);

}


int CCurveTestDlg::GetParametersFromInifile(char* path)
{
	int  Temple;
	char TempString [128] = {0};

	/* Get software version */
	GetPrivateProfileString( "Software_version", "version", "Default Version", TempString, 128, path );
	m_soft_version = TempString;

	/* Get COM number */
	Temple = GetPrivateProfileInt("COM_NUM", "COM", 0xffffffff, path);
	if (Temple == 0xffffffff)
		return Temple;
	else
		m_port_num = Temple - 1;

	((CComboBox*)GetDlgItem(IDC_COMBO_SERIAL))->SetCurSel(m_port_num);

	//-------------------------- MOTOR1 -----------------------------
	/* Get PID param */
	Temple = GetPrivateProfileInt("PID_PARAM", "KP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKP = Temple;

	Temple = GetPrivateProfileInt("PID_PARAM", "KPP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPP = Temple;

	Temple = GetPrivateProfileInt("PID_PARAM", "KPI", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPI = Temple;

	Temple = GetPrivateProfileInt("PID_PARAM", "KPD", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPD = Temple;

	Temple = GetPrivateProfileInt("PID_PARAM", "MORPM", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nSetMoSP = Temple;

	//-------------------------- MOTOR2 -----------------------------
	/* Get PID param */
	Temple = GetPrivateProfileInt("PID2_PARAM", "KP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKP2 = Temple;

	Temple = GetPrivateProfileInt("PID2_PARAM", "KPP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPP2 = Temple;

	Temple = GetPrivateProfileInt("PID2_PARAM", "KPI", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPI2 = Temple;

	Temple = GetPrivateProfileInt("PID2_PARAM", "KPD", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPD2 = Temple;

	Temple = GetPrivateProfileInt("PID2_PARAM", "MORPM", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nSetMoSP2 = Temple;


	//-------------------------- MOTOR3 -----------------------------
	/* Get PID param */
	Temple = GetPrivateProfileInt("PID3_PARAM", "KP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKP3 = Temple;

	Temple = GetPrivateProfileInt("PID3_PARAM", "KPP", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPP3 = Temple;

	Temple = GetPrivateProfileInt("PID3_PARAM", "KPI", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPI3 = Temple;

	Temple = GetPrivateProfileInt("PID3_PARAM", "KPD", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nKPD3 = Temple;

	Temple = GetPrivateProfileInt("PID3_PARAM", "MORPM", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_nSetMoSP3 = Temple;

    /* Get DC motor param */
	Temple = GetPrivateProfileInt("DC_MOTOR_PARAM", "max_pwm", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_max_pwm = Temple;

    m_position_loop_progress.SetRange32(0, m_max_pwm);

    Temple = GetPrivateProfileInt("DC_MOTOR_PARAM", "encoder_number", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_encoder_number = Temple;

    Temple = GetPrivateProfileInt("DC_MOTOR_PARAM", "reduction_ratio", 0xffffffff, path);
    if(Temple == 0xffffffff)
        return Temple;
    else
        m_dc_motor_reduction_ratio = Temple;

    // 更新变量的值到控件上
	UpdateData(FALSE);
    return 0;
}


void CCurveTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CCurveTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CCurveTestDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

//
// 字符串分割函数
//
void CCurveTestDlg::DecodeCString(CString source,  CStringArray& dest, char division)
{
	dest.RemoveAll();
	int nCount = 0;

	for(int i = 0;i < source.GetLength(); i++)
	{
		if(source.GetAt(i)== division)
		{
			nCount = i;
			dest.Add(source.Left(i));
			int nIndex = dest.GetSize()-1;
			for(int j = 0; j < nIndex; j++)
			{
				dest[nIndex] = dest[nIndex].Right(dest[nIndex].GetLength() - dest[j].GetLength() - 1);
			}
		}
	}

	dest.Add(source);
	int nIndex2 = dest.GetSize() - 1;

	for(int j = 0; j < nIndex2; j++)
	{
		dest[nIndex2] = dest[nIndex2].Right(dest[nIndex2].GetLength()-dest[j].GetLength()-1);
	}
}


void CCurveTestDlg::OnTimer(UINT nIDEvent)
{
    CString str;
	static int value_backup[5] = {-1};
	UINT pwm = abs(m_nPWMVal);

	switch (nIDEvent)
	{
	case 1:
        // speed
		m_usbSpeed = m_data_speed*10;

        // PWM duty cycle protect
        if (m_max_pwm == 0)
            m_max_pwm = 1;
		if (pwm > m_max_pwm)
			pwm = m_max_pwm;
		if (pwm != value_backup[0])
		{
			value_backup[0] = pwm;
			str.Format("[%d,%d][%.1f%%]", pwm, m_max_pwm, pwm*100.0/m_max_pwm);
			GetDlgItem(IDC_PWM_DUTY_CYCLE)->SetWindowText(str);
            m_position_loop_progress.SetPos(pwm);
		}

        // 将成员的值赋给控件
		//(GetDlgItem(IDC_EDIT_USB_SPEED))->UpdateData(FALSE);
		//(GetDlgItem(IDC_EDIT_MO_SPEED))->UpdateData(FALSE);
		//(GetDlgItem(IDC_EDIT_PWM))->UpdateData(FALSE);
		//(GetDlgItem(IDC_EDIT_POSITION_1))->UpdateData(FALSE);
		//(GetDlgItem(IDC_EDIT_POINTS))->UpdateData(FALSE);
		UpdateData(FALSE);
		m_data_speed = 0;
		break;
	default:
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

//
//  处理串口数据的线程
//
UINT CCurveTestDlg::DataThread(LPVOID pParam)
{
	long len;
	char vl_buf[1024];
	UINT i;
	// Cast the void pointer passed to the thread back to
	// a pointer of CSerialPort class
	CCurveTestDlg *port = (CCurveTestDlg*)pParam;

	while(1)
	{
		len = sio_iqueue(port->m_port_num);
		if (len > 0)
		{
			len = sio_read(port->m_port_num, vl_buf, 1024);
			// 这里有性能瓶颈，可以考虑将数据存在BUFFER中，然后开一个线程单独处理
			for (i = 0; i < len; i++)
			{
				port->DoCMDData(vl_buf[i]);
			}
		}
		else
		{
			Sleep(10);
		}
	}
	return 0;
}



void CCurveTestDlg::DoCMDData(UINT ch)
{
	static int flag = 0;
	static int pos = 0;
	int n;
	CString str;
	CStringArray arry;
	int     setpoint;
	++m_data_speed;
	++m_usb_data_size;

	//TRACE("[%.2x-%c] ", ch, ch);
	//return;

	//R1 1500 36 1000 1001;
	//A1 3000 512 1006 0;
	if (ch == ';')
	{
		m_recv_buf[pos] = 0;
		str = m_recv_buf;

        // deal with the cmd
        DecodeCString(str, arry, ' ');
        n = arry.GetSize();
        if (n == 6)
        {
            str = arry.GetAt(0);
            if (str.Compare("A1") == 0)
            {
                // MAX pwm value
                str = arry.GetAt(1);
                m_max_pwm = atoi(str);
                // encoder number
                str = arry.GetAt(2);
                m_encoder_number = atoi(str);
                // dc motor reduction ratio
                str = arry.GetAt(3);
                m_dc_motor_reduction_ratio = atoi(str);

                //-------- set the new parameter ---------
                m_position_loop_progress.SetRange32(0, m_max_pwm);
            }
            else if (str.Compare("R1") == 0)
            {
                str = arry.GetAt(1);
                m_nMoSpeed = atoi(str);
                str = arry.GetAt(2);
                m_nPWMVal  = atoi(str);
                m_dc_motor_position1 = arry.GetAt(3);
                str = arry.GetAt(4);
                setpoint = atoi(str);
                str = arry.GetAt(5);
                m_nVoltage = atoi(str);
                if (m_data_points >= VALCOUNT || m_cap_flag == FALSE)
                {
                    pos = 0;
                    return;
                }

                //store the data point and display in the graph
                m_Values[0][m_data_points].fXVal = m_data_points;
                m_Values[0][m_data_points].fYVal = m_nMoSpeed;
                m_Values[1][m_data_points].fXVal = m_data_points;
                m_Values[1][m_data_points].fYVal = m_nPWMVal;
                m_Values[2][m_data_points].fXVal = m_data_points;
                m_Values[2][m_data_points].fYVal = m_nSetMoSP;
                m_Values[3][m_data_points].fXVal = m_data_points;
                m_Values[3][m_data_points].fYVal = m_nVoltage;
                m_Values[4][m_data_points].fXVal = m_data_points;
                m_Values[4][m_data_points].fYVal = atoi(m_dc_motor_position1);
				m_Values[5][m_data_points].fXVal = m_data_points;
                m_Values[5][m_data_points].fYVal = setpoint;
                ++m_data_points;
                m_nPoints = m_data_points;
            }
            else if (str.Compare("R2") == 0)
            {
                str = arry.GetAt(1);
                m_nMoSpeed2 = atoi(str);
                str = arry.GetAt(2);
                m_nPWMVal2  = atoi(str);
                m_dc_motor_position2 = arry.GetAt(3);
                str = arry.GetAt(4);
                setpoint = atoi(str);
                if (m_data_points2 >= VALCOUNT || m_cap_flag2 == FALSE)
                {
                    pos = 0;
                    return;
                }

                //store the data point
                m_Values2[0][m_data_points2].fXVal = m_data_points2;
                m_Values2[0][m_data_points2].fYVal = m_nMoSpeed2;
                m_Values2[1][m_data_points2].fXVal = m_data_points2;
                m_Values2[1][m_data_points2].fYVal = m_nPWMVal2;
                m_Values2[2][m_data_points2].fXVal = m_data_points2;
                m_Values2[2][m_data_points2].fYVal = m_nSetMoSP2;
                m_Values2[3][m_data_points2].fXVal = m_data_points2;
                m_Values2[3][m_data_points2].fYVal = atoi(m_dc_motor_position2);
                m_Values2[4][m_data_points].fXVal = m_data_points;
                m_Values2[4][m_data_points].fYVal = setpoint;
                ++m_data_points2;
                m_nPoints2 = m_data_points2;
            }
            else if (str.Compare("R3") == 0)
            {
                str = arry.GetAt(1);
                m_nMoSpeed3 = atoi(str);
                str = arry.GetAt(2);
                m_nPWMVal3  = atoi(str);
                m_dc_motor_position3 = arry.GetAt(3);
                str = arry.GetAt(4);
                setpoint = atoi(str);
                if (m_data_points3 >= VALCOUNT || m_cap_flag3 == FALSE)
                {
                    pos = 0;
                    return;
                }

                //store the data point
                m_Values3[0][m_data_points3].fXVal = m_data_points3;
                m_Values3[0][m_data_points3].fYVal = m_nMoSpeed3;
                m_Values3[1][m_data_points3].fXVal = m_data_points3;
                m_Values3[1][m_data_points3].fYVal = m_nPWMVal3;
                m_Values3[2][m_data_points3].fXVal = m_data_points3;
                m_Values3[2][m_data_points3].fYVal = m_nSetMoSP3;
                m_Values2[3][m_data_points3].fXVal = m_data_points3;
                m_Values2[3][m_data_points3].fYVal = atoi(m_dc_motor_position3);
                m_Values3[4][m_data_points].fXVal = m_data_points;
                m_Values3[4][m_data_points].fYVal = setpoint;
                ++m_data_points3;
                m_nPoints3 = m_data_points3;
            }
        }

        pos = 0;
	}
	else if (pos < 128)
	{
		m_recv_buf[pos++] = (char)ch;
	}
	else
	{
		pos = 0;
		TRACE("CMD ERROR\n");
	}
}


void CCurveTestDlg::DisableButtuon(BOOL bSet)
{
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP2))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP3))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_START_MO))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_START_MO2))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_START_MO3))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_MO))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_MO2))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_MO3))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_PID))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_PID2))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_PID3))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_PID4))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_SP))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_SP2))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SET_SP3))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_SAVE_DATA))->EnableWindow(bSet);
	((CButton*)GetDlgItem(IDC_BUTTON_REPORT))->EnableWindow(bSet);
    ((CButton*)GetDlgItem(IDC_BUTTON_GET_PARAM))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KP))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPP))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPI))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPD))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KP2))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPP2))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPI2))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPD2))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KP3))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPP3))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPI3))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_KPD3))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_SET_MO_SP))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_SET_MO_SP2))->EnableWindow(bSet);
	((CEdit*)GetDlgItem(IDC_EDIT_SET_MO_SP3))->EnableWindow(bSet);
	((CCheckListBox*)GetDlgItem(IDC_CHECK_POSITION_LOOP))->EnableWindow(bSet);
}



//
// 打开串口
//
void CCurveTestDlg::OnButtonOpenCom()
{
	int nIndex;
	CString str;
	nIndex = ((CComboBox*)GetDlgItem(IDC_COMBO_SERIAL))->GetCurSel();
	if (nIndex == -1)
	{
		MessageBox("请选择串口", "警告", MB_ICONWARNING | MB_OK);
	}
	else
	{
		m_port_num = nIndex + 1;
		if (sio_open(m_port_num) == SIO_OK)
		{
			if (sio_ioctl (m_port_num, B115200, BIT_8 | STOP_1 | P_NONE) == SIO_OK)
			{
				TRACE("COM%d打开成功！\n", nIndex + 1);
				((CButton*)GetDlgItem(IDC_BUTTON_OPEN_COM))->EnableWindow(FALSE);
				GetDlgItem(IDC_COMBO_SERIAL)->EnableWindow(FALSE);
				((CButton*)GetDlgItem(IDC_BUTTON_CLOSE_COM))->EnableWindow(TRUE);
				str.Format("COM%d打开成功！", nIndex + 1);
				DisableButtuon(TRUE);
				// 清空串口发送和接收缓存
				sio_flush(m_port_num, 2);
			}
			else
			{
				MessageBox("串口配置失败", "Error", MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			MessageBox("串口打开失败", "Error", MB_ICONERROR | MB_OK);
		}
	}
}

//
//  TODO: 关闭串口
//
void CCurveTestDlg::OnButtonCloseCom()
{
	CString str;
	int nRet;

	nRet = sio_close(m_port_num);
	if (nRet != SIO_OK)
	{
		str.Format("关闭串口%d成功！", m_port_num);
		MessageBox(str);
		GetDlgItem(IDC_COMBO_SERIAL)->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_OPEN_COM))->EnableWindow(TRUE);
		((CButton*)GetDlgItem(IDC_BUTTON_CLOSE_COM))->EnableWindow(FALSE);
	}
	else
	{
		str.Format("关闭串口%d失败！", m_port_num);
		MessageBox(str);
	}
}


//
// 将当前的PID参数等信息存入配置文件config.ini中
//
void CCurveTestDlg::OnButtonSaveData()
{
	SavePIDParamToIniFile(m_cConfigFilePath);
	MessageBox("保存配置文件成功！", "Save", MB_ICONINFORMATION | MB_OK);
}



HBRUSH CCurveTestDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
{
	HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	//  TODO: Change any attributes of the DC here
//	if( pWnd->GetDlgCtrlID()  == IDC_COM_SHUOMING)
//		pDC->SetTextColor(RGB(255, 0, 0));

//	if (pWnd->GetDlgCtrlID() == IDC_EDIT_MSG)
//		pDC->SetTextColor(RGB(0x00, 0x00, 0xCD));

	//  TODO: Return a different brush if the default is not desired
	return hbr;
}


//
// 将当前的PID参数生成报告
// 一般用于参数已经调整OK
//
void CCurveTestDlg::OnButtonReport()
{
	double P, I, D;
	SYSTEMTIME systm;
	CString str, file_name;
	int nRet;
	CGetPIDDescription dlg;

	UpdateData(FALSE);

	nRet = dlg.DoModal();
	switch (nRet)
	{
		case IDOK:
			str = dlg.m_GetPIDDesc;
			break;
		default:
			return;
	}

	GetLocalTime(&systm);
	file_name.Format("PID_%d-%d-%d_%d-%d-%d.txt", systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond);
	FILE *fp = fopen(file_name,"w+");
	fprintf(fp,"/********************************************************\n");
	fprintf(fp," * PID参数调节报告\n");
	fprintf(fp," * 生成时间：%d-%d-%d %d:%d:%d\n", systm.wYear, systm.wMonth, systm.wDay, systm.wHour, systm.wMinute, systm.wSecond);
	fprintf(fp," * 说明：%s\n", str);
	fprintf(fp," * (C) COPYRIGHT 2013 VINY\n");
	fprintf(fp," ********************************************************/\n\n\n");
	fprintf(fp,"电机设定速度：           %d\n", m_nSetMoSP);
	P = m_nKP/1000.0 * m_nKPP / 1000.0;
	I = m_nKP/1000.0 * m_nKPI / 1000.0;
	D = m_nKP/1000.0 * m_nKPD / 1000.0;
	fprintf(fp,"P[%.5f], I[%.5f], D[%.5f]\n", P, I, D);

	fclose(fp);

	str.Format("%s生成成功！", file_name);
	MessageBox(str, "注意", MB_ICONINFORMATION | MB_OK);
}


//
// 开始采集数据1
//
//
void CCurveTestDlg::OnButtonStartCap()
{
	m_cap_flag = TRUE;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP))->EnableWindow(FALSE);
}


//
// 停止采集数据1
// 这里将收到的有效数据帧进行处理，然后在一个对话框中显示。
//
void CCurveTestDlg::OnButtonStopCap()
{
	double kp, ki, kd;

	CString str;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP))->EnableWindow(TRUE);

	m_cap_flag = FALSE;

	if (m_data_points > 0)
	{
		// 将当前的PID参数显示在标题栏上
		kp = (m_nKP/1000.0) * (m_nKPP/1000.0);
		ki = (m_nKP/1000.0) * (m_nKPI/1000.0);
		kd = (m_nKP/1000.0) * (m_nKPD/1000.0);
		str.Format("KPP[%.5f] - KPI[%.5f] - KPD[%.5f]", kp, ki, kd);

		CPIDDlg *dlg = new CPIDDlg(m_Values, DATA_NUM, m_data_points, str);
		dlg->Create(IDD_DIALOG_PID, this);
		dlg->ShowWindow(SW_SHOW);
		m_data_points = 0;
	}
}

//
// 开始采集数据2
//
void CCurveTestDlg::OnButtonStartCap2()
{
	m_cap_flag2 = TRUE;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP2))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP2))->EnableWindow(FALSE);
}


//
// 停止采集数据2，STM32中PID算法还在运行。
// 这里将收到的有效数据帧进行处理，然后在一个对话框中显示。
//
void CCurveTestDlg::OnButtonStopCap2()
{
	double kp, ki, kd;

	CString str;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP2))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP2))->EnableWindow(TRUE);

	m_cap_flag2 = FALSE;

	if (m_data_points2 > 0)
	{
		// 将当前的PID参数显示在标题栏上
		kp = (m_nKP2/1000.0) * (m_nKPP2/1000.0);
		ki = (m_nKP2/1000.0) * (m_nKPI2/1000.0);
		kd = (m_nKP2/1000.0) * (m_nKPD2/1000.0);
		str.Format("KPP[%.5f] - KPI[%.5f] - KPD[%.5f]", kp, ki, kd);

		CPIDDlg *dlg = new CPIDDlg(m_Values2, 3, m_data_points2, str);
		dlg->Create(IDD_DIALOG_PID, this);
		dlg->ShowWindow(SW_SHOW);
		m_data_points2 = 0;
	}
}

//
// 开始采集数据3
//
void CCurveTestDlg::OnButtonStartCap3()
{
	m_cap_flag3 = TRUE;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP3))->EnableWindow(TRUE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP3))->EnableWindow(FALSE);
}

//
// 停止采集数据3，STM32中PID算法还在运行。
// 这里将收到的有效数据帧进行处理，然后在一个对话框中显示。
//
void CCurveTestDlg::OnButtonStopCap3()
{
	double kp, ki, kd;

	CString str;
	((CButton*)GetDlgItem(IDC_BUTTON_STOP_CAP3))->EnableWindow(FALSE);
	((CButton*)GetDlgItem(IDC_BUTTON_START_CAP3))->EnableWindow(TRUE);

	m_cap_flag3 = FALSE;

	if (m_data_points3 > 0)
	{
		// 将当前的PID参数显示在标题栏上
		kp = (m_nKP3/1000.0) * (m_nKPP3/1000.0);
		ki = (m_nKP3/1000.0) * (m_nKPI3/1000.0);
		kd = (m_nKP3/1000.0) * (m_nKPD3/1000.0);
		str.Format("KPP[%.5f] - KPI[%.5f] - KPD[%.5f]", kp, ki, kd);

		CPIDDlg *dlg = new CPIDDlg(m_Values3, 3, m_data_points3, str);
		dlg->Create(IDD_DIALOG_PID, this);
		dlg->ShowWindow(SW_SHOW);
		m_data_points3 = 0;
	}
}


//
// 串口发送数据
//
void CCurveTestDlg::serial_send_cmd_data(char* buf, UINT len)
{
	int nRet;

	nRet = sio_write(m_port_num, buf, len);
	if (nRet < 0)
	{
		MessageBox("发送数据失败！");
	}
}

/*
 *---------------------------------------------------------------
 * S      设置PID参数
 * 10     KP参数，实际为10/1000 = 0.01
 * 1150   KPP参数，实际运算为1.15*KP
 * 110    KPI参数，实际运算为0.11*KP
 * 0      KPD参数，实际运算为0*KP
 * 2000   电机转速，单位为RPM（每分钟转速）
 *
 * example:
 * S1 10 1150 110 0 2000;
 */
void CCurveTestDlg::OnButtonSetPid()
{
	CString str;

	UpdateData(TRUE);
	str.Format("S1 %d %d %d %d %d;",
					m_nKP,
					m_nKPP,
					m_nKPI,
					m_nKPD,
					m_nSetMoSP);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

void CCurveTestDlg::OnButtonSetPid2()
{
	CString str;

	UpdateData(TRUE);
	str.Format("S2 %d %d %d %d %d;",
				m_nKP2,
				m_nKPP2,
				m_nKPI2,
				m_nKPD2,
				m_nSetMoSP2);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

void CCurveTestDlg::OnButtonSetPid3()
{
	CString str;

	UpdateData(TRUE);
	str.Format("S3 %d %d %d %d %d;",
		m_nKP3,
		m_nKPP3,
		m_nKPI3,
		m_nKPD3,
		m_nSetMoSP3);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

//
// 设置所有电机PID参数
//
void CCurveTestDlg::OnButtonSetPid4()
{
	CString str;

	UpdateData(TRUE);
	str.Format("S1 %d %d %d %d %d;S2 %d %d %d %d %d;S3 %d %d %d %d %d;",
		m_nKP,
		m_nKPP,
		m_nKPI,
		m_nKPD,
		m_nSetMoSP,
		m_nKP2,
		m_nKPP2,
		m_nKPI2,
		m_nKPD2,
		m_nSetMoSP2,
		m_nKP3,
		m_nKPP3,
		m_nKPI3,
		m_nKPD3,
		m_nSetMoSP3);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

//
// 设置电机速度1
// P1 2000;
//
void CCurveTestDlg::OnButtonSetSp()
{
	CString str;

	UpdateData(TRUE);
	str.Format("P1 %d;", m_nSetMoSP);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

//
// 设置电机速度2
// P2 2000;
//
void CCurveTestDlg::OnButtonSetSp2()
{
	CString str;

	UpdateData(TRUE);
	str.Format("M1 %d;", m_nSetMoSP2);

	MessageBox(str);
	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

//
// 设置电机速度3
// P3 2000;
//
void CCurveTestDlg::OnButtonSetSp3()
{
	CString str;

	UpdateData(TRUE);
	str.Format("P3 %d;", m_nSetMoSP3);

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}


//
// 打开电机1
// B1;
//
void CCurveTestDlg::OnButtonStartMo()
{
	CString str = "B1;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

//
// 停止电机1
// T1;
//
void CCurveTestDlg::OnButtonStopMo()
{
	CString str = "T1;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}


void CCurveTestDlg::OnButtonStartMo2()
{
	CString str = "B2;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

void CCurveTestDlg::OnButtonStopMo2()
{
	CString str = "T2;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}



void CCurveTestDlg::OnButtonStartMo3()
{
	CString str = "B3;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

void CCurveTestDlg::OnButtonStopMo3()
{
	CString str = "T3;";

	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}

void CCurveTestDlg::set_edit_text(int nID)
{
	CString str;

	(GetDlgItem(nID))->GetWindowText(str);
	if (str.IsEmpty())
	{
		(GetDlgItem(nID))->SetWindowText("0");
	}
	UpdateData(TRUE);
}


//
// 假如文本框中的内容都被清除，需要显示为0，否则UpDateData()函数会出问题
//
void CCurveTestDlg::OnChangeEditKp()
{
	set_edit_text(IDC_EDIT_KP);
}

void CCurveTestDlg::OnChangeEditKp2()
{

	set_edit_text(IDC_EDIT_KP2);
}

void CCurveTestDlg::OnChangeEditKp3()
{
	set_edit_text(IDC_EDIT_KP3);
}

void CCurveTestDlg::OnChangeEditKpd()
{
	set_edit_text(IDC_EDIT_KPD);
}

void CCurveTestDlg::OnChangeEditKpd2()
{
	set_edit_text(IDC_EDIT_KPD2);
}

void CCurveTestDlg::OnChangeEditKpd3()
{
	set_edit_text(IDC_EDIT_KPD3);
}

void CCurveTestDlg::OnChangeEditKpi()
{
	set_edit_text(IDC_EDIT_KPI);
}

void CCurveTestDlg::OnChangeEditKpi2()
{
	set_edit_text(IDC_EDIT_KPI2);
}

void CCurveTestDlg::OnChangeEditKpi3()
{
	set_edit_text(IDC_EDIT_KPI3);
}

void CCurveTestDlg::OnChangeEditKpp()
{
	set_edit_text(IDC_EDIT_KPP);
}

void CCurveTestDlg::OnChangeEditKpp2()
{
	set_edit_text(IDC_EDIT_KPP2);
}

void CCurveTestDlg::OnChangeEditKpp3()
{
	set_edit_text(IDC_EDIT_KPP3);
}

void CCurveTestDlg::OnChangeEditSetMoSp()
{
	set_edit_text(IDC_EDIT_SET_MO_SP);
}

void CCurveTestDlg::OnChangeEditSetMoSp2()
{
	set_edit_text(IDC_EDIT_SET_MO_SP2);
}

void CCurveTestDlg::OnChangeEditSetMoSp3()
{
	set_edit_text(IDC_EDIT_SET_MO_SP3);
}

void CCurveTestDlg::OnChangeEditPositionVal()
{
	set_edit_text(IDC_EDIT_POSITION_VAL);
}

//
// 设置电机位置
//
void CCurveTestDlg::OnButtonPositionLoop()
{
	CString str;

	// 从控件上更新变量值
	//(GetDlgItem(IDC_EDIT_POSITION_VAL))->UpdateData(TRUE);
	UpdateData(TRUE);

	str.Format("W1 %s;", m_SetDCMotorPosition);
	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}



//
// 获取电机相关参数，比如PWM调节范围，编码器分辨率，电机减速比
// G1;		获取1号电机参数
//
void CCurveTestDlg::OnButtonGetParam()
{
	CString str;

	str = "G1;";
	serial_send_cmd_data((LPSTR)(LPCTSTR)str, str.GetLength());
}




void CCurveTestDlg::OnCheckPositionLoop()
{
	if (IsDlgButtonChecked(IDC_CHECK_POSITION_LOOP))
	{
		// 进入位置环+速度环模式
		((CButton*)GetDlgItem(IDC_BUTTON_POSITION_LOOP))->EnableWindow(TRUE);
        ((CEdit*)GetDlgItem(IDC_EDIT_POSITION_VAL))->EnableWindow(TRUE);

	}
	else
	{
		// 进入单纯速度控制模式
		((CButton*)GetDlgItem(IDC_BUTTON_POSITION_LOOP))->EnableWindow(FALSE);
        ((CEdit*)GetDlgItem(IDC_EDIT_POSITION_VAL))->EnableWindow(FALSE);
	}
}


