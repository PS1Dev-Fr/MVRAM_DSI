
/*
	Classe pour faciliter la gestion des handles (fichiers,mallocs) et des erreurs dans une fonction
	de chargement par exemple.

	Principe de fonctionnement:
	-déclarer une instance de la classe en début de la fonction
	puis faire les ouvertures de fichiers et les mallocs qui doivent être libéré en sortant
	en utilisant les fonctions de la classe.
	en cas d'érreur faire un ReleaseAll() avant de sortir.
	Un autre moyen de fonctionner de maniere encore + transparente aurait été de faire une classe
	dérivé mais cela demande plus de modifications des sources...
  
	Patrick Bricout le 24/02/98
*/

#include "stdafx.h"

#include <stdlib.h>
#include <io.h>
#include "errHandle.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CerrHandle::CerrHandle(void)
{
	int i;
	for (i=0; i<MAX_HANDLE; i++)
		handleTable[i]=-1;

	for (i=0; i<MAX_MALLOC; i++)
		addrTable[i]=NULL;
}

CerrHandle::~CerrHandle(void)
{
}

void CerrHandle::ReleaseFile(void)
{
	int i;
	for (i=0; i<MAX_HANDLE; i++)
	{
		if (handleTable[i]!=-1)
		{
			::_close(handleTable[i]);
			handleTable[i]=-1;
		}
	}
}

void CerrHandle::ReleaseMem(void)
{
	int i;
	for (i=0; i<MAX_MALLOC; i++)
	{
		if (addrTable[i])
		{
			::free(addrTable[i]);
			addrTable[i]=NULL;
		}
	}
}

void CerrHandle::ReleaseAll(void)
{
	ReleaseFile();
	ReleaseMem();
}

int CerrHandle::getHandle(void)
{
	int i;
	for (i=0; i<MAX_MALLOC; i++)
	{
		if (addrTable[i]==NULL)
			break;
	}
	return i;
}

int CerrHandle::addMallocAddr(void *addr)
{
	int h = getHandle();
	if (h<MAX_MALLOC)
	{
		addrTable[h]=addr;
	}
	return h<MAX_MALLOC;		// return FALSE si ok
}

void *CerrHandle::malloc(size_t size)
{
	int h;
	void *addr=NULL;
	h = getHandle();
	if (h<MAX_MALLOC)
		addr=addrTable[h]=::malloc(size);
	return addr;
}

void CerrHandle::free(void *addr)
{
	int i;
	for (i=0; i<MAX_MALLOC; i++)
	{
		if (addrTable[i]==addr)
		{
			::free(addr);
			addrTable[i]=NULL;
		}
	}
}

int CerrHandle::open(const wchar_t *filename,int flags)
{
	int i;
	int handle=-1;
	for (i=0; i<MAX_HANDLE; i++)
	{
		if (handleTable[i]==-1)
			break;
	}
	if (i<MAX_HANDLE)
		handle=handleTable[i]=::_wopen(filename,flags);
	return handle;
}

void CerrHandle::close(int handle)
{
	int i;
	for (i=0; i<MAX_HANDLE; i++)
	{
		if (handleTable[i]==handle)
		{
			::_close(handleTable[i]);
			handleTable[i]=-1;
		}
	}
}
