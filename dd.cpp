
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////                                                                       ////
////  D.S.I                                                                ////
////                                                                       ////
////  Patrick,Arnaud,Claude                                                ////
////                                                                       ////
////                                                                       ////
////                                                                       ////
////                                                                       ////
////  Management of direct draw object                                     ////
////                                                                       ////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//// MANAGEMENT OF LOW LEVEL SCREEN                                        ////
///////////////////////////////////////////////////////////////////////////////

//#define INITGUID


#include "stdafx.h"
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>

#define DSI_DD_C

#include "dd.h"
#include "DdCaps.h"
#include "RgbHls.h"
#include "resource.h"
#include "DialogDdInfo.h"
#include "dsi_err.h"
#include "dsi_file.h"
#include "ddraw.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _EDITOR
extern CDialogDdInfo ddInfo;
#endif

struct Craw_header
{
	UB	header[6];
	UW	version;
	UW	w;
	UW	h;
	UW	nbcolor;
	UB	bidon2[18];
};

/****************************************************************************
*                                                                           *
*                          Globales                                         *
*                                                                           *
****************************************************************************/

static RGB rgbBlack={0,0,0};
BOOL bResized=FALSE;					//	the window has resized or some other drastic change,the entire client area should be cleared */


int NumDirtyClientRects, NumDirtyBackRects, NumDirtyZRects;

/****************************************************************************
*                                                                           *
*                          Configuration module                             *
*                                                                           *
****************************************************************************/

#define TRACE_SURFACES_LOADING_IN_DIALOG 0
#define DISPLAY_SURFACES_LOADING_IN_DIALOG 0
#define _DD_FORCE_PIXEL_FORMAT_VALUES 0     // N'interoge pas la carte. Format 5,6,5 forcé.
#define _DD_USE_GAME_MSG_FOR_ERRORS 0       // Evite les AfxMessageBox pour erreurs DirectDraw
#define _SPRITE_SHOW_ALL_ERRORS 1           // Affiche toutes les erreurs d'un blit (Csurface::draw)
#define _SPRITE_TRACE_INFOS_WHEN_BLIT 0     // TRACE toutes les erreurs d'un blit (Csurface::draw) en version _DEBUG
#define _SPRITE_DISPLAY_INFOS_WHEN_BLIT 0   // Blitter bavard (Csurface::draw)
#define _SPRITE_INC_DATA_RECT 1
#define _SPRITE_INC_SCREEN_RECT 1           // Detournement bug du blitter : Pour un écran de 640x400,
                                            // un blit avec rcScreen.right=639 n'est pas calé à droite
                                            // de l'ecran. On pousse le rectangle de 1 en horizontal.
                                            // Idem en vertical.

/*
#ifdef NDEBUG
    #define _DD_USE_GAME_MSG_FOR_ERRORS 1
#endif
*/

/****************************************************************************
*                                                                           *
*                          class Cdd                                        *
*                                                                           *
****************************************************************************/

long                   *Cdd::yoffset=NULL;
UW                     *Cdd::ptr_screen=NULL;
int                     Cdd::pitch=0;

BOOL                    Cdd::bUseBackBuffer=TRUE;
BOOL                    Cdd::bUseVideoMem=TRUE;
BOOL                    Cdd::bUseBlitsOt=TRUE;
BOOL                    Cdd::bUseBlitsBatch=FALSE;
BOOL                    Cdd::bBlitBatchSupported=TRUE;
int                     Cdd::nNbBlitsInBatch=0;
DDBLTBATCH              Cdd::tabBlitBatch        [NbMaxBlitsInBatch];
RECT                    Cdd::tabBlitBatchSrcRect [NbMaxBlitsInBatch];
RECT                    Cdd::tabBlitBatchDestRect[NbMaxBlitsInBatch];
DDBLTFX                 Cdd::tabBlitBatchBlitFX  [NbMaxBlitsInBatch];

VideoModeType           Cdd::tabVideoMode[NbMaxVideoModes];
int                     Cdd::nNbAvailableVideoModes=0;

int					    Cdd::nbBitsPerPixel		= 0;
int					    Cdd::nbBitsR			= 0;
int					    Cdd::nbBitsG			= 0;
int					    Cdd::nbBitsB			= 0;
int					    Cdd::shiftCompR 		= 8;
int					    Cdd::shiftCompG 		= 8;
int					    Cdd::shiftCompB 		= 8;
int					    Cdd::shiftR				= 0;
int					    Cdd::shiftG				= 0;
int					    Cdd::shiftB				= 0;
unsigned int		    Cdd::maskR				= 0;
unsigned int		    Cdd::maskG				= 0;
unsigned int		    Cdd::maskB				= 0;

BOOL                    Cdd::bOk                = FALSE;
BOOL                    Cdd::bLocked            = FALSE;
BOOL				    Cdd::bCapsAvailable		= FALSE;
BOOL				    Cdd::bPixelCapsAvailable= FALSE;
BOOL                    Cdd::bLoadSurfacesInInit= FALSE;

LPDIRECTDRAWCLIPPER     Cdd::lpDDClipper;		// DirectDraw Clipper surface
LPDIRECTDRAW            Cdd::lpDD;           	// DirectDraw object
LPDIRECTDRAWSURFACE     Cdd::lpDDSPrimary;   	// DirectDraw primary surface
LPDIRECTDRAWSURFACE     Cdd::lpDDSBack;      	// DirectDraw back surface
LPDIRECTDRAWSURFACE     Cdd::lpDDSMain;      	// DirectDraw memory main surface

CSurface				Cdd::tabSurface[Cdd::SurfaceLastIndex];

HWND                	Cdd::hwndMain;			// Main window handle
BOOL					Cdd::fullscreen;		// boolean for current mode
int						Cdd::xResMax;			// x,y screen resolution
int						Cdd::yResMax;
int						Cdd::nbBitPerPlane;		// screen bit depth
HDC						Cdd::DDhdc;				// hdc associated with the back
LONG					Cdd::oldstyle;

SD                      Cdd::winx;
SD                      Cdd::winy;
SD                      Cdd::winw;
SD                      Cdd::winh;

static int oldbank=-100;

/*************

  Récupére de screen3d

  **************/

UW *Cdd::get_screen_addr(SD x,SD y)
{
    return (ptr_screen+yoffset[y]+x);
}


/**************************************
* Cdd:Cdd							  *
**************************************/

Cdd::Cdd(HWND hwnd,SD w,SD h,SD wWin,SD hWin, SD nbbpp,SD full)
{
	hwndMain = hwnd;   				// handle of window

	lpDD        = NULL;
	lpDDSPrimary= NULL;
	lpDDSBack   = NULL;
	lpDDSMain   = NULL;
    lpDDClipper = NULL;

    fullscreen  = full;

    yoffset = NULL;
    ptr_screen = NULL;

	init(w,h,wWin,hWin,nbbpp);
}


void Cdd::setwin(SD x,SD y,SD w,SD h)
 {
	winx=x;
	winy=y;
	winw=w;
	winh=h;
}

/**************************************
* Cdd::~Cdd						  *
**************************************/

Cdd::~Cdd()
{
    if( yoffset )
    {
        delete [] yoffset;
    }
	release();

#if _EDITOR
	// Rafraichissement de la boite ddInfo
	if( IsWindow(ddInfo.GetSafeHwnd())
		&&
		ddInfo.m_bAutoRefresh
	  )
	{
		ddInfo.Refresh();
	}
#endif
}

/**************************************
* Cdd::makePixel
**************************************/

UW Cdd::makePixel(unsigned int r,unsigned int g,unsigned int b)
{
	return (((r&0xFF)>>shiftCompR)<<shiftR) 
		   | 
		   (((g&0xFF)>>shiftCompG)<<shiftG) 
		   | 
		   (((b&0xFF)>>shiftCompB)<<shiftB);
}

/**************************************
* Gestion du batch de blits
**************************************/

void Cdd::delAllBlitsInBatch()
{
    memset(tabBlitBatch        ,0,NbMaxBlitsInBatch*sizeof(DDBLTBATCH));
    memset(tabBlitBatchSrcRect ,0,NbMaxBlitsInBatch*sizeof(RECT      ));
    memset(tabBlitBatchDestRect,0,NbMaxBlitsInBatch*sizeof(RECT      ));
    memset(tabBlitBatchBlitFX  ,0,NbMaxBlitsInBatch*sizeof(DDBLTFX   ));

    nNbBlitsInBatch=0;
}

BOOL Cdd::addBlitInBatch(RECT *pSrcRect,RECT *pDestRect,LPDIRECTDRAWSURFACE pSurface,DWORD dwFlags,LPDDBLTFX lpDDBltFx)
{
    BOOL bRes=FALSE;
    DDBLTBATCH batch;

    if( nNbBlitsInBatch<NbMaxBlitsInBatch )
    {
        bRes = TRUE;

        tabBlitBatchSrcRect [nNbBlitsInBatch] = *pSrcRect;
        tabBlitBatchDestRect[nNbBlitsInBatch] = *pDestRect;
        tabBlitBatchBlitFX  [nNbBlitsInBatch] = *lpDDBltFx;

        batch.lprDest   = &tabBlitBatchDestRect[nNbBlitsInBatch];
        batch.lprSrc    = &tabBlitBatchSrcRect [nNbBlitsInBatch];
        batch.lpDDBltFx = &tabBlitBatchBlitFX  [nNbBlitsInBatch];
        batch.dwFlags   = dwFlags;
        batch.lpDDSSrc  = pSurface;

        tabBlitBatch[nNbBlitsInBatch] = batch;

        nNbBlitsInBatch++;
    }

    return bRes;
}

BOOL Cdd::flushBlitsBatch()
{
    BOOL                bRes=TRUE;
    HRESULT             nRes;
    LPDIRECTDRAWSURFACE lpDDWork=Cdd::lpDDSMain;

    if( bUseBlitsBatch && (nNbBlitsInBatch>0) )
    {
        if( lpDDWork )
        {
	        if (lpDDWork->IsLost() == DDERR_SURFACELOST)
	        {
		        lpDDWork->Restore();
    	    }

            if( bBlitBatchSupported )
            {
                nRes = lpDDWork->BltBatch(tabBlitBatch,nNbBlitsInBatch,0);

                if( nRes == DDERR_UNSUPPORTED )
                {
                    directDrawError(nRes); // Une fois pour le montrer
                    bBlitBatchSupported = FALSE;
                }
                else
                if( nRes != DD_OK )
                {
                    directDrawError(nRes);
                    bRes = FALSE;
                }
            }
            else
            {
                int i;

                for( i=0 ; i<nNbBlitsInBatch ; i++ )
                {
                    tabBlitBatch[i].dwFlags |= DDBLT_WAIT;

                    lpDDWork->Blt(  tabBlitBatch[i].lprDest,
                                    tabBlitBatch[i].lpDDSSrc,
                                    tabBlitBatch[i].lprSrc,
                                    tabBlitBatch[i].dwFlags,
                                    tabBlitBatch[i].lpDDBltFx
                                 );
                }
            }

            delAllBlitsInBatch();
        }
    }

    return bRes;
}

/**************************************
* Cdd::makePixel
**************************************/

void Cdd::getPixel(UW pixel,RGB_MEMBER *pr,RGB_MEMBER *pg,RGB_MEMBER *pb)
{
    *pr = ((pixel&maskR)>>shiftR)<<shiftCompR;
    *pg = ((pixel&maskG)>>shiftG)<<shiftCompG;
    *pb = ((pixel&maskB)>>shiftB)<<shiftCompB;
}

/**************************************
* Cdd::InitCapsVariables			
**************************************/

static int FirstBit(unsigned int val)
{
	int i=0,TestBit=1;

	while( (i<31) && ((val&TestBit)==0) )
	{
		TestBit*=2;
		i++;
	}

	return i;
}

static int LastBit(unsigned int val)
{
	int i=-1,TestBit=1;

	while( (i<31) && ((val&TestBit)!=0) )
	{
		TestBit*=2;
		i++;
	}

	return i;
}

BOOL Cdd::InitCapsVariables()
{
	HRESULT nDdVal=DD_OK;

	#if _DD_FORCE_PIXEL_FORMAT_VALUES

		bPixelCapsAvailable = TRUE;
		maskR			    = 0xF800;
		maskG			    = 0xFFFF-0xF800-0x001F;
		maskB			    = 0x001F;

	#else

	    if( lpDD && lpDDSPrimary )
	    {
            // Caracteristiques
            DDSURFACEDESC DDSurfaceDesc;

            DDSurfaceDesc.dwSize = sizeof(DDSURFACEDESC);

            nDdVal = lpDDSPrimary->GetSurfaceDesc(&DDSurfaceDesc);

            if( nDdVal==DD_OK )
            {
                pitch = DDSurfaceDesc.lPitch;
            }
            else
            {
                CString sError="Cdd::InitCapsVariables\nGetSurfaceDesc\n";
			    directDrawError(nDdVal,&sError,TRUE);
            }

            // Pixel format
            DDPIXELFORMAT DDPixelFormat;

            DDPixelFormat.dwSize  = sizeof(DDPIXELFORMAT);
            DDPixelFormat.dwFlags = DDPF_RGB;

            nDdVal = lpDDSPrimary->GetPixelFormat((LPDDPIXELFORMAT)&DDPixelFormat);

		    if( nDdVal != DD_OK )
		    {	
                CString sError="Cdd::InitCapsVariables\nGetPixelFormat\n";
			    directDrawError(nDdVal,&sError,TRUE);
		    }
            else
            {
                if( DDPixelFormat.dwFlags & DDPF_RGB )
                {
                     bPixelCapsAvailable= TRUE;
                     nbBitsPerPixel     = DDPixelFormat.dwRGBBitCount;
                     maskR              = DDPixelFormat.dwRBitMask;
                     maskG              = DDPixelFormat.dwGBitMask;
                     maskB              = DDPixelFormat.dwBBitMask;
                }
            }
	    }

    #endif

	if( bPixelCapsAvailable )
	{
		// Calcul des masques et nombres de pixels
		
		shiftR  = FirstBit(maskR);
		shiftG  = FirstBit(maskG);
		shiftB  = FirstBit(maskB);

		nbBitsR = LastBit(maskR>>shiftR)+1;
		nbBitsG = LastBit(maskG>>shiftG)+1;
		nbBitsB = LastBit(maskB>>shiftB)+1;
        shiftCompR = 8-nbBitsR;
        shiftCompG = 8-nbBitsG;
        shiftCompB = 8-nbBitsB;

        #if _EDITOR
	        // Rafraichissement de la boite ddInfo
	        if( IsWindow(ddInfo.GetSafeHwnd())
		        &&
		        ddInfo.m_bAutoRefresh
	          )
	        {
		        ddInfo.Refresh();
	        }
        #endif
	}

	return bPixelCapsAvailable;
}

void Cdd::getCaps()
{
    int     i;
	HRESULT nDdVal=DD_OK;
    BOOL    bDdCreatedHere=FALSE,
            bPossible=FALSE;

    bCapsAvailable = FALSE;

    if( !lpDD )
    {
        nDdVal=DirectDrawCreate( NULL, &lpDD, NULL );

        if( nDdVal==DD_OK )
        {
            bPossible=TRUE;
            bDdCreatedHere=TRUE;
        }
    }
    else
    {
        bPossible = TRUE;
    }

    memcpy( ddGeneralCapsHEL,   ddGeneralCapsHardware,  sizeof(ddGeneralCapsHEL)    );
    memcpy( ddGeneralCaps2HEL,  ddGeneralCaps2Hardware, sizeof(ddGeneralCaps2HEL)   );
    memcpy( ddCKeyCapsHEL,      ddCKeyCapsHardware,     sizeof(ddCKeyCapsHEL)       );
    memcpy( ddFXCapsHEL,        ddFXCapsHardware,       sizeof(ddFXCapsHEL)         );
    memcpy( ddFXAlphaCapsHEL,   ddFXAlphaCapsHardware,  sizeof(ddFXAlphaCapsHEL)    );
    memcpy( ddPalCapsHEL,       ddPalCapsHardware,      sizeof(ddPalCapsHEL)        );
    memcpy( ddSVCapsHEL,        ddSVCapsHardware,       sizeof(ddSVCapsHEL)         );
    memcpy( ddOtherCapsHEL,     ddOtherCapsHardware,    sizeof(ddOtherCapsHEL)      );

	if( bPossible )
	{
		DDCAPS DDDriverCaps;
        DDCAPS DDHELCaps;

        DDDriverCaps.dwSize = sizeof(DDCAPS);
        DDHELCaps.dwSize    = sizeof(DDCAPS);

        nDdVal=lpDD->GetCaps(&DDDriverCaps,&DDHELCaps);

		if( nDdVal == DD_OK )
        {
            bCapsAvailable = TRUE;

            for( i=0 ; i<ddGeneralCapsNbFlags ; i++ )
            {
                ddGeneralCapsHardware[i].nVal   = ((DDDriverCaps.dwCaps&ddGeneralCapsHardware[i].nFlag)!=0); 
                ddGeneralCapsHEL[i].nVal        = ((DDHELCaps.dwCaps&ddGeneralCapsHEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddGeneralCaps2NbFlags ; i++ )
            {
                ddGeneralCaps2Hardware[i].nVal  = ((DDDriverCaps.dwCaps2&ddGeneralCaps2Hardware[i].nFlag)!=0); 
                ddGeneralCaps2HEL[i].nVal       = ((DDHELCaps.dwCaps2&ddGeneralCaps2HEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddCKeyCapsNbFlags ; i++ )
            {
                ddCKeyCapsHardware[i].nVal      = ((DDDriverCaps.dwCKeyCaps&ddCKeyCapsHardware[i].nFlag)!=0); 
                ddCKeyCapsHEL[i].nVal           = ((DDHELCaps.dwCKeyCaps&ddCKeyCapsHEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddFXCapsNbFlags ; i++ )
            {
                ddFXCapsHardware[i].nVal        = ((DDDriverCaps.dwFXCaps&ddFXCapsHardware[i].nFlag)!=0); 
                ddFXCapsHEL[i].nVal             = ((DDHELCaps.dwFXCaps&ddFXCapsHEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddFXAlphaCapsNbFlags ; i++ )
            {
                ddFXAlphaCapsHardware[i].nVal   = ((DDDriverCaps.dwFXAlphaCaps&ddFXAlphaCapsHardware[i].nFlag)!=0); 
                ddFXAlphaCapsHEL[i].nVal        = ((DDHELCaps.dwFXAlphaCaps&ddFXAlphaCapsHEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddPalCapsNbFlags ; i++ )
            {
                ddPalCapsHardware[i].nVal   = ((DDDriverCaps.dwPalCaps&ddPalCapsHardware[i].nFlag)!=0); 
                ddPalCapsHEL[i].nVal        = ((DDHELCaps.dwPalCaps&ddPalCapsHEL[i].nFlag)!=0); 
            }

            for( i=0 ; i<ddSVCapsNbFlags ; i++ )
            {
                ddSVCapsHardware[i].nVal   = ((DDDriverCaps.dwSVCaps&ddSVCapsHardware[i].nFlag)!=0); 
                ddSVCapsHEL[i].nVal        = ((DDHELCaps.dwSVCaps&ddSVCapsHEL[i].nFlag)!=0); 
            }

            ddOtherCapsHardware[DDOTHERCAPS_TOTALMEM].nVal = DDDriverCaps.dwVidMemTotal;
            ddOtherCapsHEL[DDOTHERCAPS_TOTALMEM].nVal      = DDHELCaps.dwVidMemTotal;

            ddOtherCapsHardware[DDOTHERCAPS_FREEMEM].nVal  = DDDriverCaps.dwVidMemFree;
            ddOtherCapsHEL[DDOTHERCAPS_FREEMEM].nVal       = DDHELCaps.dwVidMemFree;
        }
        else
		{	
            CString sError="Cdd::getCaps()\n";
			directDrawError(nDdVal,&sError,TRUE);
		}

        if( bDdCreatedHere )
        {
            lpDD->Release();
            lpDD=NULL;
        }
	}
    else
    {
        AfxMessageBox(L"Cdd::getCaps() failed\nNo DirectDraw structure encoutered");
    }
}

void Cdd::enumVideoModes()
{
	HRESULT nDdVal=DD_OK;
    BOOL    bDdCreatedHere=FALSE,
            bPossible=FALSE;

    nNbAvailableVideoModes=0;

    if( !lpDD )
    {
        nDdVal=DirectDrawCreate( NULL, &lpDD, NULL );

        if( nDdVal==DD_OK )
        {
            bPossible=TRUE;
            bDdCreatedHere=TRUE;
        }
    }
    else
    {
        bPossible = TRUE;
    }

	if( bPossible )
	{
		LPDDENUMMODESCALLBACK	pCallBack=DirectDrawEnumCallbackPixelCaps;
		LPDDSURFACEDESC			pDDSd=NULL;

        nDdVal=lpDD->EnumDisplayModes(0,pDDSd,NULL,pCallBack);

		if( nDdVal != DD_OK )
		{	
            CString sError="Cdd::enumVideoModes()\n";
			directDrawError(nDdVal,&sError,TRUE);
		}

        if( bDdCreatedHere )
        {
            lpDD->Release();
            lpDD=NULL;
        }
	}
    else
    {
        AfxMessageBox(L"Cdd::enumVideoModes() failed\nNo DirectDraw structure encoutered");
    }
}
/**************************************
* Cdd::release				      *
**************************************/

void Cdd::release(void)
{
    if( lpDD != NULL )
    {
		// Il n'est plus nécessaire de releaser individuellement les surfaces 
		// d'après la doc du GDK.

		CString                 sErrorLabel="Fatal error in DirectDraw Release\n";
		HRESULT					nDdVal;

		if (fullscreen)
		{
			nDdVal=lpDD->SetCooperativeLevel(hwndMain,DDSCL_NORMAL);

			if( nDdVal != DD_OK )
			{
				sErrorLabel+="SetCooperativeLevel\n";
				directDrawError(nDdVal,&sErrorLabel);
				//return FALSE;
			}
		}
		
		if ( lpDDClipper != NULL)
		{
			lpDDClipper->Release();
			lpDDClipper = NULL;
		}

		if ( lpDDSMain != NULL )
		{
			lpDDSMain->Release();
			lpDDSMain = NULL;
		}

		if ( lpDDSPrimary != NULL )
		{
			lpDDSPrimary->Release();
			lpDDSPrimary = NULL;
		}

		lpDD->Release();

        lpDD            = NULL;
		lpDDClipper     = NULL;
		lpDDSPrimary    = NULL;
		lpDDSBack       = NULL;
		lpDDSMain       = NULL;

        for(int i=0 ; i<SurfaceLastIndex ; i++ )
        {
            tabSurface[i].m_pSurface=NULL;
        }
    }
}


/**************************************
* Cdd::reset						  *
**************************************/

void Cdd::reset(SD w,SD h,SD wWin,SD hWin,SD full)
{
	release();
	fullscreen=full;

    if( !init(w,h,wWin,hWin,nbBitPerPlane) )
    {
		release();
		PostQuitMessage( 0 );
    }

	if( fullscreen ) 
    {
        resetfullscreenstyle();
    }
}

void Cdd::resetfullscreenstyle()
{
    DWORD dwStyle;
    dwStyle = GetWindowStyle(hwndMain);
    dwStyle |= WS_POPUP;
	dwStyle &= ~WS_OVERLAPPED;
	dwStyle &= ~WS_CAPTION;
	dwStyle &= ~WS_THICKFRAME;
	dwStyle &= ~WS_MINIMIZEBOX;
    SetWindowLong(hwndMain, GWL_STYLE, dwStyle);
}



/**************************************
* Cdd::restoreAll			  		  *
**************************************/

HRESULT Cdd::restoreall(void)
{
    HRESULT  nDdVal;
    nDdVal = lpDDSPrimary->Restore();

    if( nDdVal == DD_OK )
    {
        if (lpDDSBack) 
		{
			nDdVal = lpDDSBack->Restore();
		}
		else
		{
			nDdVal = DD_OK;
		}
		if (nDdVal == DD_OK)
		{
			nDdVal = lpDDSMain->Restore();
			if (nDdVal == DD_OK)
			{
			}
		}
    }
    return nDdVal;
}




/**************************************
* Cdd::showScreen			  		  *
**************************************/

void Cdd::show()
{
	HRESULT					nDdVal;
	RECT					rcWindow;

	{
		while(1)
		{
			if(!fullscreen)
			{
				GetClientRect (hwndMain,          &rcWindow);
    			ClientToScreen(hwndMain, (LPPOINT)&rcWindow);
	    		ClientToScreen(hwndMain, (LPPOINT)&rcWindow+1);

				nDdVal = lpDDSPrimary->Blt(&rcWindow,lpDDSMain, NULL,DDBLT_WAIT,NULL);
			}
			else
			{
				RECT srcRect;
				srcRect.top    = winy;
				srcRect.left   = winx;
				srcRect.bottom = winy+winh;
				srcRect.right  = winx+winw;

				{
					//------------------------------------
					// flip
					//------------------------------------
					if( bUseBackBuffer)
					{
						lpDDSPrimary->Flip(NULL, DDFLIP_WAIT);
					}
					else
					{
						srcRect.top    = 0;
						srcRect.left   = 0;
						srcRect.bottom = winh;
						srcRect.right  = winw;
						nDdVal = lpDDSPrimary->Blt(NULL,lpDDSMain, &srcRect,0,NULL);
					}
				}
			}
			if(nDdVal==DD_OK)
			{
				break;
			}
			if( nDdVal==DDERR_SURFACELOST )
			{
				nDdVal = restoreall();
				if(nDdVal!=DD_OK)
				{
					break;
				}
			}
			if( nDdVal != DDERR_WASSTILLDRAWING )
			{
				break;
			}
		}
	}
}

#ifdef _AFXDLL

void Cdd::show(CPoint offset, HWND clipWindow)
{
	HRESULT nDdVal;
	RECT rcWindow;
	// blit the memory surface to the video ram back surface
#if 0
	nDdVal = lpDDClipper->SetHWnd(0, clipWindow);
	if( nDdVal == DD_OK )
	{
		TRACE0("Sethwnd clipper failed 1.\n");
	}
#endif

    while(1)
    {
	    GetClientRect(clipWindow ,          &rcWindow);
	    ClientToScreen(clipWindow, (LPPOINT)&rcWindow);
		ClientToScreen(clipWindow, (LPPOINT)&rcWindow+1);

		RECT rect;

		rect.left   = offset.x;
		rect.top    = offset.y;
		rect.right  = rcWindow.right  - rcWindow.left + offset.x ;
		rect.bottom = rcWindow.bottom - rcWindow.top  + offset.y  ;

		if (rect.right>=xResMax)
		{
			 rect.right     = xResMax-1;
			 rcWindow.right = rcWindow.left+rect.right;
		}
		if (rect.bottom>=yResMax)
		{
			 rect.bottom     = yResMax-1;
			 rcWindow.bottom = rcWindow.top+rect.bottom;
		}		
		nDdVal = lpDDSPrimary->Blt( &rcWindow, lpDDSMain, &rect, DDBLT_WAIT, NULL );
		
        if(nDdVal==DD_OK)
        {
            break;
        }
        if( nDdVal==DDERR_SURFACELOST )
        {
            nDdVal = restoreall();
            if(nDdVal!=DD_OK)
            {
                break;
            }
        }
        if( nDdVal != DDERR_WASSTILLDRAWING )
        {
            break;
        }
    }

#if 0
	nDdVal = lpDDClipper->SetHWnd(0, hwndMain);
	if( nDdVal == DD_OK )
	{
		TRACE0("Sethwnd clipper failed 2.\n");
	}
#endif
}

#endif

/**************************************
* Cdd::isResAvailable		  		  *
**************************************/
BOOL Cdd::isResAvailable(SD w,SD h,SD nbbpp)
{
	BOOL bFound=FALSE;

	for (int i=0;i<Cdd::nNbAvailableVideoModes;i++)
	{
		if (Cdd::tabVideoMode[i].nbpp  == 16 &&
			Cdd::tabVideoMode[i].w     == w	&&
			Cdd::tabVideoMode[i].h     == h)
			{
				bFound=TRUE;
			}
	}
	return bFound;
}

/**************************************
* Cdd::init				  		  *
**************************************/

void Cdd::releaseAllSurfaces()
{
	for( int i=0 ; i<Cdd::SurfaceLastIndex ; i++ ) 
	{
        tabSurface[i].release();
    }

    #if _EDITOR
    dlgSprites.Refresh();
    #endif
}

void Cdd::loadAllSurfaces()
{
    #if TRACE_SURFACES_LOADING_IN_DIALOG | DISPLAY_SURFACES_LOADING_IN_DIALOG
    CString s1,s2;
    #endif

    char *pName;

    #if DISPLAY_SURFACES_LOADING_IN_DIALOG
    dlgInfo.ShowWindow(SW_SHOW);
    #endif

    #if TRACE_SURFACES_LOADING_IN_DIALOG
    TRACE("\n------- SURFACES RELEASING\n");
    #endif

    releaseAllSurfaces();

    #if TRACE_SURFACES_LOADING_IN_DIALOG
    TRACE("\n------- SURFACES LOADING Mode:%d\n",mode_screen);
    #endif

	for( int i=0 ; i<Cdd::SurfaceLastIndex ; i++ ) 
	{
        pName=tabSurface[i].getFileName();

        #if TRACE_SURFACES_LOADING_IN_DIALOG | DISPLAY_SURFACES_LOADING_IN_DIALOG
        if( pName && *pName )
        {
            s1 = pName;
        }
        else
        {
            s1 = "<Undefined>";
        }
        #endif

        #if DISPLAY_SURFACES_LOADING_IN_DIALOG
        s2.Format("%d/%d",i+1,Cdd::SurfaceLastIndex);
        dlgInfo.display(s1,s2);
        #endif

        #if TRACE_SURFACES_LOADING_IN_DIALOG
        TRACE("%8s %s\t ",LPCTSTR(s2),LPCTSTR(s1));
        #endif

        if( *pName && *pName )
        {
            #if TRACE_SURFACES_LOADING_IN_DIALOG
            TRACE("Load -> ");
            #endif

            if( tabSurface[i].load() )
            #if TRACE_SURFACES_LOADING_IN_DIALOG
            {
                TRACE("Ok\n");
            }
            else
            {
                TRACE("Failed\n");
            }
            #else
            {}
            #endif
        }
        #if TRACE_SURFACES_LOADING_IN_DIALOG
        else
        {
            TRACE("Ignore\n");
        }
        #endif
	}

    #if DISPLAY_SURFACES_LOADING_IN_DIALOG
    dlgInfo.ShowWindow(SW_HIDE);
    #endif

    #if _EDITOR
    dlgSprites.Refresh();
    #endif
}

HRESULT Cdd::getSurfDesc(LPDDSURFACEDESC lpDDSurfDesc,LPDIRECTDRAWSURFACE lpDDSurf)
{
    HRESULT result;
    memset(lpDDSurfDesc, 0, sizeof(DDSURFACEDESC));
    lpDDSurfDesc->dwSize=sizeof(DDSURFACEDESC);
    result=lpDDSurf->GetSurfaceDesc( lpDDSurfDesc);
    return result;
}


BOOL Cdd::init(SD w,SD h,SD wWin,SD hWin,SD nbbpp)
{
    CString                 sErrorLabel="Fatal error in DirectDraw init\n";
    DDSURFACEDESC			ddsd;
    HRESULT					nDdVal;
    RECT					rcWork;
    RECT					rc;
    DWORD					dwStyle;
	RECT rcWindow;
    bOk = FALSE;

	if (!fullscreen)
	{
		xResMax		  = w;
		yResMax		  = h;
	}
	else
	{
		if (isResAvailable(w,h,nbbpp))
		{
			xResMax = w;
			yResMax = h;
		}
		else
		{
			xResMax = XRES_MAX;
			yResMax = YRES_MAX;
		}
	}
	winw		  = wWin;
	winh		  = hWin;
	nbBitPerPlane = nbbpp;

	// create the main DirectDraw object
	nDdVal=DirectDrawCreate( NULL, &lpDD, NULL );

	if( nDdVal!=DD_OK )
	{
        sErrorLabel+="DirectDrawCreate\n";
		directDrawError(nDdVal,&sErrorLabel);
        lpDD=NULL;
		return FALSE;
    }

	//------------------------------------------------------------------------------------
	//	FULLSCREEN
	//------------------------------------------------------------------------------------
	if( fullscreen )
	{
	    nDdVal = lpDD->SetCooperativeLevel( hwndMain, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN );
	    if( nDdVal != DD_OK )
	    {
            sErrorLabel+="SetCooperativeLevel\n";
			directDrawError(nDdVal,&sErrorLabel);
			return FALSE;
	    }
	    nDdVal = lpDD->SetDisplayMode( xResMax, yResMax, nbBitPerPlane);
	    if( nDdVal != DD_OK )
	    {
            CString sMoreInfos;
            sMoreInfos.Format(L"%dx%d %d bits per pixel\n",xResMax,yResMax,nbBitPerPlane);
            sErrorLabel+="SetDisplayMode\n"+sMoreInfos;
			directDrawError(nDdVal,&sErrorLabel);
			return FALSE;
	    }

		if (!bUseBackBuffer)
		{
			//  Create a primary surface 
			ddsd.dwSize			= sizeof( ddsd );
			ddsd.dwFlags		= DDSD_CAPS;
			ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

			nDdVal = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );

			if( nDdVal != DD_OK )
			{
				sErrorLabel+="Create primary surface\n";
				directDrawError(nDdVal,&sErrorLabel);
				return FALSE;
			}

			// Create an offscreen bitmap in main memory
			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;  
			ddsd.dwHeight = winh;
			ddsd.dwWidth  = winw;
			nDdVal = lpDD->CreateSurface( &ddsd, &lpDDSMain, NULL );
		}
		//-----------------------------------------------------------------------
		// Flip
		//-----------------------------------------------------------------------
		else
		{
			// Create an offscreen bitmap in main memory
			ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
			ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;  
			{
				ZeroMemory(&ddsd, sizeof(ddsd));
				ddsd.dwSize = sizeof(ddsd);
				ddsd.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
				ddsd.dwBackBufferCount = 2;
				ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE |
									  DDSCAPS_FLIP |
                                      DDSCAPS_VIDEOMEMORY |
									  //DDSCAPS_3DDEVICE |
									  DDSCAPS_COMPLEX;

				ddsd.dwHeight = h;
				ddsd.dwWidth  = w;

				// try to get a triple buffered video memory surface.
				nDdVal = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);

				if (nDdVal != DD_OK)
				{
					// try to get a double buffered video memory surface.
					ddsd.dwBackBufferCount = 1;
					nDdVal = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
				}

				if (nDdVal != DD_OK)
				{
					// settle for a main memory surface.
					ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
					nDdVal = lpDD->CreateSurface(&ddsd, &lpDDSPrimary, NULL);
				}

				if (nDdVal == DD_OK)
				{
					// get a pointer to the back buffer
					DDSCAPS caps;
					caps.dwCaps = DDSCAPS_BACKBUFFER;
					nDdVal = lpDDSPrimary->GetAttachedSurface(&caps, &lpDDSMain);
				}
			}
		}
	}
	//------------------------------------------------------------------------------------
	//	WINDOW
	//------------------------------------------------------------------------------------
	else
	{
    	nDdVal=lpDD->SetCooperativeLevel(hwndMain,DDSCL_NORMAL);

	    if( nDdVal != DD_OK )
	    {
            sErrorLabel+="SetCooperativeLevel\n";
			directDrawError(nDdVal,&sErrorLabel);
			return FALSE;
	    }
	    dwStyle = GetWindowStyle(hwndMain);
	    dwStyle &= ~WS_POPUP;
	    dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX;
	    SetWindowLong(hwndMain, GWL_STYLE, dwStyle);
	    SetRect(&rc, 0, 0, xResMax, yResMax);
	    AdjustWindowRectEx(&rc,GetWindowStyle(hwndMain), GetMenu(hwndMain) != NULL,
	    GetWindowExStyle(hwndMain));

	    SetWindowPos(hwndMain, NULL, 0, 0, rc.right-rc.left, rc.bottom-rc.top,
	    SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	    SetWindowPos(hwndMain, HWND_NOTOPMOST, 0, 0, 0, 0,
	    SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

	    //  make sure our window does not hang outside of the work area
	    //  this will make people who have the tray on the top or left
	    //  happy.
	    SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
	    GetWindowRect(hwndMain,&rc);
	    if (rc.left < rcWork.left) rc.left = rcWork.left;
	    if (rc.top  < rcWork.top)  rc.top  = rcWork.top;

	    SetWindowPos(hwndMain, NULL, rc.left, rc.top, 0, 0,
	        SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);

	    GetClientRect(hwndMain, &rcWindow);
	    ClientToScreen(hwndMain, (LPPOINT)&rcWindow);
	    ClientToScreen(hwndMain, (LPPOINT)&rcWindow+1);

		//  Create a primary surface 
		ddsd.dwSize			= sizeof( ddsd );
		ddsd.dwFlags		= DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		nDdVal = lpDD->CreateSurface( &ddsd, &lpDDSPrimary, NULL );

		if( nDdVal != DD_OK )
		{
			sErrorLabel+="Create primary surface\n";
			directDrawError(nDdVal,&sErrorLabel);
			return FALSE;
		}
		// Create a clipper object for non full screen display
		// now create a DirectDrawClipper object.
	    nDdVal = lpDD->CreateClipper( 0, &lpDDClipper, NULL);
	    if( nDdVal != DD_OK )
	    {
			sErrorLabel+="CreateClipper\n";
		    directDrawError(nDdVal,&sErrorLabel);
	        return FALSE;
	    }

	    nDdVal = lpDDClipper->SetHWnd(0, hwndMain);
	    if( nDdVal != DD_OK )
	    {
			sErrorLabel+="lpDDClipper->SetHWnd\n";
		    directDrawError(nDdVal,&sErrorLabel);
	    	return FALSE;
	    }

	    nDdVal = lpDDSPrimary->SetClipper(lpDDClipper);
	    if( nDdVal != DD_OK )
	    {
			sErrorLabel+="lpDDSPrimary->SetClipper\n";
		    directDrawError(nDdVal,&sErrorLabel);
			return FALSE;
	    }

		// Create an offscreen bitmap in main memory
		ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN ;//| DDSCAPS_3DDEVICE;
		ddsd.dwHeight = winh;
		ddsd.dwWidth  = winw;

        if( Cdd::bUseVideoMem )
        {
            ddsd.ddsCaps.dwCaps|= DDSCAPS_VIDEOMEMORY; 
        }
        else
        {
            ddsd.ddsCaps.dwCaps|= DDSCAPS_SYSTEMMEMORY; 
        }

        nDdVal = lpDD->CreateSurface( &ddsd, &lpDDSMain, NULL );

        if( Cdd::bUseVideoMem && (nDdVal==DDERR_OUTOFVIDEOMEMORY) )
        {
            ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
            ddsd.ddsCaps.dwCaps |=  DDSCAPS_SYSTEMMEMORY; 

            nDdVal = lpDD->CreateSurface( &ddsd, &lpDDSMain, NULL );
        }
	}

    if( nDdVal != DD_OK )
    {
        sErrorLabel+="Create main surface\n";
		directDrawError(nDdVal,&sErrorLabel);
		return FALSE;
	}

    // Init des valeurs de masques decrivant la structure RGB de cet ecran
    InitCapsVariables();


    if( bPixelCapsAvailable )
    {
        if( nbBitsPerPixel != 16 )
        {
            AfxMessageBox(L"Bad screen mode",MB_ICONEXCLAMATION|MB_OK);
            return FALSE;
        }
    }
    else
    {
        // On n'a pas pu interroger DirectDraw pour savoir les caractéristiks de l'ecran.
        // Chais pas quoi faire ici...
    }

    // Init des surfaces associées à Dd.
    // Ces surfaces doivent être initialisées (nom de fichier etc...) avant la 1ère init.
    if( bLoadSurfacesInInit )
    {
        loadAllSurfaces();
    }

    #if _EDITOR
	    // Rafraichissement de la boite ddInfo
	    if( IsWindow(ddInfo.GetSafeHwnd())
		    &&
		    ddInfo.m_bAutoRefresh
	      )
	    {
		    ddInfo.Refresh();
	    }
    #endif

    // On retient que l'init s'est bien passée.
    bOk = TRUE;

    // init yoffset table
    if( yoffset )
    {
        delete [] yoffset;
    }

    yoffset = new long [winh];

    for(int i=0;i<winh;i++)
    {
        yoffset[i]=i*w;
    }

	return TRUE;
}

/**************************************
* Cdd::createhdc			  		  *
**************************************/

/*
 * Create a Gdi Hdc associated with the back buffer drawing
 * Surface.
 * if clear==TRUE, blitclear the surface
 *
 * The hdc must be released with the DsiDirectDraw::deleteHDC
 * function.
 * Be carefull though, all Windows operations may be held between
 * createHDC and deleteHDC. Keep this time as short as possible.
 */

extern short FROMRGB(int r,int g,int b);

HDC Cdd::createhdc(BOOL clear)
{
	HDC hdc;
	LPDIRECTDRAWSURFACE     lpDDWork;
	lpDDWork = lpDDSMain;
	//*** always need to handle DDERR_SURFACELOST, this will happen
	//*** when we get switched away from.
	if (lpDDWork->IsLost() == DDERR_SURFACELOST)
		lpDDWork->Restore();
	// use the blter to do a color fill to clear the main surface
	if (clear)
	{
		DDBLTFX ddbltfx;
		ddbltfx.dwSize = sizeof(ddbltfx);
		if(winy!=0)
		{
			RECT rcDest;

			// Partie haute en noire

            ddbltfx.dwFillColor = Cdd::makePixel(0,0,0);
			rcDest.left  = 0;
			rcDest.right = XRES_MAX;
			rcDest.top   = 0;
			rcDest.bottom= winy;
			lpDDWork->Blt(&rcDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);

			// Partie basse en noire

			ddbltfx.dwSize = sizeof(ddbltfx);
			ddbltfx.dwFillColor = Cdd::makePixel(0,0,0);
			rcDest.left  = 0;
			rcDest.right = XRES_MAX;
			rcDest.top   = winy+winh;
			rcDest.bottom= YRES_MAX;
			lpDDWork->Blt(&rcDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);

			// Partie du milieu en couleur de fond
			#if 0
			ddbltfx.dwFillColor = Cdd::makePixel(21,21,31);
			rcDest.left  = 0;
			rcDest.right = XRES_MAX;
			rcDest.top   = winy;
			rcDest.bottom= winy+winh;
			lpDDWork->Blt(&rcDest,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);
			#endif
		}
		else
		{
            /*
			// pour mode wire				
			if (screen3d->wire)
			{
				ddbltfx.dwFillColor = FROMRGB(21,21,31);
				lpDDWork->Blt(NULL,NULL,NULL,DDBLT_COLORFILL | DDBLT_WAIT,&ddbltfx);
			}
            */
		}
	}

    if (lpDDWork->GetDC(&hdc) == DD_OK)
    {
		DDhdc = hdc;
		return hdc;
 	}
	else
	{
		DDhdc = NULL;
		return NULL;
	}
}

/**************************************
* Cdd::deletehdc					  *
**************************************/

void Cdd::deletehdc(HDC hdc)
{
	LPDIRECTDRAWSURFACE     lpDDWork;
	lpDDWork = lpDDSMain;
	assert(hdc == DDhdc);
    lpDDWork->ReleaseDC(DDhdc);
}

extern BOOL bUpdateScreen;

UW *Cdd::lock()
{
    if( bOk )
    {
        ASSERT(!bLocked);

        if(!bLocked)
        {
	        DDSURFACEDESC ddsd;
	        HRESULT		  nDdVal;

			if( lpDDSMain->IsLost() == DDERR_SURFACELOST )
            {
				lpDDSMain->Restore();
            }

            // Ca ne marche pas... Ce cas arrive avec QuickRes si on change de resolution en 
            // cours de route sauvagement.
            // On force à reiitialiser.
			if( lpDDSMain->IsLost() == DDERR_SURFACELOST )
            {
				bUpdateScreen=TRUE;
            }
            else
            {
	            ddsd.dwSize = sizeof(ddsd);
	            nDdVal      = lpDDSMain->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	            if (nDdVal == DD_OK)
                {
                    ptr_screen = (UW*)ddsd.lpSurface;
			        pitch = (DWORD)ddsd.lPitch/2;
                    bLocked  = TRUE;
                }
                else
	            {
			        bLocked  = FALSE;
		            directDrawError(nDdVal);
    	            //exit(0);
	            }
            }
        }
    }
    return ptr_screen;
}

void Cdd::unlock(void)
{
    if( bOk )
    {
        ASSERT(bLocked);

        if(bLocked)
        {
	        HRESULT nDdVal;
	        nDdVal=lpDDSMain->Unlock(NULL);

            if (nDdVal == DD_OK)
            {
                bLocked=FALSE;
            }
            else
	        {
		        directDrawError(nDdVal);
    	        //exit(0);
	        }
        }
    }
}

/**************************************
* Cdd::directDrawError				  *
**************************************/

void Cdd::directDrawError(HRESULT dderror,CString *pTxtResult,BOOL bDisplayInBox)
{
	CString sErrorText;
	CString sErrorLabel;

    if( dderror!=DD_OK )
    {
	    sErrorLabel.Format(L"(Err 0x%08x) ",dderror);
    }
    else
    {
        sErrorLabel=="";
    }

	switch(dderror)
	{
    case DD_OK: sErrorLabel+=""; break;

    case DDERR_ALREADYINITIALIZED : sErrorLabel+="The object has already been initialized.\n"; break;
    case DDERR_BLTFASTCANTCLIP : sErrorLabel+="A clipper object is attached to a source surface that has passed into a call to the IDirectDrawSurface::BltFast method.\n"; break;
    case DDERR_CANNOTATTACHSURFACE : sErrorLabel+="A surface cannot be attached to another requested surface.\n"; break;
    case DDERR_CANNOTDETACHSURFACE : sErrorLabel+="A surface cannot be detached from another requested surface.\n"; break;
    case DDERR_CANTCREATEDC : sErrorLabel+="Windows cannot create any more device contexts (DCs).\n"; break;
    case DDERR_CANTDUPLICATE : sErrorLabel+="Primary and 3D surfaces, or surfaces that are implicitly created, cannot be duplicated.\n"; break;
    case DDERR_CANTLOCKSURFACE : sErrorLabel+="Access to this surface is refused because an attempt was made to lock the primary surface without DCI support.\n"; break;
    case DDERR_CANTPAGELOCK : sErrorLabel+="An attempt to page lock a surface failed. Page lock will not work on a display memory surface or an emulated primary surface.\n"; break;
    case DDERR_CANTPAGEUNLOCK : sErrorLabel+="An attempt to page unlock a surface failed. Page unlock will not work on a display memory surface or an emulated primary surface.\n"; break;
    case DDERR_CLIPPERISUSINGHWND : sErrorLabel+="An attempt was made to set a clip list for a clipper object that is already monitoring an hWnd.\n"; break;
    case DDERR_COLORKEYNOTSET : sErrorLabel+="No source color key is specified for this operation.\n"; break;
    case DDERR_CURRENTLYNOTAVAIL : sErrorLabel+="No support is currently available.\n"; break;
    case DDERR_DCALREADYCREATED : sErrorLabel+="A device context has already been returned for this surface. Only one device context can be retrieved for each surface.\n"; break;
    case DDERR_DIRECTDRAWALREADYCREATED : sErrorLabel+="A DirectDraw object representing this driver has already been created for this process.\n"; break;
    case DDERR_EXCEPTION : sErrorLabel+="An exception was encountered while performing the requested operation.\n"; break;
    case DDERR_EXCLUSIVEMODEALREADYSET : sErrorLabel+="An attempt was made to set the cooperative level when it was already set to exclusive.\n"; break;
    case DDERR_GENERIC : sErrorLabel+="There is an undefined error condition.\n"; break;
    case DDERR_HEIGHTALIGN : sErrorLabel+="The height of the provided rectangle is not a multiple of the required alignment.\n"; break;
    case DDERR_HWNDALREADYSET : sErrorLabel+="The DirectDraw CooperativeLevel HWND has already been set. It cannot be reset while the process has surfaces or palettes created.\n"; break;
    case DDERR_HWNDSUBCLASSED : sErrorLabel+="DirectDraw is prevented from restoring state because the DirectDraw CooperativeLevel HWND has been subclassed.\n"; break;
    case DDERR_IMPLICITLYCREATED : sErrorLabel+="The surface cannot be restored because it is an implicitly created surface.\n"; break;
    case DDERR_INCOMPATIBLEPRIMARY : sErrorLabel+="The primary surface creation request does not match with the existing primary surface.\n"; break;
    case DDERR_INVALIDCAPS : sErrorLabel+="One or more of the capability bits passed to the callback function are incorrect.\n"; break;
    case DDERR_INVALIDCLIPLIST : sErrorLabel+="DirectDraw does not support the provided clip list.\n"; break;
    case DDERR_INVALIDDIRECTDRAWGUID : sErrorLabel+="The GUID passed to the DirectDrawCreate function is not a valid DirectDraw driver identifier.\n"; break;
    case DDERR_INVALIDMODE : sErrorLabel+="DirectDraw does not support the requested mode.\n"; break;
    case DDERR_INVALIDOBJECT : sErrorLabel+="DirectDraw received a pointer that was an invalid DirectDraw object.\n"; break;
    case DDERR_INVALIDPARAMS : sErrorLabel+="One or more of the parameters passed to the method are incorrect.\n"; break;
    case DDERR_INVALIDPIXELFORMAT : sErrorLabel+="The pixel format was invalid as specified.\n"; break;
    case DDERR_INVALIDPOSITION : sErrorLabel+="The position of the overlay on the destination is no longer legal.\n"; break;
    case DDERR_INVALIDRECT : sErrorLabel+="The provided rectangle was invalid.\n"; break;
    case DDERR_INVALIDSURFACETYPE : sErrorLabel+="The requested operation could not be performed because the surface was of the wrong type.\n"; break;
    case DDERR_LOCKEDSURFACES : sErrorLabel+="One or more surfaces are locked, causing the failure of the requested operation.\n"; break;
    case DDERR_NO3D : sErrorLabel+="No 3D is present.\n"; break;
    case DDERR_NOALPHAHW : sErrorLabel+="No alpha acceleration hardware is present or available, causing the failure of the requested operation.\n"; break;
    case DDERR_NOBLTHW : sErrorLabel+="No blitter hardware is present.\n"; break;
    case DDERR_NOCLIPLIST : sErrorLabel+="No clip list is available.\n"; break;
    case DDERR_NOCLIPPERATTACHED : sErrorLabel+="No clipper object is attached to the surface object.\n"; break;
    case DDERR_NOCOLORCONVHW : sErrorLabel+="The operation cannot be carried out because no color conversion hardware is present or available.\n"; break;
    case DDERR_NOCOLORKEY : sErrorLabel+="The surface does not currently have a color key.\n"; break;
    case DDERR_NOCOLORKEYHW : sErrorLabel+="The operation cannot be carried out because there is no hardware support for the destination color key.\n"; break;
    case DDERR_NOCOOPERATIVELEVELSET : sErrorLabel+="A create function is called without the IDirectDraw::SetCooperativeLevel method being called.\n"; break;
    case DDERR_NODC : sErrorLabel+="No device context (DC) has ever been created for this surface.\n"; break;
    case DDERR_NODDROPSHW : sErrorLabel+="No DirectDraw raster operation (ROP) hardware is available.\n"; break;
    case DDERR_NODIRECTDRAWHW : sErrorLabel+="Hardware-only DirectDraw object creation is not possible; the driver does not support any hardware.\n"; break;
    case DDERR_NODIRECTDRAWSUPPORT : sErrorLabel+="DirectDraw support is not possible with the current display driver.\n"; break;
    case DDERR_NOEMULATION : sErrorLabel+="Software emulation is not available.\n"; break;
    case DDERR_NOEXCLUSIVEMODE : sErrorLabel+="The operation requires the application to have exclusive mode, but the application does not have exclusive mode.\n"; break;
    case DDERR_NOFLIPHW : sErrorLabel+="Flipping visible surfaces is not supported.\n"; break;
    case DDERR_NOGDI : sErrorLabel+="No GDI is present.\n"; break;
    case DDERR_NOHWND : sErrorLabel+="Clipper notification requires an HWND, or no HWND has been previously set as the CooperativeLevel HWND.\n"; break;
    case DDERR_NOMIPMAPHW : sErrorLabel+="The operation cannot be carried out because no mipmap texture mapping hardware is present or available.\n"; break;
    case DDERR_NOMIRRORHW : sErrorLabel+="The operation cannot be carried out because no mirroring hardware is present or available.\n"; break;
    case DDERR_NOOVERLAYDEST : sErrorLabel+="The IDirectDrawSurface::GetOverlayPosition method is called on an overlay that the IDirectDrawSurface::UpdateOverlay method has not been called on to establish a destination.\n"; break;
    case DDERR_NOOVERLAYHW : sErrorLabel+="The operation cannot be carried out because no overlay hardware is present or available.\n"; break;
    case DDERR_NOPALETTEATTACHED : sErrorLabel+="No palette object is attached to this surface.\n"; break;
    case DDERR_NOPALETTEHW : sErrorLabel+="There is no hardware support for 16- or 256-color palettes.\n"; break;
    case DDERR_NORASTEROPHW : sErrorLabel+="The operation cannot be carried out because no appropriate raster operation hardware is present or available.\n"; break;
    case DDERR_NOROTATIONHW : sErrorLabel+="The operation cannot be carried out because no rotation hardware is present or available.\n"; break;
    case DDERR_NOSTRETCHHW : sErrorLabel+="The operation cannot be carried out because there is no hardware support for stretching.\n"; break;
    case DDERR_NOT4BITCOLOR : sErrorLabel+="The DirectDrawSurface is not using a 4-bit color palette and the requested operation requires a 4-bit color palette.\n"; break;
    case DDERR_NOT4BITCOLORINDEX : sErrorLabel+="The DirectDrawSurface is not using a 4-bit color index palette and the requested operation requires a 4-bit color index palette.\n"; break;
    case DDERR_NOT8BITCOLOR : sErrorLabel+="The DirectDrawSurface is not using an 8-bit color palette and the requested operation requires an 8-bit color palette.\n"; break;
    case DDERR_NOTAOVERLAYSURFACE : sErrorLabel+="An overlay component is called for a non-overlay surface \n"; break;
    case DDERR_NOTEXTUREHW : sErrorLabel+="The operation cannot be carried out because no texture mapping hardware is present or available.\n"; break;
    case DDERR_NOTFLIPPABLE : sErrorLabel+="An attempt has been made to flip a surface that cannot be flipped.\n"; break;
    case DDERR_NOTFOUND : sErrorLabel+="The requested item was not found.\n"; break;
//  case DDERR_NOTINITIALIZED : sErrorLabel+="An attempt was made to invoke an interface method of a DirectDraw object created by CoCreateInstance before the object was initialized.\n"; break;
    case DDERR_NOTLOCKED : sErrorLabel+="An attempt is made to unlock a surface that was not locked.\n"; break;
    case DDERR_NOTPAGELOCKED : sErrorLabel+="An attempt is made to page unlock a surface with no outstanding page locks.\n"; break;
    case DDERR_NOTPALETTIZED : sErrorLabel+="The surface being used is not a palette-based surface.\n"; break;
    case DDERR_NOVSYNCHW : sErrorLabel+="The operation cannot be carried out because there is no hardware support for vertical blank synchronized operations.\n"; break;
    case DDERR_NOZBUFFERHW : sErrorLabel+="The operation cannot be carried out because there is no hardware support for z-buffers when creating a z-buffer in display memory or when performing a z-aware blit.\n"; break;
    case DDERR_NOZOVERLAYHW : sErrorLabel+="The overlay surfaces cannot be z-layered based on their BltOrder because the hardware does not support z-layering of overlays.\n"; break;
    case DDERR_OUTOFCAPS : sErrorLabel+="The hardware needed for the requested operation has already been allocated.\n"; break;
    case DDERR_OUTOFMEMORY : sErrorLabel+="DirectDraw does not have enough memory to perform the operation.\n"; break;
    case DDERR_OUTOFVIDEOMEMORY : sErrorLabel+="DirectDraw does not have enough display memory to perform the operation.\n"; break;
    case DDERR_OVERLAYCANTCLIP : sErrorLabel+="The hardware does not support clipped overlays.\n"; break;
    case DDERR_OVERLAYCOLORKEYONLYONEACTIVE : sErrorLabel+="An attempt was made to have more than one color key active on an overlay.\n"; break;
    case DDERR_OVERLAYNOTVISIBLE : sErrorLabel+="The IDirectDrawSurface::GetOverlayPosition method is called on a hidden overlay.\n"; break;
    case DDERR_PALETTEBUSY : sErrorLabel+="Access to this palette is refused because the palette is locked by another thread.\n"; break;
    case DDERR_PRIMARYSURFACEALREADYEXISTS : sErrorLabel+="This process has already created a primary surface.\n"; break;
    case DDERR_REGIONTOOSMALL : sErrorLabel+="The region passed to the IDirectDrawClipper::GetClipList method is too small.\n"; break;
    case DDERR_SURFACEALREADYATTACHED : sErrorLabel+="An attempt was made to attach a surface to another surface to which it is already attached.\n"; break;
    case DDERR_SURFACEALREADYDEPENDENT : sErrorLabel+="An attempt was made to make a surface a dependency of another surface to which it is already dependent.\n"; break;
    case DDERR_SURFACEBUSY : sErrorLabel+="Access to this surface is refused because the surface is locked by another thread.\n"; break;
    case DDERR_SURFACEISOBSCURED : sErrorLabel+="Access to the surface is refused because the surface is obscured.\n"; break;
    case DDERR_SURFACELOST : sErrorLabel+="Access to this surface is refused because the surface memory is gone. The DirectDrawSurface object representing this surface should have the IDirectDrawSurface::Restore method called on it.\n"; break;
    case DDERR_SURFACENOTATTACHED : sErrorLabel+="The requested surface is not attached.\n"; break;
    case DDERR_TOOBIGHEIGHT : sErrorLabel+="The height requested by DirectDraw is too large.\n"; break;
    case DDERR_TOOBIGSIZE : sErrorLabel+="The size requested by DirectDraw is too large. However, the individual height and width are OK.\n"; break;
    case DDERR_TOOBIGWIDTH : sErrorLabel+="The width requested by DirectDraw is too large.\n"; break;
    case DDERR_UNSUPPORTED : sErrorLabel+="The operation is not supported.\n"; break;
    case DDERR_UNSUPPORTEDFORMAT : sErrorLabel+="The FourCC format requested is not supported by DirectDraw.\n"; break;
    case DDERR_UNSUPPORTEDMASK : sErrorLabel+="The bitmask in the pixel format requested is not supported by DirectDraw.\n"; break;
    case DDERR_UNSUPPORTEDMODE : sErrorLabel+="The display is currently in an unsupported mode.\n"; break;
    case DDERR_VERTICALBLANKINPROGRESS : sErrorLabel+="A vertical blank is in progress.\n"; break;
    case DDERR_WASSTILLDRAWING : sErrorLabel+="The previous blit operation that is transferring information to or from this surface is incomplete.\n"; break;
    case DDERR_WRONGMODE : sErrorLabel+="This surface cannot be restored because it was created in a different mode.\n"; break;
    case DDERR_XALIGN : sErrorLabel+="The provided rectangle was not horizontally aligned on a required boundary.\n"; break;

	default:sErrorLabel += "Unknown error.\n";			
	}

	if( pTxtResult )
	{
		(*pTxtResult) += sErrorLabel;
	}
	else
	{
		sErrorText = "DirectDraw error\n"+sErrorLabel;
		pTxtResult = &sErrorText;
	}

	if( bDisplayInBox )
	{

		PRINT_ERROR_FILE(pTxtResult->GetBuffer());

		MessageBox(NULL,LPCTSTR(*pTxtResult),L"DirectDraw error",MB_APPLMODAL|MB_ICONERROR|MB_OK);
 
		#ifdef _AFXDLL
			#if !_DD_USE_GAME_MSG_FOR_ERRORS
            {
				AUTO_LOCK_DD_BEGIN(FALSE)
				MessageBox(NULL,LPCTSTR(*pTxtResult),"DirectDraw error",MB_APPLMODAL|MB_ICONERROR|MB_OK);
				AUTO_LOCK_DD_END
            }
			#else
            if( Cdd::bOk )
            {
				static nMsgId=0;
				screenMsg.del(nMsgId);
				nMsgId=screenMsg.add(*pTxtResult,5000,FontInfosDebug,0,0,HorizontalCenter|VerticalCenter|WrappingMode);
            }
            else
            {
				MessageBox(NULL,LPCTSTR(*pTxtResult),"DirectDraw error",MB_APPLMODAL|MB_ICONERROR|MB_OK);
            }
			#endif	
		#else
			#ifdef _CONSOLE
				putchar(7);
				puts(sErrorText);
			#endif
		#endif
	}
}

/**************************************
* Cdd enum modes callbacks
**************************************/

HRESULT WINAPI DirectDrawEnumCallbackString(LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext)
{
	HRESULT nRet  = DDENUMRET_CANCEL;
	char *pString = (char *)lpContext;

	if( pString==NULL )
	{
		return DDERR_INVALIDPARAMS;
	}

	sprintf(pString,"");

	if( lpDDSurfaceDesc )
	{	
		/*
		if((lpDDSurfaceDesc->dwFlags&DDSD_ALL)!=0)
		{
			 sprintf(pString,"%s- DDSD_ALL\n",pString);
		}
		*/

		if( (lpDDSurfaceDesc->dwFlags&DDSD_PIXELFORMAT)!=0 )
		{
			sprintf(pString,"%s- Pixel format :\n",pString);
			DDPIXELFORMAT *pPixelFormat = &lpDDSurfaceDesc->ddpfPixelFormat;

			if( (pPixelFormat->dwFlags&DDPF_RGB) != 0 )
			{
				sprintf(pString,"%s  RGB bits per pixel : %d\n",pString,pPixelFormat->dwRGBBitCount);
				sprintf(pString,"%s  RGB masks          : 0x%04x,0x%04x,0x%04x\n",pString,
						pPixelFormat->dwRBitMask,
						pPixelFormat->dwGBitMask,
						pPixelFormat->dwBBitMask
					  );
			}
		}

		/*
		if((lpDDSurfaceDesc->dwFlags&DDSD_DDSCAPS)!=0)
		{
			 TRACE0("DDSD_DDSCAPS\n");
		}
		*/

		if((lpDDSurfaceDesc->dwFlags&DDSD_HEIGHT)!=0)
		{
			 sprintf(pString,"%s- Height : %d\n",pString,lpDDSurfaceDesc->dwHeight);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_WIDTH)!=0)
		{
			 sprintf(pString,"%s- Width : %d\n",pString,lpDDSurfaceDesc->dwWidth);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_PITCH)!=0)
		{
			 sprintf(pString,"%s- lPitch : %d\n",pString,lpDDSurfaceDesc->lPitch);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_BACKBUFFERCOUNT)!=0)
		{
			 sprintf(pString,"%s- DDSD_BACKBUFFERCOUNT\n",pString);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_ZBUFFERBITDEPTH)!=0)
		{
			 sprintf(pString,"%s- DDSD_ZBUFFERBITDEPTH\n",pString);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_ALPHABITDEPTH)!=0)
		{
			 sprintf(pString,"%s- DDSD_ALPHABITDEPTH\n",pString);
		}
#if 0	// DDSD_LPSURFACE n'est pas défini dans ddraw.h de dxsdk	
		if((lpDDSurfaceDesc->dwFlags&DDSD_LPSURFACE)!=0)
		{
			 sprintf(pString,"%s- DDSD_LPSURFACE\n",pString);
		}
#endif		
		if((lpDDSurfaceDesc->dwFlags&DDSD_CKDESTOVERLAY)!=0)
		{
			 sprintf(pString,"%s- DDSD_CKDESTOVERLAY\n",pString);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_CKDESTBLT)!=0)
		{
			 sprintf(pString,"%s- DDSD_CKDESTBLT\n",pString);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_CKSRCOVERLAY)!=0)
		{
			 sprintf(pString,"%s- DDSD_CKSRCOVERLAY\n",pString);
		}
		
		if((lpDDSurfaceDesc->dwFlags&DDSD_CKSRCBLT)!=0)
		{
			 sprintf(pString,"%s- DDSD_CKSRCBLT\n",pString);
		}

		nRet = DDENUMRET_OK;
	}

	return nRet; 
}

HRESULT WINAPI DirectDrawEnumCallbackPixelCaps(LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext)
{
	HRESULT nRet  = DDENUMRET_OK;	// Provoque la continuation des appels de callbacks par lpDD->EnumDisplayModes(...)
	DWORD *pDWord = (DWORD *)lpContext;

    ASSERT(Cdd::nNbAvailableVideoModes<Cdd::NbMaxVideoModes);

    if( Cdd::nNbAvailableVideoModes<Cdd::NbMaxVideoModes )
    {
	    if( lpDDSurfaceDesc )
	    {	
		    if( (lpDDSurfaceDesc->dwFlags&DDSD_PIXELFORMAT)
                &&
                (lpDDSurfaceDesc->dwFlags&DDSD_HEIGHT)
                &&
                (lpDDSurfaceDesc->dwFlags&DDSD_WIDTH)
              )
		    {
			    DDPIXELFORMAT *pPixelFormat = &lpDDSurfaceDesc->ddpfPixelFormat;

			    if( (pPixelFormat->dwFlags&DDPF_RGB) != 0 )
			    {
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].maskR = pPixelFormat->dwRBitMask;
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].maskG = pPixelFormat->dwGBitMask;
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].maskB = pPixelFormat->dwBBitMask;
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].nbpp  = pPixelFormat->dwRGBBitCount;
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].w     = lpDDSurfaceDesc->dwWidth;
                    Cdd::tabVideoMode[Cdd::nNbAvailableVideoModes].h     = lpDDSurfaceDesc->dwHeight;

                    Cdd::nNbAvailableVideoModes++;
                }
		    }
	    }
    }

	return nRet; 
}

CSprite *Cdd::getSprite(UINT nId)
{
    CSprite *pSprite=NULL;

    if( nId>0 && Cdd::tabSurface )
    {
        BOOL bFound=FALSE;
        int  nSurfaceIndex=0;
        UINT nSpriteIndex;

        while( !bFound && nSurfaceIndex<Cdd::SurfaceLastIndex )
        {
            CSurface *pSurface=&Cdd::tabSurface[nSurfaceIndex];

            if( pSurface )
            {
                CSprite *tabSprites = pSurface->getTabSprites();
                UINT nNbSprites=pSurface->getNbSprites();

                if( (nNbSprites>0) && tabSprites )
                {
                    nSpriteIndex = 0;

                    while( !bFound && nSpriteIndex<nNbSprites )
                    {
                        if( tabSprites[nSpriteIndex].m_nId==nId )
                        {
                            pSprite = &tabSprites[nSpriteIndex];
                            bFound  = TRUE;
                        }

                        nSpriteIndex++;
                    }
                }
            }

            nSurfaceIndex++;
        }
    }

    return pSprite;
}

/**************************************
* CSprite
**************************************/

UINT CSprite::m_nGlobalId=1;

CSprite::CSprite()
{
    init();
}


CSprite::~CSprite()
{
	if(m_segment!=NULL) 
    {
        free(m_segment);
    }
}

RGB rgbGreen={0,255,0};

void CSprite::init()
{
    m_rcData.left           = 0;
    m_rcData.top            = 0;
    m_rcData.right          = 0;
    m_rcData.bottom         = 0;
    m_nId                   = 0;
    m_pParentSurface        = NULL;
    m_rgbLow                = rgbGreen;
    m_rgbHigh               = rgbGreen;
    m_bTransparent          = FALSE;
    m_ptHotSpot.x           = 0;
    m_ptHotSpot.y           = 0;
    m_ptTranslation.x       = 0;
    m_ptTranslation.y       = 0;
    m_ptAnimTranslation.x   = 0;
    m_ptAnimTranslation.y   = 0;
    m_nAnimIndex            = 0;  
    m_nSegmentAnimIndex     =-1;
	m_segmented             = FALSE;
	m_segment               = NULL;
	m_segmentcount          = 0;
    m_bUseOt                = FALSE;
    m_nNbAnims              = 1;
}

BOOL CSprite::draw(int nX,int nZ,int nAnimIndex,RECT *pDisplayRect,float fZoom)
{
    BOOL  bRes   = FALSE;
    CRect rcData = m_rcData; // Copie en cas d'animation du sprite

    if( m_pParentSurface && m_pParentSurface->m_pSurface )
    {
        m_nAnimIndex = nAnimIndex;

        if( m_nAnimIndex>0 && m_nAnimIndex<m_nNbAnims )
        {
            rcData.top    += nAnimIndex*m_ptAnimTranslation.y;
            rcData.left   += nAnimIndex*m_ptAnimTranslation.x;
            rcData.bottom += nAnimIndex*m_ptAnimTranslation.y;
            rcData.right  += nAnimIndex*m_ptAnimTranslation.x;
        }

        if( m_bUseOt && Cdd::bUseBlitsOt)
        {
            if( m_nAnimIndex != m_nSegmentAnimIndex )
            {
                m_nSegmentAnimIndex = m_nAnimIndex;
                m_segmented         = FALSE;

                if( m_segment )
                {
                    free(m_segment);
                    m_segment=NULL;
                }
            }

		    if(!m_segmented)
		    {
                /*
			    UW *ptrpixel;
			    SD x,y;
			    SD xstart,xend;
			    int *ptrsegment;
			    DDSURFACEDESC ddsd;
			    HRESULT		  nVal;
			    SD nbseg;
			    
			    memset(&ddsd,0,sizeof(ddsd));
			    ddsd.dwSize=sizeof(ddsd);
			    nVal=m_pParentSurface->m_pSurface->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
			    if(nVal==DD_OK)
			    {
				    // compter les segments dans le sprite
				    nbseg=0;
				    for(y=rcData.top;y<=rcData.bottom;y++)
				    {
					    xstart=-1;
					    xend=-1;
					    for(x=rcData.left;x<=rcData.right;x++)
					    {
						    ptrpixel=(UW *)((void *)ddsd.lpSurface);
						    ptrpixel+=m_pParentSurface->getW()*y;
						    ptrpixel+=x;
						    if(xstart==-1)
						    {
							    if(*ptrpixel!=Cdd::maskG)
							    {
								    xstart=x;
							    }
						    }
						    else
						    {
							    if(*ptrpixel==Cdd::maskG)
							    {
								    xend=x;
								    // stocker le segment
								    nbseg++;
								    xstart=-1;
							    }
						    }
					    }
					    if(xstart!=-1)
					    {
						    nbseg++;
					    }
				    }
				    m_segmentcount=nbseg;
				    // allouer buffer pour les segments
				    ptrsegment=m_segment=(int *)(malloc(sizeof(int)*3*m_segmentcount));
				    // stocker les segments
				    nbseg=0;
				    for(y=rcData.top;y<=rcData.bottom;y++)
				    {
					    xstart=-1;
					    xend=-1;
					    for(x=rcData.left;x<=rcData.right;x++)
					    {
						    ptrpixel=(UW *)((void *)ddsd.lpSurface);
						    ptrpixel+=m_pParentSurface->getW()*y;
						    ptrpixel+=x;
						    if(xstart==-1)
						    {
							    if(*ptrpixel!=Cdd::maskG)
							    {
								    xstart=x;
							    }
						    }
						    else
						    {
							    if(*ptrpixel==Cdd::maskG)
							    {
								    // stocker le segment
								    *ptrsegment++=y-rcData.top;
								    *ptrsegment++=xstart-rcData.left;
								    *ptrsegment++=x-xstart;
								    xstart=-1;
							    }
						    }
					    }
					    if(xstart!=-1)
					    {
						    *ptrsegment++=y-rcData.top;
						    *ptrsegment++=xstart-rcData.left;
						    *ptrsegment++=x-xstart;
					    }
				    }

			        m_pParentSurface->m_pSurface->Unlock(NULL);
			    }
			    m_segmented=TRUE;
                */
		    }

		    {
                /*
			    DDSURFACEDESC ddsd;
			    HRESULT		  nVal;

			    int *ptrsegment;
			    memset(&ddsd,0,sizeof(ddsd));
			    ddsd.dwSize=sizeof(ddsd);
			    nVal=m_pParentSurface->m_pSurface->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);
			    if(nVal==DD_OK)
			    {
				    SD i,sy,sx,dx,dy,count;
				    UW *sptr;
				    UW *dptr;
				    ptrsegment=m_segment;
				    for(i=0;i<m_segmentcount;i++)
				    {
					    
					    sy=*ptrsegment++;
					    sx=*ptrsegment++;
					    count=*ptrsegment++;
					    dx= sx + m_ptTranslation.x -m_ptHotSpot.x;
					    dy= sy + m_ptTranslation.y -m_ptHotSpot.y;
					    sy+=rcData.top;
					    sx+=rcData.left;
					    dx+=nX;
					    dy+=nZ;
					    sptr=(UW *)((void *)ddsd.lpSurface);
					    sptr+=m_pParentSurface->getW()*sy;
					    sptr+=sx;
                        dptr=Cdd::get_screen_addr(0,0);
					    if(dptr!=NULL)
					    {
                            dptr=Cdd::get_screen_addr(dx,dy/2);
					        if(count>0) screen3d->ftb_hline_bitmap(dy,dx,dx+count,sptr);
					    }
				    }
				    m_pParentSurface->m_pSurface->Unlock(NULL);

                    bRes = TRUE;

                    if( pDisplayRect )
                    {
                        pDisplayRect->left   = nX;
                        pDisplayRect->top    = nZ;
                        pDisplayRect->right  = nX+rcData.right-rcData.left;
                        pDisplayRect->bottom = nZ+rcData.bottom-rcData.top;
                    }
			    }
			*/
			}
        }
        else
        {
            // Test depassement rectangle de données.
            // Fait ici à cause de la gestion des anims.
            if( rcData.right>=m_pParentSurface->getW() ) 
            {
                rcData.right=m_pParentSurface->getW()-1;
            }

            if( rcData.bottom>=m_pParentSurface->getH() ) 
            {
                rcData.bottom=m_pParentSurface->getH()-1;
            }

            if( rcData.left<0 ) 
            {
                rcData.left=0;
            }

            if( rcData.top<0 ) 
            {
                rcData.top=0;
            }

            if( (rcData.top<rcData.bottom) && (rcData.left<rcData.right) )
            {
                nX  += m_ptTranslation.x;
                nZ  += m_ptTranslation.y;
        
                nX  -= m_ptHotSpot.x;
                nZ  -= m_ptHotSpot.y;

                bRes = m_pParentSurface->draw(nX,nZ,&rcData,fZoom,m_bTransparent,&m_rgbLow,&m_rgbHigh,pDisplayRect);
            }
        }
    }

    return bRes;
}

/**************************************
* CSurface
**************************************/

CSurface::CSurface()
{
	m_pSurface			    = NULL;
	m_nW				    = 0;
	m_nH				    = 0;
	m_bBlitAvailable	    = TRUE;
    m_tabSprites            = NULL;
    m_nNbSprites            = 0;
    m_nSpecificResolution   = LOWRES;
    m_bResolutionSpecific   = FALSE;
    m_bUseVideoMem          = FALSE;
    m_bOddLinesCleared      = FALSE;
    m_pFnConstraint         = NULL;

    memset(m_sFileName,0,nFileNameMaxLen+1);
}

CSurface::~CSurface()
{
	reset();
}

void CSurface::setResolution(BOOL bResolutionSpecific,int nSpecificResolution)
{
    m_nSpecificResolution = nSpecificResolution;
    m_bResolutionSpecific = bResolutionSpecific;
}

void CSurface::setFileName(wchar_t *sFileName)
{
    if( sFileName )
    {
        wcsncpy(m_sFileName,sFileName,nFileNameMaxLen);
    }
}

void CSurface::setNbSprite(UINT nNbSprites)
{
    if( nNbSprites>0 )
    {
        if( nNbSprites>nNbMaxSprites )
        {
            nNbSprites=nNbMaxSprites;
        }

        m_nNbSprites = nNbSprites;
        m_tabSprites = new CSprite [m_nNbSprites];

        if( !m_tabSprites )
        {
            m_nNbSprites = 0;
        }
    }
}

UINT CSurface::describeSprite(CString sName,int left,int top,int right,int bottom,int xHot,int yHot,int xTranslation,int yTranslation,int xAnimTranslation,int yAnimTranslation,int nNbAnims,BOOL bTransparent,RGB *pRgbLow,RGB *pRgbHigh,BOOL bUseOt)
{
    UINT nFirstFreeSprite=0;
    UINT nId=0;
    
    while( (nFirstFreeSprite<m_nNbSprites) && (m_tabSprites[nFirstFreeSprite].m_nId!=0) )
    {
        nFirstFreeSprite++;
    }

    if( nFirstFreeSprite<m_nNbSprites )
    {
        if( !pRgbLow  ) pRgbLow=&rgbBlack;
        if( !pRgbHigh ) pRgbHigh=&rgbBlack;

        m_tabSprites[nFirstFreeSprite].m_nId                =  CSprite::m_nGlobalId++;
        m_tabSprites[nFirstFreeSprite].m_pParentSurface     =  this;
        m_tabSprites[nFirstFreeSprite].m_rcData             =  CRect(left,top,right,bottom);
        m_tabSprites[nFirstFreeSprite].m_ptHotSpot          =  CPoint(xHot,yHot);
        m_tabSprites[nFirstFreeSprite].m_ptTranslation      =  CPoint(xTranslation,yTranslation);
        m_tabSprites[nFirstFreeSprite].m_ptAnimTranslation  =  CPoint(xAnimTranslation,yAnimTranslation);
        m_tabSprites[nFirstFreeSprite].m_bTransparent       =  bTransparent;
        m_tabSprites[nFirstFreeSprite].m_rgbLow             = *pRgbLow;
        m_tabSprites[nFirstFreeSprite].m_rgbHigh            = *pRgbHigh;
        m_tabSprites[nFirstFreeSprite].m_bUseOt             =  bUseOt;
        m_tabSprites[nFirstFreeSprite].m_sName              =  sName;
        m_tabSprites[nFirstFreeSprite].m_nNbAnims           =  nNbAnims;

        nId = m_tabSprites[nFirstFreeSprite].m_nId;
    }

    return nId;
}

void CSurface::describeSprites(UINT nNbSprites,...)
{
    CRect   rcData; 
    RGB     rgb;
    UINT    i;

    if( nNbSprites>0 )
    {
        if( nNbSprites>nNbMaxSprites )
        {
            nNbSprites=nNbMaxSprites;
        }

        m_nNbSprites = nNbSprites;
        m_tabSprites = new CSprite [m_nNbSprites];

        if( m_tabSprites )
        {
            va_list marker;

            va_start(marker,nNbSprites);

            for( i=0 ; i<nNbSprites ; i++ )
            {
                m_tabSprites[i].m_nId            = CSprite::m_nGlobalId++;
                m_tabSprites[i].m_pParentSurface = this;

                rcData.left     = va_arg(marker,int);    
                rcData.top      = va_arg(marker,int);    
                rcData.right    = va_arg(marker,int);    
                rcData.bottom   = va_arg(marker,int);    

                m_tabSprites[i].m_rcData            = rcData;

                m_tabSprites[i].m_ptHotSpot.x       = va_arg(marker,int);
                m_tabSprites[i].m_ptHotSpot.y       = va_arg(marker,int);

                m_tabSprites[i].m_ptTranslation.x   = va_arg(marker,int);
                m_tabSprites[i].m_ptTranslation.y   = va_arg(marker,int);

                m_tabSprites[i].m_bTransparent      = va_arg(marker,BOOL);

                if( m_tabSprites[i].m_bTransparent )
                {
                    rgb.r = va_arg(marker,UINT);
                    rgb.g = va_arg(marker,UINT);
                    rgb.b = va_arg(marker,UINT);

                    m_tabSprites[i].m_rgbLow = rgb;

                    rgb.r = va_arg(marker,UINT);
                    rgb.g = va_arg(marker,UINT);
                    rgb.b = va_arg(marker,UINT);

                    m_tabSprites[i].m_rgbHigh = rgb;
                }
            }

            va_end(marker);
        }
        else
        {
            CString sError;
            sError.Format(L"Can't allocate %d sprite descriptors\nfor surface",m_nNbSprites);
            AfxMessageBox(sError);
            m_nNbSprites = 0;
        }
    }
}

void CSurface::reset()
{
    if( m_tabSprites )
    {
        delete [] m_tabSprites;
    }

	m_pSurface			= NULL;
	m_nW				= 0;
	m_nH				= 0;
	m_bBlitAvailable	= TRUE;
    m_tabSprites        = NULL;
    m_nNbSprites        = 0;

    memset(m_sFileName,0,nFileNameMaxLen+1);
}

HRESULT CSurface::restore()
{
    HRESULT nRet=DD_OK;

    if( m_pSurface )
    {
        if( m_pSurface->IsLost()==DDERR_SURFACELOST )
        {
            nRet = m_pSurface->Restore();

            if( nRet==DD_OK )
            {
                if( load() )
                {
                    nRet=DD_OK;
                }
                else
                {
                    nRet = DDERR_UNSUPPORTED;
                }
            }
            else
            {
                m_bBlitAvailable = FALSE;
            }
        }
    }

    return nRet;
}

HRESULT CSurface::init(int nW,int nH)
{
	// allocation de la surface
    HRESULT             nDdVal;
    DDSURFACEDESC       ddsd;

    ddsd.dwSize			= sizeof( ddsd );
    ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.dwHeight		= nW;
    ddsd.dwWidth		= nH;

    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    
    if( Cdd::bUseVideoMem )
    {
        ddsd.ddsCaps.dwCaps|= DDSCAPS_VIDEOMEMORY; 
    }
    else
    {
        ddsd.ddsCaps.dwCaps|= DDSCAPS_SYSTEMMEMORY; 
    }

    nDdVal = Cdd::lpDD->CreateSurface( &ddsd, &m_pSurface, NULL );

    if( Cdd::bUseVideoMem && (nDdVal==DDERR_OUTOFVIDEOMEMORY) )
    {
        ddsd.ddsCaps.dwCaps &= ~DDSCAPS_VIDEOMEMORY;
        ddsd.ddsCaps.dwCaps |=  DDSCAPS_SYSTEMMEMORY; 

        nDdVal = Cdd::lpDD->CreateSurface( &ddsd, &m_pSurface, NULL );
    }

    if( nDdVal != DD_OK )
    {

		m_nW = 0;
		m_nH = 0;
                        
        Cdd::directDrawError(nDdVal);
    }
	else
	{
		m_nW = nW;
		m_nH = nH;
	}

	return nDdVal;
}

BOOL CSurface::release()
{
    if( m_pSurface )
    {
        m_pSurface->Release();
        m_pSurface=NULL;
    }

    return TRUE;
}

BOOL CSurface::load()
{
	FILE				*pFile;
	struct Craw_header   header;
	int					 wb,hb,nbc;
	HRESULT				 nDdVal;
    BOOL                 bRes;

    bRes = FALSE;

    if( m_sFileName[0] )
    {
	    pFile = dsi_fopen(m_sFileName,L"rb");

	    // chargement des infos du fichier raw
	    if( pFile )
	    {
		    dsi_fread(&header,sizeof(header),1,pFile);

		    wb  = ((header.w>>8)&0x00ff) | ((header.w<<8)&0xff00);
		    hb  = ((header.h>>8)&0x00ff) | ((header.h<<8)&0xff00);
		    nbc = ((header.nbcolor>>8)&255) | ((header.nbcolor<<8)&0xff00);

            // Init de la surface
	        nDdVal = init(wb,hb);

            if( nDdVal != DD_OK )
            {
		        CString textError;
		        textError.Format(L" Erreur: Chargement de %s\nImpossible de créer surface %dx%d pixels\n",m_sFileName,wb,hb);		
		        Cdd::directDrawError(nDdVal,&textError,FALSE);
		        AfxMessageBox(textError);
	        }
            else
            {
	            // lock surface
                DDSURFACEDESC       ddsd;
	            ddsd.dwSize = sizeof(ddsd);

	            nDdVal=m_pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

	            if ( nDdVal != DD_OK)
	            {
		            Cdd::directDrawError(nDdVal);
    	            ASSERT(FALSE);
	            }
                else
                {
                    // Libération des segments de tous les sprites pour recalcul
                    CSprite *pSprite = getTabSprites();

                    if( pSprite )
                    {
                        for( UINT i=0 ; i<getNbSprites() ; i++,pSprite++ )
                        {
                            pSprite->m_segmented=FALSE;

                            if( pSprite->m_segment )
                            {
                                free(pSprite->m_segment);
                                pSprite->m_segment=NULL;
                            }
                        }
                    }

                    // Init du booléen indiquant que les lignes impaires ont été maravées.
                    m_bOddLinesCleared = FALSE;

                    // Chargement
	                UW      *pSurfacePixel = ((UW *)ddsd.lpSurface);
                    RGB     *pPal          = NULL;
                    RGB     *pLineRgb      = NULL;
                    UCHAR   *pLinePal      = NULL;

                    try
                    {
	                    if( !pSurfacePixel )
	                    {
                            ASSERT(pSurfacePixel);
                            throw 0;
                        }

                        UW  *pPixel = pSurfacePixel;

		                if( nbc==0 )
		                {
                            pLineRgb = new RGB [wb];

                            if( !pLineRgb )
                            {
                                throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY);
                            }

			                for(int y=0;y<hb;y++)
			                {
                                if( dsi_fread(pLineRgb,sizeof(RGB),wb,pFile)!=(UINT)wb )
                                {
                                    throw CdsiError(DSI_ERROR_READ_ERROR);
                                }

                                RGB *pRgb=pLineRgb;

				                for(int x=0;x<wb;x++)
				                {
					                *pPixel = Cdd::makePixel(pRgb->r,pRgb->g,pRgb->b);

					                pPixel++;
                                    pRgb++;
				                }
			                }

                            bRes = TRUE;
		                }
		                else
		                {
			                RGB *pPalItem;

			                dsi_fseek(pFile,sizeof(header),SEEK_SET);

                            pPal = new RGB [nbc];

                            if( !pPal )
			                {
                                throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY);
                            }

				            if( dsi_fread((void*)pPal,sizeof(RGB),(size_t)nbc,pFile)==(size_t)nbc )
				            {
                                pLinePal = new UCHAR [wb];

                                if( !pLinePal )
                                {
                                    throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY);
                                }

					            for(int y=0;y<hb;y++)
					            {
                                    if( dsi_fread(pLinePal,1,wb,pFile)!=(UINT)wb )
                                    {
                                        throw CdsiError(DSI_ERROR_READ_ERROR);
                                    }

                                    UCHAR *pPix=pLinePal;

						            for(int x=0;x<wb;x++)
						            {
							            pPalItem = pPal+(*pPix);
							           *pPixel   = Cdd::makePixel(pPalItem->r,pPalItem->g,pPalItem->b);
                                       
							            pPixel++;
                                        pPix++;
						            }
					            }

                                bRes = TRUE;
			                }
		                }
                    }
                    catch( CdsiError E )
                    {
                        E.displayMessageBox(m_sFileName,L"Loading surface error");    
                    }
                    catch( int n )
                    {
                        n;    
                    }

                    if( pPal )
                    {
                        delete [] pPal;
                    }

                    if( pLineRgb )
                    {
                        delete [] pLineRgb;
                    }

                    if( pLinePal )
                    {
                        delete [] pLinePal;
                    }

	                // unlock surface
	                nDdVal = m_pSurface->Unlock(NULL);

	                if ( nDdVal != DD_OK)
	                {
		                Cdd::directDrawError(nDdVal);
	                }
                }
            }

	        // Fermeture du fichier
	        dsi_fclose(pFile);
	    }
	    else
	    {
		    CString textError;
		    textError.Format(L" Erreur: LoadSurface, fichier %s non trouvé.",m_sFileName);		
			PRINT_ERROR_FILE(textError.GetBuffer());
		    //AfxMessageBox(textError);
	    }
    }

    m_bBlitAvailable = bRes;

	return bRes;
}

BOOL CSurface::changeColor(RECT *pRect,int nMinS,int nMaxS,int nMinH,int nMaxH,int nMinL,int nMaxL,int nDeltaH)
{
    RECT rcDef;
    BOOL bRes=FALSE;

    if( !pRect )
    {
        rcDef.left   = 0;
        rcDef.top    = 0;
        rcDef.right  = m_nW-1;
        rcDef.bottom = m_nH-1;

        pRect=&rcDef;
    }
    else
    {
        if( pRect->left  <0     ) pRect->left   = 0;
        if( pRect->top   <0     ) pRect->top    = 0;
        if( pRect->right >=m_nW ) pRect->right  = m_nW-1;
        if( pRect->bottom>=m_nH ) pRect->bottom = m_nH-1;
    }

    if( (pRect->left<pRect->right) && (pRect->top<pRect->bottom) )
    {
        UW              *pPixel;
        int             nJumpLine;
        int             x,y;
        HRESULT         nDdVal;
        DDSURFACEDESC   ddsd;
        RGB_MEMBER      r,g,b;
        int             h,l,s;

	    // lock surface
	    ddsd.dwSize = sizeof(ddsd);
	    nDdVal      = m_pSurface->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);

        if( nDdVal==DD_OK )
        {
            pPixel      = (UW *)ddsd.lpSurface;
            nJumpLine   = m_nW - (pRect->right-pRect->left+1);

            if( nMinH>127 )
            {
                nMinH -= 256;
            }

            for( y=pRect->top  ; y<=pRect->bottom ; y++ )
            {
                for( x=pRect->left ; x<=pRect->right ; x++,pPixel++ )
                {
                    Cdd::getPixel(*pPixel,&r,&g,&b);
                    RgbToHls(r,g,b,&h,&l,&s);

                    if( h!=HLS_H_UNDEFINED )
                    {
                        if( h>127 )
                        {
                            h -= 256;
                        }

                        if( h>=nMinH && h<=nMaxH &&
                            l>=nMinL && l<=nMaxL &&
                            s>=nMinS && s<=nMaxS 
                          )
                        {
                            h += nDeltaH;                        

                            while( h<0 )
                            {
                                h += 256;
                            }

                            while( h>255 )
                            {
                                h-=256;
                            }

                            HlsToRgb(h,l,s,&r,&g,&b);

                            *pPixel = Cdd::makePixel(r,g,b);
                        }
                    }
                }

                pPixel += nJumpLine;
            }

            // unlock surface
            m_pSurface->Unlock(NULL);

            bRes=TRUE;
        }
    }

    return bRes;
}

BOOL CSurface::draw(CPoint *pDest,CRect *pSrcRect,float fZoom,BOOL bTransparent,RGB *pRgbLow,RGB *pRgbHigh,RECT *pDisplayRect)
{
	BOOL bRes=FALSE;

	if( pDest )
	{
		bRes = draw(pDest->x,pDest->y,pSrcRect,fZoom,bTransparent,pRgbLow,pRgbHigh,pDisplayRect);
	}

	return bRes;
}

BOOL CSurface::draw(int nX,int nY,CRect *pSrcRect,float fZoom,BOOL bTransparent,RGB *pRgbLow,RGB *pRgbHigh,RECT *pDisplayRect)
{
	BOOL bRes=FALSE;

	CRect rcDest;
	int nW,nH;

	rcDest.left = nX;
	rcDest.top  = nY;

    if( fZoom==1.0f )
    {
	    if( pSrcRect )
	    {
		    nW = pSrcRect->right-pSrcRect->left+1;		
		    nH = pSrcRect->bottom-pSrcRect->top+1;		
	    }
	    else
	    {
		    nW = m_nW;		
		    nH = m_nH;		
	    }
    }
    else
    {
	    if( pSrcRect )
	    {
		    nW = (int)((pSrcRect->right-pSrcRect->left+1)*fZoom);		
		    nH = (int)((pSrcRect->bottom-pSrcRect->top+1)*fZoom);		
	    }
	    else
	    {
		    nW = (int)(m_nW*fZoom);		
		    nH = (int)(m_nH*fZoom);		
	    }
    }

	rcDest.right  = rcDest.left+nW-1;		
	rcDest.bottom = rcDest.top +nH-1;		

	bRes = draw(&rcDest,pSrcRect,bTransparent,pRgbLow,pRgbHigh,pDisplayRect);

	return bRes;
}

BOOL CSurface::draw(CRect *pDestRect,CRect *pSrcRect,BOOL bTransparent,RGB *pRgbLow,RGB *pRgbHigh,RECT *pDisplayRect)
{
	LPDIRECTDRAWSURFACE lpDDWork;
	HRESULT				nRes;
	BOOL				bRes;
	DDBLTFX				BltFx;
	CRect				rcDefaultSrcRect;
	CRect				rcDefaultDestRect;
	CRect				rcDestRect;
    RECT                rcData,rcScreen;

    ASSERT(!Cdd::bLocked);

	#if _SPRITE_DISPLAY_INFOS_WHEN_BLIT
		static UINT	nMsgId=0;
		screenMsg.del(nMsgId);
	#endif

	lpDDWork = Cdd::lpDDSMain;
	bRes     = FALSE;

    if( !pRgbLow )
    {
        pRgbLow = &rgbBlack;        
    }
    
    if( !pRgbHigh )
    {
        pRgbHigh = &rgbBlack;        
    }
    
	if( !pSrcRect )
	{
		rcDefaultSrcRect.SetRect(0,0,m_nW-1,m_nH-1);	
		pSrcRect = &rcDefaultSrcRect;
	}
	
	if( !pDestRect )
	{
		rcDefaultDestRect.SetRect(0,0,m_nW-1,m_nH-1);	
		pDestRect = &rcDefaultDestRect;
	}
	
	#if (_SPRITE_DISPLAY_INFOS_WHEN_BLIT | _SPRITE_TRACE_INFOS_WHEN_BLIT)
    {
        CString	sInfos;

		sInfos.Format(	"SURFACE BLIT DATA(%3d,%3d,%3d,%3d) --> SCREEN(%3d,%3d,%3d,%3d)\n",
						pSrcRect->left,
						pSrcRect->top,
						pSrcRect->right,
						pSrcRect->bottom,
						pDestRect->left,
						pDestRect->top,
						pDestRect->right,
						pDestRect->bottom
					 );

        #if _SPRITE_DISPLAY_INFOS_WHEN_BLIT
        {
		    nMsgId = screenMsg.add(sInfos,1000,getFont(FontInfosDebug),NoJustify,50);
        }
        #else
        {
            TRACE(LPCTSTR(sInfos));
        }
        #endif
    }
	#endif

	if( m_bBlitAvailable 
        &&
        (pSrcRect->top  < pSrcRect->bottom)
        &&
        (pSrcRect->left < pSrcRect->right )
      )
	{	
        if( (lpDDWork!=NULL) || Cdd::bUseBlitsBatch )
		{
			// Always need to handle DDERR_SURFACELOST, this will happen when we get switched away from.
			if( !Cdd::bUseBlitsBatch && (lpDDWork->IsLost()==DDERR_SURFACELOST) )
			{
				lpDDWork->Restore();
			}

			rcDestRect = *pDestRect;

            // Rectangle de données
            rcData.left     = pSrcRect->left;
            rcData.right    = pSrcRect->right;
            rcData.top      = pSrcRect->top;
            rcData.bottom   = pSrcRect->bottom;

            // Rectangle écran
            rcScreen        = *LPRECT(rcDestRect);

            // Si le rectangle dépasse de l'écran, on le clippe
            // (!) CE CLIPPING DE LA MORT NE PREND PAS LE ZOOM EN COMPTE!!
            int nMaxX = (Cdd::winx+Cdd::winw-1);
            int nMaxY = (Cdd::winy+Cdd::winh-1);

            if(rcScreen.top<Cdd::winy) 
            {
                rcData.top   += (Cdd::winy-rcScreen.top);
                rcScreen.top  =  Cdd::winy;
            }

            if(rcScreen.bottom>nMaxY) 
            {
                rcData.bottom  -= (rcScreen.bottom-nMaxY);
                rcScreen.bottom = nMaxY;
            }

            if(rcScreen.left<Cdd::winx) 
            {
                rcData.left  += (Cdd::winx-rcScreen.left);
                rcScreen.left =  Cdd::winx;
            }

            if(rcScreen.right>nMaxX) 
            {
                rcData.right  -= (rcScreen.right-nMaxX);
                rcScreen.right = nMaxX;
            }

            #if _SPRITE_INC_DATA_RECT
            {
                rcData.right++;
                rcData.bottom++;
            }
            #endif

            // Si le rectangle dépasse de l'ecran, on ne l'affiche pas
            if( (rcScreen.top<rcScreen.bottom)
                &&
                (rcScreen.left<rcScreen.right)
                &&
                (rcScreen.top>=Cdd::winy) 
                && 
                (rcScreen.bottom<(Cdd::winy+Cdd::winh)) 
                &&
                (rcScreen.left>=Cdd::winx) 
                && 
                (rcScreen.right<(Cdd::winx+Cdd::winw)) 
              )
			{
                if( pDisplayRect )
                {
                    *pDisplayRect = rcScreen;
                }

                #if _SPRITE_INC_SCREEN_RECT
                {
                    rcScreen.bottom++;
                    rcScreen.right++;
                }
                #endif

				// Préparation de la boucle de blit
				CString sError;

				#if !_SPRITE_DISPLAY_INFOS_WHEN_BLIT 
                {
					sError.Format(	L"SURFACE BLIT DATA(%3d,%3d,%3d,%3d) --> SCREEN(%3d,%3d,%3d,%3d)\n",
									rcData.left,
									rcData.top,
									rcData.right,
									rcData.bottom,
									rcScreen.left,
									rcScreen.top,
									rcScreen.right,
									rcScreen.bottom
								 );
                }
				#else
                {
					sError = "";
                }
				#endif

                DWORD   dwFlags        = DDBLT_WAIT;
                BOOL    bNoError       = TRUE;
				BOOL    bDone          = FALSE;
                BOOL    bSecondChance  = FALSE;
                HRESULT nRepairRes;

                // Init de la structure utilisée pour la transparence
                DDCOLORKEY ddColorKey;
                if( bTransparent )
                {
                    ddColorKey.dwColorSpaceLowValue  = Cdd::makePixel( pRgbLow->r,  pRgbLow->g,  pRgbLow->b  );
                    ddColorKey.dwColorSpaceHighValue = Cdd::makePixel( pRgbHigh->r, pRgbHigh->g, pRgbHigh->b );
                }

				// Init structure DDBLTFX
				memset(&BltFx,0,sizeof(BltFx));
				BltFx.dwSize=sizeof(DDBLTFX);

                if( bTransparent )
                {
                    #if 1
                    {
                        BltFx.ddckSrcColorkey= ddColorKey;
                        dwFlags |= DDBLT_KEYSRCOVERRIDE;
                    }
                    #else
                    {
                        m_pSurface->SetColorKey(DDCKEY_COLORSPACE,&ddColorKey);
                        dwFlags |= DDBLT_KEYSRC;
                    }
                    #endif
                }

                if( Cdd::bUseBlitsBatch )
                {        
                    bRes = Cdd::addBlitInBatch(&rcData,&rcScreen,m_pSurface,dwFlags,&BltFx);
                }
                else
                {
                    // Boucle de blit. Se fait au maximum 2 fois
				    // (!) ATTENTION AU BOOLÉEN bDone. Si jamais mis à TRUE => Boucle infinie
				    while( !bDone )
				    {
					    nRes = lpDDWork->Blt( &rcScreen,m_pSurface,&rcData,dwFlags,&BltFx );

					    if( nRes!=DD_OK )
					    {
                            bNoError = FALSE;

                            // Stockage de l'intitulé de l'erreur avant réparation.
						    Cdd::directDrawError(nRes,&sError,FALSE);

                            // Traitements specifiques. Au besoin, on alimente sError avec
                            // d'autres erreurs liées aux tentatives de reparations.
                            if( bSecondChance )
                            {
							    bDone			 = TRUE;
				  		      //m_bBlitAvailable = FALSE; Vrai si erreur grave de la surface (perdue par exemple...)
                                bRes             = FALSE;
                            }
                            else
                            {
                                nRepairRes = DD_OK;

						        switch( nRes )
						        {
                                    case DDERR_SURFACELOST:
                                    {
                                        sError += "REPAIR ATTEMPT AFTER SURFACE LOST :\n";
                                        nRepairRes = m_pSurface->Restore();
                                    }
						            break;
                            
                                    case DDERR_SURFACEBUSY :
						            {
                                        // Essayer un traitement sur cette erreur ici.
						            }
                                    break;

                                    case DDERR_INVALIDRECT:
                                    {
                                        bDone = TRUE; // On ne peut rien y faire
                                    }
                                    break;

                                    default : 
                                    {
                                        bDone = TRUE; // On ne peut rien y faire
                                    }
                                }

                                if( nRepairRes!=DD_OK )
                                {
                                    Cdd::directDrawError(nRepairRes,&sError,FALSE);
                                }
								else
								{
									sError+="Ok";
								}
                            }
					    }
					    else
					    {
						    bDone = TRUE;
						    bRes  = TRUE;
					    }

                        bSecondChance = TRUE;
				    }

                    // Affichage de l'ensemble des erreurs rencontrées plus haut.
                    #if _SPRITE_SHOW_ALL_ERRORS
                    {
                        if( !bNoError )
                        {
                            #if _SPRITE_TRACE_INFOS_WHEN_BLIT
                            {
                                TRACE(LPCTSTR(sError));
                            }
                            #else
                            {
                                Cdd::directDrawError(DD_OK,&sError,TRUE);
                            }
                            #endif
                        }
                    }
                    #endif
                }
			}
		}
		else
		{
			m_bBlitAvailable = FALSE;
		}
	}

	return bRes;
}

void CSurface::updateOddLineState(BOOL bOddLinesState)
{
    if( m_pSurface )
    {
        if( bOddLinesState )
        {
            if( !m_bOddLinesCleared )
            {
                if( m_nNbSprites>0 )
                {
			        DDSURFACEDESC ddsd;
			        HRESULT		  nDdVal;
			        
			        memset(&ddsd,0,sizeof(ddsd));
			        ddsd.dwSize=sizeof(ddsd);

			        nDdVal=m_pSurface->Lock(NULL,&ddsd,DDLOCK_WAIT,NULL);

                    if( nDdVal==DD_OK )
                    {
                        UW *pPixelZero = (UW*)ddsd.lpSurface;
                        if( pPixelZero )
                        {
                            m_bOddLinesCleared = TRUE;
                            CSprite *pSprite;
                        
                            // Traitement des sprites de la surfaces un par un
                            for( UINT i=0 ; i<m_nNbSprites ; i++ )
                            {
                                pSprite = &m_tabSprites[i];

                                // On vire les infos de segment pour forcer le recalcul
                                pSprite->m_segmented = FALSE;

                                if( pSprite->m_segment )
                                {
                                    free(pSprite->m_segment);
                                    pSprite->m_segment=NULL;
                                }

                                // Parcours de chaque anims du sprite (Une seule le plus souvent)
                                RECT rcData = pSprite->m_rcData;

                                for( int nAnimIndex=0 ; nAnimIndex<pSprite->m_nNbAnims ; nAnimIndex++ )
                                {
                                    // Mise en transparence des lignes impaires
                                    for( int y=rcData.top ; y<=rcData.bottom ; y++ )
                                    {
                                        if( y&0x00000001 )
                                        {
                                            UW *pPixel;
                                            int x;

                                            for( x=rcData.left, 
                                                 pPixel=pPixelZero+y*m_nW+x; 
                                                 x<=rcData.right ; 
                                                 x++, 
                                                 pPixel++ 
                                               )
                                            {
                                                *pPixel=Cdd::maskG;
                                            }
                                        }
                                    }

                                    rcData.top    += pSprite->m_ptAnimTranslation.y;
                                    rcData.bottom += pSprite->m_ptAnimTranslation.y;
                                    rcData.left   += pSprite->m_ptAnimTranslation.x;
                                    rcData.right  += pSprite->m_ptAnimTranslation.x;
                                }
                            }
                        }

                        m_pSurface->Unlock(NULL);
                    }
                    else
                    {
                        Cdd::directDrawError(nDdVal);
                    }
                }
            }
        }
        else
        {
            if( m_bOddLinesCleared )
            {
                release();
                load();
            }
        }
    }
}


/*-----------------------------------------------------

-----------------------------------------------------*/

void Cdd::updateWin(SD px,SD py,SD pw,SD ph,int fullscreen)
{
    SD i;

	if (fullscreen)
	{
		if (isResAvailable(pw,ph,nbBitPerPlane))
		{
			yResMax         = ph;
			xResMax         = pw;
			winw			= pw;
			winx			= 0;
			winh		    = ph;
			winy		    = 0;
			bUseBackBuffer  = TRUE;
		}
		else
		{
			xResMax = XRES_MAX;
			yResMax = YRES_MAX;
			winx	= 0;
			winy	= 0;
			winw	= pw;
			winh	= ph;
			bUseBackBuffer=FALSE;
		}
	}
	else
	{
		winx	= 0;
		winy	= 0;
		winw	= pw;
		winh	= ph;
		xResMax = pw;
		yResMax = ph;
	}

	setwin(winx,winy,winw,winh);
	reset(winw,winh,winw,winh,fullscreen);

    // init yoffset table
	for(i=0;i<winh;i++)
    {
		yoffset[i]=i*pitch; // granularité
    }
}

#undef DSI_DD_C