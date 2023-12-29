#include "StdAfx.h"
#include <stdio.h>
#include <io.h>
#include "PathNameTools.h"
#include "dsi_err.h"
#include "dsi_file.h"
#include "FileRawDef.h"
#include "Dd.h"
#include "Globals.h"
#include "viewdx.h"
#define FONT_C
#include "Font.h"


//////////////////////////////////////////////////////////////////////////////
// Customisation

#define DESACTIVE_FONTS 0

//////////////////////////////////////////////////////////////////////////////
// Variables statiques

int	    Cfont::nNbFonts=0;
Cfont **Cfont::tabFont=NULL;

#if _FONT_TEST
BOOL    Cfont::m_bShowStringRect=FALSE;	
#endif

//////////////////////////////////////////////////////////////////////////////
// Corps de displayCar

#define DISPLAY_CAR_BEGIN                                                               \
{                                                                                       \
	BOOL bRes = FALSE;                                                                  \
                                                                                        \
	if( (nCar>0) && (nCar<Cfont::nNbCars) && m_tabOffset[nCar] && m_tabWidth[nCar] )    \
	{                                                                                   \
        int nSaveY     = y;                                                             \
		int nCharW     = m_tabWidth[nCar];                                              \
		int nCharH     = m_nHeight;                                                     \
		int nRealCharW = nCharW;                                                        \
		int nRealCharH = nCharH;                                                        \
		UW  *pPixel;                                                                    \
                                                                                        \
		bRes = TRUE;                                                                    \
                                                                                        \
		/* Test depassement */                                                          \
		if( (x+nCharW>0)    &&                                                          \
            (y+nCharH>0)    &&                                                          \
            (x<nDisplayW)   &&                                                          \
            (y<nDisplayH) )                                                             \
		{                                                                               \
			UCHAR *pStart;                                                              \
                                                                                        \
			pStart = m_tabOffset[nCar];                                                 \
			pPixel = *ppDisplayBuffer;                                                  \
                                                                                        \
			if( y<0 )                                                                   \
			{                                                                           \
			    pPixel	-= (y*nBufferW);                                                \
				pStart	-= (y*nCharW);                                                  \
				nCharH	+=  y;                                                          \
			}                                                                           \
                                                                                        \
			if( x<0 )                                                                   \
			{                                                                           \
				pPixel	-= x;                                                           \
				pStart	-= x;                                                           \
				nCharW	+= x;                                                           \
			}                                                                           \
                                                                                        \
			if( x+nCharW > nDisplayW )                                                  \
			{                                                                           \
				nCharW = nDisplayW-x;                                                   \
			}                                                                           \
                                                                                        \
			if( y+nCharH > nDisplayH )                                                  \
			{                                                                           \
				nCharH = nDisplayH-y;                                                   \
			}                                                                           \
                                                                                        \
			if( (nCharW>0) && (nCharH>0) )                                              \
			{                                                                           \
                int nCharY = y;                                                         \
		        int nJumpInCar;                                                         \
		        int nJumpInScr;                                                         \
		        int nCptY;                                                              \
                                                                                        \
                nJumpInCar = -nCharW + nRealCharW;                                      \
				nJumpInScr = -nCharW + nBufferW;	                                    \
                                                                                        \
				nCptY = nCharH;                                                         \
				do                                                                      \
				{                                                                       \
                    {                                                                   \
		                int nCptX;                                                      \
                                                                                        \
					    nCptX = nCharW;                                                 \
					    do                                                              \
					    {                                                               

#define DISPLAY_CAR_END                                             \
						    pPixel++;                               \
						    pStart++;                               \
					    }                                           \
					    while( --nCptX );                           \
                                                                    \
					    pStart += nJumpInCar;                       \
					    pPixel += nJumpInScr;                       \
                    }                                               \
                    nCharY++;                                       \
				}                                                   \
				while( --nCptY );                                   \
			}                                                       \
		}                                                           \
                                                                    \
		*ppDisplayBuffer += nRealCharW+m_nInterCharSpace;           \
		x += nRealCharW+m_nInterCharSpace;                          \
        y = nSaveY;                                                 \
	}                                                               \
                                                                    \
	return bRes;                                                    \
}                                                                   \

                                                                    
//////////////////////////////////////////////////////////////////////////////
// Divers

Cfont *getFont(UINT nIndex,BOOL *pbOneResolution)
{
	Cfont *pFont=NULL;
    BOOL bOneResolution=FALSE;

	if( Cfont::tabFont )
	{
		if( nIndex>=(UINT)Cfont::nNbFonts )
		{
			if( Cfont::tabFont[0] )
			{
				pFont = Cfont::tabFont[0];
			}
			else
			{
				static BOOL bFirstWarning=FALSE;

				if( !bFirstWarning )
				{
					AfxMessageBox(L"No font loaded...\nIt may crash in a while\nGood luck");
					bFirstWarning = TRUE;
				}
			}
		}
		else
		{
			pFont = Cfont::tabFont[nIndex];
		}
	}

    if( pbOneResolution )
    {
        *pbOneResolution = bOneResolution;
    }

	return pFont;
}


//////////////////////////////////////////////////////////////////////////////
// Cfont

Cfont::Cfont()
{
	init();
	m_nType	= FontTypeDirect;
}

Cfont::~Cfont()
{
	free();
}

BOOL Cfont::init()
{
	m_pData[0]			= NULL;
	m_pData[1]			= NULL;
	m_pData[2]			= NULL;
	m_pData[3]			= NULL;
	m_nHeight			= 0;
	m_nLevelTop			= 0;
	m_nLevelBottom		= 0;
	m_nType				= FontTypeUndefined;
	m_nInterLineSpace	= 1;
	m_nInterCharSpace	= 1;
	m_pPal				= NULL;
	m_nPalLen			= 0;
	m_bKeepPal			= FALSE;

	memset(m_tabWidth,0,nNbCars*sizeof(m_tabWidth[0]));		
	memset(m_tabPixel,0,nNbCars*sizeof(m_tabPixel[0]));		
	memset(m_sName,0,FontNameTypeLen+1);

	int i;
	for( i=0 ; i<nNbCars ; i++ )
	{
		m_tabOffset[i] = NULL;
	}

	setForeColor(255,50,20);	// Par exemple...
	setBackColor(100,100,10);

	return TRUE;
}

void Cfont::setForeColor(RGB *pRgb)
{
    if( pRgb )
    {
	    m_foreColor = *pRgb;
    }
}

void Cfont::setBackColor(RGB *pRgb)
{
    if( pRgb )
    {
	    m_backColor = *pRgb;
    }
}

void Cfont::setForeColor(UCHAR r,UCHAR g,UCHAR b)
{
	m_foreColor.r = r;
	m_foreColor.g = g;
	m_foreColor.b = b;
}

void Cfont::setBackColor(UCHAR r,UCHAR g,UCHAR b)
{
	m_backColor.r = r;
	m_backColor.g = g;
	m_backColor.b = b;
}

void Cfont::setForeColor(UW foreColor)
{
    m_foreColor.r = ((foreColor&Cdd::maskR)>>Cdd::shiftR)<<(Cdd::shiftCompR);
	m_foreColor.g = ((foreColor&Cdd::maskG)>>Cdd::shiftG)<<(Cdd::shiftCompG);
	m_foreColor.b = ((foreColor&Cdd::maskB)>>Cdd::shiftB)<<(Cdd::shiftCompB);
}

void Cfont::setBackColor(UW backColor)
{
    m_backColor.r = ((backColor&Cdd::maskR)>>Cdd::shiftR)<<(Cdd::shiftCompR);
	m_backColor.g = ((backColor&Cdd::maskG)>>Cdd::shiftG)<<(Cdd::shiftCompG);
	m_backColor.b = ((backColor&Cdd::maskB)>>Cdd::shiftB)<<(Cdd::shiftCompB);
}

void Cfont::updateColors(RGB *pForeColor,RGB *pBackColor)
{
    BOOL bUpdate=FALSE;

    if( pForeColor )
    {
        if( memcmp(pForeColor,&m_foreColor,sizeof(RGB)) )
        {
            setForeColor(pForeColor);
            bUpdate = TRUE;
        }
    }

    if( pBackColor )
    {
        if( memcmp(pBackColor,&m_backColor,sizeof(RGB)) )
        {
            setBackColor(pBackColor );
            bUpdate = TRUE;
        }
    }

    if( bUpdate )
    {
        calcPixelTab();
    }
}

void Cfont::calcPixelTab(UINT nFirst,UINT nLast)
{
#if 0
	UINT nColor,penteR,penteG,penteB,r,g,b;

	r		= m_backColor.r;
	g		= m_backColor.g;
	b		= m_backColor.b;

	penteR	= ((m_foreColor.r - r)<<16)/255;
	penteG	= ((m_foreColor.g - g)<<16)/255;
	penteB	= ((m_foreColor.b - b)<<16)/255;

	r <<= 16;
	g <<= 16;
	b <<= 16;

	for( nColor=0 ; nColor<256 ; nColor++ )
	{
		if( (nColor>=nFirst) && (nColor<=nLast) )
		{
			m_tabPixel[nColor] = Cdd::makePixel(r>>16,g>>16,b>>16);

			r += penteR;
			g += penteG;
			b += penteB;
		}
	}
#else
	UINT r,g,b,nColor,nLevel;

	for( nColor=0 ; nColor<256 ; nColor++ )
	{
		if( (nColor>=nFirst) && (nColor<=nLast) )
		{
			nLevel = (nColor%32);

			r = (m_backColor.r*(31-nLevel) + (m_foreColor.r*nLevel ))/31;
			g = (m_backColor.g*(31-nLevel) + (m_foreColor.g*nLevel ))/31;
			b = (m_backColor.b*(31-nLevel) + (m_foreColor.b*nLevel ))/31;

			m_tabPixel[nColor] = Cdd::makePixel(r,g,b);
		}
	}

#endif
}

///////////////////////////////////////////////////////////////////////////
//	Fonctions et utilitaires pour charger une fonte
///////////////////////////////////////////////////////////////////////////

static short SwapWord(unsigned long w)
{
	return (short)(((w & 0x00FF) << 8) | ((w & 0xFF00) >> 8));
}

UCHAR *GetNextFrame(BOOL bFirstSearch,int nRefHeight,UCHAR *pFileData,int nFrameColor,int nWidth,int nHeight,int &nNextX,int &nNextY,int &nFrameWidth,int &nFrameHeight)
{
	UCHAR *pFramePixel=NULL;
	UCHAR *pPixel=nullptr;
	UCHAR *pPixelLT;
	int x,y;
	BOOL bFound;
	BOOL bError;
	BOOL bJumpWithCarHeight=TRUE;

	if( pFileData )
	{
		x      = nNextX;
		y      = nNextY;

		bFound = FALSE;
		bError = FALSE;

		while( !bError && !bFound )
		{
			pPixel = pFileData+x+y*nWidth;

			while( (x<nWidth) && !bFound )
			{
				if( *pPixel==nFrameColor )
				{
					bFound				= TRUE;
					bJumpWithCarHeight	= TRUE;
				}
				else
				{
					x++;
					pPixel++;
				}
			}

			if( !bFound )
			{
				x = 0;

				if( bFirstSearch )
				{
					y++;
				}
				else
				{
					if( bJumpWithCarHeight )
					{
						y+=nRefHeight+2;
						bJumpWithCarHeight = FALSE;
					}
					else
					{
						y++;
					}
				}

				if( y>=nHeight )
				{
					bError = TRUE;
				}
			}
		}

		if( bFound )
		{
			pFramePixel = pPixel+nWidth+1;

			pPixelLT = pPixel;
			bFound   = FALSE;

			while( (x<nWidth) && !bFound )
			{
				if( *pPixel!=nFrameColor )
				{
					bFound = TRUE;
				}
				else
				{
					x++;
					pPixel++;
				}
			}

			nNextX = x;
			nNextY = y;

			if( bFound )
			{
				nFrameWidth = int(pPixel-pPixelLT-2);

				pPixel		= pPixelLT;
				bFound		= FALSE;

				while( (y<nHeight) && !bFound )
				{
					if( *pPixel!=nFrameColor )
					{
						bFound = TRUE;
					}
					else
					{
						y++;
						pPixel+=nWidth;
					}
				}

				if( bFound )
				{
					nFrameHeight = int(pPixel-pPixelLT)/nWidth-2;
				}
			}
		}
	}

	if( !bFound || bError )
	{
		pFramePixel = NULL;
	}

	return pFramePixel;
}

BOOL Cfont::loadRaw(LPCTSTR sRawFileName,UCHAR cFirstCar,int nNbCars,int nFrameColor)
{
	BOOL	 bRes		= FALSE;
	FILE	*pFile		= NULL;
	UCHAR	*pFileData	= NULL;
	RGB		*pPalData	= NULL;
	int		 nNewBank;

	try
	{
		// Recherche d'une banque libre
		nNewBank = 0;

		while( (nNewBank<4) && m_pData[nNewBank] )
		{
			nNewBank++;
		}

		if( nNewBank>=4 )
		{
			throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY);
		}

		if( pFile=dsi_fopen(sRawFileName,L"rb") )
		{

			// Lecture header et controles
			FileRawHeaderType header;

			if( dsi_fread(&header,sizeof(header),1,pFile) != 1 )
			{
				throw CdsiError(DSI_ERROR_READ_ERROR,"(Reading header)");
			}

			header.nVersion=SwapWord(header.nVersion);
			header.nHeight=SwapWord(header.nHeight);
			header.nWidth=SwapWord(header.nWidth);
			header.nPalSize=SwapWord(header.nPalSize);

			if( strncmp(header.nId,FILE_RAW_DEF_ID,6 ))
			{
				throw CdsiError(DSI_ERROR_INVALID_HEADER,"'(Bad RAW id)");
			}

			if( /*(header.nPalSize!=256) ||*/ (header.nWidth<=0) || (header.nHeight<=0) )
			{
				throw CdsiError(DSI_ERROR_BAD_FORMAT,"(bad width or height)");
			}

			int nFileLen = dsi_filelength(pFile);

			if( (UINT)nFileLen != (sizeof(header)+(header.nWidth*header.nHeight)+header.nPalSize*3) )
			{
				throw CdsiError(DSI_ERROR_BAD_FORMAT,"(Incoherence between file size and header infos)");
			}

			nFileLen -= ( (sizeof(header) + header.nPalSize*3) );

			// Préparation buffers lecture palette et pixels
			if( !(pFileData = new UCHAR [nFileLen]) )
			{
				throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY,"(Tried to alloc file buffer)");
			}

			if( !(pPalData = new RGB [header.nPalSize]) )
			{
				throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY,"(Tried to alloc pal buffer)");
			}

			// Palette
			if( m_bKeepPal )
			{
				m_nPalLen = (UINT)header.nPalSize;
				m_pPal    = pPalData;
			}

			if( dsi_fread(pPalData,sizeof(RGB),header.nPalSize,pFile) != (UINT)header.nPalSize )
			{
				throw CdsiError(DSI_ERROR_READ_ERROR,"(Reading palette)");
			}

			// Pixels
			if( dsi_fread(pFileData,nFileLen,1,pFile) != 1 )
			{
				throw CdsiError(DSI_ERROR_READ_ERROR,"(Reading file)");
			}

			// Recherche 1er pixel cadre
			UCHAR *pPixelLT=pFileData;
			int nCnt=header.nWidth*header.nHeight;
			BOOL bFound=FALSE;

			// Calcul 1ère hauteur
			int x,y;
			int nFrameHeight,nFrameWidth;

			x = 0;
			y = 0;
			
			if( pPixelLT = GetNextFrame(TRUE,0,pFileData,nFrameColor,header.nWidth,header.nHeight,x,y,nFrameWidth,nFrameHeight) )
			{
				m_nHeight = nFrameHeight;
			}

			if( !pPixelLT || (nFrameHeight<2) || (nFrameWidth<2) )
			{
				throw CdsiError(DSI_ERROR_CANT_FIND_FIRST_CAR,"The frame color is incorrect or\nthe first found char is too small");
			}

			// Calcul de la somme des largeurs
			int nSigmaWidth=0;
			BOOL bFirstSearch=TRUE;
			x = 0;
			y = 0;
			
			while( pPixelLT = GetNextFrame(bFirstSearch,m_nHeight,pFileData,nFrameColor,header.nWidth,header.nHeight,x,y,nFrameWidth,nFrameHeight) )
			{
				nSigmaWidth += nFrameWidth;

				if( nFrameHeight != m_nHeight )
				{
					sprintf(tmp_str,"(Reading char at %dx%d",x,y);
					throw CdsiError(DSI_ERROR_BAD_HEIGHT,tmp_str);
				}

				bFirstSearch = FALSE;
			}

			// Allokation buffer interne
			m_pData[nNewBank] = new UCHAR [nSigmaWidth*m_nHeight];

			if( !m_pData[nNewBank] )
			{
				sprintf(tmp_str,"(Trying to alloc a new %d bytes bank)",nSigmaWidth*m_nHeight);
				throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY,tmp_str);
			}

			// Lecture caractères
			UCHAR *pPixelTgt  = m_pData[nNewBank];
			UCHAR cCurrentCar  = cFirstCar;

			bFirstSearch = TRUE;
			x = 0;
			y = 0;
			
			while( (cCurrentCar<cFirstCar+nNbCars) && (pPixelLT=GetNextFrame(bFirstSearch,m_nHeight,pFileData,nFrameColor,header.nWidth,header.nHeight,x,y,nFrameWidth,nFrameHeight)) )
			{
				UCHAR *pPixelSrc=pPixelLT;
				int x2,y2;
				
				if( m_tabOffset[cCurrentCar]==NULL )
				{
					m_tabOffset[cCurrentCar]=pPixelTgt;
					m_tabWidth [cCurrentCar]=nFrameWidth;

					for( y2=0 ; y2<nFrameHeight ; y2++ )
					{
						for( x2=0 ; x2<nFrameWidth ; x2++ )	
						{
							*pPixelTgt++ = *pPixelSrc++;	
						}

						pPixelSrc -= nFrameWidth;
						pPixelSrc += header.nWidth;
					}
				}
				else
				{
					sprintf(tmp_str,"(Reading char #%d at %dx%d)",cCurrentCar,x,y);
					throw CdsiError(DSI_ERROR_REDEFINED_CAR,tmp_str);
				}

				cCurrentCar++;
				bFirstSearch = FALSE;
			}

			// Si on est arrivé ici, alors on n'est pas passé par un throw, ça roule.
			bRes=TRUE;
		}
		else
		{
			throw CdsiError(DSI_ERROR_FILE_OPEN);
		}
	}
	catch( CdsiError error )
	{
		error.displayMessageBox(sRawFileName,L"Load raw font file");	
	}

	// Ciao
	if( !m_bKeepPal )
	{
		if( pPalData )
		{
			delete pPalData;
		}
	}

	if( pFileData )
	{
		delete pFileData;
	}

	if( pFile )
	{
		dsi_fclose(pFile);
	}

	return bRes;
}

BOOL Cfont::loadRaw(LPCTSTR sRawFileName,UCHAR cFirstCar,UCHAR cLastCar,int nFrameColor)
{
	BOOL bRes = loadRaw(sRawFileName,cFirstCar,cLastCar-cFirstCar+1,nFrameColor);
	return bRes;
}

BOOL Cfont::loadIff(LPCTSTR sIffFileName,UCHAR cFirstCar,int nNbCars,int nFrameColor)
{
	BOOL bRes = FALSE;
	return bRes;
}

BOOL Cfont::loadIff(LPCTSTR sIffFileName,UCHAR cFirstCar,UCHAR cLastChar,int nFrameColor)
{
	BOOL bRes = loadIff(sIffFileName,cFirstCar,cLastChar-cFirstCar+1,nFrameColor);
	return bRes;
}

BOOL Cfont::load(LPCTSTR sFileName,UCHAR cFirstCar,int nNbCars,int nFrameColor)
{
	BOOL bRes = FALSE;

	wchar_t *pExt = GetExtensionFromPathName(sFileName);

	try
	{
		if( pExt && *pExt )
		{
			if( !wcsncmp(pExt,L"raw",3) )
			{
				bRes = loadRaw(sFileName,cFirstCar,nNbCars,nFrameColor);
			}
			else
			if( !wcsncmp(pExt,L"iff",3) )
			{
				bRes = loadIff(sFileName,cFirstCar,nNbCars,nFrameColor);
			}
			else
			{
				throw CdsiError(DSI_ERROR_FORMAT_NOT_SUPPORTED);
			}
		}
		else
		{
			throw CdsiError(DSI_ERROR_BAD_DATA);
		}
	}
	catch( CdsiError error )
	{
		error.displayMessageBox(sFileName,L"Load font file");	
	}

	return bRes;
}

BOOL Cfont::load(LPCTSTR sFileName,UCHAR cFirstCar,UCHAR cLastCar,int nFrameColor)
{
	BOOL bRes = load(sFileName,cFirstCar,cLastCar-cFirstCar+1,nFrameColor);
	return bRes;
}

///////////////////////////////////////////////////////////////////////////////
//	Libération mémoire
///////////////////////////////////////////////////////////////////////////////

BOOL Cfont::free()
{
	BOOL bRes = TRUE;
	int i;
	
	for( i=0 ; i<4 ; i++ )
	{
		if( m_pData[i] )
		{
			delete [] m_pData[i];
			m_pData[i] = NULL;
		}
	}

	memset(m_tabWidth, 0,nNbCars*sizeof(m_tabWidth[0] ));		
	memset(m_tabOffset,0,nNbCars*sizeof(m_tabOffset[0]));		

	if( m_pPal )
	{
		delete [] m_pPal;
		m_pPal=NULL;
	}

	init();

	return bRes;
}

///////////////////////////////////////////////////////////////////////////////
//	CALCULS RECTANGLES/DEPLACEMENTS
///////////////////////////////////////////////////////////////////////////////

void Cfont::calcRect(UCHAR *sString,RECT *pRect,int x,int y,JustifyType nJustify)
{
	if( pRect )
	{
		int nTextWidth=0,nTextHeight=0;

		calcRect(sString,&nTextWidth,&nTextHeight);
		calcCoordEffects(sString,&x,&y,nJustify);

		pRect->left		= x;
		pRect->top		= y;
		pRect->right	= x+nTextWidth-1;
		pRect->bottom	= y+nTextHeight-1;
	}
}

void Cfont::calcRect(UCHAR *sString,int *pWidth,int *pHeight,BOOL bJustOneLine)
{
	UCHAR nCar;
	BOOL bContinue=TRUE;

	if( pWidth && pHeight && sString && *sString )
	{
		int i,nLen,nMaxWidth;

		nMaxWidth   = 0;
		*pWidth		= 0;
		*pHeight	= m_nHeight;
		nLen		= (int)strlen((char*)sString);

		for( i=0 ; bContinue && (i<nLen) ; i++ )
		{
			nCar = sString[i];

			switch( nCar )
			{
				case '\n' :	
				{
					if( !bJustOneLine )
					{
						if( nMaxWidth<*pWidth )
						{
							nMaxWidth=*pWidth;
						}

						*pWidth   = 0;
						*pHeight += m_nHeight+m_nInterLineSpace;
					}
					else
					{	
						bContinue = FALSE;
					}
				}
				break;

				default	: *pWidth += m_tabWidth[nCar] + m_nInterCharSpace;
			}
		}

		if( nMaxWidth<*pWidth )
		{
			nMaxWidth=*pWidth;
		}

		*pWidth=nMaxWidth;
	}
}

void Cfont::calcCoordEffects(UCHAR *sString,int *pX,int *pY,JustifyType nJustify)
{
	int nTextWidth   = 0;
	int nTextHeight  = 0;
	int nHorizEffect = nJustify&JustifyHorizontalMask;
	int nVertEffect  = nJustify&JustifyVerticalMask;

	if( (nHorizEffect!=HorizontalLeft) || (nVertEffect!=VerticalUp) )
	{
		calcRect(sString,&nTextWidth,&nTextHeight);
	}

	switch( nHorizEffect )
	{
		case HorizontalCenter:
		case HorizontalCenterJustified:
		{
			*pX = (Cdd::winw-nTextWidth)/2;
		}
		break;

		case HorizontalRight:
		case HorizontalRightJustified:
		{
			*pX = Cdd::winw-nTextWidth-*pX;
		}
		break;

		case HorizontalLeft:
		{
			
		}
		break;
	}

	switch( nVertEffect )
	{
		case VerticalCenter:
		{
			*pY = (Cdd::winh-nTextHeight)/2;
		}
		break;

		case VerticalDown:
		{
			*pY = Cdd::winh-nTextHeight-*pY;
		}
		break;

		case VerticalUp:
		{
			
		}
		break;
	}
}

///////////////////////////////////////////////////////////////////////////////
//	AFFICHAGE CHAINE
///////////////////////////////////////////////////////////////////////////////

BOOL CfontDirectTrame::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
int tx=nCptX&15,ty=nCptY&15;
if( *pStart>0  && ((nTrans==0)||((UINT)((tx*tx+ty*ty)&0x7F)>(nTrans<<1))) )
#if !DESACTIVE_FONTS
{
    *pPixel = m_tabPixel[*pStart];
}
#endif
DISPLAY_CAR_END

BOOL Cfont::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
{
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
if( *pStart>0 )
{
    *pPixel = m_tabPixel[*pStart];
}
#endif
DISPLAY_CAR_END
}

BOOL Cfont::displayString(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR *sString,JustifyType nJustify,int nTextWidth,int x,int y,UINT nTrans,RECT *pRect,RGB *pRgbForeColor,RGB *pRgbBackColor)
{
	BOOL bRes = FALSE;
	UCHAR nCar;
	int nInitialX=x,nInitialY=y;
	UW *pInitialDisplayBuffer;
	int nSubStringWidth,nSubStringHeight,nSubStringOffset;
	int nMaxX;

	if( sString && *sString )
	{
        UCHAR  *pSrc=sString,
               *pTgt=NULL,
               *pTgtOriginal=NULL;

        int nTextWidth=0,nTextHeight=0;        // Prise en compte de WrappingMode

        if( nJustify & WrappingMode )
        {
            calcRect(sString,&nTextWidth,&nTextHeight);
            
            if( nTextWidth>nDisplayW )
            {
                int     nW,nSrcCpt,nTgtCpt,nTgtLen,nSrcLen=(int)strlen((const char *)sString);
                BOOL    bFinished=FALSE;

                nTgtLen = nSrcLen*2;
                pTgt    = new UCHAR [nTgtLen+1];

                if( pTgt )
                {
                    pTgtOriginal = pTgt;

                    ZeroMemory(pTgt,nTgtLen+1);

                    nSrcCpt = 0;
                    nTgtCpt = 0;

                    while( !bFinished )
                    {
                        nW = 0;

                        while( (nTgtCpt<nTgtLen) && (nW+m_tabWidth[*pSrc]) < nDisplayW )
                        {
                            nW   += m_tabWidth[*pSrc];
                           *pTgt  = *pSrc;

                            nSrcCpt++;
                            nTgtCpt++;
                            pSrc++;
                            pTgt++;
                        }

                        if( nSrcCpt<nSrcLen && nTgtCpt<nTgtLen )
                        {
                            if( m_tabWidth[*pSrc]>=nDisplayW )
                            {
                                *pTgt++  = *pSrc++;
                            }

                            *pTgt++ = '\n';
                            nTgtCpt++;
                        }
                        else
                        {
                            bFinished = TRUE;
                        }
                    }

                    sString = pTgtOriginal;
                }
            }
        }

		calcRect(sString,&nTextWidth,&nTextHeight);

        if( m_nType==FontTypeAntiAlias8 )
        {
            nTrans = (((nTrans<<8)*8/100))>>8;
        }
        else
        if( m_nType==FontTypeAntiAlias8Transp )
        {
            nTrans = (((nTrans<<8)*224/100))>>8;
        }

        // Debug : dessin du cadre
		#if _FONT_TEST
		if( m_bShowStringRect )
		{	
			int i,nTextWidth=0,nTextHeight=0;
			UW *pPixel;
			pPixel = *ppDisplayBuffer;

			calcRect(sString,&nTextWidth,&nTextHeight);

			i = nTextWidth;

			while( i-- )
			{	
				*pPixel = 0x7FF; // Cyan en 5,6,5
				*(pPixel+(nTextHeight-1)*nBufferW) = 0x7FF;

				pPixel++;
			}

			pPixel = *ppDisplayBuffer;

			i = nTextHeight;

			while( i-- )
			{	
				*pPixel = 0x7FF;
				*(pPixel+nTextWidth-1) = 0x7FF;

				pPixel+=nBufferW;
			}
		
		}
		#endif

		int i,nLen;

		bRes	= TRUE;
		nLen	= (int)strlen((char*)sString);
		nMaxX   = 0;

		// Prise en compte des couleurs
		if( pRgbForeColor )
		{
			setForeColor(pRgbForeColor->r,pRgbForeColor->g,pRgbForeColor->b);	
		}
		
		if( pRgbBackColor )
		{
			setBackColor(pRgbBackColor->r,pRgbBackColor->g,pRgbBackColor->b);	
		}

        // Init du pointeur de départ
        pInitialDisplayBuffer=*ppDisplayBuffer;

		// Prise en compte des justifications centrées/droites
		int nHorizEffect = nJustify&JustifyHorizontalMask;

		nSubStringOffset=0;

		if( nHorizEffect==HorizontalCenterJustified )
		{
			calcRect(sString,&nSubStringWidth,&nSubStringHeight,TRUE);
			nSubStringOffset = (nTextWidth-nSubStringWidth)/2;
		}
		else
		if( nHorizEffect==HorizontalRightJustified )
		{
			calcRect(sString,&nSubStringWidth,&nSubStringHeight,TRUE);
			nSubStringOffset = nTextWidth-nSubStringWidth;
		}

		x += nSubStringOffset;
		*ppDisplayBuffer += nSubStringOffset;

        // Alimentation rectangle d'affichage
		if( pRect )
		{
			pRect->left  = x;
			pRect->top   = y;
		}

        // Bouclage des caractères
		for( i=0 ; i<nLen ; i++ )
		{
			nCar = sString[i];

			switch( nCar )
			{
				case '\n' :	
				{
					// Prise en compte des justifications centrées/droites
					if( nHorizEffect==HorizontalCenterJustified )
					{
						calcRect(sString+i+1,&nSubStringWidth,&nSubStringHeight,TRUE);
						nSubStringOffset = (nTextWidth-nSubStringWidth)/2;
					}
					else
					if( nHorizEffect==HorizontalRightJustified )
					{
						calcRect(sString+i+1,&nSubStringWidth,&nSubStringHeight,TRUE);
						nSubStringOffset = nTextWidth-nSubStringWidth;
					}

					if( x>nMaxX )
					{
						nMaxX = x;
					}

					x = nInitialX+nSubStringOffset;

                    // Incrément y
					y += (m_nHeight+m_nInterLineSpace);

                    // Mise à jour buffers Dd
					pInitialDisplayBuffer += (nBufferW*(m_nHeight+m_nInterLineSpace));
					*ppDisplayBuffer = pInitialDisplayBuffer + nSubStringOffset;

				}
				break;

				default	: 
                    {
                        bRes = displayCar(ppDisplayBuffer,nBufferW,nBufferH,nDisplayW,nDisplayH,nCar,x,y,nTrans);
                    }
			}
		}

		if( x>nMaxX )
		{
			nMaxX = x;
		}

		if( pRect )
		{
			pRect->right  = nMaxX-m_nInterCharSpace;
			pRect->bottom = y+m_nInterLineSpace;
		}

        if( pTgtOriginal )
        {
            delete [] pTgtOriginal;
        }
	}

	return bRes;
}

/*
BOOL Cfont::displayStringDd(UCHAR *sString,int x,int y,JustifyType nJustify,UINT nTrans,RECT *pRect,RGB *pRgbForeColor,RGB *pRgbBackColor)
{
	BOOL bRes=FALSE;

	if( screen3d && sString && *sString )
	{
        AUTO_LOCK_DD_BEGIN(TRUE);

	    UW	   *pDisplayBuffer;

	    int		xScr,yScr,
			    nScreenW,nScreenH,
			    nDisplayW,nDisplayH,
			    nTextWidth=0,nTextHeight=0;

        if( !Cscreen3d::bLocked )
        {
            goto displayStringDdExit;
        }

      //nScreenW	= screen3d->get_w();
        nScreenW	= Cscreen3d::pitch;
		nScreenH	= Cscreen3d::winh;
		nDisplayW	= Cscreen3d::winw;
		nDisplayH	= Cscreen3d::winh;

        if( !pMenu->menuActived )
        {
            if( bUseSmallFonts || mode_screen==LOWRES )
            {
                x /= 2;
                y /= 2;
            }
        }

        // Calcul des nouveaux X et Y suivant le mode de justification
		calcCoordEffects(sString,&x,&y,nJustify);

		if( (x<nDisplayW) && (y<nDisplayH) )
		{
			if( x<0 ) 
			{
				xScr = 0;
			}
			else
			{
				xScr = x;
			}

			if( y<0 ) 
			{
				yScr = 0;
			}
			else
			{
				yScr = y;
			}

			pDisplayBuffer = screen3d->get_screen_addr(xScr,yScr);

			if( pDisplayBuffer )
			{
				if( x<0 )
				{
					pDisplayBuffer += x;
				}

				if( y<0 )
				{
					pDisplayBuffer += y*nScreenW;
				}

				bRes = displayString(&pDisplayBuffer,nScreenW,nScreenH,nDisplayW,nDisplayH,sString,nJustify,nTextWidth,x,y,nTrans,pRect,pRgbForeColor,pRgbBackColor);
			}
            else
            {
                static BOOL bBeepOnce=FALSE;

                if( !bBeepOnce )
                {
                    bBeepOnce = TRUE;
                    Beep(0,0);
                }
            }
		}

        displayStringDdExit:
        AUTO_LOCK_DD_END;

	}
	return bRes;
}


BOOL Cfont::displayStringDd(CString str,int x,int y,JustifyType nJustify,UINT nTrans,RECT *pRect,RGB *pRgbForeColor,RGB *pRgbBackColor)
{
	return displayStringDd((UCHAR*)LPCTSTR(str),x,y,nJustify,nTrans,pRect,pRgbForeColor,pRgbBackColor);
}

BOOL Cfont::displayStringDd(char *sString,int x,int y,JustifyType nJustify,UINT nTrans,RECT *pRect,RGB *pRgbForeColor,RGB *pRgbBackColor)
{
	return displayStringDd((UCHAR*)sString,x,y,nJustify,nTrans,pRect,pRgbForeColor,pRgbBackColor);
}
*/

//////////////////////////////////////////////////////////////////////////////
// CfontCustomColor

CfontCustomColor::CfontCustomColor()
{
	init();
	m_nType	= FontTypeCustomColor;
}

CfontCustomColor::~CfontCustomColor()
{

}

void CfontCustomColor::calcPixelTab(UINT nFirst,UINT nLast)
{
	// Nada.
}

void CfontCustomColor::updateColors(RGB *pForeColor,RGB *pBackColor)
{
	// Nada.
}

BOOL CfontCustomColor::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
    if( *pStart>0 )
    {
	    UINT nLevel,r,g,b;

	    nLevel = (*pStart%32);

	    r = (m_backColor.r*(31-nLevel) + (m_foreColor.r*nLevel ))/31;
	    g = (m_backColor.g*(31-nLevel) + (m_foreColor.g*nLevel ))/31;
	    b = (m_backColor.b*(31-nLevel) + (m_foreColor.b*nLevel ))/31;

        *pPixel = Cdd::makePixel(r,g,b);
    }
}
#endif
DISPLAY_CAR_END

BOOL CfontCustomColorTrame::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
    int tx=nCptX&15,ty=nCptY&15;
    if( *pStart>0  && ((nTrans==0)||((UINT)((tx*tx+ty*ty)&0x7F)>(nTrans<<1))) )
    {
	    UINT nLevel,r,g,b;

	    nLevel = (*pStart%32);

	    r = (m_backColor.r*(31-nLevel) + (m_foreColor.r*nLevel ))/31;
	    g = (m_backColor.g*(31-nLevel) + (m_foreColor.g*nLevel ))/31;
	    b = (m_backColor.b*(31-nLevel) + (m_foreColor.b*nLevel ))/31;

        *pPixel = Cdd::makePixel(r,g,b);
    }
}
#endif
DISPLAY_CAR_END

//////////////////////////////////////////////////////////////////////////////
// CfontAntiAlias8

CfontAntiAlias8::CfontAntiAlias8()
{
	init();
	m_nType	= FontTypeAntiAlias8;

    // Init des tables de facteurs.

    int nDiv;
    int nTable;

    for( nDiv=7,nTable=0 ; nTable<7 ; nDiv--,nTable++ )
    {
        for( int i=0 ; i<nNbCars ; i++ )
        {
            m_tabFactor[nTable][i] = i*nDiv/8;
        }
    }
}

CfontAntiAlias8::~CfontAntiAlias8()
{

}

void CfontAntiAlias8::calcPixelTab(UINT nFirst,UINT nLast)
{
	UINT r,g,b,nColor,nLevel;

	for( nColor=nFirst ; nColor<=nLast ; nColor++ )
	{
		nLevel = (nColor%32);

		r = (m_backColor.r*(31-nLevel) + (m_foreColor.r*nLevel ))/31;
		g = (m_backColor.g*(31-nLevel) + (m_foreColor.g*nLevel ))/31;
		b = (m_backColor.b*(31-nLevel) + (m_foreColor.b*nLevel ))/31;

		m_tabPixel[nColor] = Cdd::makePixel(r,g,b);
	}
}

BOOL CfontAntiAlias8::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
	if(*pStart>0)
	{
        #if 0
        {
	        if( *pStart<32 )
	        {
		        *pPixel = m_tabPixel[*pStart];
	        }
	        else
	        // >=32 : Transparence modulée suivant page (page1:7/8, page2:6/8 ... page7:1/8)
	        {
		        UINT rS,gS,bS,r,g,b,nAntiAlias;

		        rS = (*pPixel) & Cdd::maskR;
		        gS = (*pPixel) & Cdd::maskG;
		        bS = (*pPixel) & Cdd::maskB;

		        r  = m_tabPixel[*pStart] & Cdd::maskR;
		        g  = m_tabPixel[*pStart] & Cdd::maskG;
		        b  = m_tabPixel[*pStart] & Cdd::maskB;

		        nAntiAlias = (*pStart/32);

		        r  = (( r*(7-nAntiAlias) + rS*nAntiAlias )/7) & Cdd::maskR;
		        g  = (( g*(7-nAntiAlias) + gS*nAntiAlias )/7) & Cdd::maskG;
		        b  = (( b*(7-nAntiAlias) + bS*nAntiAlias )/7) & Cdd::maskB;

		        *pPixel = r|g|b;
	        }
        }
        #else
        {
		    UINT pixRes,pixCar,pixScr,nAntiAlias,nCompAntiAlias;

            nAntiAlias = ((*pStart)>>5)+nTrans;

            if( (nAntiAlias&0xF8)==0 ) // i.e <=7
            {
                if( nAntiAlias==0 )
                {
                    *pPixel = m_tabPixel[*pStart];
                }
                else
                {
                    nCompAntiAlias = 8-nAntiAlias;

                    int *pCompAntiAlias = m_tabFactor[nCompAntiAlias];
                    int *pAntiAlias     = m_tabFactor[nAntiAlias];

                    pixScr   = (*pPixel) & Cdd::maskR;
                    pixScr >>= Cdd::shiftR;
                    pixScr <<= Cdd::shiftCompR;
		            pixCar   = m_tabPixel[*pStart] & Cdd::maskR;
                    pixCar >>= Cdd::shiftR;
                    pixCar <<= Cdd::shiftCompR;
                    pixCar   = pAntiAlias[pixCar]+pCompAntiAlias[pixScr];
                    pixCar   = pixCar>>Cdd::shiftCompR;

                    pixRes   = pixCar<<Cdd::shiftR;

		            pixScr   = (*pPixel) & Cdd::maskG;
		            pixScr >>= Cdd::shiftG;
                    pixScr <<= Cdd::shiftCompG;
		            pixCar   = m_tabPixel[*pStart] & Cdd::maskG;
		            pixCar >>= Cdd::shiftG;
                    pixCar <<= Cdd::shiftCompG;
                    pixCar   = pAntiAlias[pixCar]+pCompAntiAlias[pixScr];
                    pixCar   = pixCar>>Cdd::shiftCompG;

                    pixRes  |= (pixCar<<Cdd::shiftG);

		            pixScr   = (*pPixel) & Cdd::maskB;
		            pixScr >>= Cdd::shiftB;
                    pixScr <<= Cdd::shiftCompB;
		            pixCar   = m_tabPixel[*pStart] & Cdd::maskB;
		            pixCar >>= Cdd::shiftB;
                    pixCar <<= Cdd::shiftCompB;
                    pixCar   = pAntiAlias[pixCar]+pCompAntiAlias[pixScr];
                    pixCar   = (pixCar>>Cdd::shiftCompB);

                    *pPixel  = pixRes|(pixCar<<Cdd::shiftB);
                }
            }
        }
        #endif
	}
}
#endif
DISPLAY_CAR_END

//////////////////////////////////////////////////////////////////////////////
// CfontAntiAlias8Transp

CfontAntiAlias8Transp::CfontAntiAlias8Transp()
{
	init();
	m_nType	= FontTypeAntiAlias8Transp;
}

CfontAntiAlias8Transp::~CfontAntiAlias8Transp()
{

}

BOOL CfontAntiAlias8Transp::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
	if( *pStart>0 )
	{
	    UINT nAntiAlias;

	    if( (*pStart)&31 ) // i.e <32 
	    {
		    nAntiAlias = nTrans;
	    }
	    else
	    // >=32 : Transparence modulée suivant page (page1:7/8, page2:6/8 ... page7:1/8)
	    {
		    nAntiAlias = ((*pStart>>5)<<5)+nTrans;
	    }

		if( nAntiAlias<=224 ) // i.e <224
		{
            #if 0
            {
                UINT rS,gS,bS,r,g,b;

	            rS = (*pPixel) & Cdd::maskR;
	            gS = (*pPixel) & Cdd::maskG;
	            bS = (*pPixel) & Cdd::maskB;

	            r  = m_tabPixel[*pStart] & Cdd::maskR;
	            g  = m_tabPixel[*pStart] & Cdd::maskG;
	            b  = m_tabPixel[*pStart] & Cdd::maskB;

                r  = (( r*(224-nAntiAlias) + rS*nAntiAlias )/224) & Cdd::maskR;
	            g  = (( g*(224-nAntiAlias) + gS*nAntiAlias )/224) & Cdd::maskG;
	            b  = (( b*(224-nAntiAlias) + bS*nAntiAlias )/224) & Cdd::maskB;

                *pPixel = r|g|b;
            }
            #else
            {
                int  pixScr,pixCar;
                UINT pixRes;
                UINT pixCarRGB;
                UINT factor = ((nAntiAlias<<8)/224);

                pixCarRGB = m_tabPixel[*pStart];

	            pixScr  = (*pPixel) & Cdd::maskR;
                pixCar  = pixCarRGB & Cdd::maskR;
                pixCar  = pixCar-((factor*(pixCar-pixScr))>>8);
                pixRes  = pixCar&Cdd::maskR;

	            pixScr  = (*pPixel) & Cdd::maskG;
                pixCar  = pixCarRGB & Cdd::maskG;
                pixCar  = pixCar-((factor*(pixCar-pixScr))>>8);
                pixRes |= (pixCar&Cdd::maskG);

	            pixScr  = (*pPixel) & Cdd::maskB;
                pixCar  = pixCarRGB & Cdd::maskB;
                pixCar  = pixCar-((factor*(pixCar-pixScr))>>8);
               *pPixel  = pixRes|(pixCar&Cdd::maskB);
            }
            #endif
        }
	}
}
#endif
DISPLAY_CAR_END

//////////////////////////////////////////////////////////////////////////////
// CfontPal

CfontPal::CfontPal()
{
	init();
	m_nType	   = FontTypePal;
	m_bKeepPal = TRUE;	// Pour chargement
}

CfontPal::~CfontPal()
{
}

BOOL CfontPal::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
	if( *pStart>0 )
	{
		*pPixel = m_tabPixel[*pStart];
	}
}
#endif
DISPLAY_CAR_END

void CfontPal::calcPixelTab(UINT nFirst,UINT nLast)
{
	UINT nColor;
	RGB *pPal;

	if( m_pPal )
	{
		for( nColor=nFirst ; nColor<=nLast ; nColor++ )
		{
			pPal				= &m_pPal[nColor%m_nPalLen];
			m_tabPixel[nColor]	= Cdd::makePixel(pPal->r,pPal->g,pPal->b);	
		}
	}
}

//////////////////////////////////////////////////////////////////////////////
// CfontMoto

CfontMoto::CfontMoto()
{
	init();
	m_nType	   = FontTypeMoto;
	m_bKeepPal = TRUE;	// Pour chargement
}

CfontMoto::~CfontMoto()
{
}

BOOL CfontMoto::displayCar(UW **ppDisplayBuffer,int nBufferW,int nBufferH,int nDisplayW,int nDisplayH,UCHAR nCar,int &x,int &y,UINT nTrans)
DISPLAY_CAR_BEGIN
#if !DESACTIVE_FONTS
{
	if( *pStart>0 )
	{
		*pPixel = m_tabPixel[*pStart];
	}
    
}
#endif
DISPLAY_CAR_END

void CfontMoto::calcPixelTab(UINT nFirst,UINT nLast)
{
	UINT r,g,b,nColor;

	CfontPal::calcPixelTab(32,62);

	for( nColor=1 ; nColor<=14 ; nColor++ )
	{
		r = (m_backColor.r*(14-nColor) + (m_foreColor.r*nColor ))/14;
		g = (m_backColor.g*(14-nColor) + (m_foreColor.g*nColor ))/14;
		b = (m_backColor.b*(14-nColor) + (m_foreColor.b*nColor ))/14;

		m_tabPixel[nColor] = Cdd::makePixel(r,g,b);
	}
}

/////////////////////////////////////////////////////
// MVRAM

int        Cfont::m_nVramNbPages=0;
int        Cfont::m_nVramNbPals=0;
CVramPage *Cfont::m_tabVramPage[nNbMaxVramPages];
CVramPal  *Cfont::m_tabVramPal[nNbMaxVramPals];


CVramCar::CVramCar()
{
    m_rcCar.left   = 
    m_rcCar.right  = 
    m_rcCar.top    = 
    m_rcCar.bottom = 0;
    m_nIndex       = 0;
    m_pFont        = NULL;
    m_nPalIndex    = -1;
}


CVramPage::CVramPage()
{
    m_pBuffer=new UCHAR [256*256];
}

CVramPage::~CVramPage()
{
    if( m_pBuffer )
    {
        delete [] m_pBuffer;
    }
        
    m_pBuffer=NULL;
}

CVramPal::CVramPal(int nNbColors)
{
    if( m_pBuffer=new RGB [nNbColors] )
    {
        m_nNbColors = nNbColors;    
    }
    else
    {
        m_nNbColors = 0;
    }
}

CVramPal::~CVramPal()
{
    if( m_pBuffer )
    {
        delete [] m_pBuffer;
    }

    m_pBuffer   = NULL;
    m_nNbColors = 0;
};


BOOL Cfont::addVramPage()
{
    BOOL bRes = FALSE;

    if( m_nVramNbPages<nNbMaxVramPages )
    {
        m_tabVramPage[m_nVramNbPages] = new CVramPage;

        if( m_tabVramPage[m_nVramNbPages] && m_tabVramPage[m_nVramNbPages]->m_pBuffer )
        {
            bRes = TRUE;
            m_nVramNbPages++;
        }
    }

    return bRes;
}

void Cfont::delVramPages()
{
    for( int i=0 ; i<m_nVramNbPages ; i++ )
    {
        if( m_tabVramPage[i] )
        {
            delete m_tabVramPage[i];
            m_tabVramPage[i] = NULL;
        }
    }
}

BOOL Cfont::addVramPal(int nNbColors)
{
    BOOL bRes = FALSE;

    if( m_nVramNbPals<nNbMaxVramPals )
    {
        m_tabVramPal[m_nVramNbPals] = new CVramPal(nNbColors);

        if( m_tabVramPal[m_nVramNbPals] && m_tabVramPal[m_nVramNbPals]->m_pBuffer )
        {
            bRes = TRUE;
            m_nVramNbPals++;
        }
    }

    return bRes;
}

void Cfont::delVramPals()
{
    for( int i=0 ; i<m_nVramNbPals ; i++ )
    {
        if( m_tabVramPal[i] )
        {
            delete m_tabVramPal[i];
            m_tabVramPal[i] = NULL;
        }
    }
}

void Cfont::showPage(int nCurrentPalIndex,CProgressDlg *Pgs,CViewDx *Vdd)
{
    UCHAR *pPal=NULL;

    if( nCurrentPalIndex!=-1 )
    {
        pPal=(UCHAR *)m_tabVramPal[nCurrentPalIndex]->m_pBuffer;    
    }
    Vdd->ShowPageWH( m_tabVramPage[m_nVramNbPages-1]->m_pBuffer,pPal,256,256);
    while(!Pgs->CheckCancelButton());
}

BOOL Cfont::buildVramPages(CProgressDlg *Pgs,CViewDx *Vdd)
{
    BOOL bRes=TRUE;
    CString sErr;
    int nCurrentPalIndex;

    if( tabFont )
    {
        int nX,nY,nCarIndexInPage,nMaxH=-1;

        bRes = addVramPage();
        nX   = 0;
        nY   = 0;
        nCarIndexInPage = 0;

        for( int nFontIndex=0 ; bRes && nFontIndex<Cfont::nNbFonts ; nFontIndex++ )
        {
            Cfont *pFont = tabFont[nFontIndex];

            sErr.Format(L"Computing font #%d (%s)...",nFontIndex,pFont->m_sName);

            nCurrentPalIndex = -1;

            if( pFont->m_bKeepPal )
            {
                if( pFont->m_pPal ) 
                {
                    if( addVramPal(pFont->m_nPalLen) )
                    {
                        nCurrentPalIndex = m_nVramNbPals-1;
                        memcpy(m_tabVramPal[nCurrentPalIndex]->m_pBuffer,pFont->m_pPal,sizeof(RGB)*pFont->m_nPalLen);
                        sErr+=L"Palette created";
                    }
                    else sErr+=L"Can't create palette";
                }
                else sErr+=L"Palette not found in Cfont struct";
            }
            else sErr+=L"No palette";

            sErr+=L"\r\n";

            Pgs->AddDbgInfo(sErr);

            for( int nCar=0 ; bRes && nCar<nNbCars ; nCar++ )
            {
                if( pFont->m_tabWidth[nCar]>1 )
                {
                    if( nX&1 )
                    {
                        nX++;
                    }

                    if( nX+pFont->m_tabWidth[nCar]>=256 )
                    {
                        nX  = 0;
                        nY += nMaxH;
                        nMaxH = -1;
                    }

                    if( nY+pFont->m_nHeight>=256 )
                    {
                        showPage(nCurrentPalIndex,Pgs,Vdd);

                        if( bRes = addVramPage() )
                        {
                            nX = 0;
                            nY = 0;
                            nCarIndexInPage = 0;
                            nMaxH = -1;
                        }
                        else
                        {
                            goto PutainDeViergeMarieAPoilMalBaiseeParTrenteGorillesEnRut;
                        }
                    }

                    UCHAR *pBuffer = m_tabVramPage[m_nVramNbPages-1]->m_pBuffer+nX+nY*256;

                    if( nCarIndexInPage<CVramPage::nNbMaxCarInPage )
                    {
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_rcCar.left   = nX;
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_rcCar.top    = nY;
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_rcCar.right  = nX+pFont->m_tabWidth[nCar]-1;
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_rcCar.bottom = nY+pFont->m_nHeight-1;
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_nIndex       = nCar;    
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_pFont        = pFont;
                        m_tabVramPage[m_nVramNbPages-1]->m_tabCar[nCarIndexInPage].m_nPalIndex    = nCurrentPalIndex;

                        nCarIndexInPage++;

                        #if 0
                        {
                            int nWriteX,nWriteY;
                            bRes = pFont->displayCar(&pBuffer,256,256,256,256,nCar,nWriteX,nWriteY);
                        }
                        #else
                        {
                            UCHAR *pBufferSrc = pFont->m_tabOffset[nCar];

                            for( int y=0 ; y<pFont->m_nHeight ; y++ )
                            {
                                memcpy(pBuffer,pBufferSrc,pFont->m_tabWidth[nCar]);
                                pBufferSrc += pFont->m_tabWidth[nCar];
                                pBuffer += 256;
                            }

                            nX += pFont->m_tabWidth[nCar];
                        }
                        #endif

                        if( nMaxH<pFont->m_nHeight )
                        {
                            nMaxH = pFont->m_nHeight;
                        }
                    }
                    else
                    {
                        sErr.Format(L"Page %d,Font %d (%s),Car %d\nToo many cars for page (max=%d)",m_nVramNbPages-1,nFontIndex,pFont->m_sName,nCar,CVramPage::nNbMaxCarInPage);
                        AfxMessageBox(sErr);
                        bRes =FALSE;
                    }
                }

                PutainDeViergeMarieAPoilMalBaiseeParTrenteGorillesEnRut:;
            }

            showPage(nCurrentPalIndex,Pgs,Vdd);
        }
    }

    if( !bRes )
    {
        sErr.Format(L"Sorry...\r\n",m_nVramNbPages);
        Pgs->AddDbgInfo(sErr);

        delVramPages();
    }
    else
    {
        sErr.Format(L"Done %d pages and %d palettes\r\n",m_nVramNbPages,m_nVramNbPals);
        Pgs->AddDbgInfo(sErr);

        Beep(0,0);

        showPage(nCurrentPalIndex,Pgs,Vdd);
    }

    return bRes;
}

void Cfont::initVramData()
{
    memset(m_tabVramPage,0,sizeof(CVramPage*)*nNbMaxVramPages);
    memset(m_tabVramPal,0,sizeof(CVramPal*)*nNbMaxVramPals);
}

void Cfont::delVramData()
{
    delVramPages();
    delVramPals();
}

#undef FONT_C
