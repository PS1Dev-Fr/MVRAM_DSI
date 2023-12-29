// DSI_FILE.H

#ifndef _DSI_FILE_H
#define _DSI_FILE_H

#define DSI_FILE_FUNCTIONS	1						// Cache en lecture
#define DSI_DEBUG_FILE		0
#define DSI_SAVE_BANK		0
#define DSI_LOAD_BANK		0

#if ! DSI_FILE_FUNCTIONS
#undef DSI_SAVE_BANK
#define DSI_SAVE_BANK		0
#undef DSI_LOAD_BANK
#define DSI_LOAD_BANK		0
#endif

#define MAX_FILES_PER_BANK	500
#define MAX_BANK			12

#define DSI_FILE_CACHE_SIZE		(16*1024)			// en octets
#define DSI_MAX_OPENED_FILE		(8)

extern int nbFileReadInCache;
extern int nbFileReadOutOfCache;

long dsi_filelength(FILE* stream);

#if DSI_FILE_FUNCTIONS

#define MAX_FILE_LEN	64
// Structure utilisée pour le header de fichier bank
// l'offset est indiqué par rapport au debut du fichier bank
typedef struct
{
	char fName[MAX_FILE_LEN];
	unsigned int offset;
	unsigned int size;
	unsigned int chksum32;
} t_bankHeader;

int endDsiFile();

int addFileToCache(const wchar_t* fileName, FILE *stream);
int dsi_freadInCache(void *buffer, size_t size, size_t count, FILE *stream);
int dsi_fwriteInCache(void *buffer, size_t size, size_t count, FILE *stream);
int dsi_fseekInCache( FILE *stream, long offset, int origin );
int dsi_fgetcInCache(FILE *stream);
int removeFileFromCache(FILE *stream);


#if DSI_DEBUG_FILE

#define dsi_fopen(a,b)		deb_dsi_fopen(a,b, __FILE__, __LINE__)
FILE *deb_dsi_fopen( const wchar_t *filename, const wchar_t *mode, char *, int);

#else

FILE *dsi_fopen( const wchar_t *filename, const wchar_t *mode );

#endif

size_t dsi_fread(void *buffer, size_t size, size_t count, FILE *stream );
size_t dsi_fwrite(void *buffer, size_t size, size_t count, FILE *stream );
int dsi_fgetc( FILE *stream );
int dsi_fclose(FILE *file);
int dsi_fseek( FILE *stream, long offset, int origin );
long dsi_ftell( FILE *stream );
int getFileInfoIndex(FILE *f);

#else

#define dsi_fopen(a,b)		fopen(a,b)
#define dsi_fread(a,b,c,d)	fread(a,b,c,d)
#define dsi_fwrite(a,b,c,d)	fwrite(a,b,c,d)
#define dsi_fclose(a)		fclose(a)
#define dsi_fseek(a,b,c)	fseek(a,b,c)
#define dsi_ftell(a)		ftell(a)
#define dsi_fgetc(a)		fgetc(a)
#define	endDsiFile()

#endif


int getBankAndFile(const char *dosFileName, char *bank, char *file);

#if DSI_SAVE_BANK
int dsiCwd(char *path);
int dsiInitBank();
int dsiAddFileInBank(char *fileName, FILE *stream);
int dsiSaveAllBank();
int dsiReadBank(char *fileName, char *buffer, size_t size);
#endif

#if DSI_LOAD_BANK
FILE *fopenBank(const char *fileName, const char *mode);
int freadBank(char *buffer, size_t size, size_t count, FILE *handle);
int fgetcBank(FILE *handle);
int fseekBank( FILE *handle, long offset, int origin );
long ftellBank( FILE *handle );
long fcloseBank( FILE *handle );

int initBankFile();
int closeAllBank();

#endif



#endif // _DSI_FILE_H