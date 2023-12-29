#include "stdafx.h"

#define PATH_NAME_TOOLS_C

#include <string.h>
#include <ctype.h>

//#include "Macros.h"
#include "PathNameTools.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#undef PATH_NAME_TOOLS_VERBOSE

#define TMP_BUFFER_LEN 1024
static wchar_t pBuffer[TMP_BUFFER_LEN+1];
	
/***************************************************************************************

	FONCTIONS DIVERSES SUR LES NOMS DE FICHIERS

***************************************************************************************/

// Extrait le nom de fichier à partir d'un chemin complet.

wchar_t *GetFileNameFromPath(const wchar_t *pPath)
{
	wchar_t *pChar=(wchar_t *)pPath;

	if( pPath )
	{
		if( pChar = wcsrchr(pChar,L'\\') )
		{
			pChar++;

			if( *pChar==0 )
			{
				pChar = (wchar_t *)pPath;
			}
		}
		else
		{
			pChar=(wchar_t *)pPath;
		}
	}

	return pChar;
}


// Extrait l'extension à partir d'un nom de fichier et la transforme en minuscules

wchar_t*GetExtensionFromPathName(const wchar_t *pName)
{
	wchar_t*pChar=(wchar_t*)pName;

	if( pName )
	{
		if( pChar = wcsrchr(pChar,L'.') )
		{
			pChar++;

			// Mise en minuscules
			wchar_t*pTmpChar = pChar;

			while( *pTmpChar )
			{
				*pTmpChar = tolower(*pTmpChar);
				pTmpChar++;
			}
		}
		else
		{
			pChar=NULL;
		}
	}

	return pChar;
}

// Extrait le chemin sans le nom de fichier à partir d'un chemin complet.
// Comme on ne touche pas au nom initial et qu'il n'est pas 
// question de créer une copie ici, on écrit dans un buffer
// statique.

// => Utiliser cette fonction en la faisant suivre par un 
//    strcpy (ptr de retour=valeur temporelle)

wchar_t*GetTmpPathFromPathName(const wchar_t*pName)
{
	const wchar_t*pSlash;

	ASSERT(pName);

	memset(pBuffer,0,TMP_BUFFER_LEN);

	if( pName )
	{
		if( pSlash=wcsrchr(pName,L'\\') )
		{
			pSlash++;

			if( *pSlash )
			{
				wcsncpy(pBuffer,pName,pSlash-pName-1);
			}
		}
	}

	return pBuffer;
}

// Extrait le nom sans l'extension à partir d'un nom de fichier.
// Comme on ne touche pas au nom initial et qu'il n'est pas 
// question de créer une copie ici, on écrit dans un buffer
// statique.

// => Utiliser cette fonction en la faisant suivre par un 
//    strcpy (ptr de retour=valeur temporelle)

wchar_t*GetTmpNameFromPathName(wchar_t*pName)
{
	wchar_t*pPoint,*pSlash;

	ASSERT(pName);

	memset(pBuffer,0,TMP_BUFFER_LEN);

	if( pName )
	{
		wcsncpy(pBuffer,pName,511);

		pSlash = wcsrchr(pName,L'\\');
		pPoint = wcsrchr(pName,L'.');

		if( pSlash )
		{
			if( pSlash>pPoint )
			{
				if( wcslen(pSlash)<511 )
				{
					wcscpy(pBuffer,pSlash+1);
				}
			}
			else
			if( pPoint>pSlash+1 )
			{
				wcsncpy(pBuffer,pSlash+1,pPoint-pSlash-1);	
				pBuffer[pPoint-pSlash-1]=0;
			}
			// Dernier cas :     "DIR\.EXT" intraitable. Le chemin est renvoyé en entier

			
		}
		else
		{
			if( pPoint )
			{
				if( pPoint!=pName )
				{
					wcsncpy(pBuffer,pName,pPoint-pName);	
				}
				// Dernier cas :  ".EXT" intraitable. Le chemin est renvoyé en entier

				pBuffer[pPoint-pName]=0;
			}
		}
	}

	return pBuffer;
}

// Insère une chaine avant l'extension ".Truc"
// TOTO.EXT --> TOTO.ADDSTRING.EXT

void AddExtToFileName( char *pName,const char *pExt,unsigned int MaxLen )
{
	char *pStartName;
	char *pPoint;
	char pAfterPointBuffer[512];

	if( pName )
	{
		pStartName = strrchr(pName,'\\');

		if( !pStartName )
		{
			pStartName = pName;
		}

		pPoint = strrchr(pName,'.');

		if( pPoint>pStartName )
		{
			if( (pPoint[1]!=0) && (strlen(pPoint)>511) )
			{
				return;
			}

			if( pPoint[1]==0 )
			{
				pAfterPointBuffer[0]=0;
			}
			else
			{
				strncpy(pAfterPointBuffer,pPoint+1,511);
			}

			if( (strlen(pName) + 1 + strlen(pExt) + 1 + strlen(pAfterPointBuffer) ) < MaxLen )
			{
				pPoint[1]=0;

				strcat(pName,pExt);
				strcat(pName,".");
				strcat(pName,pAfterPointBuffer);
			}
		}
		else
		{
			if( (strlen(pName) + 1 + strlen(pExt)) < MaxLen )
			{
				if( pPoint != pStartName )
				{
					strcat(pName,".");
				}

				strcat(pName,pExt);
			}
		}
	}
}

#undef PATH_NAME_TOOLS_C