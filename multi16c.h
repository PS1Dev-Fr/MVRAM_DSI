
#ifndef __MULTI16C_H__
#define __MULTI16C_H__

class Crect
{
public:
	int left;
	int top;
	int right;
	int bottom;
};

#define MAX_PAL16	16
#define MAX_PALETTE	8

class CPalette256
{
public:
	BYTE	data[256*3];					// les palettes
	CRect	zones[MAX_PAL16];				// sous palette de 16 couleurs dans l'image
	int		nbZones;
	int		pal256;					// 1 si 1 palette 256 couleurs, 0 si 16 palettes 16 couleurs 
	//int		imgW,imgH;				// charger que sur une version récente...

	int		fread(FILE *file);
	int		fwrite(FILE *file);
	void	rescale(int oldWidth,int oldHeight,int newWidth,int newHeight);
	void	SortZones(int bankW,int bankH);
};

class Cmulti16c
{
public:
	int nbCMap;				// non utilisé
	int bankW,bankH;		// taille de la banque en mémoire
	int bSizeInfo;			// vrai si info de la taille de la bank dans le 16c => coordonnées alors automatiquement ajustées/bankW-bankH
	CSize bankSize;			// et taille originale dans bankSize
	CPalette256 cMaps[MAX_PALETTE];

	Cmulti16c(void);
	void SortZones(void);
	int load(wchar_t *fileName);
	void SetBankSize(int w,int h);
	CRect GetZone(int nMap,int nZone);
};

#endif