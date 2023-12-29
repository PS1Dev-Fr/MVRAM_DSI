
///////////////////////////////////////////////////////////////////////////////
//// GLOBAL DEFINES USED BY GRAPHIC LIB                                    ////
///////////////////////////////////////////////////////////////////////////////

#ifndef _DEF_H_
#define _DEF_H_

#include <assert.h>
#define IDIRECTPLAY2_OR_GREATER
/*
Conventions pour retour de fonctions:

Return type		Valeur		Signification

--------------------------------------------------
	BOOL		TRUE		Pas d'erreur
				FALSE		Erreur
--------------------------------------------------
	int			0			Pas d'erreur
				<0			Code d'erreur
				>0			Parametre
--------------------------------------------------
	Pointeur	NULL		Erreur
				!=NULL		Adresse valide.
				
*/

#define   XRES_MAX		640		// DD screen x,y res
#define	  YRES_MAX		480

#define   XRES_MAX_WINDOW		(640)	// DD screen x,y res
#define	  YRES_MAX_WINDOW		(480)

#define	BOOL_OK		TRUE
#define	BOOL_ERROR	FALSE
#define	INT_OK		0
#define	PTR_ERROR	NULL

#define HLS_MAXSHIFT_H	7
#define HLS_MAXSHIFT_L	5
#define HLS_MAXSHIFT_S	4

#define HLS_MAX_H		((1<<HLS_MAXSHIFT_H)-1)
#define HLS_MAX_L		((1<<HLS_MAXSHIFT_L)-1)
#define HLS_MAX_S		((1<<HLS_MAXSHIFT_S)-1)

#define HLS_SHIFT_L		(16-HLS_MAXSHIFT_L)
#define HLS_SHIFT_H		(HLS_SHIFT_L-HLS_MAXSHIFT_H)
#define HLS_SHIFT_S		(HLS_SHIFT_H-HLS_MAXSHIFT_S)

#define HLS_MASK_H		(HLS_MAX_H<<HLS_SHIFT_H)
#define HLS_MASK_L		(HLS_MAX_L<<HLS_SHIFT_L)
#define HLS_MASK_S		(HLS_MAX_S<<HLS_SHIFT_S)

#define MAXFOG			(1<<HLS_MAXSHIFT_L)


#define GFX_INLINE inline
#define GFX_BLITTER 1
#define GFX_BLITTERMIN	128

#define GFX_SHIFT       15
#define GFX_PIXELFSHIFT 15

#define GFX_MAXSCREENW  2048
#define GFX_MAXSCREENH  2048

#define GFX_NEAR_PLANE  16

#define GFX_TRUE        (0==0)
#define GFX_FALSE       (0!=0)

#ifdef	PI
#undef	PI
#endif
#define	PI	3.141592653589793238446264

typedef signed long     SD;
typedef signed short    SW;
typedef signed char     SB;

typedef unsigned long   UD;
typedef unsigned short  UW;
typedef unsigned char   UB;

#define SDMIN   0x80000000
#define SWMIN   0x8000
#define SBMIN   0x80
#define SDMAX   0x7fffffff
#define SWMAX   0x7fff
#define SBMAX   0x7f
#define UDMIN   0x0
#define UWMIN   0x0
#define UBMIN   0x0
#define UDMAX   0xffffffff
#define UWMAX   0xffff
#define UBMAX   0xff
#ifndef BOOL
typedef int	BOOL;
#endif

typedef int (*PTRFUNC)(void *); 



// ENUMs

enum
{
	MODE_TRAINING,
	MODE_ONERACE,
	MODE_CHAMPION,
};

enum
{
	DETAIL_VERYLOW,
	DETAIL_LOW,
	DETAIL_MEDIUM,
	DETAIL_HIGHT,
	DETAIL_VERYHIGHT,

	DETAIL_NBLEVEL,
};

enum
{
	SKILL_EASY,
	SKILL_MEDIUM,
	SKILL_HARD,

	SKILL_NB,
};


#define NB_MATERIAL				9




// Mode d'écran
#define LOWRES	0			// 320x240
#define MEDRES  1			// 512x384
#define HIRES	2			// 640x480

// Définition d'un type pixel RGB 24 bits
#define RGB_MIN          0
#define RGB_MAX        255

typedef unsigned char  RGB_MEMBER;
typedef struct
{
	RGB_MEMBER r;
	RGB_MEMBER g;
	RGB_MEMBER b;
}   RGB;

#define NB_TEXTURE_BANK		20
#define MAX_TEXTURE_BANK	(NB_TEXTURE_BANK+16)
#define MAXLEVEL			6

#define FILE_NAME_LEN       64

// noms des fichiers propres au level
typedef struct 
{
  char fileNameBnk      [NB_TEXTURE_BANK][FILE_NAME_LEN]; //NB_TEXTURE_BANK-2
  char fileNameLnd      [FILE_NAME_LEN];
  char fileNameFrames3d [FILE_NAME_LEN];
  char fileNameDecor    [FILE_NAME_LEN];
  char fileNameDecorInv [FILE_NAME_LEN];
  char fileNameNet      [FILE_NAME_LEN];
  char fileNameMap		[FILE_NAME_LEN];
  char fileNameMoto		[FILE_NAME_LEN];
  char fileNameMotoPocket[FILE_NAME_LEN];
  char fileNameBalise	[FILE_NAME_LEN];
  char fileNameSmallMap [FILE_NAME_LEN];
} fileLevel;

#define	MAXPILOTROT		9
#define	MAXMOTO			8

// noms des fichiers propres a la moto
typedef struct 
{
  char fileNameBnk0				[FILE_NAME_LEN];
  char fileNameBnk1				[FILE_NAME_LEN];
  char fileNameMotoSkl			[FILE_NAME_LEN];
  char fileNameRoueArriere3df   [FILE_NAME_LEN];
  char fileNameRoueArriereMap   [FILE_NAME_LEN];
  char fileNameRoueAvant3df		[FILE_NAME_LEN];
  char fileNameRoueAvantMap		[FILE_NAME_LEN];
  char fileNameRoueArriereH3df	[FILE_NAME_LEN];
  char fileNameRoueArriereHMap	[FILE_NAME_LEN];
  char fileNameRoueAvantH3df	[FILE_NAME_LEN];
  char fileNameRoueAvantHMap	[FILE_NAME_LEN];
  char fileNameMotoMedMap		[FILE_NAME_LEN];
  char fileNameMotoMed3df		[FILE_NAME_LEN];
  char fileNamePilotRot			[MAXPILOTROT][FILE_NAME_LEN];
  char fileNameCarac			[MAXMOTO][FILE_NAME_LEN];
} fileMoto;

// Parametre de la moto
typedef struct 
{
	float	p_friction_air;
	float	p_friction_sol;
	float	p_turbo_friction_sol;
	float	p_turbo_frein;
	float	p_frein;
	float	p_moteur_acceleration;
	float	p_moteur_vitesse_max;
	float	p_moto_masse;					// en kg
	float	p_retard_cadre;
	float	p_roue_acceleration_angulaire;
	float	p_roue_max_acceleration_angulaire;
	float	p_roue_vitesse_retour;
	float	p_roue_zrot;
	float	p_angle_suspension;
	float	p_roue_limite_angulaire;
	float	p_roue_axe_distance;
	float	p_roue_rayon_av;
	float	p_roue_rayon_ar;
	float	p_roue_rayon;
	float	p_real_speed_max;				// en metres/seconde
	float	vitesseMaxBoite;				// pour calculer les rapports de boite (boite longue ou courte)
	float	p_rotation_transfert;
	float	p_div_transfert;
	float	p_hauteur_cadre;
	float	p_ressor_amplitude_dec;
	float	p_ressor_amplitude_min;
	float	p_ressor_amplitude_factor;
	float	p_fourche_angleAvant;
	float	p_fourche_angleArriere;
	float	nbSpeed;
	float	regimeMax;						// en tours/min
	float	rapportDeBoite[6];				// 6 vitesses max
	float	regimeCoupleMax;				// régime du couple maxi en tours/min
	float	coupleMax;						// couple maxi en mkg
	float	couple1000;						// couple à 1000 tours/min
	float	coupleRegimeMax;				// couple au régime maxi
	float	zoneRouge;						// en pourcentage du régime maxi
	float	minAdherenceWheel;				// directivityCoef min en frein roue bloquée
	float	minAdherenceDerap[SKILL_NB];	// directivityCoef min en derapage
	float	material[NB_MATERIAL][SKILL_NB];
	int		persoAnim;						// type d'anim: 0=vitesse, 1=cross
} paramMoto;

// CheckPoint

typedef  struct
{
	int id;
	int time;
} checkPoint;


// Parametre du niveau de détail à l'écran
typedef struct 
{
	float lenRLPlane;
	float lenBPlane;
	float flatZ;
	float horizFlatZ;
	int	  mediumZone;
	int	  hightZone;
	float gouraudTexturedZ;
	float texturedZ;
	float gouraudZ;
} detailLevel;

enum
{
	RATTRAPAGE_SPEED,		// rattrapage quand derriere le joueur et dist>4096
	RATTRAPAGE_CURVE,
	RATTRAPAGE_COUPLE,
	ATTENTE1_SPEED,			// position du joueur > nbPlayers/2
	ATTENTE1_CURVE,
	ATTENTE1_COUPLE,
	ATTENTE2_SPEED,			// position du joueur <= nbPlayers/2
	ATTENTE2_CURVE,
	ATTENTE2_COUPLE,
	CURVE_RANGE,
	COUPLE_OUT_RANGE,
	COUPLE_IN_RANGE,
	SPEED_FACTOR,
	END_COMPUTER_SPEED
};

/***************************************************************************************
*                                                                                      *
*      Utilisation de nombres binaires dans les sources C(++)                          *
*                                                                                      *
***************************************************************************************/
#ifndef __BINARY__
#define __BINARY__

#define	B1(a)	((a<<0))
#define	B2(a,b)	((a<<1)|(b<<0))
#define	B3(a,b,c)	((a<<2)|(b<<1)|(c<<0))
#define	B4(a,b,c,d)	((a<<3)|(b<<2)|(c<<1)|(d<<0))
#define	B5(a,b,c,d,e)	((a<<4)|(b<<3)|(c<<2)|(d<<1)|(e<<0))
#define	B6(a,b,c,d,e,f)	((a<<5)|(b<<4)|(c<<3)|(d<<2)|(e<<1)|(f<<0))
#define	B7(a,b,c,d,e,f,g)	((a<<6)|(b<<5)|(c<<4)|(d<<3)|(e<<2)|(f<<1)|(g<<0))
#define	B8(a,b,c,d,e,f,g,h)	((a<<7)|(b<<6)|(c<<5)|(d<<4)|(e<<3)|(f<<2)|(g<<1)|(h<<0))
#define	B9(a,b,c,d,e,f,g,h,i)	((a<<8)|(b<<7)|(c<<6)|(d<<5)|(e<<4)|(f<<3)|(g<<2)|(h<<1)|(i<<0))
#define	B10(a,b,c,d,e,f,g,h,i,j)	((a<<9)|(b<<8)|(c<<7)|(d<<6)|(e<<5)|(f<<4)|(g<<3)|(h<<2)|(i<<1)|(j<<0))
#define	B11(a,b,c,d,e,f,g,h,i,j,k)	((a<<10)|(b<<9)|(c<<8)|(d<<7)|(e<<6)|(f<<5)|(g<<4)|(h<<3)|(i<<2)|(j<<1)|(k<<0))
#define	B12(a,b,c,d,e,f,g,h,i,j,k,l)	((a<<11)|(b<<10)|(c<<9)|(d<<8)|(e<<7)|(f<<6)|(g<<5)|(h<<4)|(i<<3)|(j<<2)|(k<<1)|(l<<0))
#define	B13(a,b,c,d,e,f,g,h,i,j,k,l,m)	((a<<12)|(b<<11)|(c<<10)|(d<<9)|(e<<8)|(f<<7)|(g<<6)|(h<<5)|(i<<4)|(j<<3)|(k<<2)|(l<<1)|(m<<0))
#define	B14(a,b,c,d,e,f,g,h,i,j,k,l,m,n)	((a<<13)|(b<<12)|(c<<11)|(d<<10)|(e<<9)|(f<<8)|(g<<7)|(h<<6)|(i<<5)|(j<<4)|(k<<3)|(l<<2)|(m<<1)|(n<<0))
#define	B15(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o)	((a<<14)|(b<<13)|(c<<12)|(d<<11)|(e<<10)|(f<<9)|(g<<8)|(h<<7)|(i<<6)|(j<<5)|(k<<4)|(l<<3)|(m<<2)|(n<<1)|(o<<0))
#define	B16(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p)	((a<<15)|(b<<14)|(c<<13)|(d<<12)|(e<<11)|(f<<10)|(g<<9)|(h<<8)|(i<<7)|(j<<6)|(k<<5)|(l<<4)|(m<<3)|(n<<2)|(o<<1)|(p<<0))
#define	B17(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q)	((a<<16)|(b<<15)|(c<<14)|(d<<13)|(e<<12)|(f<<11)|(g<<10)|(h<<9)|(i<<8)|(j<<7)|(k<<6)|(l<<5)|(m<<4)|(n<<3)|(o<<2)|(p<<1)|(q<<0))
#define	B18(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r)	((a<<17)|(b<<16)|(c<<15)|(d<<14)|(e<<13)|(f<<12)|(g<<11)|(h<<10)|(i<<9)|(j<<8)|(k<<7)|(l<<6)|(m<<5)|(n<<4)|(o<<3)|(p<<2)|(q<<1)|(r<<0))
#define	B19(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s)	((a<<18)|(b<<17)|(c<<16)|(d<<15)|(e<<14)|(f<<13)|(g<<12)|(h<<11)|(i<<10)|(j<<9)|(k<<8)|(l<<7)|(m<<6)|(n<<5)|(o<<4)|(p<<3)|(q<<2)|(r<<1)|(s<<0))
#define	B20(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t)	((a<<19)|(b<<18)|(c<<17)|(d<<16)|(e<<15)|(f<<14)|(g<<13)|(h<<12)|(i<<11)|(j<<10)|(k<<9)|(l<<8)|(m<<7)|(n<<6)|(o<<5)|(p<<4)|(q<<3)|(r<<2)|(s<<1)|(t<<0))
#define	B21(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u)	((a<<20)|(b<<19)|(c<<18)|(d<<17)|(e<<16)|(f<<15)|(g<<14)|(h<<13)|(i<<12)|(j<<11)|(k<<10)|(l<<9)|(m<<8)|(n<<7)|(o<<6)|(p<<5)|(q<<4)|(r<<3)|(s<<2)|(t<<1)|(u<<0))
#define	B22(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v)	((a<<21)|(b<<20)|(c<<19)|(d<<18)|(e<<17)|(f<<16)|(g<<15)|(h<<14)|(i<<13)|(j<<12)|(k<<11)|(l<<10)|(m<<9)|(n<<8)|(o<<7)|(p<<6)|(q<<5)|(r<<4)|(s<<3)|(t<<2)|(u<<1)|(v<<0))
#define	B23(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w)	((a<<22)|(b<<21)|(c<<20)|(d<<19)|(e<<18)|(f<<17)|(g<<16)|(h<<15)|(i<<14)|(j<<13)|(k<<12)|(l<<11)|(m<<10)|(n<<9)|(o<<8)|(p<<7)|(q<<6)|(r<<5)|(s<<4)|(t<<3)|(u<<2)|(v<<1)|(w<<0))
#define	B24(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x)	((a<<23)|(b<<22)|(c<<21)|(d<<20)|(e<<19)|(f<<18)|(g<<17)|(h<<16)|(i<<15)|(j<<14)|(k<<13)|(l<<12)|(m<<11)|(n<<10)|(o<<9)|(p<<8)|(q<<7)|(r<<6)|(s<<5)|(t<<4)|(u<<3)|(v<<2)|(w<<1)|(x<<0))
#define	B25(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y)	((a<<24)|(b<<23)|(c<<22)|(d<<21)|(e<<20)|(f<<19)|(g<<18)|(h<<17)|(i<<16)|(j<<15)|(k<<14)|(l<<13)|(m<<12)|(n<<11)|(o<<10)|(p<<9)|(q<<8)|(r<<7)|(s<<6)|(t<<5)|(u<<4)|(v<<3)|(w<<2)|(x<<1)|(y<<0))
#define	B26(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z)	((a<<25)|(b<<24)|(c<<23)|(d<<22)|(e<<21)|(f<<20)|(g<<19)|(h<<18)|(i<<17)|(j<<16)|(k<<15)|(l<<14)|(m<<13)|(n<<12)|(o<<11)|(p<<10)|(q<<9)|(r<<8)|(s<<7)|(t<<6)|(u<<5)|(v<<4)|(w<<3)|(x<<2)|(y<<1)|(z<<0))
#define	B27(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa)	((a<<26)|(b<<25)|(c<<24)|(d<<23)|(e<<22)|(f<<21)|(g<<20)|(h<<19)|(i<<18)|(j<<17)|(k<<16)|(l<<15)|(m<<14)|(n<<13)|(o<<12)|(p<<11)|(q<<10)|(r<<9)|(s<<8)|(t<<7)|(u<<6)|(v<<5)|(w<<4)|(x<<3)|(y<<2)|(z<<1)|(aa<<0))
#define	B28(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab)	((a<<27)|(b<<26)|(c<<25)|(d<<24)|(e<<23)|(f<<22)|(g<<21)|(h<<20)|(i<<19)|(j<<18)|(k<<17)|(l<<16)|(m<<15)|(n<<14)|(o<<13)|(p<<12)|(q<<11)|(r<<10)|(s<<9)|(t<<8)|(u<<7)|(v<<6)|(w<<5)|(x<<4)|(y<<3)|(z<<2)|(aa<<1)|(ab<<0))
#define	B29(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac)	((a<<28)|(b<<27)|(c<<26)|(d<<25)|(e<<24)|(f<<23)|(g<<22)|(h<<21)|(i<<20)|(j<<19)|(k<<18)|(l<<17)|(m<<16)|(n<<15)|(o<<14)|(p<<13)|(q<<12)|(r<<11)|(s<<10)|(t<<9)|(u<<8)|(v<<7)|(w<<6)|(x<<5)|(y<<4)|(z<<3)|(aa<<2)|(ab<<1)|(ac<<0))
#define	B30(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad)	((a<<29)|(b<<28)|(c<<27)|(d<<26)|(e<<25)|(f<<24)|(g<<23)|(h<<22)|(i<<21)|(j<<20)|(k<<19)|(l<<18)|(m<<17)|(n<<16)|(o<<15)|(p<<14)|(q<<13)|(r<<12)|(s<<11)|(t<<10)|(u<<9)|(v<<8)|(w<<7)|(x<<6)|(y<<5)|(z<<4)|(aa<<3)|(ab<<2)|(ac<<1)|(ad<<0))
#define	B31(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae)	((a<<30)|(b<<29)|(c<<28)|(d<<27)|(e<<26)|(f<<25)|(g<<24)|(h<<23)|(i<<22)|(j<<21)|(k<<20)|(l<<19)|(m<<18)|(n<<17)|(o<<16)|(p<<15)|(q<<14)|(r<<13)|(s<<12)|(t<<11)|(u<<10)|(v<<9)|(w<<8)|(x<<7)|(y<<6)|(z<<5)|(aa<<4)|(ab<<3)|(ac<<2)|(ad<<1)|(ae<<0))
#define	B32(a,b,c,d,e,f,g,h,i,j,k,l,m,n,o,p,q,r,s,t,u,v,w,x,y,z,aa,ab,ac,ad,ae,af)	((a<<31)|(b<<30)|(c<<29)|(d<<28)|(e<<27)|(f<<26)|(g<<25)|(h<<24)|(i<<23)|(j<<22)|(k<<21)|(l<<20)|(m<<19)|(n<<18)|(o<<17)|(p<<16)|(q<<15)|(r<<14)|(s<<13)|(t<<12)|(u<<11)|(v<<10)|(w<<9)|(x<<8)|(y<<7)|(z<<6)|(aa<<5)|(ab<<4)|(ac<<3)|(ad<<2)|(ae<<1)|(af<<0))

#endif


#endif




