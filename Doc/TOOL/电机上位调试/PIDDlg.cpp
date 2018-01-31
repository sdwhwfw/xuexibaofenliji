// PIDDlg.cpp : implementation file
//

#include "stdafx.h"
#include "CurveTest.h"
#include "PIDDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CPIDDlg dialog




#define IDM_SELECT                      40000
#define IDM_INSERTLABEL                 40001
#define IDM_ZOOM                        40002
#define IDM_RESET                       40003
#define IDM_PAN                         40004
#define IDM_PROPERTIES                  40005
#define IDM_LABELDOPROP                 40006
#define IDM_PRINT                       40007
#define IDM_MAGNIFIER                   40008
#define IDM_LINEARTREND                 40009
#define IDM_CUBICTREND                  40010
#define IDM_CURSOR                      40011


CPIDDlg::CPIDDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CPIDDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CPIDDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_data = NULL;
}

CPIDDlg::CPIDDlg(TDataPoint **p_data, int m, int n, CString str)
{
	int i;

	// save data
	m_data_m = m;
	m_data_n = n;
	m_data = new TDataPoint*[m];
	for(i = 0; i< m; ++i)
	{
		m_data[i] = new TDataPoint[n];
		memcpy(m_data[i], p_data[i], n * sizeof(TDataPoint));
	}
	m_sTitle = "PID Param: ";
	m_sTitle += str;
}

CPIDDlg::~CPIDDlg()
{

}

void CPIDDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CPIDDlg)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CPIDDlg, CDialog)
	//{{AFX_MSG_MAP(CPIDDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPIDDlg message handlers

void CPIDDlg::PostNcDestroy()
{
	if (m_data)
	{
		for(int i = 0; i < m_data_m; i++)
			delete []m_data[i];

		delete []m_data;
	}

	delete this;
	CDialog::PostNcDestroy();
}


BOOL CPIDDlg::OnInitDialog()
{
	int i;
	CRect rect;

	CDialog::OnInitDialog();
	GetClientRect(&rect);

	this->SetWindowText(m_sTitle);

	if (!::IsWindow(m_GraphPID.m_hWnd))
	{
		m_GraphPID.Create("XGraph", "", WS_CHILD | WS_VISIBLE, rect, this, 1002);
		m_GraphPID.ResetAll();

		for (i = 0; i < 3; i++)
		{
			m_GraphPID.SetData (m_data[i], m_data_n, i, 0, 0);
		}

        if (m_data_m == 4)
            m_GraphPID.SetData(m_data[3], m_data_n, 3, 0, 1);

        if (m_data_m == 5)
        {
            m_GraphPID.SetData(m_data[3], m_data_n, 3, 0, 1);
            m_GraphPID.SetData(m_data[4], m_data_n, 4, 0, 1);
        }

        if (m_data_m == 6)
        {
            m_GraphPID.SetData(m_data[3], m_data_n, 3, 0, 0);
            m_GraphPID.SetData(m_data[4], m_data_n, 4, 0, 1);
            m_GraphPID.SetData(m_data[5], m_data_n, 5, 0, 1);
        }

		switch (m_data_m)
		{
			case 3:
				m_GraphPID.GetCurve(0).SetLabel("SPEED");
				m_GraphPID.GetCurve(1).SetLabel("PWM");
				m_GraphPID.GetCurve(2).SetLabel("SET_SP");
				break;
            case 4:
				m_GraphPID.GetCurve(0).SetLabel("SPEED");
				m_GraphPID.GetCurve(1).SetLabel("PWM");
				m_GraphPID.GetCurve(2).SetLabel("SET_SP");
                m_GraphPID.GetCurve(3).SetLabel("POSITION");
                break;
            case 5:
                m_GraphPID.GetCurve(0).SetLabel("SPEED");
				m_GraphPID.GetCurve(1).SetLabel("PWM");
				m_GraphPID.GetCurve(2).SetLabel("SET_SP");
                m_GraphPID.GetCurve(3).SetLabel("POSITION");
                m_GraphPID.GetCurve(4).SetLabel("SET_POS");
                break;
            case 6:
                m_GraphPID.GetCurve(0).SetLabel("SPEED");
				m_GraphPID.GetCurve(1).SetLabel("PWM");
				m_GraphPID.GetCurve(2).SetLabel("SET_SP");
                m_GraphPID.GetCurve(3).SetLabel("VOLTAGE");
                m_GraphPID.GetCurve(4).SetLabel("POSITION");
                m_GraphPID.GetCurve(5).SetLabel("SET_POS");
                break;
			default:
				break;
		}

		m_GraphPID.SetShowLegend (TRUE);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


LRESULT CPIDDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == XG_RBUTTONUP)
	{
		CPoint point;
		GetCursorPos(&point);
		ScreenToClient(&point);
		CMenu menu;
		menu.CreatePopupMenu ();
		menu.AppendMenu( MF_STRING, IDM_SELECT, "Select");
		menu.AppendMenu( MF_STRING, IDM_ZOOM, "Zoom");
		menu.AppendMenu( MF_STRING, IDM_RESET, "Reset");
		menu.AppendMenu( MF_STRING, IDM_PAN, "Pan");
		menu.AppendMenu( MF_STRING, IDM_CURSOR, "Cursor");
		menu.AppendMenu( MF_STRING, IDM_INSERTLABEL, "Insert label");
		menu.AppendMenu( MF_STRING, IDM_PRINT, "Print");
//		menu.AppendMenu( MF_SEPARATOR );
//		menu.AppendMenu( MF_STRING, IDM_PROPERTIES, "Properties");
		ClientToScreen(&point);
		UINT nCmd = menu.TrackPopupMenu( TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_NONOTIFY,
										  point.x, point.y ,
										  (CXGraph*)lParam);
	}
	return CWnd::WindowProc(message, wParam, lParam);
}





