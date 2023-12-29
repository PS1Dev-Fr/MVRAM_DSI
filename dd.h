///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
////                                                                       ////
////  D.S.I                                                                ////
////                                                                       ////
////  Patrick,Arnaud,Claude                                                ////
////                                                                       ////
////                                                                       ////
////                                                                       ////
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
//// MANAGEMENT OF LOW LEVEL SCREEN                                        ////
///////////////////////////////////////////////////////////////////////////////

#ifndef _GFXDD_H_
#define _GFXDD_H_

#include <windowsx.h>
#include <ddraw.h>

#ifdef DSI_DD_C
    #define DD_EXT
#else
    #define DD_EXT extern
#endif

#include "def.h"

HRESULT WINAPI DirectDrawEnumCallbackString(LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext);
HRESULT WINAPI DirectDrawEnumCallbackPixelCaps(LPDDSURFACEDESC lpDDSurfaceDesc,LPVOID lpContext);

class CPoint;

DD_EXT BOOL bUpdateScreen;

class CSprite
{
    public:
        CSprite();
	   ~CSprite();
        void init();
        BOOL draw(int nX,int nZ,int nAnimIndex=0,RECT *pDisplayRect=NULL,float fZoom=1.0f);

        int             m_nNbAnims;
		int				m_segmented;
		int				m_segmentcount;
		int				*m_segment;
        CPoint       m_ptHotSpot;
        CPoint       m_ptTranslation;
        CPoint       m_ptAnimTranslation;
        CRect           m_rcData;
        UINT            m_nId;
        static UINT     m_nGlobalId;
        class CSurface *m_pParentSurface;
        RGB             m_rgbLow;
        RGB             m_rgbHigh;
        BOOL            m_bTransparent;
        int             m_nAnimIndex;
        int             m_nSegmentAnimIndex;
        BOOL            m_bUseOt;
        CString         m_sName;
};

class CSurface
{
	public:
		CSurface();
	   ~CSurface();

        void     setFileName     (wchar_t *sFileName);
        void     setName         (CString sName) {m_sName=sName;};
        void     setNbSprite     (UINT nNbSprites);
        void     setResolution   (BOOL bResolutionSpecific,int nSpecificResolution=LOWRES);
        void     describeSprites (UINT nNbSprites,...);
        UINT     describeSprite  (CString sName,int left,int top,int right,int bottom,int xHot=0,int yHot=0,int xTranslation=0,int yTranslation=0,int xAnimTranslation=0,int yAnimTranslation=0,int nNbAnims=1,BOOL bTransparent=TRUE,RGB *pRgbLow=NULL,RGB *pRgbHigh=NULL,BOOL bUseOt=FALSE);

		void	 reset	    ();
        BOOL     release    ();
        HRESULT  restore    ();
        BOOL     load       ();
        BOOL     changeColor(RECT *pRect,int nMinS,int nMaxS,int nMinH,int nMaxH,int nMinL,int nMaxL,int nDeltaH);
		BOOL	 draw	    (CRect *pDestRect,CRect *pSrcRect=NULL,BOOL bTransparent=TRUE,RGB *pRgbLow=NULL,RGB *pRgbHigh=NULL,RECT *pDisplayRect=NULL);
		BOOL	 draw	    (CPoint *pDest,CRect *pSrcRect=NULL,float fZoom=1.0f,BOOL bTransparent=TRUE,RGB *pRgbLow=NULL,RGB *pRgbHigh=NULL,RECT *pDisplayRect=NULL);
		BOOL	 draw	    (int nX,int nY,CRect *pSrcRect=NULL,float fZoom=1.0f,BOOL bTransparent=TRUE,RGB *pRgbLow=NULL,RGB *pRgbHigh=NULL,RECT *pDisplayRect=NULL);

        void     updateOddLineState(BOOL bOddLinesState);

		int		 getW	        () const {return m_nW;};
		int		 getH	        () const {return m_nH;};
        UINT     getNbSprites   () const {return m_nNbSprites;};
        CSprite *getTabSprites  () const {return m_tabSprites;};
        char    *getFileName    () const {return (char*)m_sFileName;};
        int      m_nSpecificResolution;
        BOOL     m_bResolutionSpecific;
        BOOL     m_bUseVideoMem;
        BOOL     m_bOddLinesCleared;
        BOOL   (*m_pFnConstraint)();

        CString  m_sName;

    // Caracteristiques générales
    enum 
    {
        nNbMaxSprites=128,
        nFileNameMaxLen=256
    };

    protected:
		HRESULT init	(int nW,int nH);
    
	public:
		BOOL				m_bBlitAvailable;
		LPDIRECTDRAWSURFACE	m_pSurface;

    private:
        UINT                m_nNbSprites;
        CSprite            *m_tabSprites;
		int					m_nW;
		int					m_nH;
        wchar_t                m_sFileName[nFileNameMaxLen+1];
};


typedef struct
{
    CString infos;
    unsigned maskR,maskG,maskB;
    int w,h,nbpp;
}   VideoModeType;


class Cdd
{
public:

    // Index des sprites (surfaces). 
    enum 
    {
        SurfaceMap=0,
        SurfaceTextLowRes,
        SurfaceTextHiRes1,
        SurfaceTextHiRes2,
        SurfaceGameOverHiRes,
        SurfaceGadget,
        SurfaceCounterTransp,
        SurfaceCounterOpaque,
        SurfaceCockpitLowRes,
        SurfaceCockpitCrossLowRes,
        SurfaceCockpitHiRes1,
        SurfaceCockpitHiRes2,
        SurfaceCockpitHiRes3,
        SurfaceCockpitHiRes4,
        SurfaceCockpitHiRes5,
        SurfaceCockpitHiRes6,
        SurfaceCockpitCrossHiRes1,
        SurfaceCockpitCrossHiRes2,
        SurfaceCockpitCrossHiRes3,
        SurfaceCockpitCrossHiRes4,
        SurfaceCockpitCrossHiRes5,
        SurfaceCockpitCrossHiRes6,
        SurfaceLastIndex
    };

    // Récupéré de Cscreen3d
    void updateWin(SD px,SD py,SD pw,SD ph,int fullscreen);

    // Ptr ecran locké
    static UW *ptr_screen;

    // Offsets
    static long *yoffset;
    static UW *get_screen_addr(SD x,SD y);

    // Gestion globale
    static BOOL                 bUseBackBuffer;
    static BOOL                 bUseVideoMem;

    // Gestion d'un batch de blitters
    enum { NbMaxBlitsInBatch=96 };
    static BOOL                 bUseBlitsOt;
    static BOOL                 bUseBlitsBatch;
    static BOOL                 bBlitBatchSupported;
    static DDBLTBATCH           tabBlitBatch[NbMaxBlitsInBatch];
    static RECT                 tabBlitBatchSrcRect[NbMaxBlitsInBatch];
    static RECT                 tabBlitBatchDestRect[NbMaxBlitsInBatch];
    static DDBLTFX              tabBlitBatchBlitFX[NbMaxBlitsInBatch];
    static int                  nNbBlitsInBatch;

    static void delAllBlitsInBatch   ();
    static BOOL addBlitInBatch       (RECT *pSrcRect,RECT *pDestRect,LPDIRECTDRAWSURFACE,DWORD dwFlags,LPDDBLTFX lpDDBltFx);
    static BOOL flushBlitsBatch      ();

    // Tableau des modes ecran possibles
    enum { NbMaxVideoModes=50 };
    static VideoModeType        tabVideoMode[NbMaxVideoModes];
    static int                  nNbAvailableVideoModes;

    static BOOL                 bOk;
    static BOOL                 bLocked;

	static HWND                	hwndMain;			// Main window handle
	static BOOL					fullscreen;		    // boolean for current mode
	static int					xResMax;			// x,y screen resolution
	static int					yResMax;
	static int					nbBitPerPlane;		// screen bit depth
	static HDC					DDhdc;				// hdc associated with the back
	static LONG					oldstyle;

	static SD					winx;
	static SD					winy;
	static SD					winw;
	static SD					winh;
    static int                  pitch;

	static LPDIRECTDRAWCLIPPER  lpDDClipper;		// DirectDraw Clipper surface
	static LPDIRECTDRAW         lpDD;           	// DirectDraw object
	static LPDIRECTDRAWSURFACE  lpDDSPrimary;   	// DirectDraw primary surface
	static LPDIRECTDRAWSURFACE  lpDDSBack;      	// DirectDraw back surface
	static LPDIRECTDRAWSURFACE  lpDDSMain;      	// DirectDraw memory main surface


    static CSurface				tabSurface[Cdd::SurfaceLastIndex];

	static BOOL					bCapsAvailable;
	static BOOL					bPixelCapsAvailable;
    static BOOL                 bLoadSurfacesInInit;

	static int					nbBitsPerPixel;

	static int					nbBitsR;
	static int					nbBitsG;
	static int					nbBitsB;

	static int					shiftCompR;
	static int					shiftCompG;
	static int					shiftCompB;

	static int					shiftR;
	static int					shiftG;
	static int					shiftB;

	static unsigned int			maskR;
	static unsigned int			maskG;
	static unsigned int			maskB;

    static CSprite             *getSprite(UINT nId);
    static void                 loadAllSurfaces();
    static void                 releaseAllSurfaces();
	static UW					makePixel(unsigned int r,unsigned int g,unsigned int b);
    static void                 getPixel(UW pixel,RGB_MEMBER *pr,RGB_MEMBER *pg,RGB_MEMBER *pb);

protected:
	void release(void);							// release all the direct draw
	HRESULT restoreall(void);					// restore the lost surface when the
	//BOOL init(SD w,SD h,SD nbbpp);
	BOOL init(SD w,SD h,SD wWin,SD hWin,SD nbbpp);
	void resetfullscreenstyle(void);

public:
	BOOL isResAvailable(SD w,SD h,SD nbbpp);	// Est-ce que la résolution est dispo sous DirectDraw
	void setwin(SD x,SD y,SD w,SD h);
	Cdd(HWND hwnd,SD w,SD h,					// Create the DsiDirectDraw object.
		SD wWin,SD hWin,SD nbbpp,SD full);		// hwnd : main app window handle
												// x	: x res
												// y	: y res
												// nbpp	: number of bits per plane
	~Cdd();										// Delete the DsiDirectDraw object
	void setviewrect(RECT rc);					// set the SCREEN coordinates for
												// direct draw. Used when it needs
												// to blit the back buffer on to
												// the window surface.
	void show(void);							// display the bak buffer surface
												// by flipping or blitting according
	void reset(SD w,SD h,SD wWin,SD hWin,SD full);
	HDC createhdc(BOOL clear);					// Returns a Hdc for the drawing
												// surface so we can draw with the
												// GDI. if clear==TRUE, the buffer
												// is cleared.
	void deletehdc(HDC hdc);					// free the hdc allocated with
												// createHDC.
	UW *lock();
	void unlock(void);

	#ifdef _AFXDLL
		void	show(CPoint offset, HWND clipWindow);
	#endif

	BOOL InitCapsVariables();	

    static void getCaps();
    static void enumVideoModes();
	static void directDrawError(HRESULT dderror,CString *pTxtResult=NULL,BOOL bDisplayInBox=TRUE);
	HRESULT Cdd::getSurfDesc(LPDDSURFACEDESC lpDDSurfDesc,LPDIRECTDRAWSURFACE lpDDSurf);
	BOOL Cdd::enumTextureFormats(void);
	BOOL Cdd::createViewport(int w,int h);

};

#endif


