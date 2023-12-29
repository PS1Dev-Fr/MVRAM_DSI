// mvramDlg.cpp : implementation file
//

#include "stdafx.h"
#include "mvram.h"
#include "mvramDlg.h"
#include "stringconversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

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
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMvramDlg dialog

CMvramDlg::CMvramDlg(wchar_t *Path,BOOL *VCalc,CWnd* pParent /*=NULL*/)
	: CDialog(CMvramDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CMvramDlg)
	m_VRSFile = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
	if(Path[0]==0)
		ReqLoadScript=0;
	else
		ReqLoadScript=1;

	SavePath=Path;

	SVCalc=VCalc;
}
void CMvramDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMvramDlg)
	DDX_Control(pDX, IDC_VCalc, m_VCalc);
	DDX_Control(pDX, IDC_EDIT2, m_Scripte);
	DDX_Text(pDX, IDC_EDIT1, m_VRSFile);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CMvramDlg, CDialog)
	//{{AFX_MSG_MAP(CMvramDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, OnButton1)
	ON_WM_DROPFILES()
	ON_EN_CHANGE(IDC_EDIT1, OnChangeEdit1)
	ON_BN_CLICKED(IDSAVE, OnSave)
	ON_EN_CHANGE(IDC_EDIT2, OnChangeEdit2)
	ON_BN_CLICKED(IDC_DD_INFO, OnDdInfo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMvramDlg message handlers

BOOL CMvramDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strAboutMenu;
	strAboutMenu.LoadString(IDS_ABOUTBOX);
	if (!strAboutMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, TRUE);		// Set small icon
	IsLoad=0;	
	IsChange=0;	
	// TODO: Add extra initialization here

	if(ReqLoadScript)
	{
		LoadScript(SavePath);
		m_VRSFile=SavePath;
		UpdateData(FALSE);
	}
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CMvramDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CMvramDlg::OnPaint() 
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
HCURSOR CMvramDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CMvramDlg::OnOK() 
{
	if(IsLoad||IsChange)
	{
		SaveScript(m_VRSFile.GetBuffer());
		CDialog::OnOK();
		wcscpy(SavePath,m_VRSFile);
		*SVCalc=m_VCalc.GetCheck();
	}
}

void CMvramDlg::OnSave() 
{
	if(IsLoad||IsChange)
		SaveScript(m_VRSFile.GetBuffer());	
}

void CMvramDlg::OnCancel() 
{
	int i;
	i=AfxMessageBox(L"Save Before Quit ?",MB_YESNOCANCEL);
	if(i==IDYES)
	{
		if(IsLoad||IsChange)
			SaveScript(m_VRSFile.GetBuffer());

		CDialog::OnCancel();
	}
	if(i==IDNO)
	{
		CDialog::OnCancel();
	}	
}

void CMvramDlg::OnButton1() 
{
	int i;
	if(IsLoad||IsChange)
	{
		i=AfxMessageBox(L"Save Before Load ?",MB_YESNOCANCEL);
		if(i==IDYES)
		{
			SaveScript(m_VRSFile.GetBuffer());
		}
		if(i!=IDCANCEL)
		{
			CFileDialog FD(FALSE,L"VRS",m_VRSFile,OFN_EXPLORER|OFN_HIDEREADONLY,L"MVram script (*.vrs)|*.vrs|All Files (*.*)|*.*||",NULL);
			INT_PTR nDialogRes=FD.DoModal();
			if(		nDialogRes==IDOK )
			{
				m_VRSFile = FD.GetPathName();
				LoadScript(m_VRSFile.GetBuffer());
				UpdateData(FALSE);
			}
		}
	}
	else
	{
		CFileDialog FD(FALSE,L"VRS",m_VRSFile,OFN_EXPLORER|OFN_HIDEREADONLY,L"MVram script (*.vrs)|*.vrs|All Files (*.*)|*.*||",NULL);
		INT_PTR nDialogRes=FD.DoModal();
		if(		nDialogRes==IDOK )
		{
			m_VRSFile = FD.GetPathName();
			LoadScript(m_VRSFile.GetBuffer());
			UpdateData(FALSE);
		}
	}
}


void CMvramDlg::OnChangeEdit1() 
{
	wchar_t *Path;
	FILE *fp;
	int i;
	UpdateData(TRUE);
	Path=m_VRSFile.GetBuffer();
	if((fp=_wfopen(Path,L"r"))!=NULL)
	{
		fclose(fp);
		if(IsLoad||IsChange)
		{
			i=AfxMessageBox(L"Save Before Load ?",MB_YESNOCANCEL);
			if(i==IDYES)
			{
				SaveScript(OldScPath.GetBuffer());
				LoadScript(Path);
				UpdateData(FALSE);

			}
			if(i==IDNO)
			{
				LoadScript(Path);
				UpdateData(FALSE);
			}	
		}
		else
		{
			LoadScript(Path);
			UpdateData(FALSE);
		}
	}
	OldScPath=m_VRSFile;
}

void CMvramDlg::OnChangeEdit2() 
{
	UpdateData(TRUE);
	IsChange=1;	
}

void CMvramDlg::OnDropFiles(HDROP hDropInfo) 
{
	MSG message;
	CWnd *cible;
	LPTSTR string=(LPTSTR)malloc(_MAX_PATH*2);

	DragQueryFile(hDropInfo,0,string,_MAX_PATH*2);
	

	GetMessage(&message,NULL,0,0);
	cible=WindowFromPoint( message.pt );

	if(GetDlgItem(IDC_EDIT1)->m_hWnd == cible->m_hWnd)
	{
		int i;
		if(IsLoad||IsChange)
		{
			i=AfxMessageBox(L"Save Before Load ?",MB_YESNOCANCEL);
			if(i==IDYES)
			{
				SaveScript(m_VRSFile.GetBuffer());
				LoadScript(string);
				m_VRSFile=string;
				UpdateData(FALSE);

			}
			if(i==IDNO)
			{
				LoadScript(string);
				m_VRSFile=string;
				UpdateData(FALSE);
			}	
		}
		else
		{
			LoadScript(string);
			m_VRSFile=string;
			UpdateData(FALSE);
		}
	}

	if(GetDlgItem(IDC_EDIT2)->m_hWnd == cible->m_hWnd)
	{
		int i;
		if(IsLoad||IsChange)
		{
			i=AfxMessageBox(L"Save Before Load ?",MB_YESNOCANCEL);
			if(i==IDYES)
			{
				SaveScript(m_VRSFile.GetBuffer());
				LoadScript(string);
				m_VRSFile=string;
				UpdateData(FALSE);

			}
			if(i==IDNO)
			{
				LoadScript(string);
				m_VRSFile=string;
				UpdateData(FALSE);
			}	
		}
		else
		{
			LoadScript(string);
			m_VRSFile=string;
			UpdateData(FALSE);
		}
	}


	free(string);
}

int CMvramDlg::LoadScript(wchar_t *Path)
{
	FILE *fp;
	char Ligne[256];
	int i;

	m_Scripte.SetSel(0,-1);
	m_Scripte.Clear();

	if((fp=_wfopen(Path,L"rt"))==NULL) 
	{
		AfxMessageBox(L"Error on LoadScript");
		return 0;
	}
	while(!feof(fp))
	{
		if(fgets(Ligne,256,fp)== NULL)
			break;
		
		i=(int)strlen(Ligne);

		if(Ligne[i-1]=='\n'||Ligne[i-1]=='\r')
			Ligne[i-1]=0;

		strcat(Ligne,"\r\n");
		std::wstring wligne = utf8_to_wstring(Ligne);

		m_Scripte.ReplaceSel(wligne.c_str());
	}
	fclose(fp);
	IsLoad=1;	
	IsChange=0;
	return 1;
}

int CMvramDlg::SaveScript(wchar_t *Path)
{
	int i,l;
	wchar_t Ligne[256];
	FILE *fp;
	if((fp=_wfopen(Path,L"wt"))==NULL)
	{
		AfxMessageBox(L"Error on SaveScript");
		return 0;
	}
	for(i=0;i<m_Scripte.GetLineCount()-1;i++)
	{
		l=m_Scripte.GetLine(i,Ligne,256);
		Ligne[l]='\n';
		Ligne[l+1]=0;

		std::string ligneutf8 = wstring_to_utf8(Ligne);

		fputs(ligneutf8.c_str(),fp);
	}
	fclose(fp);
	return 1; 
}

void CMvramDlg::OnDdInfo() 
{
}
