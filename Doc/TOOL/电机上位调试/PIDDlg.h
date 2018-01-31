#if !defined(AFX_PIDDLG_H__B5A1AFA2_DC7D_49F6_9FCB_B65F5B3DC65F__INCLUDED_)
#define AFX_PIDDLG_H__B5A1AFA2_DC7D_49F6_9FCB_B65F5B3DC65F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PIDDlg.h : header file
//

#include "xgraph10.h"

/////////////////////////////////////////////////////////////////////////////
// CPIDDlg dialog

class CPIDDlg : public CDialog
{
// Construction
public:
	CPIDDlg(CWnd* pParent = NULL);   // standard constructor
	CPIDDlg(TDataPoint **p_data, int m, int n, CString str);
	~CPIDDlg();

// Dialog Data
	//{{AFX_DATA(CPIDDlg)
	enum { IDD = IDD_DIALOG_PID };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPIDDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	virtual void PostNcDestroy();
	//for XGraph 
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CPIDDlg)
		virtual BOOL OnInitDialog();

	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	TDataPoint** m_data;
	int          m_data_m;
	int          m_data_n;
	CXGraph      m_GraphPID;
	CString      m_sTitle;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PIDDLG_H__B5A1AFA2_DC7D_49F6_9FCB_B65F5B3DC65F__INCLUDED_)
