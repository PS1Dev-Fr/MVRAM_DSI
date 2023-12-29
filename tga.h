

#ifndef _DSI_BASE_TGA_H
#define _DSI_BASE_TGA_H

#define TGA_IMG_DESC_ALPHA_CHANNEL_BITS		0x0f
#define TGA_IMG_DESC_LEFT_RIGHT				0x10
#define TGA_IMG_DESC_TOP_BOTTOM				0x20

#define TGA_IMG_TYPE_NO_DATA				0

#define TGA_IMG_TYPE_PAL_UNCOMPRESSED		1
#define TGA_IMG_TYPE_RGB_UNCOMPRESSED		2
#define TGA_IMG_TYPE_BW_UNCOMPRESSED		3

#define TGA_IMG_TYPE_PAL_COMPRESSED			9
#define TGA_IMG_TYPE_RGB_COMPRESSED			10
#define TGA_IMG_TYPE_BW_COMPRESSED			11

typedef struct TARGA
{
	unsigned char numid;	    // nb de caractères dans le champ Identification (se situe entre l'entête et la palette)
	unsigned char maptyp;       // 0:pas de palette/1:with palette
	unsigned char imgtyp;	    // 0:pas de donnée/1:pal non compressé/2:RGB non compressé/3:N&B non compressé/9:pal compressé/10:RGB compressé/11:N&B compressé
	short maporig;              // (index of the first color map entry)
	short mapsize;              // taille pal (count of color map entries)
	unsigned char mapbits;      // pal (nb de bits par entrée dans la palette)
	short xorig;                // 0
	short yorig;                // 0
	short xsize;                //
	short ysize;                //
    // Bit 16
	unsigned char pixsize;      // nb bits per pixel (8 en mode N&B)
	unsigned char imgdes;       // Image Descriptor:
/* imgdes:
 Bits 3-0: These bits specify the number of attribute bits per pixel.
           In the case of the TrueVista, these bits indicate the number of bits per pixel which are designated as Alpha Channel bits.
		   For the ICB and TARGA products, these bits indicate the number of overlay bits available per pixel.
		   See field 24 (Attributes Type) for more information.
 Bits 5&4: These bits are used to indicate the order in which pixel data is transferred from the file to the screen.
           Bit 4 is for left(0)-to-right(1) ordering and bit 5 is for top(1)-to-bottom(0) ordering as shown below.
 Bits 7&6: Must be zero to insure future compatibility.
*/
} TARGA;

typedef struct
{
	unsigned char B;
	unsigned char G;
	unsigned char R;
} TargaRgbType;

int compress_rlc8(unsigned char *ci_img, int w, int h, unsigned char *img_comp);
int test_decomp_rlc8(unsigned char *tmp_img, long csize, unsigned char *orig_img, long dsize);
void *tga_decomp_rlc(TARGA *pInfo,void *compressedBuffer,size_t compressedSize);
int decomp_rlc8(char *tmp_img, long csize, char *img);
int decomp_rlc24(char *tmp_img, long csize, TargaRgbType *img, int soutbuf);
int decomp_rlc32(char *tmp_img, long csize, TargaRgbType *img);
int load_file(wchar_t *filename, TARGA *tga_info, char *pal, char *img);
int save_tga(wchar_t *filename, TARGA *tga_info, char *pal, char *img);

void *tga_decomp_rlc(TARGA *pInfo,void *compressedBuffer);
void *convert_tga8_to_24(int nbData,void *pal, void *src);
void *convert_tga16_to_24(int nbData,void *src);
void *convert_tga24_to_24(int nbData,void *src);
void *convert_tga32_to_24(int nbData,void *src);
void *convert_tgabw_to_24(int nbData,void *src);
void *tga_extract_alpha(int nbData,void *src);
void *create_bw_palette(void);
int  flip_image(TARGA *pInfo,void *data);			// flip image vertically
int  mirror_image(TARGA *pInfo,void *data);			// flip image horizontally

#endif

