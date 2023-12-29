// mvram.h : main header file for the MVRAM application
//

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols
#ifdef MVRAM_C
    #define MVRAM_EXT
#else
    #define MVRAM_EXT extern
#endif

#define _EDITOR 1

#include "DialogDdInfo.h"
/////////////////////////////////////////////////////////////////////////////
// CMvramApp:
// See mvram.cpp for the implementation of this class
//

class CMvramApp : public CWinApp
{
public:
	CMvramApp();
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMvramApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CMvramApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

