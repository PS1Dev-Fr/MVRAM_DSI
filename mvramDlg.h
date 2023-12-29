// mvramDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMvramDlg dialog

class CMvramDlg : public CDialog
{
// Construction
public:
	CMvramDlg(wchar_t *Path,BOOL *VCalc,CWnd* pParent = NULL);	// standard constructor
// Dialog Data
	//{{AFX_DATA(CMvramDlg)
	enum { IDD = IDD_MVRAM_DIALOG };
	CButton	m_VCalc;
	CEdit	m_Scripte;
	CString	m_VRSFile;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMvramDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;
	int LoadScript(wchar_t *Path);
	int SaveScript(wchar_t *Path);
	int IsLoad;
	int IsChange;
	wchar_t *SavePath;
	BOOL *SVCalc,ReqLoadScript;
	CString OldScPath;
	// Generated message map functions
	//{{AFX_MSG(CMvramDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnButton1();
	afx_msg void OnDropFiles(HDROP hDropInfo);
	afx_msg void OnChangeEdit1();
	virtual void OnOK();
	afx_msg void OnSave();
	afx_msg void OnChangeEdit2();
	virtual void OnCancel();
	afx_msg void OnDdInfo();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
