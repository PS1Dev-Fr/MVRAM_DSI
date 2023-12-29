#ifndef FILE_RAW_DEF_H
    #define FILE_RAW_DEF_H
    
	#define FILE_RAW_DEF_ID "mhwanh"
    #define FILE_RAW_DEF_VERSION 0x0004
    
    typedef struct
    {
    	char	nId[6];
    	short	nVersion;
    	short	nWidth;
    	short	nHeight;
    	short	nPalSize;
    	short	nHorizontalDpi;
    	short	nVerticalDpi;
    	short	nGamma;
    	char	tabFiller[12];
    }	FileRawHeaderType;
    
    
/*
             Magic number  Six bytes used to identify the file as a HSI Raw
                           file:

                                0x6d 0x68 0x77 0x61 0x6e 0x68

								"mhwanh"

                  Version  An integer used to identify the version HSI
                           file:

                                0x0004

                    Width  An integer indicating the width of the image (in
                           pixels).

                   Height  An integer indicating the height of the image
                           (in pixels).

             Palette size  An integer indicating the number of entries in
                           the palette.  Range is 2 to 256.  A 0 indicates
                           a true colour image (which has no palette data).

           Horizontal DPI  An integer indicating the horizontal resolution
                           of the image, in dots per inch.  A zero
                           indicates that the resolution is unknown.  A
                           negative number is used if only the aspect ratio
                           is known.

             Vertical DPI  An integer indicating the vertical resolution of
                           the image, in dots per inch.  A zero indicates
                           that the resolution is unknown.  A negative
                           number is used if only the aspect ratio is
                           known.

                    Gamma  An integer indicating the gamma of the image,
                           scaled by 100 (a gamma of 2.2 is stored as 220).
                           A zero indicates that the gamma is not known.

                 Reserved  Twelve bytes reserved for future use.  Should be
                           set to zero when writing.
			   
*/

#endif    
