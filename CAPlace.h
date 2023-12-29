#ifndef _CAPLACE_
#define _CAPLACE_

#define APBLOCK  1
#define APIMG	 2

#define AMAXREC		1
#define AOPTIMUM	2
#define AHORIZONTAL 3
#define AVERTICAL	4
#define ASELONFORME 5


typedef struct LPic
{
	pic_info_t Pic;
	int Type;
	int x,y;
	BOOL IsZoneFilter;
	struct LPic *pLPic;
}LISTPIC;

class CAPlace
{
public:
	CAPlace::CAPlace(CVram *V,CDOut *O,CProgressDlg *Pgs,CViewDx *Vdd,BOOL IsHI);
	CAPlace::~CAPlace(void);
	
	int AutoPos(int x,int y,int w,int h);
	int SetAlgo(int Algo);
	int Set16x256Mode(void);
	int UnSet16x256Mode(void);
	int SetZoneFilter(int Val);
	int UnSetZoneFilter(void);
	int ImgPos(wchar_t*Name,int x,int y);
	int ImgAdd(wchar_t*Name);
	int ImgDCP(wchar_t*Name,int BoxCol);
	int EndAutoPos(void);
	void SetRemap16Mode(BOOL bRemapMode) {bNormalRemap16=bRemapMode;};

protected:
	CProgressDlg *SPgs;
	psxRECT Zone;
	LISTPIC *ListPic;
	CVram *Vram;
	CDOut *Out;
	CViewDx *SVdd;
	CVramAutoPos *VAPos;
	int Algo;
	int Is16x256;
	BOOL IsHI;
	BOOL IsZoneFilter;
	BOOL bNormalRemap16;

	LISTPIC *AddNewPic(void);
	int DelAllPic(void);
	int GetNbPic(void);
	int APGo(void);
};
#endif