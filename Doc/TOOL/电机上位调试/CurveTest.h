// CurveTest.h : main header file for the CURVETEST application
//

#if !defined(AFX_CURVETEST_H__15FB956D_64DF_4B10_B45B_4197BCA4274A__INCLUDED_)
#define AFX_CURVETEST_H__15FB956D_64DF_4B10_B45B_4197BCA4274A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CCurveTestApp:
// See CurveTest.cpp for the implementation of this class
//

class CCurveTestApp : public CWinApp
{
public:
	CCurveTestApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCurveTestApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CCurveTestApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CURVETEST_H__15FB956D_64DF_4B10_B45B_4197BCA4274A__INCLUDED_)
