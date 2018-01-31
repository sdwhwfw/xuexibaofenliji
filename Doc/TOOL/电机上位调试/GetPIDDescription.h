#if !defined(AFX_GETPIDDESCRIPTION_H__011750BF_6376_44A6_9C27_F536758C7767__INCLUDED_)
#define AFX_GETPIDDESCRIPTION_H__011750BF_6376_44A6_9C27_F536758C7767__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// GetPIDDescription.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CGetPIDDescription dialog

class CGetPIDDescription : public CDialog
{
// Construction
public:
	CGetPIDDescription(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CGetPIDDescription)
	enum { IDD = IDD_DIALOG_PID_SM };
	CString	m_GetPIDDesc;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CGetPIDDescription)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CGetPIDDescription)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_GETPIDDESCRIPTION_H__011750BF_6376_44A6_9C27_F536758C7767__INCLUDED_)
