#include <sys\types.h>
#include <libetc.h>
#include <libgte.h>
#include <libgpu.h>
#include <malloc.h>
#include <memory.h>
#include <dsifile.h>
#include <dsijoy.h>




typedef struct
{
	int PatNo;
	int ModeView;
	int z;
	int x,y;

}DATAJOY;

void PatInf(int Val,void *Data,void *LData);
void PatSup(int Val,void *Data,void *LData);
void Patzm(int Val,void *Data,void *LData);
void Patzp(int Val,void *Data,void *LData);
void SetModeView(int Val,void *Data,void *LData);
void ViewUp(int Val,void *Data,void *LData);
void ViewDown(int Val,void *Data,void *LData);
void ViewLeft(int Val,void *Data,void *LData);
void ViewRight(int Val,void *Data,void *LData);
void ViewTA(int Val,void *Data,void *LData);
void ViewTAY(int Val,void *Data,void *LData);




DEFJOY DefJoyBlk=
{// PAD DEFINITIONS
	NULL,
{
	Patzm,//PadLup
	Patzp,//PadLdown
	PatInf,//PadLleft
	PatSup,//PadLright
	NULL,//PadRup
	SetModeView,//PadRdown
	NULL,//PadRleft
	NULL,//PadRright
	NULL,//PadL1
	NULL,//PadL2
	NULL,//PadR1
	NULL,//PadR2
	NULL,//Padstart
	NULL//Padselect
}
,//MOUSE DEFINITIONS
{
	SetModeView,//BL
	NULL,//BR
	NULL,//X
	NULL,//Y
	NULL,//XR
	NULL//YR
}
,//NEGCON DEFINITIONS
{
	NULL,//BA
	NULL,//BB
	NULL,//BR
	NULL,//BL
	SetModeView,//BS
	NULL,//BI
	NULL,//BII
	NULL,//Up
	NULL,//Down
	PatSup,//Right
	PatInf,//Left
	NULL//Twist
}};

DEFJOY DefJoyView=
{// PAD DEFINITIONS
	NULL,
{
	ViewUp,//PadLup
	ViewDown,//PadLdown
	ViewLeft,//PadLleft
	ViewRight,//PadLright
	NULL,//PadRup
	SetModeView,//PadRdown
	NULL,//PadRleft
	NULL,//PadRright
	NULL,//PadL1
	NULL,//PadL2
	NULL,//PadR1
	NULL,//PadR2
	NULL,//Padstart
	NULL//Padselect
}
,//MOUSE DEFINITIONS
{
	SetModeView,//BL
	NULL,//BR
	NULL,//X
	NULL,//Y
	ViewTA,//XR
	ViewTAY//YR
}
,//NEGCON DEFINITIONS
{
	NULL,//BA
	NULL,//BB
	NULL,//BR
	NULL,//BL
	SetModeView,//BS
	NULL,//BI
	NULL,//BII
	ViewUp,//Up
	ViewDown,//Down
	ViewRight,//Right
	ViewLeft,//Left
	ViewTA//Twist
}};

typedef struct
{
	u_char	u0, v0;	u_short	clut;
	u_char	u1, v1;	u_short	tpage;
	u_char	u2, v2, u3, v3;
}BLKPSX;

int ModeView=0;
DRAWENV  draw[2];
DISPENV  disp[2];

BLKPSX *Blk;
int NbBlk;
FILEMALLOC fmBlk;


BOOL LoadVrm(char *filename)
{
	RECT *rc;
	u_char *Ptr;

	if(LoadFileMalloc(filename,&fmBlk)==0)
		return 0;


	Ptr=fmBlk.Adr;

	NbBlk=*(int *)Ptr;

	Ptr+=4;

	Blk=(BLKPSX *)Ptr;
	Ptr+=sizeof(BLKPSX)*NbBlk;
	rc=Ptr;
	Ptr+=sizeof(RECT);

	LoadImage(rc,(unsigned long *)Ptr);
	DrawSync(0);


	if((fmBlk.Adr=realloc(fmBlk.Adr,sizeof(BLKPSX)*NbBlk+4))==NULL)
	{
		#ifdef SNDDBGTXT
	  printf ("VB realloc Error \n");
		#endif
	  return(0);
	}

	return 1;
}

BOOL CloseVrm(void)
{
	if(FreeFile(&fmBlk)==0)
		return 0;
	return 1;
}

void Patzm(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYUP)
	{
		if(Data->z>0)
			Data->z--;

	}
}

void Patzp(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYUP)
	{
			Data->z++;
	}
}

void PatInf(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYUP)
	{
		if(Data->PatNo>0)
			Data->PatNo--;

	}
}

void PatSup(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYUP)
	{
		if(Data->PatNo<NbBlk-1)
			Data->PatNo++;
	}
}

void SetModeView(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYUP)
	{
		Data->ModeView^=1;
	}
}

void ViewUp(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYDOWN)
	{
		Data->y-=2;
	}
}
void ViewDown(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYDOWN)
	{
		Data->y+=2;
	}
}
void ViewLeft(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYDOWN)
	{
		Data->x-=2;
	}
}
void ViewRight(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;

	if(Val==KEYDOWN)
	{
		Data->x+=2;
	}
}

void ViewTA(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;
	Data->x+=Val/4;
}
void ViewTAY(int Val,void *DataI,void *LData)
{
	DATAJOY *Data=(DATAJOY *)DataI;
	Data->y+=Val/4;
}


void Vswap(void)
{
	static int id=0;
	DrawSync(0);
	VSync(0);
	PutDrawEnv(&draw[id]);
	PutDispEnv(&disp[id]);

	id = id? 0: 1;
}


void AffBlk(int x,int y,int w,int h,BLKPSX	*blk)
{
	POLY_FT4 ft4;
	int M;
	RECT rc;

	setPolyFT4(&ft4);
	SetShadeTex (&ft4,1);



	ft4.u0=blk->u0;
	ft4.v0=blk->v0;
	ft4.u1=blk->u1;
	ft4.v1=blk->v1;
	ft4.u2=blk->u2;
	ft4.v2=blk->v2;
	ft4.u3=blk->u3;
	ft4.v3=blk->v3;
	ft4.clut=blk->clut;
	ft4.tpage=blk->tpage;

	//setXYWH(&ft4,x,y,w,h);
	setXYWH(&ft4,x,y,(blk->u1-blk->u0+1)*w,(blk->v2-blk->v0+1)*h);

	DrawPrim((u_long *)&ft4);
	DrawSync(0);


	FntPrint("Coor : u0 %3d v0 %3d\n",blk->u0,blk->v0);
	FntPrint("Coor : u1 %3d v1 %3d\n",blk->u1,blk->v1);
	FntPrint("Coor : u2 %3d v2 %3d\n",blk->u2,blk->v2);
	FntPrint("Coor : u3 %3d v3 %3d\n",blk->u3,blk->v3);
	FntPrint("Tpage: %4x (%s,%3d,%3d,%3d)\n",blk->tpage,(blk->tpage>>7)&0x003?"256c":" 16c",(blk->tpage>>5)&0x003,(blk->tpage&0xf)<<(7+M),(blk->tpage<<4)&0x100);
	//M=(1-((blk->tpage16>>7)&0x001));
	FntPrint("Clut   : (%4d,%3d)\n", (blk->clut&0x3f)<<4, (blk->clut>>6));

}



void VblCallback(void)
{
	//VblFileSystem();
	asm("break 1024");
}

void VBLInit(int mode)
{
static void VblCallback(void);

	printf("Vbl Init\n");
	EnterCriticalSection();
	if (!mode)
		VSyncCallback((void *)VblCallback);
	else
		VSyncCallback(0);
	ExitCriticalSection();
}



void main(void)
{

	DATAJOY DataJoy={0,0,1,0,0};

	ResetCallback();

	ResetGraph(0);
	SetGraphDebug(0);

	InitGeom();
	SetGeomOffset(160, 128);
	SetGeomScreen(256);

	SetVideoMode(MODE_PAL);

	SetDefDrawEnv(&draw[0],   0,     0, 320, 256);
	SetDefDispEnv(&disp[0],   0,   256, 320, 256);
	SetDefDrawEnv(&draw[1],   0,   256, 320, 256);
	SetDefDispEnv(&disp[1],   0,     0, 320, 256);

	draw[0].isbg = draw[1].isbg =  1;
	draw[0].dtd = draw[1].dtd =  1;

	setRGB0(&draw[0], 0, 0, 0);
	setRGB0(&draw[1], 0, 0, 0);

	PutDrawEnv(&draw[0]);
	PutDispEnv(&disp[0]);

	SetDispMask(1);
	VBLInit(0);


	InitFileSystem(FSHD|FSACTHD,NULL,"c:\\dsed\\mvram");


	LoadVrm("tst.vrm");

	FntLoad(640,256);
 	SetDumpFnt(FntOpen(16,32,320,240,0,1024));

	InitScanJoy(TAPINIT,2);


	do
	{
		if(DataJoy.ModeView)
		{
			ScanJoy(&DefJoyView,&DataJoy);

			if(DataJoy.ModeView)
			{
				disp[1].disp.x=DataJoy.x;
				disp[1].disp.y=DataJoy.y;

				PutDispEnv(&disp[1]);
			}
			else
			{
				disp[1].disp.x=0;
				disp[1].disp.y=0;
				PutDispEnv(&disp[1]);
			}
			VSync(0);
		}
		else
		{
			ScanJoy(&DefJoyBlk,&DataJoy);
			FntPrint("PatNo %d\n",DataJoy.PatNo);
			AffBlk(30,120,DataJoy.z,DataJoy.z,&Blk[DataJoy.PatNo]);
 			FntFlush(-1);
			Vswap();
		}
	}
	while(1);

	CloseScanJoy();
	VBLInit(1);
	CloseVrm();
	CloseFileSystem();
}

