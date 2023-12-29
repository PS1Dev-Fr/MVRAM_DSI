
#include "stdafx.h"

#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>

#include "resource.h"
#include "ProgressDlg.h"
#include "cimg.h"
#include "tga.h"
#include "errHandle.h"
#include "psd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/* <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><> */

int CrgbCount::nbItem=0;
int CrgbCount::indexCount=0;
Crgb *CrgbCount::pPaletteRGB=NULL;
Cargb *CrgbCount::pPaletteARGB=NULL;
unsigned char *CrgbCount::pTrans=NULL;
unsigned char *CrgbCount::pAlpha=NULL;

CrgbCount::CrgbCount(void)
{
	Reset();
	Init();
}

CrgbCount::CrgbCount(int nColor)
{
	Init(nColor);
}

CrgbCount::~CrgbCount(void)
{
	if (lower)
		delete lower;
	if (greater)
		delete greater;
	nbItem--;
}

void CrgbCount::Reset(void)
{
	nbItem=0;
}

void CrgbCount::Init(void)
{
	count   = 0;
	lower   = NULL;
	greater = NULL;
}

void CrgbCount::Init(unsigned nColor)
{
	Init();
	SetColor(nColor);
	nbItem++;
}

void CrgbCount::SetColor(unsigned nColor)
{
	color.Write(nColor);
	luminance = color.GetLuminance();	// precalcul de la couleur
}

void CrgbCount::SetColor(Crgb &cColor)
{
	color=cColor;
	luminance = color.GetLuminance();	// precalcul de la couleur
}

void CrgbCount::SetColor(Cargb &cColor)
{
	color=cColor;
	luminance = color.GetLuminance();	// precalcul de la couleur
}

int CrgbCount::GetNbItem(void)
{
	return nbItem;
}

// No specific order

void CrgbCount::AddColor(unsigned nColor)
{
	unsigned cColor=color.Read();
	if (nColor<cColor)
	{
		if (lower)
			lower->AddColor(nColor);
		else
			lower = new CrgbCount(nColor);
	}
	else if (nColor>cColor)
	{
		if (greater)
			greater->AddColor(nColor);
		else
			greater = new CrgbCount(nColor);
	}
	else
		count++;
}

// Insert with sort by luminance

void CrgbCount::AddColorByLuminance(unsigned nColor)
{
	int cLuminance,nLuminance;
	nLuminance=Crgb(nColor).GetLuminance();
	//cLuminance=color.GetLuminance();
	cLuminance=luminance;				// optimise un poil car la luminosité est déja calculé
	if (nLuminance<cLuminance)
	{
		if (lower)
			lower->AddColorByLuminance(nColor);
		else
			lower = new CrgbCount(nColor);
	}
	else if (nLuminance>cLuminance)
	{
		if (greater)
			greater->AddColorByLuminance(nColor);
		else
			greater = new CrgbCount(nColor);
	}
	else
	{
		AddColor(nColor);
	}
}

void CrgbCount::CreatePalette(Crgb *_pPalette)
{
	ResetFillPalette();
	pPaletteRGB=_pPalette;
	FillPalette();
}

void CrgbCount::CreatePalette(Cargb *_pPalette)
{
	ResetFillPalette();
	pPaletteARGB=_pPalette;
	FillPalette();
}

void CrgbCount::CreateTrans(unsigned char *_pTrans)
{
	ResetFillPalette();
	pTrans=_pTrans;
	FillPalette();
}

void CrgbCount::CreateAlpha(unsigned char *_pAlpha)
{
	ResetFillPalette();
	pAlpha = _pAlpha;
	FillPalette();
}

void CrgbCount::ResetFillPalette(void)
{
	pPaletteARGB=NULL;
	pPaletteRGB =NULL;
	pTrans      =NULL;
	pAlpha      =NULL;
	indexCount  =0;
}

void CrgbCount::FillPalette(void)
{
	if (lower)						// si je met lower en 1er cela fait un trie automatique par ordre croissant
		lower->FillPalette();

	if (pPaletteRGB)
		*pPaletteRGB++=color;
	if (pPaletteARGB)
		*pPaletteARGB++=color;
	if (pTrans)
		*pTrans++=color.a?1:0;
	if (pAlpha)
		*pAlpha++=color.a;
	palIndex=indexCount++;

	if (greater)
		greater->FillPalette();
}

int CrgbCount::GetPaletteIndex(Crgb &_color)
{
	return GetPaletteIndex(_color.Read());
}

int CrgbCount::GetPaletteIndex(Cargb &_color)
{
	return GetPaletteIndex(_color.Read());
}

int CrgbCount::GetPaletteIndex(unsigned nColor)
{
	unsigned cColor=color.Read();
	int colorIndex;
	if (nColor<cColor)
	{
		if (lower)
			colorIndex = lower->GetPaletteIndex(nColor);
		else
			colorIndex = -1;	// Not found
	}
	else if (nColor>cColor)
	{
		if (greater)
			colorIndex = greater->GetPaletteIndex(nColor);
		else
			colorIndex = -1;	// Not found
	}
	else
		colorIndex=palIndex;
	return colorIndex;
}


/* <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><> */

/* *************************** Class pic_info_t ******************************* */

pic_info_t::pic_info_t(void)
{
	Reset();
}

void pic_info_t::Reset(void)
{
	palette=NULL;
	alphaChannel=NULL;
	data=NULL;
	bMulti16c=FALSE;
}

void pic_info_t::release(void)
{
	if (palette)
	{
		free(palette);
		palette=NULL;
	}
	if (data)
	{
		free(data);
		data=NULL;
	}
	if (alphaChannel)
	{
		free(alphaChannel);
		alphaChannel=NULL;
	}
}

void pic_info_t::createTransTableWithAlpha(void)
{
	if (alphaChannel)
	{
		for (int i=0; i<nb_colors; i++)
		{
			Trans[i]=alphaChannel[i]?1:0;
		}
	}
}

pic_info_t *pic_info_t::crop(int _x,int _y,int _w,int _h)
{
	int i,pixelSize;
	void *pImg;
	pic_info_t *pInfoImg;

	pInfoImg=new pic_info_t;
	if (pInfoImg)
	{
		// Update Class infos
		*pInfoImg=*this;
		pInfoImg->w=_w;
		pInfoImg->h=_h;
		// les infos de zonesDef ne sont pas remises a jour...
		// Crop the image
		pixelSize=(is_16m()?3:1);
		pInfoImg->data=pImg=malloc(_w*_h*pixelSize);
		if (pImg)
		{
			// recopie l'image
			for (i=_y; i<_y+_h; i++)
			{
				memcpy(pImg,get_pixel_addr(_x,i),_w*pixelSize);
				pImg=(void*)((char*)pImg+_w*pixelSize);
			}
			if (is_16m())
			{	// duplicate the palette
				pInfoImg->palette=malloc(nb_colors*3);
				if (pInfoImg->palette)
					memcpy(pInfoImg->palette,palette,nb_colors*3);
				else
				{
					goto release;
				}
			}
		}
		else
		{
		release:
			if (pInfoImg->data)
				free(pInfoImg->data);
			delete pInfoImg;
			pInfoImg=NULL;
		}
	}
	return pInfoImg;
}

/* ---------------------------------------------------------------------------- */

struct CsortData
{
	int index;
	int data;
};

int CompareData(const void *p1,const void *p2)
{
	int res=0;
	if ( ((CsortData *)p1)->data < ((CsortData *)p2)->data)
	{
		res = -1;
	}
	else if ( ((CsortData *)p1)->data > ((CsortData *)p2)->data)
	{
		res = 1;
	}
	return res;
}

int CompareIndex(const void *p1,const void *p2)
{
	int res=0;
	if ( ((CsortData *)p1)->index < ((CsortData *)p2)->index)
	{
		res = -1;
	}
	else if ( ((CsortData *)p1)->index > ((CsortData *)p2)->index)
	{
		res = 1;
	}
	return res;
}

void pic_info_t::sort_palette(void)
{
	sort_palette(0,nb_colors,0,0,w,h);
}

void pic_info_t::sort_palette(int firstIndex,int nbColors,int _x,int _y,int _w,int _h)
{
	if (!is_16m())
	{
		if (firstIndex+nbColors<=nb_colors)
		{
			int i,x,y;
			UCHAR oldAlpha[256];
			UCHAR oldTrans[256];
			Crgb oldPalette[256];
			CsortData sortLuminance[256];
			Crgb *pPal=(Crgb *)palette;

			// amélioration possible: mettre les couleurs non utilisées en fin de palette en vert fluo par exemple
			for (i=0; i<nb_colors; i++)
			{
				if (alphaChannel)
					oldAlpha[i]=alphaChannel[i];
				oldTrans[i]=Trans[i];
				oldPalette[i]=pPal[i];
				sortLuminance[i].index=i;
				sortLuminance[i].data =pPal[i].GetLuminance();	// pour trier par luminance
			}
			qsort(sortLuminance+firstIndex,nbColors,sizeof(CsortData),CompareData);
			for (i=0; i<nb_colors; i++)
			{
				sortLuminance[i].data=i;
			}
			qsort(sortLuminance+firstIndex,nbColors,sizeof(CsortData),CompareIndex);
			for (i=0; i<nb_colors; i++)
			{
				pPal[sortLuminance[i].data]=oldPalette[i];
				Trans[sortLuminance[i].data]=oldTrans[i];
				if (alphaChannel)
					alphaChannel[sortLuminance[i].data]=oldAlpha[i];
			}
			//  remap picture
			for (y=_y; y<_y+_h; y++)
			{
				unsigned char *pLine = (unsigned char *)data+w*y+_x;
				for (x=_x; x<_x+_w; x++)
				{
					*pLine=sortLuminance[*pLine].data;
					pLine++;
				}
			}
		}
	}
}

/* ---------------------------------------------------------------------------- */

int pic_info_t::count_colors(CrgbCount **pColorTree)
{
	return count_colors(0,0,w,h,pColorTree);
}

/* ---------------------------------------------------------------------------- */

// compte les couleurs et les tries par ordre de luminosité
// donc maintenant le sort_palette est inutile apres un remap avec count_colors
// (count_colors utilise la classe CrgbCount avec AddColorByLuminance)

#define _readRGB(p)				(((Crgb*)(p))->Read())
#define _readIndex(p)			(*(unsigned char*)(p))
#define _readIndexedColor(pal,p)	(((Crgb*)(pal))[*(unsigned char*)(p)].Read())
#define _readIndexedAlpha(pal,p)	(((UCHAR*)(pal))[*(unsigned char*)(p)])

int pic_info_t::count_colors(int _x,int _y,int _w,int _h,CrgbCount **pColorTree)
{
	int nbColors=0;
	if (_w && _h && nb_colors<=256)	// attention 0=16millions = gérer mais plus de 256 (ex:tim 32768) pas gerer)
	{
	#if 0
#error "pas a jour"
		int i;
		void *pImg,*pBufferImg;
		int pixelSize=(is_16m()?3:1);
		pBufferImg=pImg=malloc(_w*_h*pixelSize);
		if (pImg)
		{
			// recopie l'image (=>pourquoi est-ce que je recopiai l'image déja?)
			for (i=_y; i<_y+_h; i++)
			{
				memcpy(pImg,get_pixel_addr(_x,i),_w*pixelSize);
				pImg=(void*)((char*)pImg+_w*pixelSize);
			}
			pImg=pBufferImg;

			CrgbCount *colorCount = new CrgbCount;		// Reset instance counter
			for (i=0; i<_w*_h; i++)
			{
				int color=is_16m()?_readRGB(pImg):_readIndexedColor(palette,pImg);
				if (i==0)
					colorCount->Init(color);
				else
					colorCount->AddColor(color);
				pImg=(void*)((char*)pImg+pixelSize);
			}
			nbColors=colorCount->GetNbItem();
			free(pBufferImg);
			if (pColorTree)
				*pColorTree=colorCount;
			else
				delete colorCount;
		}
	#else
		int x,y;
		int pixelSize=(is_16m()?3:1);
		int bFirst=TRUE;
		CrgbCount *colorCount = new CrgbCount;		// Reset instance counter
		for (y=_y; y<_y+_h; y++)
		{
			void *pLine = get_pixel_addr(_x,y);
			unsigned char *pAlphaChannel= get_alpha_addr(_x,y);
			for (x=_x; x<_x+_w; x++)
			{
				int color=(is_16m()?_readRGB(pLine):_readIndexedColor(palette,pLine));
				if (alphaChannel)
					color|=(is_16m()?(*pAlphaChannel++):_readIndexedAlpha(alphaChannel,pLine))<<24;
				if (bFirst)
					colorCount->Init(color),bFirst=FALSE;
				else
					//colorCount->AddColorByLuminance(color);	// ne marche pas encore!!!
					colorCount->AddColor(color);
				pLine=(void*)((char*)pLine+pixelSize);
			}
		}
		nbColors=colorCount->GetNbItem();
		if (pColorTree)
			*pColorTree=colorCount;
		else
			delete colorCount;
	#endif
	}
	return nbColors;
}

/* ---------------------------------------------------------------------------- */

int pic_info_t::RGB_to_palette(void)
{
	int res=0;
	if (is_16m())
	{
		int nbColors;
		CrgbCount *pColorTree;

		nbColors=count_colors(&pColorTree);
		if (nbColors>0 && nbColors<=256)
		{
			//Crgb *pPalette=(Crgb *)malloc(nbColors*sizeof(Crgb));
			Crgb *pPalette=(Crgb *)malloc(256*sizeof(Crgb));		// must allocate 256 colors for fufu's comptatibilities
			if (pPalette)
			{
				int i;
				Crgb *pSrc=(Crgb *)data;
				unsigned char *pDst=nullptr,*pDstBuffer=nullptr,*pAlpha=alphaChannel,*pAlphaPalette=nullptr;

				pColorTree->CreatePalette(pPalette);
				if (alphaChannel)
				{
					pAlphaPalette=(UCHAR*)malloc(sizeof(unsigned char)*256);	// en dur 256 (je prend mes précautions!!)
					pColorTree->CreateAlpha(pAlphaPalette);
					pColorTree->CreateTrans(Trans);
				}
				pDstBuffer=pDst=(unsigned char *)malloc(w*h);
				if (pDst)
				{
					for (i=0; i<w*h; i++)
					{
						if (alphaChannel)
							*pDst++=pColorTree->GetPaletteIndex(Cargb(*pAlpha++,*pSrc++));
						else
							*pDst++=pColorTree->GetPaletteIndex(*pSrc++);
					}
					res=1;						// ok
					free(data);					// libére l'ancienne image
					data=(void*)pDstBuffer;		// et la remplace par la nouvelle
					nb_colors=nbColors;			// ajuste le nombre de couleurs
					palette=(void*)pPalette;	// place la palette (theoriquement palette doit etre NULL ici...)
					if (alphaChannel)
					{
						free(alphaChannel);
						alphaChannel=pAlphaPalette;
					}
					// normalement le sort_palette ici est inutile car count_colors trie déja les couleurs par luminosité
					sort_palette();				// et trie les couleurs par ordre croissant de luminosité
					nb_colors=256;				// must allocate 256 colors for fufu's comptatibilities
				}
				else
					free(pPalette);	// si erreur libére la palette
			}
		}
		delete pColorTree;
	}
	return res;
}

/* ---------------------------------------------------------------------------- */

/* Remapper la banque en 16 couleurs:
Principe:
 - remplir la nouvelle banque avec l'index 0 (noir quand il y en a dans la palette)
 - pour chaque zone:
   - compter les couleurs, verifier que cela soit < 16 sinon zone suivante
   - ajouter une palette dans la liste
   - trier les couleurs les couleurs suivant leurs luminosités
   - recopier la zone dans la nouvelle zone

 la routine suivante n'autorise pas des zones avec recouvrement...
*/

 /*
 code d'erreur: (8 bits de poid faible)
    0:ok
    1:bank 16 millions
	2:ce n'est pas une banque multi palette
	3:pas assez de mémoire
	4:plus de 16 couleurs dans une zone (b8-b15:nb colors,b16-b23:no de zone)
 */

int pic_info_t::RemapTo16Colors(void)
{
	int errCode=1;		// erreur:la banque doit en 256 couleurs
	if (!is_16m())
	{
		errCode=2;		// ce n'est pas une banque multi palette
		if (bMulti16c)
		{
			unsigned char *pDataBuffer=NULL;
			UCHAR *pAlphaChannelBuffer=NULL;
			Crgb *pPaletteBuffer=NULL;
			CrgbCount *pColorTree=NULL;
			try
			{
				errCode=3;
				if (pDataBuffer=(unsigned char *)malloc(w*h))	// reserver le buffer pour l'image destination
				{
					memset(pDataBuffer,0,w*h);		// clear image
					//pPaletteBuffer=(Crgb*)malloc(16*zonesDef.cMaps.nbZones*sizeof(Crgb));	// reserve la place pour les palettes
					pPaletteBuffer=(Crgb*)malloc(256*sizeof(Crgb));	// reserve la place pour les palettes
					if (pPaletteBuffer)
					{
						int x,y,z;
						if (alphaChannel)
						{
							pAlphaChannelBuffer=(UCHAR*)malloc(256*sizeof(UCHAR));
							//memset(Trans,0,sizeof(Trans));
						}
						for (z=0; z<zonesDef.cMaps[0].nbZones; z++)
						{
							int nbColors;
							CRect rect=zonesDef.GetZone(0,z);
							int _x=rect.left;
							int _y=rect.top;
							int _w=rect.Width()+1;
							int _h=rect.Height()+1;
							nbColors=count_colors(_x,_y,_w,_h,&pColorTree);		// compter les couleurs
							if (nbColors>16)									// si trop de couleur => abandonner
							{
								throw 4|(nbColors<<8)|(z<<16);
							}
							pColorTree->CreatePalette(pPaletteBuffer+16*z);		// creer la palette
							if (alphaChannel)
							{
								pColorTree->CreateTrans(Trans+16*z);
								pColorTree->CreateAlpha(pAlphaChannelBuffer+16*z);
							}
							for (y=_y; y<_y+_h; y++)			// recopier la zone
							{
								unsigned char *pSrc=(unsigned char *)get_pixel_addr(_x,y);
								unsigned char *pDst=pDataBuffer+y*w+_x;
								for (x=_x; x<_x+_w; x++)
								{
									int color=_readIndexedColor(palette,pSrc);
									if (alphaChannel)
										color|=_readIndexedAlpha(alphaChannel,pSrc)<<24;
									*pDst++=pColorTree->GetPaletteIndex(color);
									pSrc++;
								}
							}
							#if 0	// la magouille suivante foire => a debugger => inutilie maintenant les couleurs sont déja triée (16/04/98)
							{	// gruge pour trier les palettes par luminance dans une image multipalette
							#error "pas a jour car ne tient pas compte de l'alpha channel"
								void *oldPalette=palette;
								void *oldData=data;
								int oldNbColors=nb_colors;
								palette=pPaletteBuffer+16*z;		// palette utilisée pour le sort_palette()
								data=pDataBuffer;
								nb_colors=nbColors;					// si je met 16 ca bug! (idem ci-dessous)
								sort_palette(0,nbColors,_x,_y,_w,_h);		// trier la palette
								nb_colors=oldNbColors;
								palette=oldPalette;
								data   =oldData;
							}
							#endif
							delete pColorTree;
							pColorTree=NULL;
						}
					}
				}
				// met à jour les données membres
				free(palette);
				free(data);
				//nb_colors=16*zonesDef.cMaps.nbZones;
				nb_colors=256;
				palette=pPaletteBuffer;
				data=pDataBuffer;
				errCode=0;
				if (alphaChannel)
				{
					free(alphaChannel);
					alphaChannel=pAlphaChannelBuffer;
				}
			}
			catch(int code)
			{
				if (pDataBuffer)
					free(pDataBuffer);
				if (pPaletteBuffer)
					free(pPaletteBuffer);
				if (pColorTree)
					delete pColorTree;
				if (pAlphaChannelBuffer)
					free(pAlphaChannelBuffer);
				errCode=code;
			}
		}
	}
	return errCode;
}

/********************************************************************************/

CImg::CImg(CProgressDlg *Pgs)
{
	Img.palette=NULL;
	Img.data=NULL;
	SPgs=Pgs;
	Img.bMulti16c=FALSE;
}

CImg::~CImg(void)
{
}

/********************************************************************************/

void CImg::swap_bmhd_header(BMHD *bmhd)
{
	SWAP(bmhd->w);
	SWAP(bmhd->h);
	SWAP(bmhd->x);
	SWAP(bmhd->y);
	SWAP(bmhd->transp_color);
	SWAP(bmhd->page_w);
	SWAP(bmhd->page_h);
}

/********************************************************************************/
/*		CONVERSION DE FICHIER "IFF ILBM 8 PLANS" 	 		*/
/*	  (En une palette RGB et une zone d'octets (pixels) )			*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/* Parametres : - *iff_name		: Chemin et nom du fichier a convertir.	*/
/*		- *palette  		: Palette RGB.				*/
/*		- **bin	    		: Zone des donnees.			*/
/*		- *bit_map_header	: Information sur le fichier image 	*/
/*										*/
/* WARNING :		La fonction effectue une allocation memoire pour	*/
/* ---------		la zone des donnees (**bin), mais ne la libere pas.	*/
/*			" N'OUBLIEZ PAS DE LIBERER CETTE ZONE ".  		*/
/*										*/
/********************************************************************************/
/*
** Debuggee par Patrick (devrait marcher maintenant pour N plans (en tous cas
** jusqu'a 8...))
*/

int	CImg::iff_to_bin(wchar_t*iff_name, unsigned char *palette, unsigned char **bin, BMHD *bit_map_header )
{
FILE *file_handle;
unsigned char	buffer[300];

unsigned char	BMHD_flag,CMAP_flag,BODY_flag;
unsigned char	get_chunk ;
unsigned long	chunk_len;
int		repeat;
int		mem_repeat;
unsigned char	motif;
unsigned char	*ram_body=nullptr,*ptr_ram_body;		/* donnees de l'IFF copiees en memoire */
unsigned char	*line_buffer=nullptr, *ptr_line_buffer=nullptr  ;
unsigned short  bytes_per_line;
unsigned char	*ptr_bin;
int		i,y,b;
size_t		image_size;
CString s;
	if (!(file_handle = _wfopen( iff_name,L"rb")))
	{
		s.Format(L"Can't open \"%s\"\r\n",iff_name);
		SPgs->AddDbgInfo(s);
		return FALSE;
	}

	if ( fread(buffer,1,12,file_handle)!=12 )
	{
		fclose(file_handle);
		s.Format(L"_read File Error1\r\n");
		SPgs->AddDbgInfo(s);
		return FALSE;
	}
	if ( strncmp((const char *)&buffer[0], "FORM", 4)!=0 )
	{
		fclose(file_handle);
		s.Format(L"Error : No FORM Chunk\r\n");
		SPgs->AddDbgInfo(s);
		return FALSE;
	}
	if ( strncmp((const char *)&buffer[8], "ILBM", 4)!=0 )
	{
		fclose(file_handle);
		s.Format(L"Error : No ILBM Chunk\r\n");
		SPgs->AddDbgInfo(s);
		return FALSE;
	}

	BMHD_flag = FALSE ;
	CMAP_flag = FALSE ;
	BODY_flag = FALSE ;

	while ( fread(buffer,1,8,file_handle)==8 )
	{
		get_chunk = FALSE ;
		chunk_len  = buffer[4]*16777216 + buffer[5]*65536 + buffer[6]*256 + buffer[7];

		if ( strncmp((const char *) buffer, "BMHD", 4)==0 )
		{
			if ( BMHD_flag==TRUE )
			{
				fclose(file_handle);
				s.Format(L"Error : Two BMHD Chunk\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
			}
			if ( fread(bit_map_header,1,chunk_len,file_handle)!=chunk_len )
			{
				fclose(file_handle);
				s.Format(L"_read File Error2\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
			}
			swap_bmhd_header(bit_map_header);

			bytes_per_line = (bit_map_header->w+7)>>3;

			/* Patrick: taille superieur de 128 voir systeme de decompression pourri... */
			/* normalement 1 de marge aurait suffit... */
			line_buffer = (unsigned char *) malloc(bytes_per_line+129);

			BMHD_flag =  TRUE ;
			get_chunk = TRUE ;
		}

		if ( strncmp((const char *) buffer, "CMAP", 4)==0 )
		{
			if ( CMAP_flag==TRUE )
			{
				fclose(file_handle);
				s.Format(L"Error : Two CMAP Chunk\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
			}
			if ( fread(palette,1,chunk_len,file_handle)!=chunk_len )
			{
				fclose(file_handle);
				s.Format(L"_read File Error3\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
			}
			CMAP_flag =  TRUE ;
			get_chunk = TRUE ;
		}

		if ( strncmp((const char *) buffer, "BODY", 4)==0 )
		{
		   if ( BODY_flag==TRUE )
		   {
				fclose(file_handle);
				s.Format(L"Error : Two BODY Chunk\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
		   }

		   ram_body = (unsigned char *) malloc(chunk_len);
		   ptr_ram_body = ram_body;

		   fread(ram_body,1,chunk_len,file_handle);

		   /* reserve un buffer d'image en 16 bits */
		   image_size = bit_map_header->w * bit_map_header->h * sizeof(short);
		   *bin =(unsigned char *)malloc(image_size);
		   ptr_bin = *bin;
		   memset(ptr_bin,0,image_size);

		   for (y=0 ; y<bit_map_header->h ; y++ )
		   {
		   	for (b=0; b<bit_map_header->nb_planes; b++)
			{
			    ptr_line_buffer = line_buffer;
			    if (bit_map_header->compression)
			    {
					mem_repeat = 0;
					while (mem_repeat < bytes_per_line )
					{
						repeat = *(ptr_ram_body++);

						if ( repeat>128 )
						{
							repeat = 257-repeat ;
							mem_repeat += repeat ;
							motif = *(ptr_ram_body++);
							while(repeat--)
								*(ptr_line_buffer++) = motif;
						}
						else
						{
							repeat++;
							mem_repeat += repeat ;
							while (repeat--)
								*(ptr_line_buffer++) = *(ptr_ram_body++);
						}
					}
					if(mem_repeat>bytes_per_line)
					{
						//printf("mem_rep >%d %d\n",mem_repeat,bytes_per_line);
					}
				}
			    else
			    {
				for (i=0 ; i< bytes_per_line; i++)
					*(ptr_line_buffer++) = *(ptr_ram_body++);
			    }

			    ptr_bin = *bin + y * bit_map_header->w;

			    for (i=0; i<bit_map_header->w; i++)
				*ptr_bin++ |=  (((*(line_buffer+(i>>3)))>>(7-(i&7)))&1)<<b;
			}

		    }
		    BODY_flag =  TRUE ;
		    get_chunk = TRUE ;
		}

		if (get_chunk == FALSE )
		{
			if ( fread(buffer,1,chunk_len,file_handle)!=chunk_len )
			{
				fclose(file_handle);
				s.Format(L"_read File Error4\r\n");
				SPgs->AddDbgInfo(s);
				return FALSE;
			}
			if ( ( chunk_len & 1 ) == 1 )
				if ( fread(buffer,1,1,file_handle)!=1 )
				{
					fclose(file_handle);
					s.Format(L"_read File Error5\r\n");
					SPgs->AddDbgInfo(s);
					return FALSE;
				}
 		}
	}
	free(line_buffer);
	free(ram_body);
	fclose(file_handle);

	return TRUE;
}

/********************************************************************************/

int CImg::load_iff(wchar_t*filename, pic_info_t *pinfo)
{
	BMHD bmhd;
	int ok;

	ok = 0;

	if (pinfo->palette = malloc(256*3))
	{
		ok = iff_to_bin(filename,(UCHAR *)pinfo->palette,(UCHAR **)&pinfo->data,&bmhd);
		pinfo->w = bmhd.w;
		pinfo->h = bmhd.h;
		pinfo->nb_colors = 1<<bmhd.nb_planes;
		if(pinfo->nb_colors<=16)
		{
				pinfo->nb_colors=16;
		}
		if(pinfo->nb_colors>16)
		{
			pinfo->nb_colors=256;
		}

	}

	return ok;
}

/********************************************************************************/

void CImg::swap_raw_header(RAW_HEADER *header)
{
	SWAP(header->version);
	SWAP(header->w);
	SWAP(header->h);
	SWAP(header->nb_colors);
	SWAP(header->dpi_w);
	SWAP(header->dpi_h);
}

/********************************************************************************/
/* attention gestion des erreurs/des mallocs/des handles foireuse dans cette routine */

int CImg::load_raw(wchar_t*filename, pic_info_t *pinfo)
{
	FILE *file;
	RAW_HEADER	header;
	int ret;
	CString s;

	memset(pinfo,0,sizeof(pic_info_t));

	ret = FALSE;

	if (file = _wfopen(filename,L"rb"))
	{
		if (fread(&header,sizeof(RAW_HEADER),1,file)==1)						// get header
		{
			swap_raw_header(&header);
			if ((strncmp(header.header,RAW_HEADER_NAME,sizeof(RAW_HEADER_NAME)-1)==0) && (header.version==4))
			{
				int pixelSize;
				if (header.nb_colors<0)	// en 16M -1 ou 0
					header.nb_colors = 0;

				pixelSize = header.nb_colors?1:3;
				pinfo->w         = header.w;
				pinfo->h         = header.h;
				pinfo->nb_colors = header.nb_colors;

				if (pinfo->nb_colors)
				{
					// c'est du propre ca nico....
					if(pinfo->nb_colors<=16)
					{
						pinfo->nb_colors=16;
					}
					if(pinfo->nb_colors>16)
					{
						pinfo->nb_colors=256;
					}
				}

				if (header.nb_colors)
				{
					if (pinfo->palette = malloc(pinfo->nb_colors*3))
					{
						if (fread(pinfo->palette,3,header.nb_colors,file)!=(size_t)header.nb_colors)	// get palette
						{
							ret = FALSE;
							s.Format(L"Error on _read pal of raw %s\r\n",filename);
							SPgs->AddDbgInfo(s);
						}
						else
							ret=TRUE;
					}
					else
					{
						s.Format(L"Not enought memory in load_raw\r\n");
						SPgs->AddDbgInfo(s);
						ret = FALSE;
					}
				}
				else
				{
					s.Format(L"Warning:\"%s\" is 16M colors file\r\n",filename);
					SPgs->AddDbgInfo(s);
					ret=TRUE;
				}
				if (ret)
				{
					if (pinfo->data = malloc(pinfo->w*pinfo->h*pixelSize))
					{
						if (fread(pinfo->data,1,pinfo->w*pinfo->h*pixelSize,file)!=(size_t)pinfo->h*pinfo->w*pixelSize)		// get data
						{
							ret = FALSE;
							s.Format(L"Error on _read data of raw %s\r\n",filename);
							SPgs->AddDbgInfo(s);
						}
						else
							ret=TRUE;
					}
					else
					{
						s.Format(L"Not enought memory in load_raw\r\n");
						SPgs->AddDbgInfo(s);
						ret = FALSE;
					}
				}
			}
		}
		fclose(file);
	}
	return ret;
}

/********************************************************************************/

int CImg::save_raw(wchar_t*filename, pic_info_t *pinfo)
{
	FILE *file;
	RAW_HEADER	header;
	int ret;
	CString s;

	ret = FALSE;

	if (file = _wfopen(filename,L"wb"))
	{
		memset(&header,0,sizeof(RAW_HEADER));
		strncpy(header.header,RAW_HEADER_NAME,sizeof(RAW_HEADER_NAME)-1);
		header.version   = 4;
		header.w         = pinfo->w;
		header.h         = pinfo->h;
		header.nb_colors = pinfo->nb_colors;
		swap_raw_header(&header);

		if (fwrite(&header,sizeof(RAW_HEADER),1,file)==1)							// write header
		{
			if(header.nb_colors)
			{
				if (fwrite(pinfo->palette,3,pinfo->nb_colors,file)==(size_t)pinfo->nb_colors)	// write palette
				{
					if (fwrite(pinfo->data,1,pinfo->w*pinfo->h,file)==(size_t)pinfo->h*pinfo->w)			// write data
					{
						ret = TRUE;
					}
					else
					{
						s.Format(L"Error on save raw %s\r\n",filename);
						SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					s.Format(L"Error on save raw %s\r\n",filename);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			{
				if (fwrite(pinfo->data,1,pinfo->w*pinfo->h*3,file)==(size_t)pinfo->h*pinfo->w*3)			// write data
				{
					ret = TRUE;
				}
				else
				{
					s.Format(L"Error on save raw %s\r\n",filename);
					SPgs->AddDbgInfo(s);
				}

			}
		}
		fclose(file);
	}

	return ret;
}

/********************************************************************************/

int CImg::LoadTim(wchar_t*filename)
{
	FILE *file;
	TIM_HEADER header;
	TIM_CLUT clut;
	TIM_DATA data;
	CString s;
	int ret,size;
	pic_info_t pinfo;
	ret = TRUE;

	memset(pinfo.Trans,0,256);
	s.Format(L"_read file \"%s\"\r\n",filename);
	SPgs->AddDbgInfo(s);

	if (file = _wfopen(filename,L"rb"))
	{
		fread(&header,1,sizeof(TIM_HEADER),file);

		if(header.ID!=0x10)
		{
			s.Format(L"%s file is not tim\r\n",filename);
			SPgs->AddDbgInfo(s);
			return FALSE;
		}

		switch(header.Flag&CLUTCMASK)
		{
			case CLUT4:

				if(header.Flag&ISCLUT)
				{
					fread(&clut,1,sizeof(TIM_CLUT),file);
					pinfo.nb_colors=16;
					if((pinfo.palette=malloc(pinfo.nb_colors*2))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);
						return FALSE;
					}
					fread(pinfo.palette,1,clut.NbC-sizeof(TIM_CLUT),file);
					pinfo.TClut=clut.rc;
					pinfo.T=1;

					fread(&data,1,sizeof(TIM_DATA),file);

					pinfo.TData=data.rc;
					pinfo.w=data.rc.w*4;
					pinfo.h=data.rc.h;
					size=data.Size-sizeof(TIM_DATA);

					if((pinfo.data=(USHORT *)malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						free(pinfo.palette);
						fclose(file);
						return FALSE;
					}

					fread(pinfo.data,1,size,file);

					ret=TRUE;
				}
				else
				{
					s.Format(L"%s file is not tim\r\n",filename);
					SPgs->AddDbgInfo(s);
					ret = FALSE;
				}
			break;
			case CLUT8:
				if(header.Flag&ISCLUT)
				{
					fread(&clut,1,sizeof(TIM_CLUT),file);
					pinfo.nb_colors=256;
					if((pinfo.palette=malloc(pinfo.nb_colors*2))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);
						return FALSE;
					}
					fread(pinfo.palette,1,clut.NbC-sizeof(TIM_CLUT),file);
					pinfo.TClut=clut.rc;
					pinfo.T=1;

					fread(&data,1,sizeof(TIM_DATA),file);

					pinfo.TData=data.rc;
					pinfo.w=data.rc.w*2;
					pinfo.h=data.rc.h;

					size=data.Size-sizeof(TIM_DATA);

					if((pinfo.data=malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						free(pinfo.palette);
						fclose(file);
						return FALSE;
					}
					fread((UCHAR *)pinfo.data,1,size,file);
					ret=TRUE;
				}
				else
				{
					s.Format(L"%s file is not tim\r\n",filename);
					SPgs->AddDbgInfo(s);
					ret = FALSE;
				}
			break;
			case CLUT16:
				s.Format(L"%s, tim16\r\n",filename);
				SPgs->AddDbgInfo(s);			
				if(header.Flag&ISCLUT)
				{
					return FALSE;
				}
				else
				{
					pinfo.palette=NULL;
					pinfo.nb_colors=32768;
					fread(&data,1,sizeof(TIM_DATA),file);
					pinfo.T=1;
					pinfo.TData=data.rc;
					pinfo.w=data.rc.w;
					pinfo.h=data.rc.h;

					size=data.Size-sizeof(TIM_DATA);
			
					if((pinfo.data=malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);					
						return FALSE;
					}
					fread((UCHAR *)pinfo.data,1,size,file);
					ret=TRUE;
				}

			break;
			case CLUT24:
				s.Format(L"for file %s, tim24 format not supported\r\n",filename);
				SPgs->AddDbgInfo(s);
				ret = FALSE;// to do !!
				break;
				if(header.Flag&ISCLUT)
				{
					return TRUE;
				}
				else
				{
					pinfo.nb_colors=-1;
				}
			break;
		}
		fclose(file);
	}
	else
	{
		s.Format(L"Error on _read tim %s\r\n",filename);
		SPgs->AddDbgInfo(s);
	}

	Img=pinfo;
	CString s2;
	s=filename;
	s2=s.Right(s.GetLength()-s.ReverseFind('\\')-1);
	wcscpy(Img.Name,	s2.Left(s2.ReverseFind(L'.')));

	return ret;
}

/********************************************************************************/

int CImg::SaveTim(wchar_t*filename, pic_info_t *pinfo)
{
	FILE *file;
	TIM_HEADER header;
	TIM_CLUT clut;
	TIM_DATA data;

	int ret;

	ret = FALSE;


	if (file = _wfopen(filename,L"wb"))
	{
		header.ID=0x10;
		if(pinfo->nb_colors>16)
		{
			header.Flag=CLUT8|ISCLUT;
		}
		else
		{
			header.Flag=CLUT4|ISCLUT;
		}

		fwrite(&header,1,sizeof(TIM_HEADER),file);

		switch(header.Flag&CLUTCMASK)
		{
			case CLUT4:

				clut.NbC=sizeof(TIM_CLUT)+16*sizeof(USHORT);
				clut.rc=pinfo->TClut;
				fwrite(&clut,1,sizeof(TIM_CLUT),file);

				/*for(i=0;i<(int)16;i++)
				{
					PClut[i]=Trans_Pal(*((UCHAR *)pinfo->palette+i*3),*((UCHAR *)pinfo->palette+i*3+1),*((UCHAR *)pinfo->palette+i*3+2))
					PClut[i]|=(pinfo->Trans[i]<<15);
				}*/

				//fwrite(&PClut,1,clut.NbC,file);
				fwrite((USHORT *)pinfo->palette,1,clut.NbC-sizeof(TIM_CLUT),file);
				data.Size=(pinfo->TData.w*pinfo->TData.h)*2+sizeof(TIM_DATA);
				//pinfo->TData.w/=4;
				data.rc=pinfo->TData;
				fwrite(&data,1,sizeof(TIM_DATA),file);

				// no trans !!!

				fwrite((UCHAR *)pinfo->data,1,data.Size-sizeof(TIM_DATA),file);

			break;
			case CLUT8:

				clut.NbC=sizeof(TIM_CLUT)+256*sizeof(USHORT);
				clut.rc=pinfo->TClut;
				fwrite(&clut,1,sizeof(TIM_CLUT),file);

				/*for(i=0;i<(int)256;i++)
				{
					PClut[i]=Trans_Pal(*((UCHAR *)pinfo->palette+i*3),*((UCHAR *)pinfo->palette+i*3+1),*((UCHAR *)pinfo->palette+i*3+2))
					PClut[i]|=(pinfo->Trans[i]<<15);
				}*/

				//fwrite(&PClut,1,clut.NbC,file);
				fwrite((USHORT *)pinfo->palette,1,clut.NbC-sizeof(TIM_CLUT),file);
				data.Size=(pinfo->TData.w*pinfo->TData.h)*2+sizeof(TIM_DATA);
				//pinfo->TData.w/=2;
				data.rc=pinfo->TData;
				fwrite(&data,1,sizeof(TIM_DATA),file);
				fwrite((UCHAR *)pinfo->data,1,data.Size-sizeof(TIM_DATA),file);
			break;
			case CLUT16:
				return TRUE; // to do !!
			break;
			case CLUT24:
				return TRUE; //to do !!
			break;
		}


		fclose(file);
		ret=TRUE;
	}

	return ret;
}

/********************************************************************************/

int CImg::load_tim(wchar_t*filename, pic_info_t *pinfo)
{
	FILE *file;
	TIM_HEADER header;
	TIM_CLUT clut;
	TIM_DATA data;
	USHORT PClut[256],*PData;
	UCHAR *Ptr;
	CString s;
	int ret,i,size;

	ret = TRUE;

	if (file = _wfopen(filename,L"rb"))
	{
		fread(&header,1,sizeof(TIM_HEADER),file);

		if(header.ID!=0x10)
		{
			s.Format(L"%s file is not tim\r\n",filename);
			SPgs->AddDbgInfo(s);
			return FALSE;
		}

		switch(header.Flag&CLUTCMASK)
		{
			case CLUT4:

				if(header.Flag&ISCLUT)
				{
					fread(&clut,1,sizeof(TIM_CLUT),file);
					pinfo->nb_colors=16;
					if((pinfo->palette=malloc(pinfo->nb_colors*3))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);
						return FALSE;
					}
					fread(&PClut,1,clut.NbC-sizeof(TIM_CLUT),file);
					pinfo->TClut=clut.rc;
					pinfo->T=1;
					for(i=0;i<(int)pinfo->nb_colors;i++)
					{			
						*((UCHAR*)pinfo->palette+i*3+0)=CLUT_R(PClut[i]);
						*((UCHAR*)pinfo->palette+i*3+1)=CLUT_V(PClut[i]);
						*((UCHAR*)pinfo->palette+i*3+2)=CLUT_B(PClut[i]);
						pinfo->Trans[i]=CLUT_T(PClut[i]);
					}
					fread(&data,1,sizeof(TIM_DATA),file);
				
					pinfo->TData=data.rc;
					pinfo->w=data.rc.w*4;
					pinfo->h=data.rc.h;
					size=data.Size-sizeof(TIM_DATA);

					if((PData=(USHORT *)malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						free(pinfo->palette);
						fclose(file);
						return FALSE;
					}
				
					fread(PData,1,size,file);

					if((pinfo->data=malloc(size*2))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						free(PData);
						free(pinfo->palette);
						fclose(file);					
						return FALSE;
					}
				
					Ptr=(UCHAR *)pinfo->data;

					for(i=0;i<size/2;i++)
					{					
						*(Ptr++)= PData[i]&0x000f;
						*(Ptr++)=(PData[i]&0x00f0)>>4;
						*(Ptr++)=(PData[i]&0x0f00)>>8;
						*(Ptr++)=(PData[i]&0xf000)>>12;
					}

					ret=TRUE;
					free(PData);
				}
				else
				{
					s.Format(L"%s file is not tim\r\n",filename);
					SPgs->AddDbgInfo(s);			
					ret = FALSE;
				}
			break;
			case CLUT8:
				if(header.Flag&ISCLUT)
				{
					fread(&clut,1,sizeof(TIM_CLUT),file);
					pinfo->nb_colors=256;
					if((pinfo->palette=malloc(pinfo->nb_colors*3))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);
						return FALSE;
					}
					fread(&PClut,1,clut.NbC-sizeof(TIM_CLUT),file);
					pinfo->TClut=clut.rc;
					pinfo->T=1;
					for(i=0;i<(int)pinfo->nb_colors;i++)
					{			
						*((UCHAR *)pinfo->palette+i*3+0)=CLUT_R(PClut[i]);
						*((UCHAR *)pinfo->palette+i*3+1)=CLUT_V(PClut[i]);
						*((UCHAR *)pinfo->palette+i*3+2)=CLUT_B(PClut[i]);
						pinfo->Trans[i]=CLUT_T(PClut[i]);
					}
					fread(&data,1,sizeof(TIM_DATA),file);
				
					pinfo->TData=data.rc;
					pinfo->w=data.rc.w*2;
					pinfo->h=data.rc.h;

					size=data.Size-sizeof(TIM_DATA);
			
					if((pinfo->data=malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						free(pinfo->palette);
						fclose(file);					
						return FALSE;
					}
					fread((UCHAR *)pinfo->data,1,size,file);
					ret=TRUE;
				}
				else
				{
					s.Format(L"%s file is not tim\r\n",filename);
					SPgs->AddDbgInfo(s);			
					ret = FALSE;
				}
			break;
			case CLUT16:
				s.Format(L"%s, tim16\r\n",filename);
				SPgs->AddDbgInfo(s);			
				if(header.Flag&ISCLUT)
				{
					return FALSE;
				}
				else
				{
					pinfo->palette=NULL;
					pinfo->nb_colors=32768;
					fread(&data,1,sizeof(TIM_DATA),file);
					pinfo->T=1;
					pinfo->TData=data.rc;
					pinfo->w=data.rc.w;
					pinfo->h=data.rc.h;

					size=data.Size-sizeof(TIM_DATA);
			
					if((pinfo->data=malloc(size))==NULL)
					{
						s.Format(L"No memory for load tim %s\r\n",filename);
						SPgs->AddDbgInfo(s);
						fclose(file);					
						return FALSE;
					}
					fread((UCHAR *)pinfo->data,1,size,file);
					ret=TRUE;
				}

			break;
			case CLUT24:
				s.Format(L"for file %s, tim24 format not supported\r\n",filename);
				SPgs->AddDbgInfo(s);			
				ret = FALSE;// to do !!
				break;
				if(header.Flag&ISCLUT)
				{
					return TRUE;
				}
				else
				{
					pinfo->nb_colors=-1;
				}
			break;
		}
		fclose(file);
	}
	else
	{
		s.Format(L"Error on _read tim %s\r\n",filename);
		SPgs->AddDbgInfo(s);
	}


	return ret;
}

/********************************************************************************/

enum
{
	TGA_MODE_UNSUPPORTED,
	TGA_MODE_PALETTE,
	TGA_MODE_BW,
	TGA_MODE_RGB,
};

// Return 0:Ok <>0 :erreurCode

int CImg::load_tga(wchar_t*filename,pic_info_t *pinfo)
{
CString s;
int handle, res;
int palsize, datasize, pixsize;
int imgw, imgh, imgMode,nb_colors;
TARGA tgaInfo;
char *pal,*img=NULL;
unsigned char *alphaChannel=NULL;
CerrHandle h;

    if ((handle = h.open(filename, O_RDONLY|O_BINARY )) == -1 )
	{
		s.Format(L"Cannot open file %s\n", filename); // [Error]
		SPgs->AddDbgInfo(s);
		return -1;
	}

    // get header
    _read(handle, &(tgaInfo.numid), 1);
    _read(handle, &(tgaInfo.maptyp), 1);
    _read(handle, &(tgaInfo.imgtyp), 1);
    _read(handle, &(tgaInfo.maporig), 2);
    _read(handle, &(tgaInfo.mapsize), 2);
    _read(handle, &(tgaInfo.mapbits), 1);
    _read(handle, &(tgaInfo.xorig), 2);
    _read(handle, &(tgaInfo.yorig), 2);
    _read(handle, &(tgaInfo.xsize), 2);
    _read(handle, &(tgaInfo.ysize), 2);
    _read(handle, &(tgaInfo.pixsize), 1);
    _read(handle, &(tgaInfo.imgdes), 1);

	// nombre de couleurs
	nb_colors=tgaInfo.maporig+tgaInfo.mapsize;
	// mode
	switch (tgaInfo.imgtyp)
	{	// non compressé
		case 1:  imgMode = TGA_MODE_PALETTE; break;
		case 2:  imgMode = TGA_MODE_RGB;     break;
		case 3:  imgMode = TGA_MODE_BW;      break;
		// compressé
		case 9:  imgMode = TGA_MODE_PALETTE; break;
		case 10: imgMode = TGA_MODE_RGB;     break;
		case 11: imgMode = TGA_MODE_BW;      break;
		// autres
		default: imgMode = TGA_MODE_UNSUPPORTED;
	}

	// check somes parameters
	if (tgaInfo.maporig)
	{
		//s.Format("File \"%s\":Unsupported image format (Map Origin != 0)\n",filename); // [Error]
		s.Format(L"Unsupported image format (Map Origin != 0)\n"); // [Error]
		SPgs->AddDbgInfo(s);
		h.ReleaseAll();
		return -1;
	}
	if (nb_colors>256)
	{
		s.Format(L"Unsupported palettised mode (more than 256 colors(%d))\n",nb_colors); // [Error]
		SPgs->AddDbgInfo(s);
		h.ReleaseAll();
		return -1;
	}
	if (imgMode==TGA_MODE_PALETTE)			// mode palettisé non compressé/compressé
	{
		if ((tgaInfo.mapbits != 16) && (tgaInfo.mapbits != 24) && (tgaInfo.mapbits != 32))
		{
			s.Format(L"Unsupported palettised mode with %d bits/color\n",tgaInfo.mapbits); // [Error]
			SPgs->AddDbgInfo(s);
			h.ReleaseAll();
			return -1;
		}
	}
	else if (imgMode==TGA_MODE_RGB)	// mode RGB non compressé/compressé
	{
		if ((tgaInfo.pixsize != 16) && (tgaInfo.pixsize != 24) && (tgaInfo.pixsize != 32))
		{
			s.Format(L"Unsupported color mode with %d bits/pixel\n",tgaInfo.pixsize); // [Error]
			SPgs->AddDbgInfo(s);
			h.ReleaseAll();
			return -1;
		}
	}
	else if (imgMode==TGA_MODE_UNSUPPORTED)
	{
		s.Format(L"Unsupported mode (mode:%d)\n",tgaInfo.imgtyp); // [Error]
		SPgs->AddDbgInfo(s);
		h.ReleaseAll();
		return -1;
	}

	// compute some infos
	imgw = tgaInfo.xsize;
    imgh = tgaInfo.ysize;

    pixsize = tgaInfo.pixsize;
    palsize = tgaInfo.mapsize * ((tgaInfo.mapbits+7)/8) * tgaInfo.maptyp;
    datasize = imgw*imgh*((pixsize+7)/8);

	/* saute le commentaire */
	_lseek(handle,tgaInfo.numid,SEEK_CUR);

	// Charge la palette
    if (tgaInfo.maptyp != 0)
    {
		if ((pal = (char *)h.malloc(palsize)) == NULL)
		{
			h.ReleaseAll();
			return -1;
		}
		if ((res = _read(handle, pal, palsize)) < 0)
		{
			h.ReleaseAll();
			return -1;
		}
	}
    else
        pal = NULL;

	// charge l'image
	switch (tgaInfo.imgtyp)
	{
		case 1:				// palettisé non compressé
		case 2:				// RGB non compressé
		case 3:				// Noir & Blanc non compressé
			if ((img = (char *) h.malloc(datasize)) == NULL)
			{
				s.Format(L"Cannot allocate memory\n"); // [Error]
				SPgs->AddDbgInfo(s);
				h.ReleaseAll();
				return -1;
			}
			if ((res = _read(handle, img, datasize)) < 0)
			{
				h.ReleaseAll();
				return -1;
			}
			break;
		case 9:				// palettisé compressé RLE
		case 10:			// RGB en RLE
		case 11:			// Noir & Blanc en RLE
			{
				char *tmp_img;
				long fpos,csize;
				fpos  = _lseek(handle, 0, SEEK_CUR);			// get current position
				csize = _lseek(handle, 0, SEEK_END) - fpos;	// calculate len of data

				if ((tmp_img = (char *) h.malloc(csize+20)) == NULL)
				{
					s.Format(L"Cannot allocate memory L:%d\n",__LINE__); // [Error]
					SPgs->AddDbgInfo(s);
					h.ReleaseAll();
					return -1;
				}
				res = _lseek(handle, fpos, SEEK_SET);
				if ((res = _read(handle, tmp_img, csize)) < 0)
				{
					h.ReleaseAll();
					return -1;
				}
				if ((img = (char*)tga_decomp_rlc(&tgaInfo,tmp_img,csize))==NULL)
				{
					s.Format(L"Erreur lors de la decompression de l'image\n"); // [Error]
					SPgs->AddDbgInfo(s);
					h.ReleaseAll();
					return -1;
				}
				h.free(tmp_img);
				h.addMallocAddr(img);
			}
			break;
		default:
			s.Format(L"Image of type %d are not supported\n",tgaInfo.imgtyp); // [Error]
			SPgs->AddDbgInfo(s);
			h.ReleaseAll();
			return -1;
	}

	if (tgaInfo.imgdes & TGA_IMG_DESC_LEFT_RIGHT)
		mirror_image(&tgaInfo,img);
	if (!(tgaInfo.imgdes & TGA_IMG_DESC_TOP_BOTTOM))
		flip_image(&tgaInfo,img);

	if (imgMode==TGA_MODE_PALETTE)
	{
		char *newPal=NULL;
		// convertie la palette en 24 bits standard
		if (tgaInfo.mapbits == 16)
		{
			newPal = (char*)convert_tga16_to_24(tgaInfo.mapsize,pal);
		}
		else if (tgaInfo.mapbits == 24)
		{
			newPal = (char*)convert_tga24_to_24(tgaInfo.mapsize,pal);
		}
		else if (tgaInfo.mapbits == 32)
		{
			newPal = (char*)convert_tga32_to_24(tgaInfo.mapsize,pal);
			alphaChannel = (unsigned char*)tga_extract_alpha(tgaInfo.mapsize,pal);
		}
		h.free(pal);
		pal=newPal;
	}
	else if (imgMode==TGA_MODE_RGB)
	{
		char *newImg=NULL;
		// convertie l'image en 24 bits standard
		if (tgaInfo.pixsize == 16)
		{
			newImg = (char*)convert_tga16_to_24(imgw*imgh,img);
		}
		else if (tgaInfo.pixsize == 24)
		{
			newImg = (char*)convert_tga24_to_24(imgw*imgh,img);
		}
		else if (tgaInfo.pixsize == 32)
		{
			newImg = (char*)convert_tga32_to_24(imgw*imgh,img);
			alphaChannel = (unsigned char*)tga_extract_alpha(imgw*imgh,img);
		}
		h.free(img);
		img=newImg;
		if (pal)			// palette possible si couleur de bord
		{
			h.free(pal);
			pal = NULL;
		}
	}
	else if (imgMode==TGA_MODE_BW)
	{
		if (pal)			// palette possible si couleur de bord
		{
			h.free(pal);
			pal = NULL;
		}
		nb_colors=256;
		pal=(char *)create_bw_palette();
	}

	// & update pic_info

	pinfo->w            = tgaInfo.xsize;
	pinfo->h            = tgaInfo.ysize;
	pinfo->nb_colors    = nb_colors;
	pinfo->palette      = pal;
	pinfo->data         = img;
	pinfo->alphaChannel = alphaChannel;
   
	h.close(handle);
   
    return 0;
}

/********************************************************************************/

void *CImg::load_file(wchar_t*name, size_t *ret_size)
{
FILE *file;
void *buffer;
size_t size;

	buffer = NULL;

	if (file = _wfopen(name,L"rb"))
	{
		fseek(file,0,SEEK_END);
		size = ftell(file);
		rewind(file);

		if (buffer = malloc(size))
		{
			fread(buffer,1,size,file);
		}
		fclose(file);
	}

	if (ret_size)
		*ret_size = size;

	return buffer;
}

/********************************************************************************/
void LayerError(void *pParm,const char* layername)
{
	CProgressDlg *SPgs=(CProgressDlg *)pParm;
	CString s;
	s.Format(L"PSD ERROR : %s : Layer(%s)\r\n",Cpsd::s_sPsdMessageError[PSD_TOO_MUCH_COLORS_IN_A_LAYER],layername);
	SPgs->AddDbgInfo(s);			

}
/********************************************************************************/
int CImg::load_psd(wchar_t*filename,pic_info_t *pinfo)
{
	int ErrCode,i;
	unsigned char *pPixel;
	unsigned short *pPalette16;
	unsigned char *pPal;
	unsigned char *pData;
	TARGA Tga;
	int exitCode=0;
	Cpsd PsdFile;
	CString s;
	wchar_t Path[512];

	if(PsdFile.IsPsd(filename))
	{
		if((ErrCode=PsdFile.Load(filename))==PSD_OK)
		{

			ErrCode=PsdFile.ColorReducing (pPixel,pPalette16,LayerError,SPgs);

			if(ErrCode!=PSD_OK)
			{
				if(ErrCode!=PSD_TOO_MUCH_COLORS_IN_A_LAYER)
				{
					s.Format(L"PSD ERROR : %s \r\n",PsdFile.s_sPsdMessageError[ErrCode]);
					SPgs->AddDbgInfo(s);			
				}
			}
			else
			{
				pinfo->w            = PsdFile.GetWidth();
				pinfo->h            = PsdFile.GetHeight();
				pinfo->nb_colors    = 256;
				pinfo->palette      = malloc(256*3);
				pinfo->data         = malloc(pinfo->w*pinfo->h);
				pinfo->alphaChannel = (unsigned char*) malloc(256);
				pinfo->bMulti16c=TRUE;
				pinfo->zonesDef.cMaps[0].nbZones=PsdFile.GetLayerCount();
				pPal=(unsigned char*)pinfo->palette;
				pData=(unsigned char*)pinfo->data;
				for(i=0;i<256;i++)
				{
					pPal[i*3+0]=CLUT_R(pPalette16[i])<<3;
					pPal[i*3+1]=CLUT_V(pPalette16[i])<<3;
					pPal[i*3+2]=CLUT_B(pPalette16[i])<<3;
					pinfo->alphaChannel[i]=CLUT_T(pPalette16[i]);
				}

				memcpy(pinfo->data,pPixel,pinfo->w*pinfo->h);

				s=filename;
				wcscpy(Path,s.Left(s.ReverseFind(L'.')));
				wcscat(Path,L"_16C.TGA");

				Tga.numid       = 0;
				Tga.maptyp      = 1;						// 1 avec palette
				Tga.imgtyp      = 9;						// 9 compresse avec palette
				Tga.maporig     = 0;						// 0
				Tga.mapsize     = 256;						// taille pal                      
				Tga.mapbits     = 24;						// pal                             
				Tga.xorig       = 0;						// 0                               
				Tga.yorig       = 0;						// 0                               
				Tga.xsize       = pinfo->w;					//
				Tga.ysize       = pinfo->h;					//
				Tga.pixsize     = 8;						// nb bits per pixel               
				Tga.imgdes      = TGA_IMG_DESC_TOP_BOTTOM;	//                                 

				void *pTgaPal = convert_tga24_to_24(256,pPal);
				save_tga(Path,&Tga,(char*)pTgaPal,(char *)pData);
				free(pTgaPal);

				#if 0
				{
					s=filename;
					strcpy(Path,s.Left(s.ReverseFind('.')));
					strcat(Path,"_16C_d.TGA");

					char *pDataTgaZone = (char*)malloc(pinfo->w*pinfo->h);

					for(i=0;i<pinfo->w*pinfo->h;i++)
					{
						pDataTgaZone[i]=pData[i]>>4;
					}

					pTgaPal=convert_tga24_to_24(256,pPal);
					save_tga(Path,&Tga,(char*)pTgaPal,(char *)pDataTgaZone);
					free(pTgaPal);
					free(pDataTgaZone);
				}
				#endif

				for(i=0;i<pinfo->w*pinfo->h;i++)
				{
					pData[i]&=0xf; // seulement les 16 couleurs
				}



				exitCode=1;

				delete [] pPalette16;
				delete [] pPixel;
			}
		}
		else
		{
			s.Format(L"PSD ERROR : %s \r\n",PsdFile.s_sPsdMessageError[ErrCode]);
			SPgs->AddDbgInfo(s);			
		}
	}
	return exitCode;
}

int CImg::Load(wchar_t*filename)
{
	void *buffer;
	size_t size;
	int exitCode;
	CString s;
	wchar_t Path[512];
	char STmp[256];
	pic_info_t pinfo;
	FILE *fp;
	int t;


	exitCode = 0;
	memset(pinfo.Trans,0,256);
	s.Format(L"_read file \"%s\"\r\n",filename);
	SPgs->AddDbgInfo(s);			

	try
	{
		if (buffer = load_file(filename,&size))
		{
			BOOL IsPSD=FALSE;
			if (size>12)
			{
				wchar_t *pExt;
				if (pExt=wcsrchr(filename,L'.'))
				{
					// FILE FORMAT:PSD
					if (_wcsicmp(pExt,L".PSD")==0)
					{
						exitCode = load_psd(filename,&pinfo);

						// ===>
						Img=pinfo;
						CString s2;
						s=filename;
						s2=s.Right(s.GetLength()-s.ReverseFind(L'\\')-1);
						wcscpy(Img.Name,	s2.Left(s2.ReverseFind(L'.')));

						IsPSD=TRUE;

					}
					else
					// FILE FORMAT:TGA
					if (_wcsicmp(pExt,L".TGA")==0)
					{
						exitCode = !load_tga(filename,&pinfo);
						pinfo.T=0;
					}
					// FILE FORMAT:IFF
					else if ((strncmp((const char *)buffer,"FORM",4)==0) && (strncmp((char*)buffer+8,"ILBM",4)==0))
					{
						exitCode = load_iff(filename,&pinfo);
						pinfo.T=0;
					}
					// FILE FORMAT:RAW
					else if ((_wcsicmp(pExt,L".RAW")==0) && (strncmp((const char *)buffer,RAW_HEADER_NAME,sizeof(RAW_HEADER_NAME)-1)==0))
					{
						exitCode = load_raw(filename,&pinfo);
						pinfo.T=0;
					}
					// FILE FORMAT:TIM
					else if ((_wcsicmp(pExt,L".TIM")==0) && (*(int *)buffer==0x10))
					{
						exitCode=load_tim(filename,&pinfo);
					}
					// FILE FORMAT:INCONNU
					else
					{
						s.Format(L"file \"%s\":unknow picture format\r\n",filename);
						SPgs->AddDbgInfo(s);			
					}
				}
			}
			free(buffer);

			s=filename;
			wcscpy(Img.Path,s.Left(s.ReverseFind(L'\\')));

			if (exitCode && !IsPSD)	// si l'image est chargé correctement => suite ... (mais on ne fais rien dans le cas du PSD !!)
			{
				// compte le nombre de couleurs utilisées,et passe les images 16 Millions de couleurs en 256 couleurs si c'est possible
				// projet pour l'an 2000: passer en 16 bits les images de plus de 256 couleurs.
				int nbColors=pinfo.count_colors();
				if (nbColors>256)
				{
					s.Format(L"ERROR:image \"%s\" contains too many colors (%d):not processed\r\n",filename,nbColors);
					SPgs->AddDbgInfo(s);
					free_pic();
					exitCode=0;		// sortie => pas ok
				}
				else
				{
					if (pinfo.is_16m())
					{
						pinfo.RGB_to_palette();		// error not checked...
						s.Format(L"Warning:Image \"%s\" was palettised (%d colors)\r\n",filename,nbColors);
						SPgs->AddDbgInfo(s);
					}
					else
					{
						s.Format(L"Image \"%s\" contain %d colors\r\n",filename,(nbColors==0)?pinfo.nb_colors:nbColors);
						SPgs->AddDbgInfo(s);
						pinfo.createTransTableWithAlpha();
					}
					if (pinfo.alphaChannel)
					{
						s.Format(L"Image \"%s\" have an alphaChannel\r\n",filename,nbColors);
						SPgs->AddDbgInfo(s);
					}
				}
				// gros patch de merde
				if (pinfo.palette && pinfo.nb_colors<=256)		// theoriquement pas de 16M ici avec une palette
				{
					// on est obligée de faire ca pour ne pas modifier le load_tga avec un malloc de 256 en dur pour les palettes
					// => on le fait ici
					char *pal=(char*)malloc(256*sizeof(Crgb));
					memset(pal,0,256*sizeof(Crgb));
					memcpy(pal,pinfo.palette,pinfo.nb_colors*sizeof(Crgb));
					free(pinfo.palette);
					pinfo.palette=pal;
				}

				// ===>
				Img=pinfo;
				CString s2;
				s=filename;
				s2=s.Right(s.GetLength()-s.ReverseFind(L'\\')-1);
				wcscpy(Img.Name,	s2.Left(s2.ReverseFind(L'.')));

				// Regarde si un fichier .T (pour les transparences) existe et le charge le cas échéant...
				s=filename;
				wcscpy(Path,s.Left(s.ReverseFind(L'.')));
				wcscat(Path,L".T");

				fp=_wfopen(Path,L"r");
				if(fp!=NULL)
				{
					CString s;
					s.Format(L"Use trans file %s\r\n",Path);
					SPgs->AddDbgInfo(s);
					do
					{
						fgets(STmp,256,fp);
						t=atoi(STmp);
						if(t>=0||t<256)
							Img.Trans[t]=1;
					}
					while(!feof(fp));
					fclose(fp);
				}

				// Regarde si un fichier .16C existe et le charge le cas échéant...
				{
					Img.bMulti16c=FALSE;
					Img.zonesDef.SetBankSize(Img.w,Img.h);
					s=filename;
					wcscpy(Path,s.Left(s.ReverseFind('.')));
					wcscat(Path,L".16C");
					// charger les infos 16C
					if (!Img.zonesDef.load(Path))
						Img.bMulti16c=TRUE;
				}
				if (Img.bMulti16c)
				{
					s.Format(L"Image size (W:%3d H:%3d) (16c with%s bankSize(%d,%d))\r\n",Img.w,Img.h,Img.zonesDef.bSizeInfo?"":"out",Img.zonesDef.bankSize.cx,Img.zonesDef.bankSize.cy);
					SPgs->AddDbgInfo(s);
					// si image multi palette il faut convertir l'image en entiere en 16 couleurs
					int res=Img.RemapTo16Colors();
					if (res==0)
					{
						s.Format(L"Image \"%s\" successfully converted in multi palettes 16 colors\r\n",filename);
						SPgs->AddDbgInfo(s);
					}
					else
					{
						if ((res&0xff)==4)
							s.Format(L"Image \"%s\" can't be converted in multi palettes 16 colors (%d colors in the zone %d)\r\n",filename,(res>>8)&0xff,(res>>16)&0xff);
						else
							s.Format(L"Image \"%s\" can't be converted in multi palettes 16 colors\r\n",filename);
						SPgs->AddDbgInfo(s);
					}
				}
			}
		}
	}
	catch(int code)
	{
		exitCode=code;
	}

	return exitCode;
}

/********************************************************************************/

int CImg::Save(wchar_t *filename,int Mode)
{
	return 0;
}

/********************************************************************************/

void CImg::free_pic(pic_info_t *pInfo)
{
	if (pInfo)
	{
		pInfo->release();
	}
	else
	{
		Img.release();
	}	
}

