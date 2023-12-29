#ifndef _PSXDEF_
#define _PSXDEF_
// NP 1996

#define Trans_Pal(r,v,b)		(((r)>>3)+(((v)>>3)<<5)+(((b)>>3)<<10)/*+(1<<15)*/)

#define CLUT4					(0) 
#define CLUT8					(1)
#define CLUT16					(2)	
#define CLUT24					(4)
#define CLUTCMASK				(7)
#define ISCLUT					(8)

#define CLUT_R(c)				((c)&0x1f)
#define CLUT_V(c)				(((c)>>5)&0x1f)
#define CLUT_B(c)				(((c)>>10)&0x1f)
#define CLUT_T(c)				((c)>>15)

#define getClut(x, y) 					(((y)<<6)|(((x)>>4)&0x3f))
#define getTPage(tp, abr, x, y)	((((tp)&0x3)<<7)|(((abr)&0x3)<<5)|(((y)&0x100)>>4)|(((x)&0x3ff)>>6))
#define getTPagePC(tp, x, y)		((((tp)&0x3)<<14)|((y)&0xf00)|(((x)&0x3fff)>>6))

#define CLUT_X(c)				(((c)&0x3f)<<4)
#define CLUT_Y(c)				(((c)>>6)&0x1ff)

#define TPAGE_X(t)				(((t)<<6)&0x3ff)
#define TPAGE_Y(t)				(((t)<<4)&0x100)
#define TPAGE_TP(t)				(((t)>>7)&0x3)
#define TPAGE_ABR(t)			(((t)>>5)&0x3)

#define TPAGEPC_X(t)				(((t)<<6)&0x3fff)
#define TPAGEPC_Y(t)				((t)&0xF00)
#define TPAGEPC_TP(t)				(((t)>>14)&0x3)

typedef struct 
{
	short x,y,w,h;
}psxRECT;

#endif
