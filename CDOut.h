#ifndef _CDOUT_
#define _CDOUT_

#include "psx.h"

#define CFONT	0x53544E46	//FNTS
#define CBLK	0x534B4C42	//BLKS
#define CBLK2	0x324B4C42	//BLKS
#define CPAGE	0x45474150	//PAGE
#define CMDEC	0x4345444D	//MDEC
#define CDBIN	0x4E494244	//DBIN
#define CDPAL	0x4C415044	//DPAL
#define CSTxx	0x78785453	//STxx

#define FT_PAL		1
#define FT_INTER	2
#define FT_MOTO		3
#define FT_ALIAS	4
#define FT_0		0x00
#define	FT_90		0x40
#define FT_180		0x80
#define FT_270		0xc0

#define PALRAMRGB	0
#define PALRAMCLUT	1
#define PALVRAM		2

typedef struct
{
	int Type;
	int Nb;
}CHUNK;

typedef struct Chunk
{
	int Type;
	struct Chunk *pSuiv;
}CHUNKLIST;


typedef struct
{
	short Mode;
	short NbCol;
}DATAPALRAMS;

typedef struct
{
	short Mode;
	short NbCol;
	UCHAR Pal[256*4];
}DATAPALRAM;

typedef struct
{
	short Mode;
	short NbCol;
	USHORT Clut;
	USHORT Dummy;
}DATAPALVRAM;

typedef struct
{
	UCHAR *Data;
	int Size;
}DATABININFO;

typedef struct ODPAL
{
	int Type;
	struct ODPAL *pSuiv;
	char Name[64];
	union
	{
		short Mode;
		DATAPALRAM Ram;
		DATAPALVRAM VRam;
	}Data;
}ODPALLIST;

typedef struct ODBIN
{
	int Type;
	struct ODBIN *pSuiv;
	char Name[64];
	DATABININFO DBin;
}ODBINLIST;

typedef struct
{
	int Size;
	USHORT w,h;
}MDECINFO;

typedef struct
{
	MDECINFO Mdec;
	UCHAR *Data;
}MDECINFOD;

typedef struct OMDEC
{
	int Type;
	struct OMDEC *pSuiv;
	char Name[64];
	MDECINFOD Mdec;
}OMDECLIST;

typedef struct
{
	UCHAR L1,L2;
	short Sw;
}SWPLIST;

typedef struct
{
	UCHAR h,NbC,First,Type;
	char SpW,SpH,Sp,NbSwp;
}FNTHDRS;

typedef struct
{
	UCHAR h,NbC,First,Type;
	char SpW,SpH,Sp,Mode;
	USHORT NbSwp,dummy;
	UCHAR r1,v1,b1,D2;
	UCHAR r2,v2,b2,D3;
	USHORT Clut,TPage;
}FNTHDR;

typedef struct
{
	UCHAR x,y,w,h;
}FNTBODY;

typedef struct
{
	FNTHDR Hdr;
	SWPLIST SwpList[256];
	UCHAR *PCalcSwp;	FNTBODY Body[256];
	USHORT TPagePC[256];
}FNTCLIST;

typedef struct OFNT
{
	int Type;
	struct OFNT *pSuiv;
	char Name[64];
	BOOL IsZoneFilter;
	FNTCLIST FCList;
}OFNTLIST;

typedef struct
{
	UCHAR	u0, v0;	USHORT	clut;
	UCHAR	u1, v1;	USHORT	tpage;
	UCHAR	u2, v2, u3, v3;
}BLKPSX;
typedef struct ODPSX
{
	int Type;
	struct ODPSX *pSuiv;
	char Name[64];
	BOOL IsZoneFilter;
	BLKPSX Blk;
}ODPSXLIST;


typedef struct OPAGE
{
	int Type;
	struct OPAGE *pSuiv;
	char Name[64];
	BLKPSX Blk;
}OPAGELIST;

typedef struct OST
{
	int Type;
	struct OST *pSuiv;
	char Name[64];
	int SType;
	CString Txt;
}OSTATICTXT;

typedef struct 
{
	int SType;
	int TxtSize;
}DATAST;

typedef struct
{
	int Pos;
	int Type;
}OUTHEAD;

class CDOut
{
public:
	CDOut::CDOut(CProgressDlg *Pgs,CViewDx *Vdd);
	CDOut::~CDOut(void);
	int AddData(const char *Name,DATABININFO DBin);
	int AddData(const char *Name,MDECINFOD Mdec);
	int AddData(const char *Name,FNTCLIST Fnt,BOOL IsZoneFilter);
	int AddData(const char *Name,BLKPSX Blk);
	int AddData(const char *Name,BLKPSX Blk,BOOL IsZoneFilter);
	int AddData(const char *Name,psxRECT &rc,USHORT tpage,USHORT clut);
	int AddData(const char *Name,DATAPALVRAM DPal);
	int AddData(const char *Name,DATAPALRAM DPal);
	int AddData(const char *Name,int SType,CString Txt);
	int SaveVRM(wchar_t*Path,wchar_t *PathH);
	int SaveVRM2(wchar_t*Path,wchar_t *PathH);
	int SaveTIM(wchar_t *Path,class CVram *Vram);
	int SavePC(wchar_t*Path,wchar_t *PathH,class CVram *Vram);
	int DelAllData(void);
	int GetNbPic(void);
protected:
	CHUNKLIST *List;
	CProgressDlg *SPgs;
	CViewDx *SVdd;
};

typedef struct 
{
	UCHAR r,g,b;
}RGB24DEF;

typedef struct
{
	RGB24DEF Pix[256*256];
	UCHAR Alpha[256*256];
	int Mode;
	BOOL IsFont;
}PAGE24B;

class CPage24
{
public:
	CPage24::CPage24(CProgressDlg *Pgs,CViewDx *Vdd,int NbPage);
	CPage24::~CPage24(void);
	void AddBlk(int Page,class CVram *Vram,BLKPSX *Data,BOOL IsZoneFilter,BOOL IsFont);
	int SavePage24(wchar_t *Path);
protected:
	PAGE24B *Page;
	CProgressDlg *SPgs;
	CViewDx *SVdd;
	int NbPages;
};

#endif
