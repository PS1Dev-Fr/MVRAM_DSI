//  ProgressDlg.cpp : implementation file
// CG: This file was added by the Progress Dialog component

#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "stringconversion.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

CProgressDlg::CProgressDlg(UINT nCaptionID,wchar_t *Path)
{
	FILE *fp;
	m_nCaptionID = CG_IDS_PROGRESS_CAPTION;
	if (nCaptionID != 0)
		m_nCaptionID = nCaptionID;

    m_bCancel=FALSE;
    m_nLower=0;
    m_nUpper=100;
    m_nStep=10;
    //{{AFX_DATA_INIT(CProgressDlg)
    // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
    m_bParentDisabled = FALSE;

	CString s,s2;
	s=Path;
	s2=s.Left(s.ReverseFind(L'.'));
	wcscpy(SavePath,s2+".tmp");

	if((fp=_wfopen(SavePath,L"wt"))==NULL)
	{
		// Error
	}
	fclose(fp);
}

CProgressDlg::~CProgressDlg()
{
    if(m_hWnd!=NULL)
      DestroyWindow();
	//SaveDbgInfo();
}

int  CProgressDlg::AddDbgInfo(LPCTSTR Txt)
{
	int Pos,i;
	m_DbgInfo.SetSel(0xffff,TRUE);
	Pos = m_DbgInfo.LineIndex((i = m_DbgInfo.GetLineCount() - 1)) + m_DbgInfo.LineLength(i);
	m_DbgInfo.SetSel(Pos,Pos,TRUE);
	m_DbgInfo.ReplaceSel(Txt);
    PumpMessages();
	return 1;
}

int  CProgressDlg::AddDbgInfoC(LPCTSTR Txt)
{
	//int Pos,i;
	m_DbgInfo.SetSel(0xffff,TRUE);
	m_DbgInfo.Clear( );
	m_DbgInfo.ReplaceSel(Txt);
    PumpMessages();
	return 1;
}

int  CProgressDlg::SaveDbgInfo()
{
	int i,l;
	wchar_t Ligne[256];
	FILE *fp;
	if((fp=_wfopen(SavePath,L"at"))==NULL) return 0;
	CTime t=CTime::GetCurrentTime();
	fprintf(fp,"// Infos file created by MVram (%02d/%02d/%04d %02d:%02d:%02d)\n\n",t.GetDay(),t.GetMonth(),t.GetYear(),t.GetHour(),t.GetMinute(),t.GetSecond());
	for(i=0;i<m_DbgInfo.GetLineCount()-1;i++)
	{
		l=m_DbgInfo.GetLine(i,Ligne,256);
		Ligne[l]='\n';
		Ligne[l+1]=0;
        std::string ligneutf8 = wstring_to_utf8(Ligne);
        fputs(ligneutf8.c_str(), fp);
    }
	fclose(fp);
	m_DbgInfo.SetReadOnly(FALSE);
	m_DbgInfo.EmptyUndoBuffer();
	m_DbgInfo.SetSel(0,-1,TRUE);
	m_DbgInfo.Clear();
	m_DbgInfo.SetSel(0,0,TRUE);
	m_DbgInfo.SetModify();
	m_DbgInfo.SetReadOnly(TRUE);
	return 1; 
}

void CProgressDlg::OnMaxtextDbginfo() 
{
	SaveDbgInfo();
}

BOOL CProgressDlg::DestroyWindow()
{
    ReEnableParent();
	SaveDbgInfo();
    return CDialog::DestroyWindow();
}

void CProgressDlg::ReEnableParent()
{
    if(m_bParentDisabled && (m_pParentWnd!=NULL))
      m_pParentWnd->EnableWindow(TRUE);
    m_bParentDisabled=FALSE;
}

BOOL CProgressDlg::Create(CWnd *pParent)
{
    // Get the true parent of the dialog
    m_pParentWnd = CWnd::GetSafeOwner(pParent);

    // m_bParentDisabled is used to re-enable the parent window
    // when the dialog is destroyed. So we don't want to set
    // it to TRUE unless the parent was already enabled.

    if((m_pParentWnd!=NULL) && m_pParentWnd->IsWindowEnabled())
    {
      m_pParentWnd->EnableWindow(FALSE);
      m_bParentDisabled = TRUE;
    }

    if(!CDialog::Create(CProgressDlg::IDD,pParent))
    {
      ReEnableParent();
      return FALSE;
    }

    return TRUE;
}

void CProgressDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CProgressDlg)
	DDX_Control(pDX, IDC_DBGINFO, m_DbgInfo);
    DDX_Control(pDX, CG_IDC_PROGDLG_PROGRESS, m_Progress);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CProgressDlg, CDialog)
    //{{AFX_MSG_MAP(CProgressDlg)
	ON_EN_MAXTEXT(IDC_DBGINFO, OnMaxtextDbginfo)
	ON_BN_CLICKED(IDC_OUT, OnOut)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CProgressDlg::SetStatus(LPCTSTR lpszMessage)
{
    ASSERT(m_hWnd); // Don't call this _before_ the dialog has
                    // been created. Can be called from OnInitDialog
    CWnd *pWndStatus = GetDlgItem(CG_IDC_PROGDLG_STATUS);

    // Verify that the static text control exists
    ASSERT(pWndStatus!=NULL);
    pWndStatus->SetWindowText(lpszMessage);
}


void CProgressDlg::SetRange(int nLower,int nUpper)
{
    m_nLower = nLower;
    m_nUpper = nUpper;
    m_Progress.SetRange(nLower,nUpper);
}
  
int CProgressDlg::SetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.SetPos(nPos);
    UpdatePercent(nPos);
    return iResult;
}

int CProgressDlg::SetStep(int nStep)
{
    m_nStep = nStep; // Store for later use in calculating percentage
    return m_Progress.SetStep(nStep);
}

int CProgressDlg::OffsetPos(int nPos)
{
    PumpMessages();
    int iResult = m_Progress.OffsetPos(nPos);
    UpdatePercent(iResult+nPos);
    return iResult;
}

int CProgressDlg::StepIt()
{
    PumpMessages();
    int iResult = m_Progress.StepIt();
    UpdatePercent(iResult+m_nStep);
    return iResult;
}

void CProgressDlg::PumpMessages()
{
    // Must call Create() before using the dialog
    ASSERT(m_hWnd!=NULL);

    MSG msg;
    // Handle dialog messages
    while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
    {
      if(!IsDialogMessage(&msg))
      {
        TranslateMessage(&msg);
        DispatchMessage(&msg);  
      }
    }
}

BOOL CProgressDlg::CheckMemCancelButton()
{
    // Process all pending messages
    PumpMessages();

    BOOL bResult = m_bCancel;

    return bResult;
}
BOOL CProgressDlg::CheckCancelButton()
{
    // Process all pending messages
    PumpMessages();

    // Reset m_bCancel to FALSE so that
    // CheckCancelButton returns FALSE until the user
    // clicks Cancel again. This will allow you to call
    // CheckCancelButton and still continue the operation.
    // If m_bCancel stayed TRUE, then the next call to
    // CheckCancelButton would always return TRUE

    BOOL bResult = m_bCancel;
    m_bCancel = FALSE;

    return bResult;
}

void CProgressDlg::UpdatePercent(int nNewPos)
{
    CWnd *pWndPercent = GetDlgItem(CG_IDC_PROGDLG_PERCENT);
    int nPercent;
    
    int nDivisor = m_nUpper - m_nLower;
	
	if(nDivisor<=0) return; // m_nLower should be smaller than m_nUpper

    int nDividend = (nNewPos - m_nLower);
	if(nDividend<0) return;	// Current position should be greater than m_nLower

    nPercent = nDividend * 100 / nDivisor;

    // Since the Progress Control wraps, we will wrap the percentage
    // along with it. However, don't reset 100% back to 0%
    if(nPercent!=100)
      nPercent %= 100;

    // Display the percentage
    CString strBuf;
    strBuf.Format(_T("%d%c"),nPercent,_T('%'));

	CString strCur; // get current percentage
    pWndPercent->GetWindowText(strCur);

	if (strCur != strBuf)
		pWndPercent->SetWindowText(strBuf);
}
    
/////////////////////////////////////////////////////////////////////////////
// CProgressDlg message handlers

BOOL CProgressDlg::OnInitDialog() 
{
    CDialog::OnInitDialog();
    m_Progress.SetRange(m_nLower,m_nUpper);
    m_Progress.SetStep(m_nStep);
    m_Progress.SetPos(m_nLower);

	CString strCaption;
	VERIFY(strCaption.LoadString(m_nCaptionID));
    SetWindowText(strCaption);
	m_DbgInfo.SetSel(0,-1);
	m_DbgInfo.Clear();

    return TRUE;  
}



void CProgressDlg::OnOut() 
{
	// TODO: Add your control notification handler code here
    m_bCancel=TRUE;
	
}
