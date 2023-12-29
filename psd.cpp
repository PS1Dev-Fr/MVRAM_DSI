// lecture du PSD en 32 bits

#include "stdafx.h"
#pragma warning (disable : 4786)
#include "psd.h"
#include <map>
#include <set>
#include <assert.h>

#define PSD_FLAG_VISIBLE 2

char* Cpsd::s_sPsdMessageError[] =
{
	"No Error",
	"Invalid PSD file.",
	"PSD file not found.",
	"This PSD file is not in 24 bits RGB format.",
	"PSD file: out of memory",
	"Too much colors in the picture (>256)",
	"Too much colors in a layer (>16)",
	"Too much layers (>16)"
};

void ReadInt(FILE* pF, int&nI)
{
	char c;
	char tmp[4];
	if (fread (tmp, 1 ,4, pF)!=4)
		throw PSD_INVALIDE;
	c = tmp[0];
	tmp[0]=tmp[3];
	tmp[3]=c;
	c = tmp[1];
	tmp[1]=tmp[2];
	tmp[2]=c;
	nI = *((int*)tmp);
}

void ReadShort(FILE* pF, short&sI)
{
	char c;
	char tmp[2];
	if (fread (tmp, 1, 2, pF)!=2)
		throw PSD_INVALIDE;
	c = tmp[0];
	tmp[0]=tmp[1];
	tmp[1]=c;
	sI = *((short*)tmp);
}

void ReadUChar(FILE* pF, unsigned char&cI)
{
	if (fread (&cI, 1, 1, pF)!=1)
		throw PSD_INVALIDE;
}

// constructeur
Cpsd::Cpsd()
{
	m_pMerged = NULL;
	m_pAlpha = NULL;
}

// destructeur
Cpsd::~Cpsd ()
{
	Clear();
}

int Cpsd::GetWidth() const
{
	return m_nWidth;
}

int Cpsd::GetHeight() const
{
	return m_nHeight;
}

int Cpsd::GetLayerCount() const
{
	return (int)m_vectorLayer.size();

}

psdPixel* Cpsd::GetLayer(int nLayer) const
{
	if (nLayer>=m_vectorLayer.size())
		return NULL;
	return m_vectorLayer[nLayer];
}

void Cpsd::Clear ()
{
	layerVector::iterator ite = m_vectorLayer.begin();
	while (ite != m_vectorLayer.end())
	{
		delete (*ite);
		ite++;
	}
	layerNameVector::iterator ite2 = m_vectorLayerName.begin();
	while (ite2 != m_vectorLayerName.end())
	{
		delete (*ite2);
		ite2++;
	}
	m_vectorLayer.clear();
	m_vectorLayerName.clear();
	if (m_pMerged)
		delete m_pMerged;
	if (m_pAlpha)
		delete m_pAlpha ;
}

int Cpsd::Load(const wchar_t* sFileName)
{
	char cEntete[4];
	short nVersion;
	short nDepth;
	short nMode;
	short nChannel;
	short nLayerCount;
	bool bAlphaChannel4MergedResult;
	int nLength;
	int nLayerAndMaskLength;
	int nLayer;
	int nRealWidth;
	short nDataFormat;
	int nPLineSize;
	unsigned char* pLine = NULL;
	// efface les layers deja chargés
	Clear();
	// ouvre le fichier
	FILE* pF = _wfopen(sFileName, L"rb");
	if (!pF)
		return PSD_FILE_NOT_FOUND;
	try
	{
		// lire l'entete
		if (fread (cEntete, 1, 4, pF)!=4)
			throw PSD_INVALIDE;
		// verfie l'entete
		if ((cEntete[0] != '8') ||
			(cEntete[1] != 'B') ||
			(cEntete[2] != 'P') ||
			(cEntete[3] != 'S'))
			throw PSD_INVALIDE;
		// lire la version
		ReadShort (pF, nVersion);
		if (nVersion != 1)
			throw PSD_INVALIDE;
		// skip 6 octets
		if (fseek (pF, 6, SEEK_CUR))
			throw PSD_INVALIDE;
		// lire le nombre de channel
		ReadShort (pF, nChannel);
		/*if ((nChannel!=3)&&(nChannel!=4))
			throw PSD_ONLY_24_BPP_RGB;*/
		// lire la taille de l'image
		ReadInt(pF, m_nHeight);
		ReadInt(pF, m_nWidth);
		// largeur d'une ligne du fichier
		nRealWidth = m_nWidth+((m_nWidth&1)?1:0);
		// lire le nombre de bits par channel
		ReadShort(pF, nDepth);
		if (nDepth != 8)
			throw PSD_ONLY_24_BPP_RGB;
		// lire le mode de couleurs
		ReadShort(pF, nMode);
		// seulement RGB
		if (nMode != 3)
			throw PSD_ONLY_24_BPP_RGB;
		// lire le length field de la prochaine section qui devrait être à zéro...
		ReadInt(pF, nLength);
		if (nLength)
			throw PSD_INVALIDE;
		// lire la taille des images resources...
		ReadInt(pF, nLength);
		// on les zap
		if (fseek(pF, nLength, SEEK_CUR))
			throw PSD_INVALIDE;
		// lire la taille de la section layer and mask
		ReadInt(pF, nLayerAndMaskLength);
		// allouer une ligne...
		pLine = new unsigned char[nRealWidth];
		nPLineSize = nRealWidth;
		// lire les layres et les masks...
		if (nLayerAndMaskLength)
		{
			int nSizeLayerSection;
			// taille des infos de layer...
			//if (fseek(pF, 4, SEEK_CUR))
			ReadInt (pF, nSizeLayerSection);
			// lire le nombre de layer...
			ReadShort(pF, nLayerCount);
			nSizeLayerSection-=2;
			// attention au premier channel alpha...
			if (nLayerCount<0)
			{
				nLayerCount = -nLayerCount;
				bAlphaChannel4MergedResult = true;
			}
			else
				bAlphaChannel4MergedResult = false;
			// liste les layers
			typedef std::vector<int> vectInt;
			typedef std::vector<vectInt> vectVectInt;
			typedef std::vector<char*> vectChar;
			vectVectInt listLayer;
			vectChar listNameLayer;
			// lire les infos de tous les layers...
			for (nLayer=0; nLayer<nLayerCount; nLayer++)
			{
				int nTop, nLeft, nBottom, nRight;
				short nRed = -1;
				short nGreen = -1;
				short nBlue = -1;
				short nAlpha = -1;
				short nTrans = -1;
				short nChannelCount;
				unsigned char cOpacity;
				unsigned char cFlag;
				// lire le rect de ce layer
				ReadInt (pF, nTop);
				ReadInt (pF, nLeft);
				ReadInt (pF, nBottom);
				ReadInt (pF, nRight);
				nSizeLayerSection-=16;
				// lire le nombre de channel dans ce layer...
				ReadShort (pF, nChannelCount);
				nSizeLayerSection-=2;
				// lire le channel information pour tous les channels...
				for (int nChannelIte=0; nChannelIte<nChannelCount; nChannelIte++)
				{
					short nChannelType;
					int nChannelDataLength;
					ReadShort(pF, nChannelType);
					ReadInt(pF, nChannelDataLength);
					nSizeLayerSection-=6;
					switch (nChannelType)
					{
					case 0:
						// RED
						nRed=nChannelIte;
						break;
					case 1:
						// GREEN
						nGreen = nChannelIte;
						break;
					case 2:
						// BLUE
						nBlue = nChannelIte;
						break;
					case -1:
						// TRANSPARENCE
						nTrans = nChannelIte;
						break;
					}
				}
				// lire un block
				char cBlend[4];
				if (fread (cBlend, 1, 4, pF)!=4)
					throw PSD_INVALIDE;
				nSizeLayerSection-=4;
				// verfie le block
				if ((cBlend[0] != '8') ||
					(cBlend[1] != 'B') ||
					(cBlend[2] != 'I') ||
					(cBlend[3] != 'M'))
					throw PSD_INVALIDE;
				// zap 4 octets
				if (fseek(pF, 4, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=4;
				// lire l'opacité
				ReadUChar(pF, cOpacity);
				nSizeLayerSection-=1;
				// zap 1 octet
				if (fseek(pF, 1, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=1;
				// lire les flags pour ce layer
				if (fread(&cFlag, 1, 1, pF)!=1)
					throw PSD_INVALIDE;
				nSizeLayerSection-=1;
				// zap 1 octet
				if (fseek(pF, 1, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=1;
				// lire la taille de extra data field...
				int nExtraField;
				ReadInt (pF, nExtraField);
				nSizeLayerSection-=4;
				/*// zap cette zone
				if (fseek(pF, nLength, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=nLength;*/
				// lire la taille du tableau de mask
				ReadInt (pF, nLength);
				nSizeLayerSection-=4;
				nExtraField-=4;
				// zap cette table
				if (fseek(pF, nLength, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=nLength;
				nExtraField-=nLength;
				// lire la taille du layer blending ranges data
				ReadInt (pF, nLength);
				nSizeLayerSection-=4;
				nExtraField-=4;
				// zap cette table
				if (fseek(pF, nLength, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=nLength;
				nExtraField-=nLength;
				// on va pouvoir lire le nom du layer...
				char nNameSize;
				// taille de la chaine
				nNameSize=fgetc(pF);
				nSizeLayerSection--;
				nExtraField--;
				int nTmp=nNameSize;
				char *cName;
				if (nNameSize==0)
				{
					cName=new char[strlen("Fond")+1];
					strcpy(cName, "Fond");
				}
				else
				{
					cName = new char[nNameSize+1];
					char* pDst = cName;
					while (nNameSize-->0)
					{
						*(pDst++)=getc(pF);
						nSizeLayerSection--;
						nExtraField--;
					}
					*(pDst++)=0;
				}
				nTmp = (4-((nNameSize+1)&3))&3;
				while (nTmp-->0)
				{
					fgetc(pF);
					nSizeLayerSection--;
					nExtraField--;
				}
				assert(nExtraField>=0);
				// on zap le reste de ce field...
				if (fseek(pF, nExtraField, SEEK_CUR))
					throw PSD_INVALIDE;
				nSizeLayerSection-=nExtraField;
				// on ajoute à la liste de layer
				vectInt listInt;
				listInt.push_back(nRed);
				listInt.push_back(nGreen);
				listInt.push_back(nBlue);
				listInt.push_back(nAlpha);
				listInt.push_back(nTrans);
				listInt.push_back(nTop);
				listInt.push_back(nBottom);
				listInt.push_back(nLeft);
				listInt.push_back(nRight);
				listInt.push_back(nChannelCount);
				listInt.push_back((int)cFlag);
				listLayer.push_back(listInt);
				listNameLayer.push_back(cName);
			}	
			// lire le format du layer
			for (nLayer=0; nLayer<nLayerCount; nLayer++)
			{
				int nTop, nLeft, nBottom, nRight;
				short nRed = -1;
				short nGreen = -1;
				short nBlue = -1;
				short nAlpha = -1;
				short nTrans = -1;
				short nChannelCount;
				int nClipLeft = 0;
				int nClipTop = 0;
				int nClipRight = 0;
				int nClipBottom = 0;
				int nFlag = 0;
				// liste le layer...
				assert (nLayer<listLayer.size());
				assert (listLayer[nLayer].size()==11);
				nRed = listLayer[nLayer][0];
				nGreen = listLayer[nLayer][1];
				nBlue = listLayer[nLayer][2];
				nAlpha = listLayer[nLayer][3];
				nTrans = listLayer[nLayer][4];
				nTop = listLayer[nLayer][5];
				nBottom = listLayer[nLayer][6];
				nLeft = listLayer[nLayer][7];
				nRight = listLayer[nLayer][8];
				nChannelCount = listLayer[nLayer][9];
				nFlag = listLayer[nLayer][10];
				// clipping ?
				if (nLeft<0)
					nClipLeft = -nLeft;
				if (nTop<0)
					nClipTop = -nTop;
				if (nRight>m_nWidth)
					nClipRight = nRight-m_nWidth;
				if (nBottom>m_nHeight)
					nClipBottom = nBottom-m_nHeight;
				// layer valide?
				assert ((nRed!=-1)&&(nGreen!=-1)&&(nBlue!=-1));
				{
					bool bTrans = false;
					psdPixel* pBitmap=nullptr;
					int nLayerHeight;
					int nLayerWidth;
					int nLayerRealWidth;
					int nMaxWidth;
					if ((nFlag&PSD_FLAG_VISIBLE)==0)
					{
						pBitmap = new psdPixel[m_nWidth*m_nHeight];
						if (!pBitmap)
							throw PSD_OUT_OF_MEMORY;
						if (nTrans==-1)
							// efface le layer avec du noir si il n'y a pas de masque de transparence
							for (int nI=0; nI<m_nWidth*m_nHeight; nI++)
								pBitmap[nI] = PSD_BLACK;
						else
							// efface le layer avec du transparent si il y a un masque de transparence
							for (int nI=0; nI<m_nWidth*m_nHeight; nI++)
								pBitmap[nI] = PSD_TRANSPARENT;
								//pBitmap[nI] = PSD_BLACK;
						// ajoute un layer
						m_vectorLayer.push_back(pBitmap);
						m_vectorLayerName.push_back(listNameLayer[nLayer]);
					}
					else
						delete listNameLayer[nLayer];
					nLayerHeight = nBottom-nTop;
					nLayerWidth = nRight-nLeft;
					nLayerRealWidth = nLayerWidth;
					//nLayerRealWidth = nLayerWidth+((nLayerWidth&1)?1:0);
					nMaxWidth = nLayerWidth-nClipRight;
					// pLine assez grand?
					if (nPLineSize < nLayerRealWidth)
					{
						delete pLine;
						pLine = new unsigned char[nLayerRealWidth];
						nPLineSize = nLayerRealWidth;
					}
					// layer valide...
					for (int nChannelIte=0; nChannelIte<nChannelCount; nChannelIte++)
					{
						// clipping top
						int nZapTop = nClipTop;
						// clipping bottom
						int nDrawHeight = nLayerHeight-nClipTop-nClipBottom;
						// compresse?
						ReadShort(pF, nDataFormat);
						nSizeLayerSection-=2;
						if (nDataFormat==1)
						{
							// lire la taille du scan line
							//ReadShort (pF, nLayerFileWidth);
							// skip les tailles de scanline
							if(fseek(pF, 2*nLayerHeight, SEEK_CUR))
								throw PSD_INVALIDE;
							nSizeLayerSection-=2*nLayerHeight;
						}
						psdPixel* pPixel;
						int nShift;
						psdPixel nMask;
						// pointeur sur la layer...
						pPixel = pBitmap+(nLeft+nClipRight)+(nTop+nClipTop)*m_nWidth;
						// quel channel?
						nShift=32;
						if (nRed == nChannelIte)
						{
							bTrans = false;
							nShift=16;
							nMask = 0x0000ffff;
						}
						if (nGreen == nChannelIte)
						{
							bTrans = false;
							nShift=8;
							nMask = 0x00ff00ff;
						}
						if (nBlue == nChannelIte)
						{
							bTrans = false;
							nShift=0;
							nMask = 0x00ffff00;
						}
						/*if (nAlpha == nChannelIte)
						{
							bTrans = false;
							nShift=24;
							nMask = 0x00ffffff;
						}*/
						if (nTrans == nChannelIte)
						{
							bTrans = true;
							//nShift=32;
							nShift=24;
						}
						if ((nFlag&PSD_FLAG_VISIBLE))
							nShift=32;
						//assert (nShift!=32);
						for (int nLine=0; nLine<nLayerHeight; nLine++)
						{
							// nombre d'octets lus...
							int nByteReaded = 0;
							// compresse ou non?
							if (nDataFormat==0)
							{
								// RAW data...
								// lire une ligne
								if (fread(pLine, 1, nLayerRealWidth, pF)!=nLayerRealWidth)
									throw PSD_INVALIDE;
								nSizeLayerSection-=nLayerRealWidth;
								nByteReaded+=nLayerRealWidth;
							}
							else
							{
								if (nDataFormat==1)
								{
									// RLE data...
									int nReaded;
									signed char cFirst;
									int nToWrite;
									unsigned char* pLinePtr = pLine;
									// lire une ligne
									nToWrite = nLayerRealWidth;
									while (nToWrite>0)
									{
										nReaded = fgetc(pF);
										nSizeLayerSection--;
										nByteReaded++;
										if (nReaded == EOF)
											throw PSD_INVALIDE;
										cFirst = nReaded;
										if (cFirst&0x80)
										{
											if (cFirst!=0x80)
											{
												int nCountToDo = -cFirst+1;
												nReaded = fgetc(pF);
												nSizeLayerSection--;
												nByteReaded++;
												if (nReaded == EOF)
													throw PSD_INVALIDE;
												unsigned char cR = nReaded;
												for (int nCount=0; nCount<nCountToDo; nCount++)
													*(pLinePtr++) = cR;
												nToWrite-=nCountToDo;
											}
										}
										else
										{
											int nToDo=cFirst+1;
											if (fread(pLinePtr, 1, nToDo, pF)!=nToDo)
												throw PSD_INVALIDE;
											nSizeLayerSection-=nToDo;
											nByteReaded+=nToDo;
											pLinePtr+=nToDo;
											nToWrite-=nToDo;
										}
									}
									if(nToWrite!=0)
										throw PSD_INVALIDE;
									assert (pLine+nLayerWidth == pLinePtr);
								}
								else
									throw PSD_INVALIDE;
							}
							// pad pixel?
							/*if (nByteReaded&1)
								fgetc(pF);*/
							// channel valide?
							if (nShift!=32)
							{
								// nombre de ligne à sauter pour le clipping top
								if (nZapTop<=0)
								{
									// nombre de lignes dont ont doit tenir compte
									if (nDrawHeight>0)
									{
										// channel transparency mask?
										if (bTrans)
										{
											for (int nX=nClipLeft; nX<nMaxWidth; nX++)
											{
												if (pLine[nX] != 0)
													pPixel[nX] = PSD_BLACK;
												else
													pPixel[nX] = PSD_TRANSPARENT;
											}
										}
										else
										{
											// channel RGBA
											for (int nX=nClipLeft; nX<nMaxWidth; nX++)
											{
												if (pPixel[nX]!=PSD_TRANSPARENT)
												{
													pPixel[nX] &= nMask;
													pPixel[nX] |= ((psdPixel)(pLine[nX]))<<nShift;
													if (pPixel[nX]==PSD_TRANSPARENT)
														pPixel[nX] = 0x02000000;
												}
											}
										}
										nDrawHeight--;
									}
									pPixel+=m_nWidth;
								}
								else
									nZapTop--;
							}
						}
					}
				}
			}
			// bit de fin...
			if (nSizeLayerSection)
			{
				assert (nSizeLayerSection==1);
				fgetc(pF);
			}
			// lire la taille des masks
			if (fread(&nLength, 1, 4, pF)!=4)
				throw PSD_INVALIDE;
			// skip les masks...
			if (fseek(pF, nLength, SEEK_CUR))
				throw PSD_INVALIDE;
		}
		// si pas de layer, lire l'image
		// lire l'image...
		// compression ou pas de compression?
		int nShift = 16;
		psdPixel* pBitmap=nullptr;
		psdPixel* pPixel;
		// lire le format de l'image
		ReadShort(pF, nDataFormat);
		// skiper la tailles des scanlines
		if (nDataFormat==1)
		{
			if (fseek(pF, 2*m_nHeight*nChannel, SEEK_CUR))
				throw PSD_INVALIDE;
		}
		if (!nLayerAndMaskLength)
		{
			// allouer de la memoire pour le bitmap
			pBitmap = new psdPixel[m_nWidth*m_nHeight];
			if (!pBitmap)
			throw PSD_OUT_OF_MEMORY;
			// efface le layer
			memset(pBitmap, PSD_BLACK, m_nWidth*m_nHeight*sizeof(psdPixel));
			// ajoute un layer
			m_vectorLayer.push_back(pBitmap);
			char *pString = new char[strlen("Fond")+1];
			strcpy (pString, "Fond");
			m_vectorLayerName.push_back(pString);
		}
		else
		{
			if (((nChannel>=4)&&(!bAlphaChannel4MergedResult))||((nChannel>=5)&&bAlphaChannel4MergedResult))
			{
				// allouer de la memoire pour le channel alpha
				pBitmap = new psdPixel[m_nWidth*m_nHeight];
				if (!pBitmap)
				throw PSD_OUT_OF_MEMORY;
				// efface le layer
				memset(pBitmap, PSD_BLACK, m_nWidth*m_nHeight*sizeof(psdPixel));
				// ajoute un cannal alpha
				m_pAlpha = pBitmap;
			}	
		}
		// pointeur sur l'image
		pPixel = pBitmap;
		// lire l'image
		for (int nChannelRGB=0; nChannelRGB<nChannel; nChannelRGB++)
		{
			if (!bAlphaChannel4MergedResult)
			{
				switch (nChannelRGB)
				{
				case 0:
					nShift = 16;
					break;
				case 1:
					nShift = 8;
					break;
				case 2:
					nShift = 0;
					break;
				case 3:
					nShift = 24;
					break;
				default:
					nShift = 32;
					break;
				}
			}
			else
			{
				switch (nChannelRGB)
				{
				case 0:
					nShift = 32;
					break;
				case 1:
					nShift = 16;
					break;
				case 2:
					nShift = 8;
					break;
				case 3:
					nShift = 0;
					break;
				case 4:
					nShift = 24;
					break;
				default:
					nShift = 32;
					break;
				}
			}
			psdPixel* pPixel;
			// pointeur sur la layer...
			pPixel = pBitmap;
			for (int nLine=0; nLine<m_nHeight; nLine++)
			{
				// compresse ou non?
				if (nDataFormat==0)
				{
					// RAW data...
					// lire une ligne
					if (fread(pLine, 1, nRealWidth, pF)!=nRealWidth)
						throw PSD_INVALIDE;
				}
				else
				{
					if (nDataFormat==1)
					{
						// RLE data...
						int nReaded;
						int nToRead;
						signed char cFirst;
						unsigned char* pLinePtr = pLine;
						// lire une ligne
						nToRead = m_nWidth;
						while (nToRead>0)
						{
							nReaded = fgetc(pF);
							if (nReaded == EOF)
								throw PSD_INVALIDE;
							cFirst = nReaded;
							if (cFirst&0x80)
							{
								if (cFirst!=0x80)
								{
									int nCountToDo = -cFirst+1;
									nReaded = fgetc(pF);
									if (nReaded == EOF)
										throw PSD_INVALIDE;
									unsigned char cR = nReaded;
									for (int nCount=0; nCount<nCountToDo; nCount++)
										*(pLinePtr++) = cR;
									nToRead -= nCountToDo;
								}
							}
							else
							{
								int nToDo=cFirst+1;
								if (fread(pLinePtr, 1, nToDo, pF)!=nToDo)
									throw PSD_INVALIDE;
								pLinePtr+=nToDo;
								nToRead -= nToDo;
							}
						}
						if(nToRead!=0)
							throw PSD_INVALIDE;
						// pad char
						if(m_nWidth&1)
							if(fgetc(pF) == EOF)
								throw PSD_INVALIDE;
					}
					else
						throw PSD_INVALIDE;
				}
				// pointeur sur la ligne
				psdPixel* pLineDstPtr = pPixel;
				unsigned char* pLineSrcPtr = pLine;
				if (!nLayerAndMaskLength)
				{
					for (int nX=0; nX<m_nWidth; nX++)
					{
						*pLineDstPtr |= ((psdPixel)(*pLineSrcPtr))<<nShift;
						pLineDstPtr++;
						pLineSrcPtr++;
					}
				}
				else
				{
					if (nShift == 24)
					for (int nX=0; nX<m_nWidth; nX++)
					{
						*pLineDstPtr = ((psdPixel)(*pLineSrcPtr))<<nShift;
						pLineDstPtr++;
						pLineSrcPtr++;
					}
				}
				pPixel+=m_nWidth;
			}
		}
	}
	catch(int nMsg)
	{
		Clear();
		fclose (pF);
		if (pLine)
			delete pLine;
		return nMsg;
	}
	// plave l'alpha dans les layers
	if (m_pAlpha)
	{
		layerVector::iterator iteLayer = m_vectorLayer.begin();
		while (iteLayer != m_vectorLayer.end())
		{
			// poitneur sur la source
			psdPixel *pSrc = *iteLayer;
			// nombre de pixels
			int nPixelCount = m_nWidth*m_nHeight;
			for (int nPixel=0; nPixel<nPixelCount; nPixel++)
			{
				if (*pSrc!=PSD_TRANSPARENT)
				{
					// y'a de l'alpha?
					*pSrc &= 0x00ffffff;
					*pSrc |= m_pAlpha[nPixel];
				}
				pSrc++;
			}
			iteLayer++;
		}
		delete m_pAlpha;
		m_pAlpha = NULL;
	}
	fclose (pF);
	if (pLine)
		delete pLine;
	return PSD_OK;
}

psdPixel* Cpsd::GetMerged()
{
	if (m_pMerged)
		return m_pMerged;
	m_pMerged = new psdPixel[m_nWidth*m_nHeight];
	// tout noir
	memset (m_pMerged, PSD_BLACK, m_nWidth*m_nHeight*sizeof(psdPixel));
	// pas de layer?
	if (m_vectorLayer.size()==0)
		return m_pMerged;
	// remplir les layers un à un...
	layerVector::iterator iteLayer = m_vectorLayer.begin();
	while (iteLayer != m_vectorLayer.end())
	{
		// pointeur sur la destination
		psdPixel *pDst = m_pMerged;
		// poitneur sur la source
		psdPixel *pSrc = *iteLayer;
		// nombre de pixels
		int nPixelCount = m_nWidth*m_nHeight;
		for (int nPixel=0; nPixel<nPixelCount; nPixel++)
		{
			if (*pSrc!=PSD_TRANSPARENT)
				*pDst = *pSrc;
			pDst++;
			pSrc++;
		}
		iteLayer++;
	}
	return m_pMerged;
}

#define Trans_Pal(r,v,b)			(((r)>>3)+(((v)>>3)<<5)+(((b)>>3)<<10)/*+(1<<15)*/)

int Cpsd::ColorReducing (unsigned char*& pPixel, unsigned short*& pPalette, void(*funError)(void *pParam,const char* layername),void *pParam)
{
	// quelques types...
	typedef std::map<psdPixel, int , std::less<psdPixel> > MapPixel;
	typedef std::set<psdPixel> SetPixel;

	// verifie qu'il y'a moins de 16 layers...
	if (GetLayerCount()>16)
		return PSD_TOO_MUCH_LAYER;
	// quelques variables...
	int nPixelCount;
	layerVector::iterator iteLayer;
	// nombre de points dans ce bitmap
	nPixelCount = m_nWidth*m_nHeight;
	// vérifie le nombre de couleurs dans chaque layer.
	iteLayer = m_vectorLayer.begin();
	// numero de layer
	int nLayerCount = 0;
	// color count
	int nColorCount = 0;
	// alloc memoire...
	// image
	pPixel = new unsigned char[m_nWidth*m_nHeight];
	if (!pPixel)
		return PSD_OUT_OF_MEMORY;
	memset (pPixel, 0, m_nWidth*m_nHeight);
	// palette
	pPalette = new unsigned short[256];
	if (!pPalette)
	{
		// efface Reduced
		delete pPixel;
		// NULL
		pPixel = NULL;
		return PSD_OUT_OF_MEMORY;
	}
	BOOL bOk = TRUE;
	// pour tous les layers
	while (iteLayer != m_vectorLayer.end())
	{
		int nColorCount2=nColorCount;
		MapPixel setPixel;
		int nPixelToScan = nPixelCount;
		// pointeur sur le calque
		psdPixel* pPixelLayer = *iteLayer;
		unsigned char* pDst = pPixel;
		// index dans la palette
		// scan tous les points
		while (nPixelToScan-->0)
		{
			// lit le pixel
			psdPixel nPixel = *(pPixelLayer++);
			// le pixel est-il transparent?
			if (nPixel!=PSD_TRANSPARENT)
			{
				MapPixel::iterator ite=setPixel.find(nPixel);
				if (ite==setPixel.end())
				{
					setPixel.insert(MapPixel::value_type(nPixel, nColorCount2++));
					ite=setPixel.find(nPixel);
				}
				*pDst=(*ite).second;
			}
			pDst++;
		}
		// 16 couleurs dans ce layer?
		if (setPixel.size() > 16)
		{
			// non...
			funError (pParam,m_vectorLayerName[nLayerCount]);
			bOk = FALSE;
		}
		else
		{
			// fabrique la palette
			MapPixel::iterator itePalette = setPixel.begin();
			// pointeur sur la palette
			while (itePalette != setPixel.end())
			{
				nColorCount2=(*itePalette).second;
				// lire le pixel associe.
				psdPixel nPixel = (*itePalette).first;
				// transforme en 15 bits avec bit alpha...
				if (nPixel&0x80000000)
				{
					// avec alpha
					pPalette[nColorCount2]=0x8000;
				}
				else
				{
					// pas d'alpha
					pPalette[nColorCount2]=0;
				}
				// rgb en 15 bits
				pPalette[nColorCount2]|=Trans_Pal((nPixel&0xff0000)>>16,(nPixel&0xff00)>>8,nPixel&0xff);
				itePalette++;
			}
		}
		nColorCount+=16;
		nLayerCount++;
		iteLayer++;
	}
	if (!bOk)
	{
		delete pPixel;
		delete pPalette;
		return PSD_TOO_MUCH_COLORS_IN_A_LAYER;
	}
	// ********* tout est ok... color reducing.
	return PSD_OK;
}

BOOL Cpsd::IsPsd(const wchar_t* sFileName)
{
	char cEntete[4];
	FILE* pF = _wfopen(sFileName, L"rb");
	if (!pF)
		return FALSE;
	// lire l'entete
	if (fread (cEntete, 1, 4, pF)!=4)
	{
		fclose (pF);
		return FALSE;
	}
	fclose (pF);
	// verfie l'entete
	if ((cEntete[0] != '8') ||
		(cEntete[1] != 'B') ||
		(cEntete[2] != 'P') ||
		(cEntete[3] != 'S'))
		return FALSE;
	return TRUE;
}