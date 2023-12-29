

#ifndef _DSIFILETEXT
	#define _DSIFILETEXT

	#include <stdio.h>

	const int DefaultTextLen = 256;

	/****************************************************************************
	*                                                                           *
	*                          class CfileText		                            *
	*                                                                           *
	****************************************************************************/

    class CfileText
	{	
		protected:		
			FILE *fh;	
		public:
			 CfileText();
			 CfileText(const wchar_t *sName,int nOpenMode=OpenReadOnly);
			~CfileText();

            int     m_nCurrentCol;
            int     m_nCurrentLine;

            const   char cEndBlock;
            const   char cStartComment;

			void    open(const wchar_t *sName=NULL,int nOpenMode=OpenReadOnly);
			void    close();

			void    openRead(const wchar_t *sName=NULL);
			void    openReadWrite(const wchar_t *sName=NULL);

			int		scanToken(char text[]);
			void	findToken(char *text);
 			
			float	getFloat();
			int		getInt();
			void	getString(char *dest);

			void	putFloat(float f,int nNbCarsBeforePoint=4,int nNbCarsAfterPoint=4);
			void	putInt(int i,int nNbCars=0);
			void	putString(char *s,int nNbCars=0);
			void    putChar(char c,int nNbCars=0);

			float	getTokenFloat(char *text);
 			int		getTokenInt(char *text);
			void	getTokenString(char *text, char *dest);

            char    seekEndOfComment();
			void    rewind();

			static  char   *getSampleFileName(char *pPath);

			static  char	m_sTmpFileName[DefaultTextLen];

			wchar_t			m_sFileName[DefaultTextLen];
			int				m_nOpenMode;


			enum { OpenModeUndefined,OpenReadOnly,OpenReadWrite };
	};

	/****************************************************************************
	*                                                                           *
	*                          class CscriptFont	                            *
	*                                                                           *
	****************************************************************************/

	class CscriptFont : public CfileText
	{
		public:
			CscriptFont(wchar_t *sDataPath,wchar_t *sFileName);
		   ~CscriptFont();

			BOOL	load(void(*pCB)(CString s1,int n),int *pNbItems,BOOL bOnlyCount=FALSE);
			static char *sExtension;
            CString m_sDataPath;
	};

#endif
