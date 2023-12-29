/*
	Base de travail pour moulinettes sur fichiers .TGA
	
	Alain RAMOND 10/10/94
	Patrick Bricout 24/02/98
*/

#include "stdafx.h"

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <io.h>
#include <fcntl.h>
#include <string.h>
#include <stdint.h>

#include "tga.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define SWAP(a)			{(a) = (sizeof(a)==4 ? swap_l((unsigned int)(a)) : sizeof(a)==2 ? swap_w((unsigned short)(a)) : (a)) ;}

#if 1//def __386__
int	pc_data=0;
#else
int	pc_data=1;
#endif

/*----------------------------------------------------------------------*/
/*  SWAP_W								*/
/*  Swaps an Intel word (2 bytes) into Motorola format (or vice versa)	*/
/*                                                                      */
/*                                                                      */
/*  Input   : Word to convert						*/
/*  Output  : Word converted						*/
/*  Uses    : Nothing							*/
/*  Modify  : Nothing							*/
/*----------------------------------------------------------------------*/
unsigned short swap_w (unsigned short data)
{
	if (pc_data)
	  return (unsigned short)((data>>8) | (data<< 8));
	else
	  return (unsigned short)data;
} 

/*----------------------------------------------------------------------*/
/*  SWAP_L								*/
/*  Swaps an Intel long word (4 bytes) into Motorola format 		*/
/*                                                                      */
/*                                                                      */
/*  Input   : Long word to convert					*/
/*  Output  : Long word converted					*/
/*  Uses    : Nothing							*/
/*  Modify  : Nothing							*/
/*----------------------------------------------------------------------*/
unsigned int swap_l (unsigned int data)
{
	if (pc_data)
	  return (swap_w((unsigned short)(data>>16)) | (swap_w((unsigned short)(data & 0xffff)) << 16));
	else
	  return data;
} 



/*----------------------------------------------------------------------*/

int compress_rlc8(unsigned char *ci_img, int w, int h, unsigned char *img_comp)
{
int r,c, nb_rep, nb_cop, last_car, car;
unsigned char 	*ptrs, *ptrtmp;
unsigned char 	*ptrd;
int mode, size_comp;

#define MODE_COP	0
#define MODE_DUP	1

ptrs = (unsigned char *)ci_img;
ptrd = (unsigned char *)img_comp;
size_comp = 0;

for (r = 0; r<h; r++)
    {
    c = 1;				
    
    mode = MODE_COP;
    ptrtmp = ptrs;
    last_car = *ptrs++;
    nb_rep = nb_cop = 1;
		
    while (c<w)
        {
	car = *ptrs;
	
	if (mode == MODE_COP)
	    {
	    if (nb_cop == 0x80)
		{
		size_comp += nb_cop+1;
		*ptrd++ = nb_cop-1;
		while (nb_cop-- > 0)
		    {
		    *ptrd++ = *ptrtmp++;
		    }
		ptrtmp = ptrs;
		nb_cop = 1;
		mode = MODE_COP;
		}
	    else if (car != last_car)
	        {
		nb_cop++;
		}
	    else
	        {
		if (nb_cop > 1)
		    {
		    nb_cop--;
		    size_comp += nb_cop+1;
		    *ptrd++ = nb_cop-1;
		    while (nb_cop-- > 0)
		        {
		        *ptrd++ = *ptrtmp++;
		        }
		    }
		
		ptrtmp = ptrs-1;
		nb_rep = 2;
		mode = MODE_DUP;
		}
	    last_car = car;	
	    }
	else if (mode == MODE_DUP)
	    {
	    if ((nb_rep == 0x80) || (car != last_car))
		{
		size_comp += 2;
		*ptrd++ = nb_rep + 0x7F;
		*ptrd++ = last_car;
		
		ptrtmp = ptrs;
		nb_cop = 1;
		mode = MODE_COP;
		last_car = car;
		}
	    else /*if  (car == last_car)*/
	        {
		nb_rep++;
		}
	    }
	    
	c++; 
	ptrs++;
	
	
	}
	
	/* on a fini une ligne */
	
	if (mode == MODE_COP)
	    {
	    size_comp += nb_cop+1;
	    *ptrd++ = nb_cop-1;
	    while (nb_cop-- > 0)
		{
		*ptrd++ = *ptrtmp++;
		}
	    }
	else if (mode == MODE_DUP)
	    {
	    size_comp += 2;
	    *ptrd++ = nb_rep + 0x7F;
	    *ptrd++ = last_car;
	    }
    }

return size_comp;

}


/*----------------------------------------------------------------------*/

int test_decomp_rlc8(unsigned char *tmp_img, long csize, unsigned char *orig_img, long dsize)
{
int 		i;
long		size;
unsigned char	rep;
unsigned char 	*ptrs;
unsigned char 	*ptrd;
register unsigned char	tmp_col;

i=0;
ptrs = tmp_img;
ptrd = orig_img;
size = csize;

while ((size > 0) && (dsize >0))
    {
    if ((rep = *ptrs++) & 0x80 )			/* decompression */
        {
	rep -= 0x7f;
	
	tmp_col = *ptrs++;
	size -= 2;
	dsize -= rep;
	while(rep--)		
	    {
	    if (*ptrd++ != tmp_col)
	       {
	       printf("Error in decomp test, found %4d instead of %4d at %d of end\n",tmp_col,*(ptrd-1), size ); // [Error]
	       }
	    }
	}
    else
        {
	rep++;
	size--;
	dsize -= rep;
	size -= rep;
	while(rep--)				
	    {
	    if (*ptrd++ != *ptrs++)
	        printf("Error in decomp test, found %4d instead of %4d at %d of end\n",*(ptrs-1),*(ptrd-1), size ); // [Error]
	    }
	}
    }

if (size > 0)
    {
    printf("Error while decomp test (%d %d) \n", size, dsize); // [Error]
    }

return 0;
}



/*----------------------------------------------------------------------*/

void *tga_decomp_rlc(TARGA *pInfo,void *compressedBuffer,size_t compressedSize)
{
	int rep;
	void *outBuffer;
	char *pSrc,*pDst;
	char tmpBuffer[4];
	int pixelSize,imgSize;

	outBuffer = NULL;
	pixelSize = (pInfo->pixsize+7)/8;
	imgSize   = pInfo->xsize*pInfo->ysize;
	if (pixelSize>0 && pixelSize<=4 && imgSize>0 && compressedSize>0)
	{
		outBuffer=malloc(pixelSize*imgSize);
		pSrc = (char *)compressedBuffer;
		pDst = (char *)outBuffer;
		while (imgSize>0 && compressedSize>0)
		{
			rep = *pSrc++;
			compressedSize--;
			if (rep<0)					/* decompression */
			{
				if (compressedSize>=(size_t)pixelSize)
				{
					rep += 129;
					memcpy(tmpBuffer,pSrc,pixelSize);
					pSrc+=pixelSize;
					if ((imgSize-rep)<0)		// blindage si l'image compressée est endommagée
						rep = imgSize;			// erreur lors de la decompression
					imgSize-=rep;
					while(rep--)
					{
						memcpy(pDst,tmpBuffer,pixelSize);
						pDst+=pixelSize;
					}
					compressedSize-=pixelSize;
				}
				else
					compressedSize=0;		// erreur de decompression
			}
			else
			{
				rep++;
				if ((imgSize-rep)<0)		// blindage si l'image compressée est endommagée
					rep = imgSize;			// erreur lors de la decompression
				imgSize-=rep;
				if (compressedSize>=(size_t)pixelSize*rep)
				{
					memcpy(pDst,pSrc,pixelSize*rep);
					pDst+=pixelSize*rep;
					pSrc+=pixelSize*rep;
					compressedSize-=pixelSize*rep;
				}
				else
					compressedSize=0;		// erreur de decompression
			}
		}
	}
	return outBuffer;
}

/*----------------------------------------------------------------------*/

int decomp_rlc8(char *tmp_img, long csize, char *img)
{
	int 		i;
	signed char	rep;
	char 		*ptrs;
	unsigned char 	*ptrd;
	unsigned char	tmp_col;

	i=0;
	ptrs = tmp_img;
	ptrd = (unsigned char *)img;
	while (csize > 0)
	{
		if ((rep = *ptrs++) & 0x80)			/* decompression */
		{
			rep -= 0x7f;
			tmp_col = *ptrs++;
			csize -= 2;
			while(rep--)		
			{
				*ptrd++ = tmp_col;
			}
		}
		else
		{
			rep++;
			csize--;
			while(rep--)				/* recopie simple */
			{
				*ptrd++ = *ptrs++;
				csize--;
			}
		}
	}
	return 0;
}

/*----------------------------------------------------------------------*/

int decomp_rlc24(char *tmp_img, long csize, TargaRgbType *img, int soutbuf)
{
int 		i, outs;
signed char	rep;
signed char 	*ptrs;
TargaRgbType 	*ptrd;
unsigned char	B, G, R;

i=0;
outs = 0;
ptrs = (signed char *)tmp_img;
ptrd = img;
while (csize > 0)
    {
    if ((rep = *ptrs++) & 0x80)			/* decompression */
        {
	rep -= 0x7f;
	
	B = *ptrs++;
	G = *ptrs++;
	R = *ptrs++;
	csize -= 4;
	while(rep--)		
	    {
	    if (outs+3 > soutbuf)
	        goto outfull;
	    ptrd->B = B;
	    ptrd->G = G;
	    ptrd->R = R;
	    ptrd++;
	    outs += 3;
	    }
	}
    else
        {
	csize--;
	rep++;
	while(rep--)				/* recopie simple */
	    {
	    if (outs+3 > soutbuf)
	        goto outfull;
	    ptrd->B = *ptrs++;
	    ptrd->G = *ptrs++;
	    ptrd->R = *ptrs++;
	    ptrd++;
	    csize -= 3;
	    outs += 3;
	    }
	}
    }
    
if (csize < 0)
    {
    printf("Decompression suspecte !\n"); // [Error]
    }    
return 0;

outfull:
printf("Warning : Output buffer full before end of decompression\n"); // [Error]
return -1;

}

/*----------------------------------------------------------------------*/

int decomp_rlc32(char *tmp_img, long csize, TargaRgbType *img)
{
int 		i, outs;
signed char	rep, *ptrs;
TargaRgbType 	*ptrd;
unsigned char	B, G, R;

i=0;
outs = 0;
ptrs = (signed char *)tmp_img;
ptrd = img;
while (csize > 0)
    {
    if ((rep = *ptrs++) & 0x80)			/* decompression */
        {
	rep -= 0x7f;
	
	B = *ptrs++;
	G = *ptrs++;
	R = *ptrs++;
	ptrs++;
	csize -= 5;
	while(rep--)		
	    {
	    ptrd->B = B;
	    ptrd->G = G;
	    ptrd->R = R;
	    ptrd++;
	    outs+=3;
	    }
	}
    else
        {
	csize--;
	rep++;
	while(rep--)				/* recopie simple */
	    {
	    ptrd->B = *ptrs++;
	    ptrd->G = *ptrs++;
	    ptrd->R = *ptrs++;
	    ptrs++;
	    ptrd++;
	    csize -= 4;
	    outs+=3;
	    }
	}
    }

return 0;
}




/*----------------------------------------------------------------------*/

void swap_tga_header(TARGA *tga_info)
{
    SWAP(tga_info->maporig);
    SWAP(tga_info->mapsize);
    SWAP(tga_info->xorig);
    SWAP(tga_info->yorig);
    SWAP(tga_info->xsize);
    SWAP(tga_info->ysize);
}

/*----------------------------------------------------------------------*/

int load_file(wchar_t *filename, TARGA *tga_info, char *pal, char *img)
{
int handle, res;
int palsize, datasize, pixsize;
int imgw, imgh;

    if ((handle = _wopen(filename, O_RDONLY|O_BINARY )) == -1 )
	{
		//printf("Cannot open file %s\n", filename); // [Error]
	return -1;
	}
    
    res = _read(handle, &(tga_info->numid), 1);
    res = _read(handle, &(tga_info->maptyp), 1);
    res = _read(handle, &(tga_info->imgtyp), 1);
    res = _read(handle, &(tga_info->maporig), 2);
    res = _read(handle, &(tga_info->mapsize), 2);
    res = _read(handle, &(tga_info->mapbits), 1);
    res = _read(handle, &(tga_info->xorig), 2);
    res = _read(handle, &(tga_info->yorig), 2);
    res = _read(handle, &(tga_info->xsize), 2);
    res = _read(handle, &(tga_info->ysize), 2);
    res = _read(handle, &(tga_info->pixsize), 1);
    res = _read(handle, &(tga_info->imgdes), 1);
    
	swap_tga_header(tga_info);
    
    imgw = tga_info->xsize;
    imgh = tga_info->ysize;

    pixsize = tga_info->pixsize;
    
    palsize = tga_info->mapsize * tga_info->mapbits / 8 * tga_info->maptyp;
    
    datasize = imgw*imgh*pixsize/8;


    /* sauter l'eventuel commentaire ici */

    if (tga_info->maptyp != 0)
    {
		/* _read palette */
		
		if ((pal = (char *)malloc(palsize)) == NULL)
		{
			return -1;
		}
		
		if ((res = _read(handle, pal, palsize)) < 0)
		{
			return -1;
		}
	}
    else
        pal = NULL;
	
    if ((img = (char *) malloc(datasize)) == NULL)
	{
		printf("Cannot allocate memory.\n"); // [Error]
		_close(handle);
		return -1;
	}
	
    if (tga_info->imgtyp == 02)				// RGB non compressé
    {
		if ((res = _read(handle, img, datasize)) < 0)
		{
			return -1;
		}
	}
    else if (tga_info->imgtyp == 10)		// RGB en RLE
    {
		char *tmp_img;
		long csize;
																
		csize = _lseek(handle, 0, SEEK_END) - 18;
																
		if ((tmp_img = (char *) malloc(csize+20)) == NULL)
		{
			printf("Cannot allocate memory L%d.\n", __LINE__); // [Error]
			_close(handle);
			return -1;
		}

		res = _lseek(handle, 18, SEEK_SET);
																
		if ((res = _read(handle, tmp_img, csize)) < 0)
		{
			return -1;
		}
																
		/*printf("Decompressing ...\n");*/
																
		if (tga_info->pixsize == 24)
        {
			decomp_rlc24(tmp_img, csize, (TargaRgbType*)img, datasize);
        }
		else 
        if (tga_info->pixsize == 32)
        {
			decomp_rlc32(tmp_img, csize, (TargaRgbType*)img);
        }
		else
        if (tga_info->pixsize == 8)
        {
			decomp_rlc8(tmp_img, csize, img);
        }
		else
		{
			printf("Only 8, 24 and 32 bits compressed images are supported\n"); // [Error]
			_close(handle);
			return -1;
		}
																	
		free(tmp_img);
	}
    else
	{
		printf("Image of type %d are not supported\n",tga_info->imgtyp); // [Error]
		_close(handle);
		return -1;
	}


    _close(handle);
    
    return 0;
}

/*----------------------------------------------------------------------*/

int save_tga(wchar_t *filename, TARGA *tga_info, char *pal, char *img)
{
    int 	handle, res;
    int		imgw, imgh, pixsize, datasize;
    int		palsize;
	short   *ptr;
	int		i;

    imgw    = tga_info->xsize;
    imgh    = tga_info->ysize;
    pixsize = tga_info->pixsize;
    palsize = tga_info->mapsize * tga_info->mapbits / 8 * tga_info->maptyp;
    datasize= imgw*imgh*pixsize/8;
    
    if ((handle = _wopen(filename, O_WRONLY | O_BINARY | O_CREAT | O_TRUNC, 0666 )) == -1)
    {
        return -1;
    }

	/* prepare l'entete et sauve l'entete */
	
	swap_tga_header(tga_info);
	res = _write(handle, &(tga_info->numid), 1);
	res = _write(handle, &(tga_info->maptyp), 1);
	res = _write(handle, &(tga_info->imgtyp), 1);
	res = _write(handle, &(tga_info->maporig), 2);
	res = _write(handle, &(tga_info->mapsize), 2);
	res = _write(handle, &(tga_info->mapbits), 1);
	res = _write(handle, &(tga_info->xorig), 2);
	res = _write(handle, &(tga_info->yorig), 2);
	res = _write(handle, &(tga_info->xsize), 2);
	res = _write(handle, &(tga_info->ysize), 2);
	res = _write(handle, &(tga_info->pixsize), 1);
	res = _write(handle, &(tga_info->imgdes), 1);
	swap_tga_header(tga_info);

	/* sauve la palette */
	if (palsize)
	{
		ASSERT(pal);
		res = _write(handle, pal, palsize);
	}

	if (tga_info->imgtyp !=	TGA_IMG_TYPE_NO_DATA)
	{
		if (   (tga_info->imgtyp == TGA_IMG_TYPE_PAL_UNCOMPRESSED)
		    || (tga_info->imgtyp == TGA_IMG_TYPE_RGB_UNCOMPRESSED)
			/*|| (tga_info->imgtyp == TGA_IMG_TYPE_BW_UNCOMPRESSED) */ )
		{
			/* not compressed */

			/* sauve l'image */
			//res = _write(handle, img, datasize);
			{	// Gere le pitch dans la sauvegarde.
				ptr = (short *)img; 
				ptr+=(imgh-1)*imgw;
				for (i=0;i<imgh;i++)
				{
					_write(handle, ptr, imgw*2);
					ptr -= imgw;
				}
			}
		}
		else if (   (tga_info->imgtyp == TGA_IMG_TYPE_PAL_COMPRESSED)
		         || (tga_info->imgtyp == TGA_IMG_TYPE_RGB_COMPRESSED)
		       /*|| (tga_info->imgtyp == TGA_IMG_TYPE_BW_COMPRESSED) */ )
		{
			/* compression RLC */
			unsigned char 	*img_comp=NULL;
			long		size_comp=0;

			if (pixsize != 8)
			{
				printf("Compression only supported for 256 color images\n"); // [Error]
			}

			/* sauve l'image */
    
			if ((img_comp = (unsigned char *)malloc(datasize*2))==0)
			{
				printf("Cannot allocate memory.\n"); // [Error]
				_close(handle);
				return -1;
			}

			size_comp = compress_rlc8((unsigned char *)img, imgw, imgh, img_comp);
			test_decomp_rlc8(img_comp,size_comp,(unsigned char *)img, datasize);
			res = _write(handle, img_comp, size_comp);
    
			free(img_comp);
    
			printf("Compression RLC : %d->%d octets\n",datasize,size_comp); // [Error]
		}
		else
		{
			printf("Warning: unknow Targa type in \"save_tga()\"\n");
		}
	}

    _close(handle);

    return 0;
}

/*----------------------------------------------------------------------*/
// pas d'inversion R & B entre palette et destination

void *convert_tga8_to_24(int nbData,void *pal, void *src)
{
	unsigned char  *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData*3);
	if (pDst)
	{
		int i;
		unsigned char *pPal=(unsigned char *)pal;
		unsigned char *pSrc=(unsigned char *)src;
		for (i=0; i<nbData; i++)
		{
			unsigned char index=*pSrc++;
			*pDst++=pPal[index*3];
			*pDst++=pPal[index*3+1];
			*pDst++=pPal[index*3+2];
		}
	}
	return pDstBuffer;
}

/*----------------------------------------------------------------------*/

void *convert_tga16_to_24(int nbData,void *src)
{
	int i;
	unsigned short *pSrc=(unsigned short *)src;
	unsigned char  *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData*3);
	if (pDst)
	{
		for (i=0; i<nbData; i++)
		{
			unsigned short data=*pSrc++;
			*pDst++=((data>>10)&0x1f)<<3;
			*pDst++=((data>> 5)&0x1f)<<3;
			*pDst++=((data    )&0x1f)<<3;
		}
	}
	return (void *)pDstBuffer;
}

/*----------------------------------------------------------------------*/

void *convert_tga24_to_24(int nbData,void *src)
{
	int i;
	unsigned char *pSrc=(unsigned char *)src;
	unsigned char *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData*3);
	if (pDst)
	{
		for (i=0; i<nbData; i++)
		{
			unsigned char r,g,b;
			b=*pSrc++;
			g=*pSrc++;
			r=*pSrc++;
			*pDst++=r;
			*pDst++=g;
			*pDst++=b;
		}
	}
	return (void *)pDstBuffer;
}

/*----------------------------------------------------------------------*/

void *convert_tga32_to_24(int nbData,void *src)
{
	int i;
	unsigned long *pSrc=(unsigned long *)src;
	unsigned char *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData*3);
	if (pDst)
	{
		for (i=0; i<nbData; i++)
		{
			unsigned long data=*pSrc++;
			*pDst++=(unsigned char)(data>>16);	// R
			*pDst++=(unsigned char)(data>> 8);	// G
			*pDst++=(unsigned char)(data    );	// B
		}
	}
	return (void *)pDstBuffer;
}

/*----------------------------------------------------------------------*/

// input must be a pointer to a 32 bits picture

void *tga_extract_alpha(int nbData,void *src)
{
	int i;
	unsigned long *pSrc=(unsigned long *)src;
	unsigned char *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData);
	if (pDst)
	{
		for (i=0; i<nbData; i++)
		{
			unsigned long data=*pSrc++;
			*pDst++=(unsigned char)(data>>24);
		}
	}
	return (void *)pDstBuffer;
}

/*----------------------------------------------------------------------*/

void *convert_tgabw_to_24(int nbData,void *src)
{
	int i;
	unsigned char *pSrc=(unsigned char *)src;
	unsigned char *pDstBuffer,*pDst;

	pDstBuffer=pDst=(unsigned char *)malloc(nbData*3);
	if (pDst)
	{
		for (i=0; i<nbData; i++)
		{
			unsigned char data=*pSrc++;
			*pDst++=data;
			*pDst++=data;
			*pDst++=data;
		}
	}
	return (void *)pDstBuffer;
}

/*----------------------------------------------------------------------*/

void *create_bw_palette(void)
{
	int i;
	unsigned char *pPalBuffer,*pPal;
	
	pPalBuffer=pPal=(unsigned char *)malloc(256*3);
	if (pPal)
	{
		for (i=0; i<256; i++)
		{
			*pPal++=i;
			*pPal++=i;
			*pPal++=i;
		}
	}
	return (void *)pPalBuffer;
}

/*----------------------------------------------------------------------*/
#if 0
void memswap(void *p1,void *p2,size_t _size)	// by Arnaud!
{
		__asm
		{
			mov	ecx,[_size]
			mov	esi,[p1]
			mov	edi,[p2]
			mov	edx,ecx
			shr	ecx,3
			je	short _little
_loop:		mov	eax,[esi]
			mov	ebx,[esi+4]
			xchg	eax,[edi]
			xchg	ebx,[edi+4]
			mov	[esi],eax
			mov	[esi+4],ebx
			add	esi,8
			add	edi,8
			dec	ecx
			jne	short _loop
_little:	mov	ecx,edx
			and	ecx,7
			je	short _over
_loop2:		mov	al,[esi]
			xchg	al,[edi]
			mov	[esi],al
			inc	esi
			inc	edi
			dec	ecx
			jne	short _loop2
_over:
		}
}
#else

void memswap(void* p1, void* p2, size_t _size) {
	uint8_t* ptr1 = (uint8_t*)p1;
	uint8_t* ptr2 = (uint8_t*)p2;
	uint8_t temp;

	// Swap in 8 byte blocks
	while (_size >= sizeof(uint64_t)) {
		uint64_t temp64 = *(uint64_t*)ptr1;
		*(uint64_t*)ptr1 = *(uint64_t*)ptr2;
		*(uint64_t*)ptr2 = temp64;

		ptr1 += sizeof(uint64_t);
		ptr2 += sizeof(uint64_t);
		_size -= sizeof(uint64_t);
	}

	// Swap remaining bytes one by one
	while (_size > 0) {
		temp = *ptr1;
		*ptr1 = *ptr2;
		*ptr2 = temp;

		ptr1++;
		ptr2++;
		_size--;
	}
}
#endif

/*----------------------------------------------------------------------*/

int flip_image(TARGA *pInfo,void *data)				// flip image vertically
{
	int y,lineSize;
	char *pTopPtr,*pBottomPtr;

	lineSize=((pInfo->pixsize+7)/8)*pInfo->xsize;

	pTopPtr=(char*)data;
	pBottomPtr=pTopPtr+pInfo->ysize*lineSize;
	for (y=0; y<pInfo->ysize/2; y++)
	{
		pBottomPtr-=lineSize;
		memswap(pBottomPtr,pTopPtr,lineSize);
		pTopPtr+=lineSize;
	}
	return TRUE;
}

/*----------------------------------------------------------------------*/

int mirror_image(TARGA *pInfo,void *data)			// flip image horizontally
{
	int x,y,pixelSize,lineSize;

	pixelSize=(pInfo->pixsize+7)/8;
	lineSize=pixelSize*pInfo->xsize;
	for (y=0; y<pInfo->ysize; y++)
	{
		char *pLeftPtr,*pRightPtr;
		pLeftPtr=(char*)data+lineSize*y;
		pRightPtr=pLeftPtr+lineSize;
		for (x=0; x<pInfo->xsize/2; x++)
		{
			pRightPtr-=pixelSize;
			memswap(pRightPtr,pLeftPtr,pixelSize);
			pLeftPtr+=pixelSize;
		}
	}
	return TRUE;
}
