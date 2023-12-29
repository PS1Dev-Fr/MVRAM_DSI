#ifndef _CVRAM_
#define _CVRAM_

#include "ViewDx.h"
#include "CDOut.h"
#include "cimg.h"

#define offset_blocs_x  10
#define offset_blocs_y  10

#define UBYTE UCHAR
#define UWORD USHORT




#define ROT_0	0
#define ROT_90	1
#define ROT_180	2
#define ROT_270	3

typedef struct
{
	wchar_t	Name[512];
	wchar_t	Path[512];
	LONG 	num;			//  Num‚ro dans la banque
	LONG 	l;				//  Largeur en pixels
	LONG 	l16;			//  Largeur original 16c en pixels
	LONG 	h;				//  Hauteur en pixels
	LONG 	ox;				//  Point chaud (x)
	LONG 	oy;				//  Point chaud (y)
	LONG 	taille;			//  Taille (largeur x hauteur)
	WORD	NCol;			//	Nombre de couleurs
	WORD	NPal;			//  Nombre de palettes pour les zones multiPalette
	UBYTE* 	data;			//  Pointe sur le graphe
	UBYTE*	pal;			//  Pointe sur la palette
	LONG 	ok;				//  Flag indiquant si il a ‚t‚ plac‚
	LONG 	page;			//  Num‚ro de la page de texture
	LONG 	xpos;			//  Position X dans la VRam
	LONG 	ypos;			//  Position Y dans la VRam
	LONG	SurfTrans;		//  Surface Transparamte
	BOOL	RotType;		//  Type de rotation
	int	NSpr;				//  Sprite ou Non Sprite
	int wr,hr;				//	Taille original
	int EquNo;
	UBYTE* 	dataO;			//  Pointe sur le graphe Original
	LONG 	tailleO;        //  Taille (largeur x hauteur)
	UCHAR	*Trans;
	BOOL IsZoneFilter;
} SPRITE;


typedef struct 
{
	ULONG	Num;
	UBYTE   VRamOccup[256*256];
	UBYTE   VRamCoul[256*256];
	UWORD   VRamNum[256*256];
	psxRECT rc;
	int		VRamStat;
}VRAMPAGE;

class CVram
{
public:
	CVram::CVram(CProgressDlg *Pgs,int x=320,int y=0,int w=1024-320,int h=512);
	CVram::~CVram(void);

	void Show(CViewDx *Vdd);
	psxRECT *GetZone(void);
	int SetZone(int x=320,int y=0,int w=1024-320,int h=512);
	int SetPalZone(int x,int y,int w,int h);
	UWORD PutPal(UCHAR *pal,UCHAR *Trans,int NbCol);
	UWORD PutPalFnt(UCHAR *pal,UCHAR *Trans,int NbCol);
	UWORD *GetPal(USHORT clut);

	int LoadPage(psxRECT *rc,UBYTE *Data,UBYTE *DataM,USHORT *DataC);
	int StorePage(psxRECT *rc,UBYTE *Data,UBYTE *DataM,USHORT *DataC);

	int LoadImage(psxRECT *rc,UBYTE *Data);
	int StoreImage(psxRECT *rc,UBYTE *Data);
	int StoreImageT(psxRECT *rc,UBYTE *Data);
	int MoveImage(psxRECT *rc,int x,int y);
	int ClearImage(psxRECT *rc,UBYTE r,UBYTE v,UBYTE b);

	int LoadImageB(psxRECT *rc,UBYTE *Data);
	int StoreImageB(psxRECT *rc,UBYTE *Data);
	int StoreImageBT(psxRECT *rc,UBYTE *Data);
	int MoveImageB(psxRECT *rc,int x,int y);

	int Load(wchar_t *filename);
	int Save(wchar_t *filename);
	int SaveRaw(wchar_t *filename);

protected:
	CProgressDlg *SPgs;
	psxRECT Zone,PalZone;
	USHORT *VRamData,*VRamDataM,*VRamDataC;
};


class CVramAutoPos
{

public:
	CVramAutoPos::CVramAutoPos(CProgressDlg *Pgs,CViewDx *Vdd);
	CVramAutoPos::~CVramAutoPos(void);

	VRAMPAGE	*Page;

	int		UsedPages;
	int		NumPage;
	int		max_page;

	void AfficheBlocVRamColor(int n);
	int SetZone(psxRECT &Zone,CVram *VRam);
	void SetSprite(SPRITE *ListSprite,int Nb,int Nb16);
	void ToVram(CVram *Vram);
	void Save(CVram *Vram,CDOut *Out,BOOL IsHI);
	void SaveFNT(CVram *Vram,CDOut *Out,int Type,int First,int Sp,int SpW,int SpH,BOOL IsHI);
	void SaveFNT(CVram *Vram,CDOut *Out,FNTCLIST &Fnt,BOOL IsHI);
	int ExecAlgoMaxRecouvre(void);
	int ExecAlgoOptimum(void);
	int ExecAlgoHorizontal(void);
	int ExecAlgoVertical(void);
	int ExecAlgoSelonForme(void);
	void	Set16x256(void){No16x256=1;}
	void	UnSet16x256(void){No16x256=0;}

protected:
	CProgressDlg *SPgs;
	CViewDx *SVdd;
	void AfficheBlocActuel(LONG n);
	void InitVramX(int Page,int x);
	void InitVramY(int Page,int y);
	void InitVramW(int Page,int x);
	void InitVramH(int Page,int y);
	void CVramAutoPos::Box(int x,int y,int w,int h);
	int No16x256;
	int NbSpr16;

	UBYTE		Scr[640*480];
	UBYTE		Pal[256*3];
	SPRITE		*Sprites;
	int			NbSprites;
	int			VRamTotal;
};


class CVramSprites
{
public:
	CVramSprites::CVramSprites(int NbSprites,CProgressDlg *Pgs);
	CVramSprites::~CVramSprites();

	SPRITE	*Sprites;
	int		NbSprites;
	int		Add(pic_info_t *Pic,int IsZoneFilter);
	void	Rot(int Type);
	int		Sort(void);
	int		UnSort(void);
	int		Remap16(void);
	int		PSX16(void);
	int		Diff(void);
	void	SetRemap16Mode(BOOL bRemapMode) {bNormalRemap16=bRemapMode;};

	int Nb16;
	BOOL bNormalRemap16;

protected:
	CProgressDlg *SPgs;
	int NbSMax;
};



#endif
