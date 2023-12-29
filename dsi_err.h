#ifndef _DSI_ERROR_H
#define _DSI_ERROR_H

#include <stdio.h>

#define DSI_ERROR_FILE	1


#if DSI_ERROR_FILE
void initErrorFile();
int addWarnToErrorFile(wchar_t *string);
extern wchar_t tempText[];							// String pour utilisations temporaires
extern int ds_nb_err_file;
#define PRINT_ERROR_FILE(a)			addWarnToErrorFile(a)
#else
#define PRINT_ERROR_FILE(a)			
#endif



	typedef enum
	{
		DSI_ERROR_UNKNONW_ERROR			=	0,
		DSI_ERROR						=  -1,
		DSI_ERROR_FILE_OPEN				=  -2,
		DSI_ERROR_FILE_EOF				=  -3, 
		DSI_ERROR_FILE_NOT_OPEN			=  -4,
		DSI_ERROR_FILE_WRITE			=  -5,
		DSI_ERROR_BAD_DATA				=  -6,
		DSI_ERROR_BAD_FORMAT			=  -7,
		DSI_ERROR_FILE_ALREADY_OPENED	=  -8,
		DSI_ERROR_INVALID_HEADER		=  -9,
		DSI_ERROR_NOT_ENOUGH_MEMORY		= -10,
		DSI_ERROR_BAD_FILE_DATA			= -11,
		DSI_ERROR_CANT_FIND_FIRST_CAR	= -12,
		DSI_ERROR_BAD_HEIGHT			= -13,
		DSI_ERROR_READ_ERROR			= -14,
		DSI_ERROR_REDEFINED_CAR			= -15,
		DSI_ERROR_FORMAT_NOT_SUPPORTED	= -16,
		DSI_ERROR_TOO_MANY_BANKS		= -17,
        DSI_ERROR_END_BLOCK             = -18,
	}	dsiErrorType;

	#define DSI_NB_ERRORS 19

	/****************************************************************************
	*                                                                           *
	*                          class CdsiError                                  *
	*                                                                           *
	****************************************************************************/

	class CdsiError			// Exception handler
	{
		public:
			
			enum {LocalErrorCommentLen=256};

			CdsiError() 
				{ 
				  m_nError=DSI_ERROR; 
			      m_sErrorInfos[0]=0;
				  m_sErrorInfos[LocalErrorCommentLen]=0;
                  m_nLine=-1;
                  m_nCol=-1;
			    }

			CdsiError(dsiErrorType nError) 
				{ 
				  m_nError=nError; 
			      m_sErrorInfos[0]=0;
				  m_sErrorInfos[LocalErrorCommentLen]=0;
                  m_nLine=-1;
                  m_nCol=-1;
			    }

			CdsiError(dsiErrorType nError,int nLine,int nCol) 
				{ 
				  m_nError=nError; 
			      m_sErrorInfos[0]=0;
				  m_sErrorInfos[LocalErrorCommentLen]=0;
                  m_nLine=nLine;
                  m_nCol=nCol;
			    }

			CdsiError(dsiErrorType nError,const char *pErrorInfos) 
				{ 
				  m_nError=nError; 
				  if(pErrorInfos)
				  {
					strncpy(m_sErrorInfos,pErrorInfos,LocalErrorCommentLen);
				  }
				  else
				  {
					m_sErrorInfos[0]=0;
				  }
				  m_sErrorInfos[LocalErrorCommentLen]=0;
                  m_nLine=-1;
                  m_nCol=-1;
			    }

		   ~CdsiError() {}

		    void setError(dsiErrorType err) { m_nError = err; }
			void displayMessageBox(const wchar_t *pFileName=NULL,const wchar_t *pComment=NULL);

            int             m_nLine,m_nCol;
			char			m_sErrorInfos[LocalErrorCommentLen+1];
			dsiErrorType	m_nError;
			static  char   *m_sErrorTxt[DSI_NB_ERRORS+1];
	};	

	#ifdef _DSI_ERROR_C
		char *CdsiError::m_sErrorTxt[DSI_NB_ERRORS+1] = 
		{
			"(Unknown error)",
			""/*"(Undocumented error)"*/,
			"File open error",
			"End of file error",
			"Not opened file error",
			"Write error",
			"Bad data",
			"Bad format",
			"File already opened",
			"Invalid header",
			"Not enough memory",
			"Bad data in file",
			"Can't find first char.",
			"One char has a bad height",
			"Reading error. File corrupted",
			"Char redefined",
			"This format is not supported",
			"Too many banks",
            "End of block encountered"
		};
	#endif

#endif