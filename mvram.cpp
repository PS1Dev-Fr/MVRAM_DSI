// mvram.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"

#define MVRAM_C

#include "viewdx.h"
#include "mvram.h"
#include "mvramDlg.h"
#include "ProgressDlg.h"
#include "cscript.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMvramApp

BEGIN_MESSAGE_MAP(CMvramApp, CWinApp)
	//{{AFX_MSG_MAP(CMvramApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMvramApp construction

CMvramApp::CMvramApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CMvramApp object

CMvramApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CMvramApp initialization

BOOL CMvramApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

	wchar_t Path[256]=L"";
	BOOL VCalc=0,AutoQuit=0;
	INT_PTR nResponse;
	int SaveRaw=0;
	if (m_lpCmdLine[0] != '\0')
	{
		LPTSTR       c;
		int i=0,epath=1,Edit=0;
		CString s;

		for(c=m_lpCmdLine; *c != '\0'; c++)
		{
			if(*c=='/')
			{
				*c++;
				switch( *c )
				{
					case '?':
					case 'h':
						s.Format(L"     MVram for windows    \n\n(c)1996 D.S.I. NP\nVer %s - %s\nUsage : \n\n MVram Path [/q] [/e] [/d] [/h] \n\n /q : AutoQuit \n /d : DxView \n /e : Edit script\n  /r : save raw file of vram\n /h : This help\n",__DATE__,__TIME__);
						AfxMessageBox(s,MB_ICONINFORMATION);
						return(0);
					break;
					case 'q': 
						AutoQuit=1; 
					break;
					case 'd':
						VCalc=1;
					break; 
					case 'e':
					{
						CMvramDlg dlg(Path,&VCalc);
						m_pMainWnd = &dlg;
						nResponse = dlg.DoModal();
						Edit=1;
					}
					break;
					case 'r':
						SaveRaw=1;
					break;
				}
			}
			else if(*c!=' '&&epath)
			{
				Path[i++]=*c;
				if(*(c+1)==' ')
					epath=0;
			}
		}

		Path[i]=0;	
		if(!Edit)
			nResponse = IDOK;
	}
	else
	{
		CMvramDlg dlg(Path,&VCalc);
		m_pMainWnd = &dlg;
		nResponse = dlg.DoModal();
	}

	if (nResponse == IDOK)
	{
		CString OutName,InfoName;
		{
			//VCalc=1; 
			HICON m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
			CProgressDlg Pgs(IDS_MVRAMP,Path);
			Pgs.Create();
			Pgs.SetIcon(m_hIcon, TRUE);		// Set big icon
			Pgs.SetIcon(m_hIcon, TRUE);		// Set small icon
			Pgs.ShowWindow(SW_HIDE);
			Pgs.RedrawWindow();
			Pgs.UpdateWindow();
			Pgs.ShowWindow(SW_SHOW);

			//CViewDx *Cvdd=new CViewDx(640,480,VCalc);
			CViewDx *Cvdd=new CViewDx(256,256,VCalc);
			Pgs.SetActiveWindow();
			Cvdd->SetIcon(m_hIcon, TRUE);		// Set big icon
			Cvdd->SetIcon(m_hIcon, TRUE);		// Set small icon
			Cvdd->RedrawWindow();
			Cvdd->UpdateWindow();

			//m_pMainWnd = Cvdd;

			CScript S(&Pgs,Cvdd,SaveRaw);

			S.Exec(Path);

			Pgs.GetDlgItem(IDC_OUT)->SetWindowText(L"Quit");

			if(!AutoQuit)
				while(!Pgs.CheckCancelButton());

			delete Cvdd;

			OutName=S.GetOutName();
			InfoName=Pgs.GetInfoName();
		}
		{
			// déplace le fichier d'info
			OutName+=".inf";
			FILE *file=_wfopen(OutName,L"r");
			if (file)
			{
				fclose(file);
				_wremove(OutName);
			}
			_wrename((LPCTSTR)InfoName,(LPCTSTR)OutName);
		}
	}
	else if (nResponse == IDCANCEL)
	{		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}

#undef MVRAM_C