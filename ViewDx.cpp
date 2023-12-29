// ViewDx.cpp : implementation file
//

#include "stdafx.h"
#include "ViewDx.h"
#include "DialogDdInfo.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CViewDx


CViewDx::CViewDx(int w,int h,BOOL VCalc)
{
	RECT rc;
	EnableAutomation();
	FVCalc=VCalc;
	if(FVCalc)
	{
		rc.left=0;
		rc.top=0;
		rc.right=w;
		rc.bottom=h;
		Create(NULL,L"MVRAM DxView",/*WS_CAPTION|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_SYSMENU*/WS_OVERLAPPEDWINDOW,rc);
		ShowWindow(SW_SHOW);
		UpdateWindow();
	}
}

CViewDx::~CViewDx()
{
}

BOOL CViewDx::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	// TODO: Add your specialized code here and/or call the base class

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	CString strMenu;
	if (!strMenu.IsEmpty())
	{
		pSysMenu->AppendMenu(MF_SEPARATOR);
		pSysMenu->AppendMenu(MF_STRING,0x0010, strMenu);
	}

	if(FVCalc)
	{
        USHORT *Ptr;
        dd=new 	Cdd(m_hWnd,lpcs->cx,lpcs->cy,lpcs->cx,lpcs->cy,2,0);
		dd->enumVideoModes();
		dd->getCaps();
		Ptr=dd->lock();
        memset(Ptr,0,lpcs->cx*lpcs->cy*2);
		dd->unlock();
		dd->show();
	}
	return CFrameWnd::OnCreateClient(lpcs, pContext);
}

void CViewDx::OnPaint() 
{
	CPaintDC dc(this); // device context for painting
	// TODO: Add your message handler code here
	
	// Do not call CFrameWnd::OnPaint() for painting messages
	if(FVCalc)
	{
		dd->show();
	}
}


BOOL CViewDx::DestroyWindow() 
{
	// TODO: Add your specialized code here and/or call the base class
	if(FVCalc)
		delete  dd;	
	FVCalc=0;
	return CFrameWnd::DestroyWindow();
}

void CViewDx::OnFinalRelease()
{
	// When the last reference for an automation object is released
	// OnFinalRelease is called.  The base class will automatically
	// deletes the object.  Add additional cleanup required for your
	// object before calling the base class.

	CFrameWnd::OnFinalRelease();
}


BEGIN_MESSAGE_MAP(CViewDx, CFrameWnd)
	//{{AFX_MSG_MAP(CViewDx)
	ON_WM_PAINT()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

USHORT CViewDx::Pal2DDMem(UCHAR r,UCHAR g,UCHAR b)
{
	return(dd->makePixel(r,g,b));
}

void CViewDx::ShowPage(UCHAR *Data,UCHAR *palette)
{
	int i,j;
	USHORT *Ptr;	
	UCHAR c,*d;
	if(FVCalc)
	{
		Ptr=dd->lock();
		d=Data;
		for(j=0;j<480;j++)
		{
			for(i=0;i<640;i++)
			{
				c=*d++;
				*Ptr++=dd->makePixel(*(palette+c*3+0),*(palette+c*3+1),*(palette+c*3+2));
			}
		}	
		dd->unlock();
		dd->show();
	}
}

void CViewDx::ShowPageColor(USHORT *Data)
{
	int i,j;
	USHORT *Ptr,*d;
	if(FVCalc)
	{
		Ptr=dd->lock();
		d=Data;
		
		for(j=0;j<256;j++)
		{
			for(i=0;i<256;i++)
			{
				*Ptr++=*d++;
			}
		}
		dd->unlock();
		dd->show();
	}
}

void CViewDx::ShowPageColor24(UCHAR *Data)
{
	int i,j;
	USHORT *Ptr;
	UCHAR  *d;
	if(FVCalc)
	{
		Ptr=dd->lock();
		d=Data;
		
		for(j=0;j<256;j++)
		{
			for(i=0;i<256;i++)
			{
				*Ptr++=dd->makePixel(*(d+0),*(d+1),*(d+2));
				d+=3;
			}
		}
		dd->unlock();
		dd->show();
	}
}


void CViewDx::ShowPageWH(UCHAR *Data,UCHAR *palette,int w,int h)
{
	int i,j;
	USHORT *Ptr;	
	UCHAR c,*d;
    UCHAR Pal[256*3];
    
    if(palette==NULL)
    {
        for(i=0;i<256*3;i++)
        {
            Pal[i]=((i/3)%32)*8;
        }
        palette=Pal;
    }
	if(FVCalc)
	{
		Ptr=dd->lock();
		d=Data;
		for(j=0;j<h;j++)
		{
			for(i=0;i<w;i++)
			{
				c=*d++;
				*(Ptr+i+j*640)=dd->makePixel(*(palette+c*3+0),*(palette+c*3+1),*(palette+c*3+2));
			}
		}	
		dd->unlock();
		dd->show();
	}
}

void CViewDx::ShowVRAM(USHORT *Data,int w,int h)
{
	USHORT *Ptr;	
	if(FVCalc)
	{
		Ptr=dd->lock();
		memcpy(Ptr,Data,w*h*2);
		dd->unlock();
		dd->show();	
	}
}


void CViewDx::OnClose() 
{
	// TODO: Add your message handler code here and/or call default
	
	//CFrameWnd::OnClose();
}

void CViewDx::OnSysCommand( UINT nID, LPARAM lParam )
{
	if ((nID& 0xFFF0) == 0x0010)
	{
	}
	else
	{
		CFrameWnd::OnSysCommand(nID,lParam);
	}	
}

void CViewDx::NewSize(int w,int h)
{
	if(FVCalc)
		dd->reset(w,h,w,h,0);
}

HICON CViewDx::SetIcon( HICON hIcon, BOOL bBigIcon )
{
	if(FVCalc)
		return(CFrameWnd::SetIcon(hIcon,bBigIcon));
	return 0;
}
  

BOOL CViewDx::RedrawWindow( LPCRECT lpRectUpdate, CRgn* prgnUpdate, UINT flags)
{
	if(FVCalc)
		return(CFrameWnd::RedrawWindow(lpRectUpdate,prgnUpdate,flags));
	return 0;
}

 void CViewDx::UpdateWindow( )
 {
	if(FVCalc)
		CFrameWnd::UpdateWindow( );
 }
