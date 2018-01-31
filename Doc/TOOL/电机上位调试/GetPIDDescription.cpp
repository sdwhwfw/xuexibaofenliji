// GetPIDDescription.cpp : implementation file
//

#include "stdafx.h"
#include "CurveTest.h"
#include "GetPIDDescription.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CGetPIDDescription dialog


CGetPIDDescription::CGetPIDDescription(CWnd* pParent /*=NULL*/)
	: CDialog(CGetPIDDescription::IDD, pParent)
{
	//{{AFX_DATA_INIT(CGetPIDDescription)
	m_GetPIDDesc = _T("");
	//}}AFX_DATA_INIT
}


void CGetPIDDescription::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CGetPIDDescription)
	DDX_Text(pDX, IDC_EDIT_PID_DESC, m_GetPIDDesc);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CGetPIDDescription, CDialog)
	//{{AFX_MSG_MAP(CGetPIDDescription)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CGetPIDDescription message handlers

void CGetPIDDescription::OnOK() 
{
	UpdateData(TRUE);
	TRACE("m_GetPIDDesc = %s\n", m_GetPIDDesc);
	CDialog::OnOK();
}
