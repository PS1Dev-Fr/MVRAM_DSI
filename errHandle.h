
#ifndef __ERRHANDLE_H__
#define __ERRHANDLE_H__

class CerrHandle
{
	#define MAX_HANDLE		32		//  une gestion dynamique aurait été + judicieuse mais ... (pas le temp)
	#define MAX_MALLOC		32

	int handleTable[MAX_HANDLE];
	void *addrTable[MAX_MALLOC];

	int getHandle(void);

public:
	CerrHandle(void);
	~CerrHandle(void);

	// sorte de surchage des fonctions C
	int  addMallocAddr(void *addr);			// a utiliser si un malloc n'est pas fait avec cette classe... 
	void *malloc(size_t size);
	void free(void *addr);
	int  open(const wchar_t *filename,int flags);
	void close(int handle);

	void ReleaseFile(void);
	void ReleaseMem(void);
	void ReleaseAll(void);
};

#endif
