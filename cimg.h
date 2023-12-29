#ifndef _CIMG_
#define _CIMG_

#include "psx.h"
#include "multi16c.h"

#define swap_w(data) ((data>>8) | (data<< 8))
#define swap_l(data) (swap_w((unsigned short)(data>>16)) | (swap_w((unsigned short)(data & 0xffff)) << 16))
#define SWAP(a)		 {(a) = (sizeof(a)==4 ? swap_l((unsigned int)(a)) : sizeof(a)==2 ? swap_w((unsigned short)(a)) : (a)) ;}

#define SAVE_IFF 0
#define SAVE_RAW 1
#define SAVE_TIM 2


/* Prototypes de classes */

class CProgressDlg;


/* Format IFF */

#define IFF_HEADER_NAME	"FORMILBM"

typedef	struct
{
	unsigned short	w,h;
	short			x,y;
	unsigned char	nb_planes;
	unsigned char	masking;
	unsigned char	compression;
	unsigned char	pad1;
	unsigned short	transp_color;
	unsigned char	x_aspect, y_aspect;
	short			page_w,page_h;
} BMHD ;

/* Format: RAW

   offset 0     : 0x6d6877616168 "mhwanh" : entete
   offset 6     : no de version (0x0004 only supported)
   offset 8     : taille en X
   offset 10    : taille en Y
   offset 12    : nb de couleur (2-255) (0 ou -24 en true color)
   offset 14    : taille Horizontale en DPI
   offset 16    : taille Vertivale en DPI
   offset 18    : Correction Gamma * 100 (2.2 => 220)
   offset 20-31 : reserver
   offset 32    : palette
   offset ???   : image (1 octet par point ou 3 en true color)
*/

#define RAW_HEADER_NAME	"mhwanh"

typedef union
{
	struct
	{
		char			header[6];		// 0x6d6877616168 "mhwanh"
		unsigned short	version;		// no de version (0x0004 only supported)
		unsigned short	w,h;			// pic size
		unsigned short	nb_colors;		// nb de couleur (2-255) (0 ou -24 en true color)
		unsigned short	dpi_w,dpi_h;	// taille en DPI
	};
	char	align[32];
} RAW_HEADER;



typedef struct
{
	ULONG	ID;
	ULONG	Flag;
} TIM_HEADER;

typedef struct
{
	ULONG NbC;
	psxRECT rc;
}TIM_CLUT;

typedef struct
{
	ULONG Size;
	psxRECT rc;
}TIM_DATA;

// Classe RGB 24 bits
class Crgb
{
public:
	unsigned char r,g,b;

	// tous plein de constructeurs différents... cool
	Crgb(void)
	{
		r=g=b=0;
	}
	Crgb(unsigned __c)
	{
		Write(__c);
	}
	Crgb(unsigned __r,unsigned __g,unsigned __b)
	{
		r=__r,g=__g,b=__b;
	}

	unsigned Read(void)
	{
		return (r<<16)|(g<<8)|b;
	}
	void Write(unsigned __c)
	{
		r=__c>>16;
		g=__c>>8;
		b=__c;
	}
	unsigned GetLuminance(void)	// normalisé sur 1000
	{
		return r*299+g*587+b*114;
	}
};

// Classe RGB 32 bits (avec canal alpha)
class Cargb : public Crgb
{
public:
	unsigned char a;

	// tous plein de constructeurs différents... cool
	Cargb(void)
	{
		a=0;
	}
	Cargb(unsigned nColor)
	{
		Write(nColor);
	}
	Cargb(Crgb __c)
	{
		a=0;
		r=__c.r;
		g=__c.g;
		b=__c.b;
	}
	Cargb(unsigned __a,Crgb __c)
	{
		a=__a;
		r=__c.r;
		g=__c.g;
		b=__c.b;
	}
	Cargb(unsigned __r,unsigned __g,unsigned __b)
	{
		a=0;
		r=__r;
		g=__g;
		b=__b;
	}
	Cargb(unsigned __a,unsigned __r,unsigned __g,unsigned __b)
	{
		a=__a;
		r=__r;
		g=__g;
		b=__b;
	}

	unsigned Read(void)
	{
		return (a<<24)|Crgb::Read();
	}
	void Write(unsigned c)
	{
		a=c>>24;
		Crgb::Write(c);
	}
};

inline int operator == (Crgb const &a, Crgb const &b)
{
	return (a.r==b.r) && (a.g==b.g) && (a.b==b.b);
}

/* arbre binaire pour compter des couleurs en 16 millions de couleurs */
/* Attention cette classe n'est pas toute à fait clean a cause des statics */
/* elle ne peut donc être déclarée qu'une seule fois à la fois! */

class CrgbCount
{
	static int	nbItem;				// pour compter le nombre d'instance
	static Crgb	*pPaletteRGB;		// pour creer une palette RGB
	static Cargb *pPaletteARGB;		// pour creer une palette ARGB
	static unsigned char *pAlpha;	// pour creer une palette Alpha seul
	static unsigned char *pTrans;	// pour creer un tableau pour le bit de transparence sur psx
	static int	indexCount;			// pour creer les index de la palette

	Cargb		color;
	//char		pad;			// alignement de la structure
	int			luminance;		// precalcul de la luminosité de la couleur (bof...)
	int			count;			// nombre d'occurence pour cette couleur
	int			palIndex;		// l'index dans la palette pour cette couleur
	CrgbCount	*lower;
	CrgbCount	*greater;

public:
	CrgbCount(void);
	CrgbCount(int nColor);
	~CrgbCount(void);
	void SetColor(unsigned nColor);
	void SetColor(Crgb &cColor);
	void SetColor(Cargb &cColor);
	void Reset(void);
	void Init(void);
	void Init(unsigned nColor);
	void AddColor(unsigned nColor);
	void AddColorByLuminance(unsigned nColor);
	int GetNbItem(void);
	void CreatePalette(Crgb *_pPalette);
	void CreatePalette(Cargb *_pPalette);
	void CreateAlpha(unsigned char *_pAlpha);
	void CreateTrans(unsigned char *_pTrans);
	void ResetFillPalette(void);
	void FillPalette(void);
	int GetPaletteIndex(unsigned nColor);
	int GetPaletteIndex(Crgb &_color);
	int GetPaletteIndex(Cargb &_color);
};

struct pic_info_t
{
	wchar_t		  Name[512];
	wchar_t		  Path[512];
	int			  w,h;
	int			  nb_colors;
	void	      *palette;
	void		  *data;
	unsigned char *alphaChannel;	// solution bancale mais facile! (canal alpha pour l'image)
	char		  T;				// 0 if not tim 
	psxRECT		  TClut;
	psxRECT		  TData;
	UCHAR		  Trans[256];
	int			  bMulti16c;		// multi 16 colors...
	Cmulti16c	  zonesDef;		// valide si bMulti vrai

	pic_info_t(void);
	void Reset(void);
	void release(void);
	pic_info_t *crop(int x,int y,int w,int h);
	int count_colors(int _x,int _y,int _w,int _h,CrgbCount **pColorTree=NULL);
	int count_colors(CrgbCount **pColorTree=NULL);
	int RGB_to_palette(void);
	void sort_palette(void);
	void sort_palette(int firstIndex,int nbColors,int _x,int _y,int _w,int _h);
	int  RemapTo16Colors(void);
	void createTransTableWithAlpha(void);

	// Implicite inline
	int GetNbPal(void)
	{
		return bMulti16c?zonesDef.cMaps[0].nbZones:1;
	}

	int is_16m(void)
	{
		return (nb_colors==0);
	}
	size_t get_image_size(void)
	{
		size_t size=w*h;
		if (is_16m())
			size*=3;
		return size;
	}
	void *get_pixel_addr(int x,int y)
	{
		void *pPixel;
		if (is_16m())
			pPixel=(void*)((char*)data+y*w*3+x*3);
		else
			pPixel=(void*)((char*)data+y*w+x);
		return pPixel;
	}
	unsigned char *get_alpha_addr(int x,int y)
	{
		return alphaChannel?alphaChannel+y*w+x:NULL;
	}
};


class CImg
{
public:	
	pic_info_t Img;

	CImg::CImg(CProgressDlg *Pgs);
	CImg::~CImg(void);
	int Load(wchar_t*filename);
	int Save(wchar_t*filename,int Mode);
	void free_pic(pic_info_t *I=NULL);
	int load_tga(wchar_t*filename,pic_info_t *pinfo);
	int LoadTim(wchar_t*filename);
	int SaveTim(wchar_t*filename, pic_info_t *pinfo);
	int save_raw(wchar_t*filename, pic_info_t *pinfo);
	int load_psd(wchar_t*filename,pic_info_t *pinfo);

protected:
	CProgressDlg *SPgs;

	void swap_bmhd_header(BMHD *bmhd);	
	int	iff_to_bin(wchar_t*iff_name, unsigned char *palette, unsigned char **bin, BMHD *bit_map_header );
	int load_iff(wchar_t*filename, pic_info_t *pinfo);
	void swap_raw_header(RAW_HEADER *header);
	int load_raw(wchar_t*filename, pic_info_t *pinfo);
	void *load_file(wchar_t*name, size_t *ret_size);
	int load_tim(wchar_t*filename, pic_info_t *pinfo);
};


#ifdef  __cplusplus
extern "C" {
#endif

int CompareData(const void *p1,const void *p2);
int CompareIndex(const void *p1,const void *p2);

#ifdef  __cplusplus
}
#endif


#endif
