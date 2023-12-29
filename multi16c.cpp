
/*
	Gestion d'images avec plusieurs palettes de 16 couleurs par image.
*/

#include "stdafx.h"
#include "multi16c.h"

// Evolution des versions:
// 0xf003:
//   int : no de version
//   int : nb of cmap
//   sizeof(cmap)*nb of cmap
// 0xf004:
//   int : bankW \ largeur et hauteur de la bank avec laquelle a été sauver les infos
//   int : bankH /
#define CUR_PAL_VERSION		(0xF004)

int	CPalette256::fwrite(FILE *file)
{
	int res,bOk=FALSE;
	if (::fwrite(&nbZones, sizeof(nbZones),1,file)==1)
	{
		if (::fwrite(&pal256, sizeof(pal256),1,file)==1)
		{
			res=0;
			if (nbZones)
			{
				res = (int)::fwrite(zones,sizeof(CRect),nbZones,file);
			}
			if (res==nbZones)
			{
				if ((int)::fwrite(data,3,256,file)==256)		// La palette
				{
					bOk=TRUE;
				}
			}
		}
	}
	return bOk;
}

int	CPalette256::fread(FILE *file)
{
	int res,bOk=FALSE;
	if ((int)::fread(&nbZones,sizeof(nbZones),1,file)==1)
	{
		if ((int)::fread(&pal256,sizeof(pal256),1,file)==1)
		{
			res=0;
			if (nbZones)
			{
				res = (int)::fread(zones,sizeof(CRect),nbZones,file);
				for (int i=0; i<nbZones; i++)
				{
					zones[i].NormalizeRect();
				}
			}
			if (res==nbZones)
			{
				if ((int)::fread(data,3,256,file)==256)
				{
					bOk=TRUE;
				}
			}
		}
	}
	return bOk;
}

void CPalette256::rescale(int oldWidth,int oldHeight,int newWidth,int newHeight)
{
	for (int i=0; i<nbZones; i++)
	{
		zones[i].top    = (int)((double)zones[i].top*(double)newHeight/(double)oldHeight);
		zones[i].bottom = (int)((double)zones[i].bottom*(double)newHeight/(double)oldHeight);
		zones[i].left   = (int)((double)zones[i].left*(double)newWidth/(double)oldWidth);
		zones[i].right  = (int)((double)zones[i].right*(double)newWidth/(double)oldWidth);
	}
}

/* ------------------------------------------------------------------------------------------------ */

typedef struct
{
	int   key;
	CRect zone;
	BYTE  pal[16*3];
} SORT_STRUCT;

int __cdecl compareKey(const void *p1,const void *p2)
{
	int comp;
	if (((SORT_STRUCT*)p1)->key > ((SORT_STRUCT*)p2)->key)
	{
		comp=1;
	}
	else if (((SORT_STRUCT*)p1)->key < ((SORT_STRUCT*)p2)->key)
	{
		comp=-1;
	}
	else
	{
		comp=0;
	}
	return comp;
}

void CPalette256::SortZones(int bankW,int bankH)
{
	int i;
	SORT_STRUCT sortData[MAX_PAL16];

	for (i=0; i<nbZones; i++)
	{
		int x,y;
		// calcul le barycentre de la zone
		x = (zones[i].left + zones[i].right)/2;
		y = (zones[i].top  + zones[i].bottom)/2;
		// divise le barycentre par autoriser une imprécision sur les coordonnées
		// (toujours par rapport à la taille de la banque pour avoir au final des coordonnées pour une banque de 32x32)
		x = (x+bankW/64)/(bankW/32);	// (x+n/2)/n
		y = (y+bankH/64)/(bankH/32);

		sortData[i].key  = (y<<16)|(x&0xffff);			// construit la clef pour le trie
		sortData[i].zone = zones[i];					// et copie les données a trier (zone
		memcpy(sortData[i].pal,&data[16*3*i],16*3);		// et palette)
	}
	// Trie suivant les coordonnees
	qsort(sortData,nbZones,sizeof(SORT_STRUCT),compareKey);

	// recopie les données triées
	for (i=0; i<nbZones; i++)
	{
		zones[i]=sortData[i].zone;
		memcpy(&data[16*3*i],sortData[i].pal,16*3);
	}
}

/* <><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><><> */

Cmulti16c::Cmulti16c(void)
{
	SetBankSize(256,256);
	bSizeInfo=FALSE;
	bankSize.cx=0;
	bankSize.cy=0;
}

void Cmulti16c::SetBankSize(int w,int h)
{
	bankW=w;
	bankH=h;
}

CRect Cmulti16c::GetZone(int nMap,int nZone)
{
	CRect rect=cMaps[nMap].zones[nZone];
	if (!bSizeInfo)
	{
		if (bankW<=128 && bankH<=128)
		{
			rect.top>>=1;
			rect.bottom>>=1;
			rect.left>>=1;
			rect.right>>=1;
		}
	}
	return rect;
}

/* ------------------------------------------------------------------------------------------------ */

BOOL Cmulti16c::load(wchar_t *infoFile)
{
	BOOL bOk=FALSE;
	FILE *myFile = _wfopen(infoFile, L"rb");
	if (myFile)
	{
		int version = 0xF000;
		if (fread(&version,sizeof(version),1,myFile)==1)
		{
			if (version < 0xf003)
			{
				AfxMessageBox(L"La version du fichier .16C est incorrecte.\nInfos de palette perdues.");
			}
			else
			{
				if (fread(&nbCMap,sizeof(nbCMap),1,myFile)==1)
				{
					int i;
					for (i=0; i<nbCMap; i++)
					{
						if (!cMaps[i].fread(myFile))
							break;
					}
					if (i==nbCMap)
					{
						bSizeInfo=FALSE;
						if (version <0xf004)
						{
							bOk=TRUE;
						}
						else
						{
							if (fread(&bankSize,sizeof(bankSize),1,myFile)==1)
							{
								bSizeInfo=TRUE;
								if (bankSize.cx!=bankW || bankSize.cy!=bankH)
								{
								#if 0
									int rep = AfxMessageBox("Le fichier .16c ne correspond pas avec la taille\n"
										                    "de la banque actuelle voulez-vous l'adapter?\n",MB_YESNO );
									if (rep==IDYES)
								#endif		
									{
										for (int i=0; i<nbCMap; i++)
										{
											cMaps[i].rescale(bankSize.cx,bankSize.cy,bankW,bankH);
										}
									}
								}
								bOk=TRUE;
							}
						}
					}
				}
			}
		}
		fclose(myFile);
		SortZones();
	}
	return !bOk;
}

#if 0
int Cmulti16c::load(char *fileName)
{
	FILE *file;
	int errCode=1;	// Open Error

	if (file=fopen(fileName,"rb"))
	{
		int version;
		if (fread(&version,sizeof(version),1,file)==1)
		{
			if (version == CUR_PAL_VERSION)
			{
				fread(&nbCMap,sizeof(nbCMap),1,file);
				for (int i=0; i<nbCMap; i++)
				{
					cMaps[i].fread(file);
				}
				errCode=0;	// ok
			}
		}
		fclose(file);
	}
	return errCode;
}
#endif

/* --------------------------------------------------------------------------------------- */

void Cmulti16c::SortZones(void)
{
	int i;
	for (i=0; i<nbCMap; i++)
		cMaps[i].SortZones(bankW,bankH);
}

