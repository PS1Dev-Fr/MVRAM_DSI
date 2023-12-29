#ifndef PATH_NAME_TOOLS_H
	#define PATH_NAME_TOOLS_H

	#ifdef HEADER_VERBOSE
		#pragma message("...PathNameTools.h")
	#endif

	wchar_t*GetFileNameFromPath		(const wchar_t *pPath);
	wchar_t*GetTmpNameFromPathName	(const wchar_t *pName);
	wchar_t*GetExtensionFromPathName	(const wchar_t *pName);
	wchar_t*GetTmpPathFromPathName	(const wchar_t *pName);

	void AddExtToFileName			(wchar_t *pName,const wchar_t *pExt,unsigned int MaxLen);
#endif