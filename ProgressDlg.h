// ProgressDlg.h : header file
// CG: This file was added by the Progress Dialog component

/////////////////////////////////////////////////////////////////////////////
// CProgressDlg dialog

#ifndef __PROGRESSDLG_H__
#define __PROGRESSDLG_H__

class CProgressDlg : public CDialog
{
// Construction / Destruction
public:
    CProgressDlg(UINT nCaptionID = 0,wchar_t *Path=L"mvram.inf");   // standard constructor
    ~CProgressDlg();

    BOOL Create(CWnd *pParent=NULL);

    // Checking for Cancel button
    BOOL CheckCancelButton();
    BOOL CheckMemCancelButton();
    // Progress Dialog manipulation
    void SetStatus(LPCTSTR lpszMessage);
    void SetRange(int nLower,int nUpper);
    int  SetStep(int nStep);
    int  SetPos(int nPos);
    int  OffsetPos(int nPos);
    int  StepIt();
	int  AddDbgInfo(LPCTSTR Txt);
	int AddDbgInfoC(LPCTSTR Txt);
	int  SaveDbgInfo();
	CString GetInfoName(void)	{return (CString)SavePath;}
        
// Dialog Data
    //{{AFX_DATA(CProgressDlg)
	enum { IDD = CG_IDD_PROGRESS };
	CEdit	m_DbgInfo;
    CProgressCtrl	m_Progress;
	//}}AFX_DATA

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CProgressDlg)
    public:
    virtual BOOL DestroyWindow();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    //}}AFX_VIRTUAL

// Implementation

protected:
	wchar_t SavePath[512];
	UINT m_nCaptionID;
    int m_nLower;
    int m_nUpper;
    int m_nStep;
    
    BOOL m_bCancel;
    BOOL m_bParentDisabled;

    void ReEnableParent();

    virtual void OnOK() {}; 
    void UpdatePercent(int nCurrent);
    void PumpMessages();

    // Generated message map functions
    //{{AFX_MSG(CProgressDlg)
    virtual BOOL OnInitDialog();
	afx_msg void OnMaxtextDbginfo();
	afx_msg void OnOut();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // __PROGRESSDLG_H__
