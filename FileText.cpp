
#include "stdafx.h"

/****************************************************************************
*    					    Includes.	                                    *
*---------------------------------------------------------------------------*
* Uncomment the following lines if you don't use the precompiled header     *
* pchdsi.h                                                                  *         
*                                                                           *
****************************************************************************/

#define nTmpStrLen 1024
char sTmpBuffer[nTmpStrLen];
char sTmp[nTmpStrLen];

#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <ctype.h>
#include <assert.h>
#include "PathNameTools.h"
#include "FileText.h"
#include "dsi_err.h"
#include "dsi_file.h"
#include "Font.h" // Si le tableau global Cfont::tabFont s'y trouve (au lieu d'être dans Globals.h)

/****************************************************************************
*                                                                           *
*                          class CfileText                                  *
*                                                                           *
****************************************************************************/

char CfileText::m_sTmpFileName[DefaultTextLen];

/**************************************
* CfileText::CfileText				  *
**************************************/

/*
 * default constructor for derived classes
 */

CfileText::CfileText() : cEndBlock('}'),cStartComment(';')
{                        
	fh              = NULL;
	m_sFileName[0]  = '\0';
	m_nOpenMode     = OpenModeUndefined;
    m_nCurrentCol   = 0;
    m_nCurrentLine  = 0;
}  // CfileText::CfileText

/**************************************
* CfileText::CfileText				  *
**************************************/

/*
 * prepares the name and the open mode
 */

CfileText::CfileText(const wchar_t *sFileName,int nOpenMode) : cEndBlock(L'}'),cStartComment(L';')
{
	fh			    = NULL;
	m_nOpenMode     = nOpenMode;
    m_nCurrentCol   = 0;
    m_nCurrentLine  = 0;

	if( sFileName && *sFileName )
	{
		wcscpy(m_sFileName,sFileName);
	}
	else
	{
		m_sFileName[0] = '\0';
	}
} // CfileText::CfileText

/**************************************
* CfileText::~CfileText				  *
**************************************/

/*
 * close file
 */

CfileText::~CfileText()
{
	if (fh) dsi_fclose(fh);

} // CfileText::~CfileText

/**************************************
* CfileText::open				  *
**************************************/

/*
 * open sFileName with WRITE/READ TEXT attributes
 */

void CfileText::open(const wchar_t *sFileName,int nOpenMode)
{
	m_nOpenMode = nOpenMode;

	if( sFileName && *sFileName )
	{
		wcscpy(m_sFileName,sFileName);
	}
	// else : on garde le nom par defaut.

	try
	{
		if( fh )
		{
			throw CdsiError(DSI_ERROR_FILE_ALREADY_OPENED);
		}

		switch( nOpenMode )
		{
			case OpenReadOnly:	fh = dsi_fopen(m_sFileName,L"rt");	break;
			case OpenReadWrite:	fh = dsi_fopen(m_sFileName,L"wrt");	break;
		}

		if( !fh )
		{
			throw CdsiError(DSI_ERROR_FILE_OPEN);
		}
	}
	catch( CdsiError E )
	{
		E.displayMessageBox(m_sFileName);
	};

} // CfileText::open

/**************************************
* CfileText::openRead					  *
**************************************/

/*
 * open sFileName with READ/TEXT attributes
 */

void CfileText::openRead(const wchar_t *sFileName)
{
	open(sFileName,OpenReadOnly);
} // CfileText::openRead

/**************************************
* CfileText::openReadWrite			  *
**************************************/

/*
 * open sFileName with READ/WRITE/TEXT attributes
 */

void CfileText::openReadWrite(const wchar_t *sFileName)
{
	open(sFileName,OpenReadWrite);
} // CfileText::openReadWrite

/**************************************
* CfileText::close					  *
**************************************/

/*
 * open sFileName with READ/WRITE/TEXT attributes
 */

void CfileText::close()
{
	if( fh )
	{
		dsi_fclose(fh);
		fh = NULL;
	}
} // CfileText::close

/**************************************
* CfileText::seekEndOfComment		  *
**************************************/

/*
 * go until end of comment (EOL)
 *
 */

char CfileText::seekEndOfComment()
{
    if( !fh )
    {
        throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
    }

    char c;

    do
    {
        c=dsi_fgetc(fh);

		if (c==EOF) 
        {
            throw CdsiError(DSI_ERROR_FILE_EOF);
        }
    }
    while(c!='\n');

    m_nCurrentCol = 0;
    m_nCurrentLine++;

    return c;
}

/**************************************
* CfileText::scanToken				  *
**************************************/

/*
 * get next token, result in text[].
 *
 * note: the ':' char is taken as a separator
 */

int CfileText::scanToken(char text[])
{
	char c;
	int i=0;

	do
	{
        c=dsi_fgetc(fh);

        if( c=='\n' )
        {
            m_nCurrentCol = 0;
            m_nCurrentLine++;
        }
        else
        {
            m_nCurrentCol++;
        }

		if (c==EOF) 
        {
            throw CdsiError(DSI_ERROR_FILE_EOF);
        }
        else
        if (c==cEndBlock)
        {
            throw CdsiError(DSI_ERROR_END_BLOCK);
        }
        else
        if (c==cStartComment)
        {
            c=seekEndOfComment();
        }
	} 
    while ( isspace(c) || (c==':') );
	
	do
	{
		text[i++] = c;
        c = dsi_fgetc(fh);

        if( c=='\n' )
        {
            m_nCurrentCol = 0;
            m_nCurrentLine++;
        }
        else
        {
            m_nCurrentCol++;
        }

		if (c==EOF)
        {
            throw CdsiError(DSI_ERROR_FILE_EOF);
        }
	} 
    while ( !isspace(c) && (c!=':') && (c!=cEndBlock) && (c!=cStartComment) );	 

    if (c==cStartComment)
    {
        c=seekEndOfComment();
    }

	text[i]=0;
	return i;

} // CfileText::scanToken

/**************************************
* CfileText::getFloat				  *
**************************************/

/*
 * return next token as a float
 */

float CfileText::getFloat()
{

	float v;
    scanToken(sTmpBuffer);
	if (sscanf(sTmpBuffer,"%f",&v) == EOF) throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
	return v;
} // CfileText::getFloat

/**************************************
* CfileText::getString				  *
**************************************/

/*
 * return next token as a string
 */

void CfileText::getString(char *dest)
{
    scanToken(dest);
} // CfileText::getString

/**************************************
* CfileText::getInt					  *
**************************************/

/*
 * return next token as an int
 */

int CfileText::getInt()
{
	int v;
    scanToken(sTmpBuffer);
	if (sscanf(sTmpBuffer,"%d",&v) == EOF) throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
	return v;

} // CfileText::getInt

/**************************************
* Fonctions statiques de préparation de format scanf/printf
**************************************/
static char sFormat[32];

static void CalcFormat(int nNbCars,char *sPureFormat)
{
	if( nNbCars>0 )
	{
		sprintf(sFormat,"%%%d%s",nNbCars,sPureFormat);
	}
	else
	{
		sprintf(sFormat,"%%%s",sPureFormat);
	}
}

static void CalcFloatFormat(int nNbCarsBeforePoint,int nNbCarsAfterPoint,char *sPureFormat)
{
	sprintf(sFormat,"%%%d.%d%s",nNbCarsBeforePoint+nNbCarsAfterPoint,nNbCarsAfterPoint,sPureFormat);
}

/**************************************
* CfileText::putFloat				  *
**************************************/

/*
 * writes a float value
 */

void CfileText::putFloat(float v,int nNbCarsBeforePoint,int nNbCarsAfterPoint)
{
	if (!fh) 
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
	}
	
	CalcFloatFormat(nNbCarsBeforePoint,nNbCarsAfterPoint,"f");

	if (fprintf(fh,sFormat,v) < 0) 
	{
		throw CdsiError(DSI_ERROR_FILE_WRITE);
	}
} // CfileText::putFloat

/**************************************
* CfileText::putString				  *
**************************************/

/*
 * writes a string
 */

void CfileText::putString(char *s,int nNbCars)
{
	if (!fh) 
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
	}
	
	if (!s) 
	{
		throw CdsiError(DSI_ERROR_FILE_WRITE);
	}

	CalcFormat(nNbCars,"s");

	if (fprintf(fh,sFormat,s) < 0) 
	{
		throw CdsiError(DSI_ERROR_FILE_WRITE);
	}
} // CfileText::putString

/**************************************
* CfileText::putInt					  *
**************************************/

/*
 * writes an integer
 */

void CfileText::putInt(int i,int nNbCars)
{
	if (!fh)
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
	}

	CalcFormat(nNbCars,"d");

	if (fprintf(fh,sFormat,i) < 0) 
	{
		throw CdsiError(DSI_ERROR_FILE_WRITE);
	}
} // CfileText::putInt

/**************************************
* CfileText::putChar				  *
**************************************/

/*
 * writes a character
 */

void CfileText::putChar(char c,int nNbCars)
{
	if (!fh) 
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
	}
	
	CalcFormat(nNbCars,"c");

	if (fprintf(fh,sFormat,c) < 0) 
	{
		throw CdsiError(DSI_ERROR_FILE_WRITE);
	}
} // CfileText::putChar

/**************************************
* CfileText::findToken				  *
**************************************/

/*
 * Find the next token == text
 */

void CfileText::findToken(char *text)
{
	char tmp[80];

	if (!fh)
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
		return;
	}

	scanToken(tmp);
	while (strcmp(tmp,text) != 0)
	{
		scanToken(tmp);
	}

} // CfileText::findToken

/**************************************
* CfileText::getTokenFloat			  *
**************************************/

/*
 * find a token==text and return the next value as float
 */

float CfileText::getTokenFloat(char *text)
{
	findToken(text);
	return getFloat();

} // CfileText::getTokenFloat

/**************************************
* CfileText::getTokenInt			  *
**************************************/

/*
 * find a token==text and return the next value as int
 */

int CfileText::getTokenInt(char *text)
{
	findToken(text);
	return getInt();

}  // CfileText::getTokenInt

/**************************************
* CfileText::getTokenString			  *
**************************************/

/*
 * find a token==text and return the next value as string
 */

void CfileText::getTokenString(char *text, char *dest)
{
	findToken(text);
	getString(dest);

}  // CfileText::getTokenString


/**************************************
* CfileText::rewind					  *
**************************************/

/*
 * rewinds the file pointer
 */

void CfileText::rewind()
{
	if (!fh)
	{
		throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
		return;
	}

    m_nCurrentCol   = 0;
    m_nCurrentLine  = 0;

	::rewind(fh);

}  // CfileText::rewind

/**************************************
* CfileText::getSampleFileName		  *
**************************************/

/*
 * fills and return global string sTmp with the file name without path nor extension.
 * This is a static function. It can be called by CfileText::getSampleFileName(path) from
 * any point of source.
 */

char *CfileText::getSampleFileName(char *pPath)
{
	char *pSlash,*pPoint;

	memset(sTmp,0,nTmpStrLen);

	if( pPath && *pPath )
	{
		pSlash = strrchr(pPath,'\\');

		if( pSlash )
		{
			pPath = pSlash+1;
		}

		if( *pPath )
		{
			int nMaxCpyLen = nTmpStrLen-1-(int)strlen(pPath);

			if( (nMaxCpyLen<=0) || (nMaxCpyLen>=nTmpStrLen) )
			{
				nMaxCpyLen=nTmpStrLen-1;
			}

			strncpy(sTmp,pPath,nMaxCpyLen);

			pPoint = strrchr(pPath,'.');

			if( pPoint )
			{
				*pPoint = '\0';	
			}
		}
	}

	return sTmp;
} // *CfiletText::getSampleFileName


/****************************************************************************
*                                                                           *
*                          class CscriptFont								*
*                                                                           *
*  This file is read-only. The constructor opens it with OpenReadOnly mode  *  
*  and the destructor closes it.                                            *
*                                                                           *
****************************************************************************/

char *CscriptFont::sExtension="spt";

CscriptFont::CscriptFont(wchar_t *sDataPath,wchar_t *sFileName)
{
    m_sDataPath = sDataPath;
	openRead(sFileName);
}

CscriptFont::~CscriptFont()
{
	close();
}

BOOL CscriptFont::load(void(*pCB)(CString s1,int n),int *pNbItems,BOOL bOnlyCount)
{
	BOOL bOk=FALSE,bCont=TRUE;
	char pFileName[DefaultTextLen];
	char pFontType[DefaultTextLen];
	CdsiError error;
	int	nIndex;


	try				   
	{
        if( !pNbItems )
        {
            throw CdsiError(DSI_ERROR_BAD_DATA);
        }

		if( !fh )
		{
			throw CdsiError(DSI_ERROR_FILE_NOT_OPEN);
		}

		bOk	= TRUE;

		// Lecture du nombre de fontes
		rewind();
		try
		{
			(*pNbItems) = getTokenInt("FontNumber");
		}
		catch( CdsiError E )
		{
			E.displayMessageBox(m_sFileName,L"Reading font number");
			throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
		};

        if( bOnlyCount )
        {
            throw 1;
        }

		if( ((*pNbItems)<0) || ((*pNbItems)>32) )
		{
			throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
		}

		Cfont::tabFont = new LPCfont [(*pNbItems)];

		// Lecture des types
		rewind();
		try
		{
			nIndex = getTokenInt("FontTypes");
			bCont  = TRUE;

			while( bCont )
			{
                getString(sTmp); // Nom de la fonte
				getString(pFontType);

				if( (nIndex<0) || (nIndex>=(*pNbItems)) )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				if( !pFontType )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				if( !strncmp(pFontType,"Direct",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new Cfont;
				}
				else
				if( !strncmp(pFontType,"DirectTrame",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontDirectTrame;
				}
				else
				if( !strncmp(pFontType,"CustomColor",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontCustomColor;
				}
				else
				if( !strncmp(pFontType,"CustomColorTrame",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontCustomColorTrame;
				}
				else
				if( !strncmp(pFontType,"AntiAlias8",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontAntiAlias8;
				}
				else
				if( !strncmp(pFontType,"AntiAlias8Transp",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontAntiAlias8Transp;
				}
				else
				if( !strncmp(pFontType,"Pal",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontPal;
				}
				else
				if( !strncmp(pFontType,"Moto",DefaultTextLen) )
				{
					Cfont::tabFont[nIndex]=new CfontMoto;
				}
				else
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				if( !Cfont::tabFont[nIndex] )
				{
					throw CdsiError(DSI_ERROR_NOT_ENOUGH_MEMORY);
				}

                strncpy(Cfont::tabFont[nIndex]->m_sName,sTmp,FontNameTypeLen);

				nIndex = getInt();

				if( nIndex==-1 )
				{
					bCont=FALSE;
				}
			}
		}
		catch( CdsiError E )
		{
			if( E.m_nError!=DSI_ERROR_FILE_EOF )
			{
				E.displayMessageBox(m_sFileName,L"Reading font types");
				throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
			}
		};

		// Lecture des interlignes
		rewind();
		try
		{
			nIndex = getTokenInt("FontSpacings");
			bCont  = TRUE;

			while( bCont )
			{
				if( (nIndex<0) || (nIndex>=(*pNbItems)) )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

                if( Cfont::tabFont[nIndex] )
                {
				    Cfont::tabFont[nIndex]->m_nInterCharSpace=getInt();
				    Cfont::tabFont[nIndex]->m_nInterLineSpace=getInt();
                }

				nIndex = getInt();

				if( nIndex==-1 )
				{
					bCont=FALSE;
				}
			}
		}
		catch( CdsiError E )
		{
			if( E.m_nError!=DSI_ERROR_FILE_EOF )
			{
				E.displayMessageBox(m_sFileName,L"Reading font spacings");
				throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
			}
		};

		// Lecture des couleurs initiales
		rewind();
		try
		{
			RGB_MEMBER r,g,b;

			nIndex = getTokenInt("FontForeColors");
			bCont  = TRUE;

			while( bCont )
			{
				if( (nIndex<0) || (nIndex>=(*pNbItems)) )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				r = getInt();
				g = getInt();
				b = getInt();

                if( Cfont::tabFont[nIndex] )
                {
				    Cfont::tabFont[nIndex]->setForeColor(r,g,b);
                }

				nIndex = getInt();

				if( nIndex==-1 )
				{
					bCont=FALSE;
				}
			}
		}
		catch( CdsiError E )
		{
			if( E.m_nError!=DSI_ERROR_FILE_EOF )
			{
				E.displayMessageBox(m_sFileName,L"Reading font fore colors");
				throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
			}
		};

		// Lecture des couleurs de fond initiales
		rewind();
		try
		{
			RGB_MEMBER r,g,b;

			nIndex = getTokenInt("FontBackColors");
			bCont  = TRUE;

			while( bCont )
			{
				if( (nIndex<0) || (nIndex>=(*pNbItems)) )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				r = getInt();
				g = getInt();
				b = getInt();

                if( Cfont::tabFont[nIndex] )
                {
				    Cfont::tabFont[nIndex]->setBackColor(r,g,b);
                }

				nIndex = getInt();

				if( nIndex==-1 )
				{
					bCont=FALSE;
				}
			}
		}
		catch( CdsiError E )
		{
			if( E.m_nError!=DSI_ERROR_FILE_EOF )
			{
				E.displayMessageBox(m_sFileName,L"Reading font back colors");
				throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
			}
		};

		// Lecture des données
		rewind();
		try
		{
			int nFirstCar,nLastCar,nFrameColor;

			nIndex = getTokenInt("FontData");
			bCont  = TRUE;

			while( bCont )
			{
				if( (nIndex<0) || (nIndex>=(*pNbItems)) )
				{
					throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				}

				nFirstCar	= getInt();
				nLastCar	= getInt();
				nFrameColor	= getInt();

				getString(pFileName);

                if( pCB )
                {
                    CString s;
                    s.Format(L"%s",Cfont::tabFont[nIndex]->m_sName);
                    pCB(s,0);
                }

                if( Cfont::tabFont[nIndex] )
                {
                    CString sDataFile;

                    sDataFile  = m_sDataPath;
                    sDataFile += pFileName;

				    if( !Cfont::tabFont[nIndex]->load((LPCTSTR)sDataFile,nFirstCar,nLastCar,nFrameColor) )
				    {
					    throw CdsiError(DSI_ERROR_BAD_FILE_DATA);
				    }
                }
				
				nIndex = getInt();

				if( nIndex==-1 )
				{
					bCont=FALSE;
				}
			}
		}
		catch( CdsiError E )
		{
			if( E.m_nError!=DSI_ERROR_FILE_EOF )
			{
				E.displayMessageBox(m_sFileName,L"Loading fonts data");
				bOk = FALSE;
			}
		};
	}
    catch( int b )
    {
        if( b==1 )
        {
            bOk = TRUE;
        }
    }
	catch( CdsiError E )
	{
		E.displayMessageBox(m_sFileName,L"Loading fonts script");
		bOk = FALSE;
	};

	return bOk;
}

