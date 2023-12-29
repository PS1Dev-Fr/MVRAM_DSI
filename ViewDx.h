// ViewDx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CViewDx frame
#ifndef _VIEWDX_
#define _VIEWDX_

#include "dd.h"
class CViewDx : public CFrameWnd
{
public:
	CViewDx(int w,int h,BOOL VCalc);
	virtual ~CViewDx();
// Attributes

// Operations
	void ShowPage(UCHAR *Data,UCHAR *palette);
    void ShowPageWH(UCHAR *Data,UCHAR *palette,int w,int h);
	void ShowVRAM(USHORT *Data,int w,int h);
	USHORT Pal2DDMem(UCHAR r,UCHAR g,UCHAR b);
	void ShowPageColor(USHORT *Data);
	void ShowPageColor24(UCHAR *Data);

	int FVCalc;


public:
	Cdd *dd;
	void NewSize(int w,int h);
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CViewDx)
	public:
	virtual void OnFinalRelease();
	virtual BOOL DestroyWindow();
	virtual HICON SetIcon( HICON hIcon, BOOL bBigIcon );
	virtual BOOL RedrawWindow( LPCRECT lpRectUpdate = NULL, CRgn* prgnUpdate = NULL, UINT flags = RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE ); 
	virtual void UpdateWindow( );
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual void OnSysCommand( UINT nID, LPARAM lParam );
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CViewDx)
	afx_msg void OnPaint();
	afx_msg void OnClose();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
#endif

/////////////////////////////////////////////////////////////////////////////
