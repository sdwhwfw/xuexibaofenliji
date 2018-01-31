// XCPos.cpp : implementation file
//

#include "stdafx.h"
#include "PMAC数控系统.h"
#include "XCPos.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
#define  PI 3.1415926
extern  char xap[255],cap[255];

/////////////////////////////////////////////////////////////////////////////
// CXCPos

IMPLEMENT_DYNCREATE(CXCPos, CFormView)

CXCPos::CXCPos()
	: CFormView(CXCPos::IDD)
{
	//{{AFX_DATA_INIT(CXCPos)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_newBrush.CreateSolidBrush(RGB(0,0,140));
	m_Font.CreateFont(
		-30,                        // nHeight
		10,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial");                 // lpszFacename
	m_FontTemp.CreateFont(
		-20,                        // nHeight
		10,                         // nWidth
		0,                         // nEscapement
		0,                         // nOrientation
		FW_NORMAL,                 // nWeight
		FALSE,                     // bItalic
		FALSE,                     // bUnderline
		0,                         // cStrikeOut
		ANSI_CHARSET,              // nCharSet
		OUT_DEFAULT_PRECIS,        // nOutPrecision
		CLIP_DEFAULT_PRECIS,       // nClipPrecision
		DEFAULT_QUALITY,           // nQuality
		DEFAULT_PITCH | FF_SWISS,  // nPitchAndFamily
		"Arial");                 // lpszFacename
}

CXCPos::~CXCPos()
{
}

void CXCPos::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	 //{{AFX_DATA_MAP(CXCPos)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CXCPos, CFormView)
	//{{AFX_MSG_MAP(CXCPos)
	ON_WM_CTLCOLOR()
	ON_WM_CREATE()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CXCPos diagnostics

#ifdef _DEBUG
void CXCPos::AssertValid() const
{
	CFormView::AssertValid();
}

void CXCPos::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CXCPos message handlers

HBRUSH CXCPos::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	HBRUSH hbr = CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
	switch(nCtlColor)
	{
	case CTLCOLOR_DLG:
		return m_newBrush;
	case CTLCOLOR_EDIT:
//      pDC->SetBkMode(TRANSPARENT);
		pDC->SetTextColor(RGB(255,0,0));
        return m_newBrush;
	}
	return hbr;
}

void CXCPos::OnInitialUpdate() 
{
	CFormView::OnInitialUpdate();
	m_X.SubclassDlgItem(IDC_X,this);
	m_X.SetBkColor(RGB(0,0,140));
	m_X.SetTextColor(RGB(0,255,0));
	m_X.SetFont(&m_Font);
	
	m_XAP.SubclassDlgItem(IDC_XAP,this);
	m_XAP.SetBkColor(RGB(0,0,140));
	m_XAP.SetTextColor(RGB(255,0,0));
	m_XAP.SetFont(&m_Font);
	
	m_MM.SubclassDlgItem(IDC_MM,this);
	m_MM.SetBkColor(RGB(0,0,140));
	m_MM.SetTextColor(RGB(0,255,0));
	m_MM.SetFont(&m_FontTemp);
	
	m_C.SubclassDlgItem(IDC_C,this);
	m_C.SetBkColor(RGB(0,0,140));
	m_C.SetTextColor(RGB(0,255,0));
	m_C.SetFont(&m_Font);
	
	m_CAP.SubclassDlgItem(IDC_CAP,this);
	m_CAP.SetBkColor(RGB(0,0,140));
	m_CAP.SetTextColor(RGB(255,0,0));
	m_CAP.SetFont(&m_Font);
	
	m_RAD.SubclassDlgItem(IDC_RAD,this);
	m_RAD.SetBkColor(RGB(0,0,140));
	m_RAD.SetTextColor(RGB(0,255,0));
	m_RAD.SetFont(&m_FontTemp);
}

int CXCPos::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;
	this->SetTimer(1,80,NULL);	
	return 0;
}

void CXCPos::OnTimer(UINT nIDEvent) 
{
	CHAR buf1[255],buf2[255];
	
	sprintf(buf1,"%10.3lf",atof(xap)/3072000);//单位:mm,其中3072000=Ix08*32/编码器分辨率＝96*32/1um=3072000
	sprintf(buf2,"%10.3lf",atof(cap)*PI/966113280);//单位:rad,其中966113280=Ix08*32*刻线数*栅距/2=96*32*31449*20/2
	
	m_XAP.SetWindowText(buf1);
	m_CAP.SetWindowText(buf2);
	
	CFormView::OnTimer(nIDEvent);
}
