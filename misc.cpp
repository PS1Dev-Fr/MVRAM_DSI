
#include "stdafx.h"
#include "misc.h"

/* Si ext est NULL alors l'extension est enleve */

void ChangeExtension(char *string, char *ext)
{
	char *slatchPtr,*extPtr;

	slatchPtr=strrchr(string,'\\');
	extPtr=strrchr(string,'.');
	if (extPtr<slatchPtr)
		extPtr=NULL;
	if (extPtr==NULL)
		extPtr=string+strlen(string);

	if (ext)
	{
		if (*ext!='.')
			*extPtr++='.';
		strcpy(extPtr,ext);
	}
	else
		*extPtr=0;
}
