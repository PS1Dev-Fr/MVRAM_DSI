// DSI_FILE.CPP
// Gestion de fichiers (fopen, fread, fclose)


#include <stdio.h>
//#include <io.h>
#include <string.h>

#include "StdAfx.h"

#include "dsi_file.h"
#include "dsi_err.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Si le define suivant est a 0, on recopie simplement les fichiers en entrée
// dans la banque
// Le mettre à 0 pour pouvoir modifier le contenu des fichiers de manière a 
// ne sauver que ce qui est util.
#define SAVE_ORIGINAL_FILE_CONTENT	1

#if DSI_FILE_FUNCTIONS

//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//						CACHE EN LECTURE
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

typedef struct {
	FILE	*handle;
	char	*readBuf;
	int		cacheStartOffset;				// Adresse à laquelle correspond le début du cache
	int		nbByteInCache;
	int		fileCurPosition;				// Position courante dans le fichier
	int		fileSize;
	int		cacheHit;
#if DSI_SAVE_BANK
	char	fileName[255];
#endif
#if DSI_DEBUG_FILE
	char	*srcFile;
	int		srcLine;
#endif
} t_dsiFileCache;


t_dsiFileCache	dsiFileCache[DSI_MAX_OPENED_FILE];
int nbFileInCache = 0;
t_dsiFileCache *pfc;				// Pointeur 

int nbFileReadInCache=0;
int nbFileReadOutOfCache=0;

int loadDataFromBank=0;
int saveDataInBank=1;













//-------------------------------------------------------------------
#if DSI_DEBUG_FILE
FILE *deb_dsi_fopen( const wchar_t *filename, const wchar_t *mode, char *srcFile, int numLine)
#else
FILE *dsi_fopen( const wchar_t *filename, const wchar_t *mode )
#endif
{
#if DSI_LOAD_BANK
	FILE *res;
	res = fopenBank(filename, mode);
	if (res)
		return res;
	// Sinon on essai d'ouvrir le fichier hors de la banque

#endif // DSI_LOAD_BANK

	// Gère les erreurs lors de l'ouverture des fichiers en affichant un message
	FILE *fTmp;

	if ((fTmp = _wfopen( filename, mode )) == NULL)
	{	
#if DSI_ERROR_FILE
#if DSI_DEBUG_FILE
		sprintf(tempText,L"Error for opening file %s in line %d srcfile %s\n", filename, numLine, srcFile);
#else
		swprintf(tempText,L"Error for opening file %s\n", filename);
#endif
		addWarnToErrorFile(tempText);
		//sprintf(tempText,"Warning: File %s cannot be opened in mode %s\n", filename, mode);
		//addWarnToErrorFile(tempText);
#endif
	}
	else
	{

		if (wcschr(mode, 'r') && ! (wcschr(mode, 't') || wcschr(mode, '+') || wcschr(mode, 'w')))
			addFileToCache(filename, fTmp);

#if DSI_ERROR_FILE
#if DSI_DEBUG_FILE
		sprintf(tempText,"Opening file %s in mode %s (%s,%d)\n", filename, mode, srcFile, numLine);
		addWarnToErrorFile(tempText);
#endif
#endif

	}

	return fTmp;
}

//-------------------------------------------------------------------
int addFileToCache(const wchar_t *fileName, FILE *stream)
{

	if (! stream)
		return 0;

	int i=0;
	pfc = dsiFileCache;
	do
	{
		if (! pfc->handle)
		{


#if 0 //DSI_ERROR_FILE
			if (i > 0)
			{
				sprintf(tempText,"Opening file %s in %d\n", fileName, i);
				addWarnToErrorFile(tempText);
			}
#endif



			if (! pfc->readBuf)
			{
				pfc->readBuf = new char[DSI_FILE_CACHE_SIZE];
			}
			pfc->handle = stream;
			pfc->nbByteInCache = 0;
			pfc->fileCurPosition = 0;
			pfc->cacheStartOffset = 0;
			pfc->cacheHit = 0;
			pfc->fileSize = -1;

#if DSI_SAVE_BANK
			if (fileName)
				strcpy(pfc->fileName, fileName);
			else
				strcpy(pfc->fileName, "Unknown file");
#endif
			return 1;
		}
		pfc++;
	}
	while (++i< DSI_MAX_OPENED_FILE);

	return 0;
}


//-------------------------------------------------------------------
size_t dsi_fwrite(void *buffer, size_t size, size_t count, FILE *stream )
{

#if DSI_LOAD_BANK
	// Cannot write to bank file
	return 0;
#else
	return dsi_fwriteInCache(buffer, size, count, stream);
#endif

}


//-------------------------------------------------------------------
// Write in cache not implemented
int dsi_fwriteInCache(void *buffer, size_t size, size_t count, FILE *stream)
{
	if (size <= 0)
		return 0;
	if (count <= 0)
		return 0;
	if (! buffer)
		return 0;

	int i=0;
	pfc = dsiFileCache;
	do
	{
		if (pfc->handle==stream)
		{
			return 0;
		}
		pfc++;
	}
	while (++i< DSI_MAX_OPENED_FILE);

	return (int)fwrite(buffer, size, count, stream);

}



//-------------------------------------------------------------------
size_t dsi_fread(void *buffer, size_t size, size_t count, FILE *stream )
{

#if DSI_LOAD_BANK
	if ((unsigned int)stream <= DSI_MAX_OPENED_FILE)
		return freadBank((char *)buffer, size, count, stream);
	else
#endif
		return dsi_freadInCache(buffer, size, count, stream);

}

//-------------------------------------------------------------------
int dsi_freadInCache(void *buffer, size_t size, size_t count, FILE *stream)
{
	if (size <= 0)
		return 0;
	if (count <= 0)
		return 0;
	if (! buffer)
		return 0;

	int i=0;
	pfc = dsiFileCache;
	do
	{
		if (pfc->handle==stream)
		{

			unsigned int n = ((int)size != 1 ? (int)count*(int)size : (int)count);

			#if DSI_DEBUG_FILE
			memset(buffer, 0, n);
			int debFilePos = pfc->fileCurPosition;
			int debNbByte = n;
			#endif


			unsigned int byteRead = 0;
			unsigned int cacheCurPos;
			unsigned int cacheLeft;


			while (n > 0)
			{
				ASSERT (pfc->nbByteInCache >= 0);
				ASSERT (pfc->nbByteInCache <= DSI_FILE_CACHE_SIZE);

				cacheCurPos = pfc->fileCurPosition - pfc->cacheStartOffset;
				ASSERT(cacheCurPos <= DSI_FILE_CACHE_SIZE);
				cacheLeft = pfc->nbByteInCache - cacheCurPos;
				ASSERT(cacheLeft <= DSI_FILE_CACHE_SIZE);

				if (cacheLeft == 0)
				{
					if (n < DSI_FILE_CACHE_SIZE)
					{
						// On alimente le cache
						int res = (int)fread(pfc->readBuf, 1, DSI_FILE_CACHE_SIZE, stream);
						pfc->nbByteInCache = res;
						pfc->cacheStartOffset = pfc->fileCurPosition;

						cacheCurPos = 0;
						cacheLeft = pfc->nbByteInCache;
					}
					else
					{
						// On lit directement dans le buffer destination et on evite une
						// recopie

						int res = (int)fread(((char *)buffer)+byteRead, 1, n, stream);
						pfc->fileCurPosition += n;
						pfc->cacheStartOffset = pfc->fileCurPosition;
						pfc->nbByteInCache = 0;
						byteRead += n;
						n = 0;

						nbFileReadOutOfCache++;
					}
				}

				if (n > 0)
				{
					int toRead = n;
					if (cacheLeft < n)		
					{
						// On n'a pas assez d'infos dans le cache
						// On copie ce qu'on a 
						toRead = cacheLeft;
					}

					if (cacheLeft != 0)
					{
						// On lit dans le cache
						memcpy(((char *)buffer)+byteRead, pfc->readBuf+cacheCurPos, toRead);
						pfc->fileCurPosition += toRead;
						
						n -= toRead;
						byteRead += toRead;

						pfc->cacheHit++;

						nbFileReadInCache++;
					}
					else
					{
						// End Of File
						break;
					}
				}
			}

			ASSERT (byteRead <= size*count);

			int countRead = ((int)size > 1 ? byteRead/(int)size : byteRead);

#if DSI_DEBUG_FILE
			if (countRead)
			{
				// On compare les data lues avec celles du fichier
				//int debFilePos = pfc->fileCurPosition;
				//int debNbByte = n;

				char *buffTmp;

				long curPos = ftell(stream);
				fseek(stream, debFilePos, SEEK_SET);

				buffTmp = new char[debNbByte];

				fread(buffTmp, size, count, stream);

				// Comparaison

				for (int k=0; k<debNbByte; k++)
				{
					ASSERT(buffTmp[k] == *((char *)buffer +k));
				}

				delete buffTmp;

				fseek(stream, curPos, SEEK_SET);
			}
#endif


			return countRead;
		
		}
		pfc++;
	}
	while (++i< DSI_MAX_OPENED_FILE);

	return (int)fread(buffer, (int)size, (int)count, stream);

}

//-------------------------------------------------------------------
int dsi_fgetc( FILE *stream )
{

#if DSI_LOAD_BANK
	if ((unsigned int)stream <= DSI_MAX_OPENED_FILE)
	{
#if 1
		return fgetcBank(stream);
#else
		char c;
		int res = freadBank(&c, 1, 1, stream);
		if (res == 1)
			return (int)c;
		else
			return EOF;
#endif
	}
	else
#endif
	{
		return dsi_fgetcInCache(stream);
	}

}

//-------------------------------------------------------------------
int dsi_fgetcInCache(FILE *stream)
{
#if 0
		unsigned char c;
		int res = dsi_fread(&c, 1, 1, stream);

		if (res == 1)
			return (int)c;
		else
			return EOF;
#else
		unsigned char c = 0xff;
		
		int i=0;
		pfc = dsiFileCache;
		do
		{
			if (pfc->handle==stream)
			{
				unsigned int n = 1;

				if (pfc->fileCurPosition < 0)
				{
					// End of file
					return 0;
				}
			
				unsigned int cacheCurPos;
				unsigned int cacheLeft;


				ASSERT (pfc->nbByteInCache >= 0);
				ASSERT (pfc->nbByteInCache <= DSI_FILE_CACHE_SIZE);

				cacheCurPos = pfc->fileCurPosition - pfc->cacheStartOffset;
				ASSERT(cacheCurPos <= DSI_FILE_CACHE_SIZE);
				cacheLeft = pfc->nbByteInCache - cacheCurPos;
				ASSERT(cacheLeft <= DSI_FILE_CACHE_SIZE);

				if (cacheLeft == 0)
				{
					// On alimente le cache
					int res = (int)fread(pfc->readBuf, 1, DSI_FILE_CACHE_SIZE, stream);
					pfc->nbByteInCache = res;
					pfc->cacheStartOffset = pfc->fileCurPosition;

					cacheCurPos = 0;
					cacheLeft = pfc->nbByteInCache;
				}

				if (cacheLeft != 0)
				{
					// On lit dans le cache
					c = *(pfc->readBuf+cacheCurPos);
					pfc->fileCurPosition ++;
					pfc->cacheHit++;
				}
				else
				{
					// End Of File
					return EOF;
				}
				return (int)c;		
			}
			pfc++;
		}
		while (++i< DSI_MAX_OPENED_FILE);

		return fgetc(stream);
#endif
	}

//-------------------------------------------------------------------
long dsi_ftell( FILE *stream )
{
#if DSI_LOAD_BANK
	if ((unsigned int)stream <= DSI_MAX_OPENED_FILE)
		return ftellBank(stream);
	else
#endif
	{
		int i=0;
		pfc = dsiFileCache;
		do
		{
			if (pfc->handle==stream)
			{
				return pfc->fileCurPosition;
			}
			pfc++;
		}
		while (++i< DSI_MAX_OPENED_FILE);

		return ftell(stream);
	}
}


//-------------------------------------------------------------------
int dsi_fseek( FILE *stream, long offset, int origin )
{

#if DSI_LOAD_BANK
	if ((unsigned int)stream <= DSI_MAX_OPENED_FILE)
		return fseekBank(stream, offset, origin);
	else
#endif
	{
		return dsi_fseekInCache(stream, offset, origin);
	}
}



//-------------------------------------------------------------------
int dsi_fseekInCache( FILE *stream, long offset, int origin )
{
	int i=0;
	pfc = dsiFileCache;
	do
	{
		if (pfc->handle==stream)
		{
			long absOfs;


			if (origin == SEEK_SET)
			{
				absOfs = offset;
			}
			else if (origin == SEEK_CUR)
			{
				absOfs = pfc->fileCurPosition + offset;
			}
			else if (origin == SEEK_END)
			{
				//ASSERT(FALSE);
				//return -1;

				long curPos = ftell(stream);
				fseek(stream, 0, SEEK_END);
				pfc->fileSize = ftell(stream);
				absOfs = pfc->fileSize - offset;
				fseek(stream, curPos, SEEK_SET);
			}
			else
			{
				ASSERT(FALSE);
				return -1;
			}

			if ((absOfs >= pfc->cacheStartOffset) && (absOfs < pfc->cacheStartOffset + pfc->nbByteInCache))
			{
				// On seek dans le cache
				pfc->fileCurPosition = absOfs;
				return 0;
			}
			else
			{
				int res = fseek( stream, absOfs, SEEK_SET );
				pfc->cacheStartOffset = pfc->fileCurPosition = ftell(stream);
				pfc->nbByteInCache = 0;		// Il faudra recharger
				return res;
			}
		}
		pfc++;
	}
	while (++i< DSI_MAX_OPENED_FILE);

	return fseek(stream, offset, origin);

}



//-------------------------------------------------------------------
int removeFileFromCache(FILE *stream)
{
	int i=0;
	pfc = dsiFileCache;
	do
	{
		if (pfc->handle==stream)
		{
#if DSI_SAVE_BANK
			if (saveDataInBank)
				dsiAddFileInBank(pfc->fileName, stream);
#endif

			pfc->handle=NULL;

			/*if (pfc->cacheHit)
				nbFileReadInCache++;
			else
				nbFileReadOutOfCache++;*/

			return 1;
		}
		pfc++;
	}
	while (++i< DSI_MAX_OPENED_FILE);

	/*nbFileReadOutOfCache++;*/
	return 0;
}




//-------------------------------------------------------------------
int endDsiFile()
{
	pfc = dsiFileCache;
	for (int i=0; i< DSI_MAX_OPENED_FILE; i++)
	{
		if (pfc->readBuf)
		{
			delete(pfc->readBuf);
			pfc->readBuf = NULL;
		}
		pfc++;
	}
	return 1;
}


//-------------------------------------------------------------------
int dsi_fclose(FILE *file)
{	

#if DSI_LOAD_BANK
	if ((unsigned int)file <= DSI_MAX_OPENED_FILE)
		return fcloseBank(file);
	else
#endif
	{
	removeFileFromCache(file);

	int res = fclose(file);

	return res;
	}
}


//-------------------------------------------------------------------
// Renvoi le nom de la banque qui contient le fichier ainsi que le 
// nouveau nom du fichier dans cette banque.
int getBankAndFile(const char *dosFileName, char *bank, char *file)
{
char tempFileName[MAX_FILE_LEN];
char sep[] = "\\/";

	strcpy(tempFileName, dosFileName);
	_strupr(tempFileName);				// Convertit en majuscules

	char *token = strtok(tempFileName, sep);
	if (!strncmp("DATA", token,4))
	{
		// On recupère le nom de la banque
		token = strtok(NULL, sep);
		if (token)
			strcpy(bank, token);
		else
			return 0;

		// On recupère le nom du fichier dans la banque
		token = strtok(NULL, sep);
		if (token)
			strcpy(file, token);
		else
			return 0;
		token = strtok(NULL, sep);
		while (token)
		{
			strcat(file, "\\");
			strcat(file, token);
			token = strtok(NULL, sep);
		}
		return 1;
	}
	else
	{
		return 0;
	}
}


#if DSI_SAVE_BANK


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//					CREATION DES BANQUES
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
typedef struct
{
#if SAVE_ORIGINAL_FILE_CONTENT
	char	dosFileName[MAX_FILE_LEN];
#endif
	char	fileName[MAX_FILE_LEN];
	unsigned int size;
	unsigned int crc32;
	unsigned int chksum32;
	char *data;
} t_bankFileInfo;




typedef struct
{
	char folderName[MAX_FILE_LEN];
	int nbFiles;
	t_bankFileInfo fileList[MAX_FILES_PER_BANK];
	unsigned int size;
} t_fileBank;


t_fileBank dsiFileBank[MAX_BANK];
int dsiNbBank=0;
char curDirectory[MAX_FILE_LEN];

unsigned int maxFileNameLen = 0;


//-------------------------------------------------------------------
int dsiCwd(char *path)
{
	// Verifier si il y a .. dans le chemin
	strcpy(curDirectory, path);
	return 1;
}


//-------------------------------------------------------------------
int dsiInitBank()
{

	dsiNbBank = 0;
	for (int i=0; i < MAX_BANK; i++)
	{
		dsiFileBank[i].folderName[0] = 0;
		dsiFileBank[i].nbFiles = 0;
		dsiFileBank[i].size = 0;
		for (int j=0; j < MAX_FILES_PER_BANK; j++)
		{
			dsiFileBank[i].fileList[j].fileName[0] = 0;
			dsiFileBank[i].fileList[j].data=NULL;
			dsiFileBank[i].fileList[j].size=0;
		}
	}
	return 1;
}

//-------------------------------------------------------------------
t_bankFileInfo *findFileInBank(char *fileName)
{
	for (int i=0; i < dsiNbBank; i++)
	{
		t_bankFileInfo *pFile = dsiFileBank[i].fileList;
		for (int j=0; j < dsiFileBank[i].nbFiles; j++)
		{
			if (! strncmp(fileName, pFile->fileName, MAX_FILE_LEN))
				return pFile;
			
			pFile++;
		}
	}
	return NULL;

}



//-------------------------------------------------------------------
// Si le fichier existe on ajoute les datas a la fin, sinon on ajoute
// le fichier à la bonne banque
// Une banque par sous-répertoire de DATA
// retourne 0 si une erreur se produit

// Attention, si un fichier existe déja dans la base, les ouvertures
// suivantes ne sont pas prises en compte même pour des accès en ecriture
// qui ont pu modifier le contenu.

// Optimisation : ne pas stocker les data mais seulement leur taille
// et copier le fichier lorsqu'on fait un save all
int dsiAddFileInBank(char *fileName, FILE *stream)
{
	// Trouver la banque a laquelle appartient le fichier
	// (en fonction du répertoire sous data)

	char bankName[MAX_FILE_LEN];
	char bankFileName[MAX_FILE_LEN];

	if (getBankAndFile(fileName, bankName, bankFileName))
	{
		int bankFound = -1;
		for (int i=0; i<dsiNbBank; i++)
		{
			// On cherche le nom  de la banque
			if (! strcmp(dsiFileBank[i].folderName, bankName))
			{
				bankFound = i;
				break;
			}
		}

		if (bankFound == -1)
		{
			// On n'a pas trouve la banque, on en cree une nouvelle
			if (dsiNbBank < MAX_BANK)
			{
				strcpy(dsiFileBank[i].folderName, bankName);
				dsiFileBank[i].nbFiles = 0;
				bankFound = i;
				dsiNbBank++;

#if DSI_ERROR_FILE
				sprintf(tempText,"Creating bank %s for file %s\n", bankName, bankFileName);
				addWarnToErrorFile(tempText);
#endif

			}
			else
			{
#if DSI_ERROR_FILE
				sprintf(tempText,"Error for adding file %s in bank %s, increase MAX_BANK\n", bankFileName, bankName);
				addWarnToErrorFile(tempText);
#endif
				return 0;
			}
		}


		if (strlen(bankFileName) > maxFileNameLen)
			maxFileNameLen = strlen(bankFileName);

		// On a la banque
		// On recherche le fichier

		t_bankFileInfo *pFile = dsiFileBank[i].fileList;
		for (int j=0; j < dsiFileBank[i].nbFiles; j++)
		{
			if (! strncmp(bankFileName, pFile->fileName, MAX_FILE_LEN))
			{
				// On a trouvé le fichier
#if DSI_ERROR_FILE
				sprintf(tempText,"File %s in bank %s read twice or more\n", bankFileName, bankName);
				addWarnToErrorFile(tempText);
#endif

				return 0;
			}
			
			pFile++;
		}

		// On n'a pas trouvé le fichier, on l'ajoute

		if (dsiFileBank[i].nbFiles < MAX_FILES_PER_BANK)
		{
			strcpy(pFile->fileName,bankFileName);
			dsiFileBank[i].nbFiles++;


			// On lit le fichier et on l'ajoute dans la banque
			if (! stream)
			{
				PRINT_ERROR_FILE("Stream is NULL\n");
				return 0;
			}

			if (fseek(stream,0,SEEK_END))
			{
				PRINT_ERROR_FILE("Cannot seek to end of file\n");
				return 0;
			}

			long size = ftell(stream);

#if SAVE_ORIGINAL_FILE_CONTENT
			strcpy(pFile->dosFileName, fileName);
#else
			if (fseek(stream,0,SEEK_SET))
			{
				PRINT_ERROR_FILE("Cannot seek to start of file\n");
				return 0;
			}

			pFile->data = new char[size];

			if (! pFile->data)
			{
				PRINT_ERROR_FILE("Cannot allocate memory\n");
				return 0;
			}

			int res = fread(pFile->data, 1, size, stream);

			if (res != size)
			{
				PRINT_ERROR_FILE("Cannot read file\n");
				return 0;
			}
#endif

			pFile->size = size;

			// On devrait calculer le checksum ici

		}
		else
		{
#if DSI_ERROR_FILE
			sprintf(tempText,"Error for adding file %s in bank %s, increase MAX_FILES_PER_BANK\n", bankFileName, bankName);
			addWarnToErrorFile(tempText);
#endif
			return 0;
		}
		

		return 1;
	}

	return 0;
}

//-------------------------------------------------------------------
// Génère les fichiers bank sur disque dur (.BKF bank file)
// Attention, cette fonction libère les blocs de data correspondants aux fichiers
int dsiSaveAllBank()
{
	int res;

#if DSI_ERROR_FILE
	sprintf(tempText,"Maximum size of file name in banks : %d\n", maxFileNameLen);
	addWarnToErrorFile(tempText);
#endif


	for (int i=0; i < dsiNbBank; i++)
	{
		char bankName[MAX_FILE_LEN];

		strcpy(bankName, "DATA\\");
		strcat(bankName, dsiFileBank[i].folderName);
		strcat(bankName, ".BKF");

		FILE *stream = fopen(bankName, "wb");
		// On sauve la table des fichiers

		if (! stream)
		{
			PRINT_ERROR_FILE("Cannot open bank file\n");
			return 0;
		}

		t_bankHeader *head = new t_bankHeader[dsiFileBank[i].nbFiles];
		unsigned int curOffset = dsiFileBank[i].nbFiles*sizeof(t_bankHeader) + sizeof(dsiFileBank[i].nbFiles);

		for (int j=0; j < dsiFileBank[i].nbFiles; j++)
		{
			head[j].offset = curOffset;
			head[j].size = dsiFileBank[i].fileList[j].size;
			strcpy(head[j].fName, dsiFileBank[i].fileList[j].fileName);

			curOffset += head[j].size;
		}

		// On sauve le nombre de fichiers dans la banque
		res = fwrite(&dsiFileBank[i].nbFiles, sizeof(dsiFileBank[i].nbFiles), 1, stream);
		if (res != 1)
		{
			PRINT_ERROR_FILE("Cannot write bank file num\n");
			return 0;
		}

		// On sauve le header
		res = fwrite(head, sizeof(t_bankHeader), dsiFileBank[i].nbFiles, stream );
		if (res != dsiFileBank[i].nbFiles)
		{
			PRINT_ERROR_FILE("Cannot write bank file header\n");
			return 0;
		}


		// On sauve les data des fichiers
		for (j=0; j < dsiFileBank[i].nbFiles; j++)
		{
			unsigned int size = dsiFileBank[i].fileList[j].size;
			char *data = dsiFileBank[i].fileList[j].data;

#if SAVE_ORIGINAL_FILE_CONTENT
			// On lit le contenu du fichier 
			FILE *inFile = fopen(dsiFileBank[i].fileList[j].dosFileName, "rb");
			if (! inFile)
			{
				PRINT_ERROR_FILE("Cannot open file\n");
				return 0;
			}

			data = new char[size];

			if (! data)
			{
				PRINT_ERROR_FILE("Cannot allocate memory\n");
				return 0;
			}

			int res = fread(data, 1, size, inFile);

			if (res != (int)size)
			{
				PRINT_ERROR_FILE("Cannot read file\n");
#if DSI_ERROR_FILE
				sprintf(tempText,"Read %d bytes for file %s  instead of %u.\n", res, dsiFileBank[i].fileList[j].dosFileName, size);
				addWarnToErrorFile(tempText);
#endif
				return 0;
			}
			fclose(inFile);
#endif
			res = fwrite(data, 1, size, stream);			
			if (res != (int)size)
			{
				PRINT_ERROR_FILE("Cannot write bank file data\n");
				return 0;
			}

			// On libère la memoire
			delete data;
			dsiFileBank[i].fileList[j].data = NULL;
			dsiFileBank[i].fileList[j].size = 0;
		}


		if (fclose(stream))
		{
			PRINT_ERROR_FILE("Cannot close bank file\n");
			return 0;
		}
#if DSI_ERROR_FILE
		sprintf(tempText,"%d files (%d bytes) saved in bank file %s.\n", dsiFileBank[i].nbFiles, curOffset, bankName);
		addWarnToErrorFile(tempText);
#endif

	}

	return 1;
}

#endif		//	DSI_SAVE_BANK


//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//					LECTURE DANS LES BANQUES
//-------------------------------------------------------------------
//-------------------------------------------------------------------
//-------------------------------------------------------------------

#if DSI_LOAD_BANK

#define MAX_BANK_NAME	16

typedef struct
{
	int bankNum;			// index dans la liste des banques ouvertes
	int filePos;			// position courante dans le fichier
	int fileNum;			// index de fichier dans la table d'entête
} t_bankFileOpened;


typedef struct
{
	FILE *bankStream;
	char name[MAX_BANK_NAME];
	unsigned int pos;		// position courante dans la banque
	int	nbFiles;
	int lastFOpened;		// Indice du dernier fichier ouvert dans la banque
	t_bankHeader *fileList;
} t_bankOpened;


t_bankFileOpened myBankFilesOpened[DSI_MAX_OPENED_FILE];
int nbBankFilesOpened=0;

t_bankOpened myBanks[MAX_BANK];
int nbBank=-1;



//-------------------------------------------------------------------
int initBankFile()
{
	nbBank = 0;
	nbBankFilesOpened = 0;
	for (int i=0; i<MAX_BANK; i++)
	{
		myBanks[i].bankStream = 0;
	}

	for (i=0; i<DSI_MAX_OPENED_FILE; i++)
	{
		myBankFilesOpened[i].bankNum = -1;		
		myBankFilesOpened[i].fileNum = -1;
		myBankFilesOpened[i].filePos = -1;
	}
	return 1;
}



//-------------------------------------------------------------------
// Ouvre un fichier appartenant à une banque comme fopen
// ouvre la banque qui contient ce fichier si ce n'est pas déja fait
// Attention on renvoi l'index dans ma table des fichiers
// 0 = erreur comme d'hab
FILE *fopenBank(const char *fileName, const char *mode)
{
	char bankName[MAX_FILE_LEN];
	char bankFileName[MAX_FILE_LEN];
	int bankNum=-1;
	int fileNum=-1;

	if (nbBank == -1)
	{
		initBankFile();
	}

	// Si on lit dans les banques, on ne sauve pas celles ci
	saveDataInBank=0;

	if (getBankAndFile(fileName, bankName, bankFileName))
	{
		for (int i=0; i<MAX_BANK; i++)
		{
			if (!myBanks[i].bankStream)
				break;
			if (!strncmp(bankName, myBanks[i].name,MAX_BANK_NAME))
			{
				bankNum = i;
				break;
			}
		}
		if (bankNum < 0)
		{
			if (i >= MAX_BANK)
			{
#if DSI_ERROR_FILE
				sprintf(tempText,"Error : MAX_BANK must be increased.\n");
				addWarnToErrorFile(tempText);
#endif
				return NULL;
			}
			bankNum = i;

			// On doit ouvrir la banque qui n'a pas été trouvée
			char dosBankName[MAX_FILE_LEN];

			strcpy(dosBankName, "DATA\\");
			strcat(dosBankName, bankName);
			strcat(dosBankName, ".BKF");

			FILE *stream = fopen(dosBankName, "rb");
			addFileToCache(dosBankName, stream);

			if (! stream)
			{
				// La banque n'existe pas, il faut faire un open simple
#if DSI_ERROR_FILE
				sprintf(tempText,"Warning : bank file %s doesn't exist.\n", dosBankName);
				addWarnToErrorFile(tempText);
#endif
				return NULL;
			}

			myBanks[bankNum].bankStream = stream;
			myBanks[bankNum].pos = 0;
			myBanks[bankNum].lastFOpened = 0;
			strcpy(myBanks[bankNum].name, bankName);

			int nbFiles;
			int res = dsi_freadInCache(&nbFiles, sizeof(nbFiles), 1, stream);
			if (res != 1)
				return NULL;
			myBanks[bankNum].nbFiles = nbFiles;

			myBanks[bankNum].fileList = new t_bankHeader[nbFiles];

			if (!myBanks[bankNum].fileList)
			{
				return NULL;
			}

			res = dsi_freadInCache(myBanks[bankNum].fileList, sizeof(t_bankHeader), nbFiles, stream);
			if (res != nbFiles)
				return NULL;

			myBanks[bankNum].pos = sizeof(nbFiles) + sizeof(t_bankHeader)*nbFiles;
		}

		// On recherche le fichier dans la banque
		// on commence notre recherche à partir du dernier fichier accédé
		// car les fichiers sont stockés dans la banque dans l'ordre où ils sont accédés

		t_bankHeader *pF = &myBanks[bankNum].fileList[myBanks[bankNum].lastFOpened];
		for (i=myBanks[bankNum].lastFOpened; i <myBanks[bankNum].nbFiles; i++)
		{
			if (!strcmp(bankFileName, pF->fName))
			{
				// On a trouvé le fichier
				fileNum = i;
				break;
			}
			pF++;
		}

		if (fileNum == -1)
		{
			// On essaie de rechercher le fichier depuis le début de la liste
			pF = myBanks[bankNum].fileList;
			for (i=0; i <myBanks[bankNum].lastFOpened; i++)
			{
				if (!strcmp(bankFileName, pF->fName))
				{
					// On a trouvé le fichier
					fileNum = i;
					break;
				}
				pF++;
			}
		}

		if (fileNum == -1)
		{
			// Fichier non trouvé dans la banque
#if DSI_ERROR_FILE
			sprintf(tempText,"Error : file %s not found in bank %s.\n", bankFileName, bankName);
			addWarnToErrorFile(tempText);
#endif
			return NULL;
		}


		for (i=0; i<DSI_MAX_OPENED_FILE; i++)
		{
			// on cherche un élément de la table qui soit libre
			if (myBankFilesOpened[i].bankNum < 0)		
			{
				myBankFilesOpened[i].bankNum = bankNum;
				myBankFilesOpened[i].fileNum = fileNum;
				myBankFilesOpened[i].filePos = 0;
				return (FILE *)(i+1);
			}

		}
#if DSI_ERROR_FILE
		sprintf(tempText,"Error : DSI_MAX_OPENED_FILE must be increased.\n");
		addWarnToErrorFile(tempText);
#endif
		return NULL;
	}
	else
	{
#if DSI_ERROR_FILE
		sprintf(tempText,"Error : could not extract name of bank and file from %s.\n", fileName);
		addWarnToErrorFile(tempText);
#endif
		return NULL;
	}
}

//-------------------------------------------------------------------
// On ne ferme pas la banque car elle va surement etre encore utilisée
// Appeler closeAllBank() pour fermer toutes les banques une fois
// qu'on à chargé toutes les données.
long fcloseBank( FILE *handle )
{
	if ((!handle) || ((unsigned int)handle > DSI_MAX_OPENED_FILE))
		return EOF;

	if (myBankFilesOpened[(int)handle - 1].bankNum != -1)
	{
		myBankFilesOpened[(int)handle - 1].bankNum = -1;			// Fichier fermé
		return 0;
	}
	else
	{
		return EOF;
	}

}

//-------------------------------------------------------------------
int closeAllBank()
{
	nbBank = -1;
	for (int i=0; i<MAX_BANK; i++)
	{
		if (myBanks[i].bankStream)
		{
			removeFileFromCache(myBanks[i].bankStream);
			fclose(myBanks[i].bankStream);
			myBanks[i].bankStream = NULL;
		}
	}
	for (i=0; i<DSI_MAX_OPENED_FILE; i++)
	{
		myBankFilesOpened[i].bankNum = -1;		
		myBankFilesOpened[i].fileNum = -1;
		myBankFilesOpened[i].filePos = -1;
	}
	return 1;
}


//-------------------------------------------------------------------
// Lit des données dans un fichier banque (avec un cache de 32 Ko)
// Le handle a passer est compris entre 1 et DSI_MAX_OPENED_FILE inclu
int freadBank(char *buffer, size_t size, size_t count, FILE *handle)
{
	int res;
	int toRead, canRead;
	int newCount = count;

	if ((!handle) || ((unsigned int)handle > DSI_MAX_OPENED_FILE))
		return 0;

	int curBank = myBankFilesOpened[(int)handle - 1].bankNum;
	if (curBank < 0)
		return 0;			// Fichier non ouvert

	int curFile = myBankFilesOpened[(int)handle - 1].fileNum;

	toRead = size*count;
	canRead = myBanks[curBank].fileList[curFile].size - myBankFilesOpened[(int)handle - 1].filePos;
	if (canRead < toRead)
	{
		//On va arriver à la fin du fichier
		toRead = canRead;
		newCount = canRead / size;
	}

	unsigned int bankNewPos = myBanks[curBank].fileList[curFile].offset + myBankFilesOpened[(int)handle - 1].filePos;
	if (bankNewPos != myBanks[curBank].pos)
	{
		// On doit faire un seek car on n'est pas bien positionné dans la banque
		res = dsi_fseek(myBanks[curBank].bankStream, bankNewPos, SEEK_SET);
		if (res)
		{
#if DSI_ERROR_FILE
		sprintf(tempText,"Error : could not seek at %u in bank %s.\n", bankNewPos, myBanks[curBank].name);
		addWarnToErrorFile(tempText);
#endif
		return 0;
		}
		myBanks[curBank].pos = bankNewPos;
	}

	// On lit 
	res = dsi_freadInCache(buffer, size, newCount,  myBanks[curBank].bankStream);
	myBanks[curBank].pos += res*size;
	myBankFilesOpened[(int)handle - 1].filePos += res*size;

	return res;
}


//-------------------------------------------------------------------
int fgetcBank(FILE *handle)
{
	int res;
	int canRead;
	
	if ((!handle) || ((unsigned int)handle > DSI_MAX_OPENED_FILE))
		return EOF;

	int curBank = myBankFilesOpened[(int)handle - 1].bankNum;
	if (curBank < 0)
		return EOF;			// Fichier non ouvert

	int curFile = myBankFilesOpened[(int)handle - 1].fileNum;

	canRead = myBanks[curBank].fileList[curFile].size - myBankFilesOpened[(int)handle - 1].filePos;
	if (canRead < 1)
	{
		//On est à la fin du fichier
		return EOF;
	}

	unsigned int bankNewPos = myBanks[curBank].fileList[curFile].offset + myBankFilesOpened[(int)handle - 1].filePos;
	if (bankNewPos != myBanks[curBank].pos)
	{
		// On doit faire un seek car on n'est pas bien positionné dans la banque
		res = dsi_fseek(myBanks[curBank].bankStream, bankNewPos, SEEK_SET);
		if (res)
		{
#if DSI_ERROR_FILE
		sprintf(tempText,"Error : could not seek at %u in bank %s.\n", bankNewPos, myBanks[curBank].name);
		addWarnToErrorFile(tempText);
#endif
		return EOF;
		}
		myBanks[curBank].pos = bankNewPos;
	}

	// On lit 
	res = dsi_fgetcInCache(myBanks[curBank].bankStream);
	myBanks[curBank].pos ++;
	myBankFilesOpened[(int)handle - 1].filePos ++;

	return res;
}



//-------------------------------------------------------------------
// Retourne 0 si pas d'erreurs
int fseekBank( FILE *handle, long offset, int origin )
{
	if ((!handle) || ((unsigned int)handle > DSI_MAX_OPENED_FILE))
		return 1;

	int curBank = myBankFilesOpened[(int)handle - 1].bankNum;
	if (curBank < 0)
		return 1;			// Fichier non ouvert

	int curFile = myBankFilesOpened[(int)handle - 1].fileNum;

	unsigned int bankNewPos = myBanks[curBank].fileList[curFile].offset ;
	unsigned int newFilePos = 0;

	switch(origin)
	{
	case SEEK_SET:
		newFilePos = offset;
		break;
	case SEEK_CUR:
		newFilePos = myBankFilesOpened[(int)handle - 1].filePos + offset;
		break;
	case SEEK_END:
		newFilePos = myBankFilesOpened[(int)handle - 1].filePos + myBanks[curBank].fileList[curFile].size - offset;
		break;
	default:
		break;
	}
	bankNewPos += newFilePos;

	int res = dsi_fseekInCache(myBanks[curBank].bankStream, bankNewPos, SEEK_SET);

	if (!res)
	{
		myBankFilesOpened[(int)handle - 1].filePos = newFilePos;
		myBanks[curBank].pos = bankNewPos;
	}
#if DSI_ERROR_FILE
	else
	{
		sprintf(tempText,"Error : could not seek at %u in bank %s.\n", bankNewPos, myBanks[curBank].name);
		addWarnToErrorFile(tempText);
	}
#endif


	return res;
}

//-------------------------------------------------------------------
long ftellBank( FILE *handle )
{
	if ((!handle) || ((unsigned int)handle > DSI_MAX_OPENED_FILE))
		return -1L;

	if (myBankFilesOpened[(int)handle - 1].bankNum < 0)
		return -1L;

	return myBankFilesOpened[(int)handle - 1].filePos;
}




#endif		//	DSI_LOAD_BANK


#endif		//	DSI_FILE_FUNCTIONS


//-------------------------------------------------------------------
long dsi_filelength(FILE* stream)
{

#if DSI_LOAD_BANK
	if ((unsigned int)stream <= DSI_MAX_OPENED_FILE)
	{
		int bankNum = myBankFilesOpened[(int)stream - 1].bankNum;
		int fileNum = myBankFilesOpened[(int)stream - 1].fileNum;

		return myBanks[bankNum].fileList[fileNum].size;
	}
	else
#endif
	{
		//return _filelength(stream->_file);

		long size;
		int i=0;
		pfc = dsiFileCache;
		do
		{
			if (pfc->handle==stream)
			{
				size = pfc->fileSize;
				break;
			}
			pfc++;
		}
		while (++i< DSI_MAX_OPENED_FILE);

		if (size < 0)
		{
			long curPos = dsi_ftell(stream);
			dsi_fseek(stream, 0, SEEK_END);
			size = dsi_ftell(stream);
			dsi_fseek(stream, curPos, SEEK_SET);
		}
		return size;
	}
}


