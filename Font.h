#ifndef _DSI_FONT_H
	#define _DSI_FONT_H

	#define FontNameTypeLen 15
	typedef char FontNameType[FontNameTypeLen+1];

    #include "Resource.h"
	#include "Def.h"
	#include "ProgressDlg.h"

	#ifdef FONT_C
		#define FONT_EXT
	#else
		#define FONT_EXT extern
	#endif

	// ATTENTION : Ces constantes refletent les index dans le script DATA\Fonts.spt
	typedef enum
	{
		FontChronoLowRes,
		FontMotoLowRes,
		FontPlayerLowRes,
		FontTimeLowRes,
		FontChronoHiRes,
		FontMotoHiRes,
		FontPlayerHiRes,
		FontTimeHiRes,
		FontInfosDebug,
		FontFadeToBlack,
		FontSpritesLowRes,
		FontSpritesHiRes,
        FontFadeToBlackTransp,
        FontGameMsgLowRes,
        FontGameMsgHiRes,
        FontSpeedLcdHiRes,
        FontSpeedLcdLowRes,
        FontMenuHiRes,
        FontSymbol,
        FontPosLowRes,
        FontPosHiRes,
		FontMenuLabel,
        FontMotorLcdHiRes,
        FontMotorLcdLowRes,
	}	FontTabIndexType;

	typedef enum
	{
		FontTypeUndefined=0,
		FontTypeDirect,
		FontTypeDirectTrame,
		FontTypeCustomColor,
		FontTypeCustomColorTrame,
		FontTypeAntiAlias8,
		FontTypeAntiAlias8Transp,
		FontTypePal,
		FontTypeMoto,
		FontTypeLastIndex
	}	FontType;

	#define NoJustify					 33
	#define HorizontalLeft				  1
	#define HorizontalCenter			  2
	#define HorizontalCenterJustified	  4
	#define HorizontalRight				  8
	#define HorizontalRightJustified	 16
	#define VerticalUp					 32
	#define VerticalCenter				 64
	#define VerticalDown				128
    #define WrappingMode                256

	typedef int JustifyType;

	#define JustifyHorizontalMask 0x1f
	#define JustifyVerticalMask   0xe0

    class CVramCar
    {
    public:
        CVramCar();
        RECT            m_rcCar;
        int             m_nIndex;
        class Cfont    *m_pFont;
        int             m_nPalIndex;
    };

    class CVramPage
    {
    public:
        enum {nNbMaxCarInPage=1024};

        CVramPage();
       ~CVramPage();

        UCHAR   *m_pBuffer;
        CVramCar m_tabCar[nNbMaxCarInPage];
    };

    class CVramPal
    {
    public:
        CVramPal(int nNbColors);
       ~CVramPal();

        RGB    *m_pBuffer;
        int     m_nNbColors;
    };

	class Cfont
	{
		// Fonctions
		public:
			Cfont();
		   ~Cfont();

		    virtual BOOL	init			();

			virtual	BOOL	load 			(LPCTSTR sFileName,   UCHAR cFirstCar,int  nNbCars, int nFrameColor);
			virtual	BOOL	load 			(LPCTSTR sFileName,   UCHAR cFirstCar,UCHAR cLastCar,int nFrameColor);																			 
			virtual	BOOL	free			();

                    /*
					BOOL	displayStringDd	(UCHAR *sString,int x,int y,JustifyType nJustify=NoJustify,unsigned int nTrans=0,RECT *pRect=NULL,RGB *pRgbForeColor=NULL,RGB *pRgbBackColor=NULL);
					BOOL	displayStringDd	(char *sString,int x,int y,JustifyType nJustify=NoJustify,unsigned int nTrans=0,RECT *pRect=NULL,RGB *pRgbForeColor=NULL,RGB *pRgbBackColor=NULL);
					BOOL	displayStringDd	(CString str,int x,int y,JustifyType nJustify=NoJustify,unsigned int nTrans=0,RECT *pRect=NULL,RGB *pRgbForeColor=NULL,RGB *pRgbBackColor=NULL);
                    */
					BOOL	displayString	(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR *sString,JustifyType nJustify,int nTextWidth,int x,int y,unsigned int nTrans,RECT *pRect=NULL,RGB *pRgbForeColor=NULL,RGB *pRgbBackColor=NULL);
			virtual BOOL	displayCar		(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);

					RGB		getForeColor	() {return m_foreColor;};
					RGB		getBackColor	() {return m_backColor;};
                    UINT	getPalLen       () {return m_nPalLen;};

					void	setForeColor	(RGB *pRgb);
					void	setBackColor	(RGB *pRgb);
					void	setForeColor	(UCHAR r,UCHAR g,UCHAR b);
					void	setBackColor	(UCHAR r,UCHAR g,UCHAR b);
					void	setForeColor	(UW foreColor);
					void	setBackColor	(UW backColor);
            virtual void    updateColors    (RGB *pForeColor,RGB *pBackColor=NULL);

			virtual void	calcPixelTab	(UINT nFirst=1,UINT nLast=255);
			        void	calcRect		(UCHAR *sString,int *pWidth,int *pHeight,BOOL bJustOneLine=FALSE);	
			        void	calcRect		(UCHAR *sString,RECT *pRect,int x,int y,JustifyType nJustify=NoJustify);	
			        void	calcRect		(CString sString,int *pWidth,int *pHeight,BOOL bJustOneLine=FALSE) {calcRect((UCHAR*)LPCTSTR(sString),pWidth,pHeight,bJustOneLine);};	
			        void	calcRect		(CString sString,RECT *pRect,int x,int y,JustifyType nJustify=NoJustify) {calcRect((UCHAR*)LPCTSTR(sString),pRect,x,y,nJustify);};	
					void	calcCoordEffects(UCHAR *sString,int *pX,int *pY,JustifyType nJustify); 


            // Partie propre à mVram
            enum {nNbMaxVramPages=16};
            enum {nNbMaxVramPals=32};
            static  int          m_nVramNbPages;
            static  int          m_nVramNbPals;
            static  CVramPage   *m_tabVramPage[nNbMaxVramPages];
            static  CVramPal    *m_tabVramPal[nNbMaxVramPals];

            static  void         initVramData();
            static  BOOL         buildVramPages(class CProgressDlg *Pgs,class CViewDx *Vdd);
            static  void         showPage(int nCurrentPalIndex,CProgressDlg *Pgs,CViewDx *Vdd);
            static  void         delVramData();

        private:
            static  BOOL         addVramPage();
            static  void         delVramPages();
            static  BOOL         addVramPal(int nNbColors);
            static  void         delVramPals();

		protected:
			virtual	BOOL	loadRaw			(LPCTSTR sRawFileName,UCHAR cFirstCar,int  nNbCars, int nFrameColor);
			virtual	BOOL	loadIff			(LPCTSTR sIffFileName,UCHAR cFirstCar,int  nNbCars, int nFrameColor);
			virtual	BOOL	loadRaw			(LPCTSTR sRawFileName,UCHAR cFirstCar,UCHAR cLastCar,int nFrameColor);
			virtual	BOOL	loadIff			(LPCTSTR sIffFileName,UCHAR cFirstCar,UCHAR cLastCar,int nFrameColor);

		
		// Data
		public:
			enum { nNbCars=256 };

			FontType		 m_nType;				// Type de la fonte
			FontNameType	 m_sName;				// Nom de la fonte
			UCHAR			*m_tabOffset[nNbCars];	// Tableau de pixels fontes
			UW				 m_tabPixel[nNbCars];	// Tableau de pixels écran précalculés
			int				 m_tabWidth[nNbCars];	// Tableau de largeurs
			int				 m_nHeight;				// Hauteur de TOUS les caractères
			int				 m_nLevelTop;
			int				 m_nLevelBottom;
			int				 m_nInterLineSpace;		// Interligne
			int				 m_nInterCharSpace;		// Espace entre 2 caractères

			#if _FONT_TEST
			static BOOL		 m_bShowStringRect;	
			#endif

	        static int	     nNbFonts;
            static Cfont     **tabFont;

	protected:
			UCHAR			*m_pData[4];			// Tableau de buffers pixels fontes
			RGB				 m_foreColor;
			RGB				 m_backColor;
			RGB				*m_pPal;
			UINT			 m_nPalLen;
			BOOL			 m_bKeepPal;
	};

	typedef Cfont *LPCfont;

	class CfontDirectTrame : public Cfont
	{
		public:
			BOOL	displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	class CfontCustomColor : public Cfont
	{
		public:
			CfontCustomColor();
		   ~CfontCustomColor();

            void    updateColors (RGB *pForeColor,RGB *pBackColor=NULL);
			void	calcPixelTab (UINT nFirst=1,UINT nLast=255);
			BOOL	displayCar   (UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	class CfontCustomColorTrame : public CfontCustomColor
	{
		public:
			BOOL	displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	class CfontAntiAlias8 : public Cfont
	{
		public:
			CfontAntiAlias8();
		   ~CfontAntiAlias8();

			void	calcPixelTab(UINT nFirst=1,UINT nLast=255);
			BOOL	displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);

            int m_tabFactor[7][256];
	};

	class CfontAntiAlias8Transp : public CfontAntiAlias8
	{
		public:
			CfontAntiAlias8Transp();
		   ~CfontAntiAlias8Transp();

			BOOL displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	class CfontPal : public Cfont
	{
		public:
			CfontPal();
		   ~CfontPal();
			virtual void	calcPixelTab(UINT nFirst=1,UINT nLast=255);
			        BOOL	displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	class CfontMoto : public CfontPal
	{
		public:
			CfontMoto();
		   ~CfontMoto();

			void	calcPixelTab(UINT nFirst=1,UINT nLast=255);
			BOOL	displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,unsigned int nTrans=0);
	};

	// Fonctions hors-classes
	Cfont *getFont(UINT nIndex,BOOL *pbOneResolution=NULL);

	#undef FONT_EXT
#endif