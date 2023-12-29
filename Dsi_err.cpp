#include "StdAfx.h"

#define _DSI_ERROR_C

#include <stdio.h>

#include "dsi_err.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if DSI_ERROR_FILE

int ds_err_to_file = -1;
int ds_nb_err_file = 0;
wchar_t tempText[160];							// String pour utilisations temporaires

//-------------------------------------------------------------------
void initErrorFile()
{
	FILE *errFile;
	if (ds_err_to_file == -1)
	{
		if ((errFile=fopen("c:\\delphine.err", "wt")) != NULL)
		{
			int size;
			char text[]="Error occured during game : \n";
			size = (int)strlen(text);
			fwrite(text, sizeof(char) ,size, errFile);

			fclose(errFile);
			ds_err_to_file = 1;
		}
		else
		{
			ds_err_to_file = 0;
		}
	}
}


//-------------------------------------------------------------------
int addWarnToErrorFile(wchar_t *string)
{
if (ds_err_to_file == -1)
	initErrorFile();

ds_nb_err_file++;		// On incremente le nombre d'erreur meme si on ne peut
						// pas écrire dans le fichier

if (ds_err_to_file)
    {
    unsigned int size;

    size = (int)wcslen(string)*2;

	FILE *errFile;
	if ((errFile=fopen("c:\\error.err", "at+")) == NULL)
	{
		ds_err_to_file = 0;
		return 0;
	}

    if (fwrite(string, sizeof(char) ,size, errFile) < size)
    {
		fclose(errFile);
        ds_err_to_file = 0;
        return 0;
    }
	fclose(errFile);

    }
return 1;
}



#endif		// DSI_ERROR_FILE

//-------------------------------------------------------------------
void CdsiError::displayMessageBox(const wchar_t *pFileName,const wchar_t *pComment)
{
	CString sFinalText;
    CString sComment;
    CString sTmp;

	sComment="";

	if( pFileName && *pFileName )
	{
		sComment += "File ";
        sComment += pFileName;
	}

    if( m_nLine>=0 )
    {
        sTmp.Format(L"   Line:%d",m_nLine);
        sComment += sTmp;    
    }

    if( m_nCol>=0 )
    {
        sTmp.Format(L"   Col:%d",m_nCol);
        sComment += sTmp;    
    }

    if( sComment.GetLength()>0 )
    {
        sComment += '\n';
    }

	if( pComment && *pComment )
	{
        sComment += pComment;
		sComment += '\n';
	}

	if( (m_nError<=0) && (-m_nError<DSI_NB_ERRORS) )
	{
        sComment += '\n';
		sComment += m_sErrorTxt[-m_nError];
        sComment += '\n';
	}

	sFinalText  = m_sErrorInfos;
	sFinalText += "\n";
	sFinalText += sComment;

    addWarnToErrorFile(sFinalText.GetBuffer());
	AfxMessageBox(sFinalText);
}

#undef _DSI_ERROR_C