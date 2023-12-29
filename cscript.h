#ifndef _CSCRIPT_
#define _CSCRIPT_

#include "CImg.h"
#include "cvram.h"
#include "CaPlace.h"

#define AUTOP	  1
#define FONTS     2
#define PAGES     3

class CScript
{	
public:
	CScript(CProgressDlg *Pgs,CViewDx *Vdd,BOOL SaveRaw);
	~CScript(void);
	int Exec(wchar_t *Path);
	CString GetOutName(void) { return OutName; }

private:
	FILE *OpenScript(wchar_t *fileName);
	void CloseScript(void);

	// pour la récursivité de Exec()
	int InitMode,PalInit;

	int DoExec(wchar_t *Path);
	void InitExec(void);

	// pour la récursivité de AutoPos()
	CAPlace *pPlace;
	int DoAutoPos(int x,int y,int w,int h);
	int ParseCommonCommand(char *Cmd);

protected:
	void Filtre(char *Line);
	char Ligne[512], Cmd[64], TmpStr[64], TmpStr2[64], TmpStr3[64];
	wchar_t	CPath[512],CPathH[512],Name[512],NameH[512],Name2[512];
	CString OutName;
	int P2,P3,P4,P5,P6,P7,P8,P9,P10,NbParam,NoLine;
	BOOL bNormalRemap16;
	BOOL IsZoneFilter;
	BOOL IsHI;
	BOOL SaveRaw;
	int FontNbPageW;
	FILE *fp;
	CProgressDlg *SPgs;
	CVram *Vram;
	CDOut *Out;
	CViewDx *SVdd;
	int AutoPos(int x,int y,int w,int h);	
	int LoadTim(wchar_t *Path,int Mode,int t);
	int Fonts(wchar_t *Name,char *sType,int Spd,int Rot,int x,int y,int h,int First,int Sp,int SpW,int SpH,int FntNPSize);
	int FontsI(wchar_t *Name,wchar_t *NameFti,int Spd,int x,int y,int h,int Mode);
	int StrEnvVar(char *Str);
	int SetZoneFilter(int Val);
	int UnSetZoneFilter(void);
};

#endif