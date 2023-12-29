#ifndef RGB_HLS_H
	#define RGB_HLS_H

    #include "Def.h"

	#ifdef HEADER_VERBOSE
		#pragma message("...RgbHls.h")
	#endif

	#define MAX_H			0x00FF
	#define HLS_H_UNDEFINED 0x0100
	#define MAX_L			0x00FF
	#define MAX_S   		0x00FF

	#define MAX_RGB 		0x003F
	#define MAX_SCREEN_RGB	0x00FF

	void RgbToHls(RGB_MEMBER pix_r,RGB_MEMBER pix_g,RGB_MEMBER pix_b,int *hls_h,int *hls_l,int *hls_s);
	void HlsToRgb(int hls_h,int hls_l,int hls_s,RGB_MEMBER *pix_r,RGB_MEMBER *pix_g,RGB_MEMBER *pix_b);

	void RgbToHlsDouble(double r,double g,double b,double *h,double *l,double *s);
	void HlsToRgbDouble(double h,double l,double s,double *r,double *g,double *b);

#endif
