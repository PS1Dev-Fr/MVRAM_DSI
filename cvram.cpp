#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "ViewDx.h"
#include "cimg.h"
#include "cvram.h"
#include "stringconversion.h"
#include "scanne.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

//****************************************************************


CVramAutoPos::CVramAutoPos(CProgressDlg *Pgs,CViewDx *Vdd)
{
	int i;
	Sprites=NULL;

	ScanData *pScanData=GetScanData();



	pScanData->LogPtr=Scr;
	UsedPages=0;
	NumPage=0;
	max_page=0;
	Page=NULL;
	NbSprites=0;
	SPgs=Pgs;
	SVdd=Vdd;
	for(i=0;i<256*3;i++) Pal[i]=rand();
	Pal[0]=0;
	Pal[1]=0;
	Pal[2]=0;
	No16x256=1;

	memset(Scr,0,640*480);
}

CVramAutoPos::~CVramAutoPos(void)
{
	if(Page!=NULL)
		delete [] Page;
}

void CVramAutoPos::InitVramX(int p,int x)
{
	int i,j;
	ScanData* pScanData = GetScanData();

	pScanData->OccupePtr=Page[p].VRamOccup;
	pScanData->CouleurPtr=Page[p].VRamCoul;
	pScanData->PVRamNum=Page[p].VRamNum;

	for(j=0;j<256;j++)
	{
		for(i=0;i<x;i++)
		{
			*(pScanData->OccupePtr +j*256+i)=0xff;
			*(pScanData->CouleurPtr +j*256+i)=(((i+j)&1)*20)+18;
			*(pScanData->PVRamNum +j*256+i)=(((i+j)&1)*0xffff);
		}
	}
}

void CVramAutoPos::InitVramY(int p,int y)
{
	int i,j;

	ScanData* pScanData = GetScanData();

	pScanData->OccupePtr = Page[p].VRamOccup;
	pScanData->CouleurPtr = Page[p].VRamCoul;
	pScanData->PVRamNum = Page[p].VRamNum;

	for(j=0;j<y;j++)
	{
		for(i=0;i<256;i++)
		{
			*(pScanData->OccupePtr +j*256+i)=0xff;
			*(pScanData->CouleurPtr +j*256+i)=(((i+j)&1)*20)+18;
			*(pScanData->PVRamNum +j*256+i)=(((i+j)&1)*0xffff);
		}
	}
}

void CVramAutoPos::InitVramW(int p,int x)
{
	int i,j;
	ScanData* pScanData = GetScanData();

	pScanData->OccupePtr = Page[p].VRamOccup;
	pScanData->CouleurPtr = Page[p].VRamCoul;
	pScanData->PVRamNum = Page[p].VRamNum;

	for(j=0;j<256;j++)
	{
		for(i=x;i<256;i++)
		{
			*(pScanData->OccupePtr +j*256+i)=0xff;
			*(pScanData->CouleurPtr +j*256+i)=(((i+j)&1)*20)+18;
			*(pScanData->PVRamNum +j*256+i)=(((i+j)&1)*0xffff);
		}
	}
}

void CVramAutoPos::InitVramH(int p,int y)
{
	int i,j;
	ScanData* pScanData = GetScanData();

	pScanData->OccupePtr = Page[p].VRamOccup;
	pScanData->CouleurPtr = Page[p].VRamCoul;
	pScanData->PVRamNum = Page[p].VRamNum;

	for(j=y;j<256;j++)
	{
		for(i=0;i<256;i++)
		{
			*(pScanData->OccupePtr +j*256+i)=0xff;
			*(pScanData->CouleurPtr +j*256+i)=(((i+j)&1)*20)+18;
			*(pScanData->PVRamNum +j*256+i)=(((i+j)&1)*0xffff);
		}
	}
}



int CVramAutoPos::SetZone(psxRECT &Zone,CVram *VRam)
{
	int i,j,k,x,y,w,h;
	int xd,yd,wd,hd;
	int NbPageW,NbPageH;
	CString s;

	x=Zone.x;
	y=Zone.y;
	w=Zone.w;
	h=Zone.h;
	
	xd=(x&255); 
	yd=(y&255);

	wd=xd+w;
	hd=yd+h;

	NbPageW=(wd+255)/256;
	NbPageH=(hd+255)/256;
	
	UsedPages=0;
	NumPage=0;
	max_page=NbPageW*NbPageH;
	Page=new VRAMPAGE[max_page];

	if(Page==NULL) return 0;
	ScanData* pScanData = GetScanData();


	k=0;
	for(j=0;j<NbPageH;j++)
	{
		for(i=0;i<NbPageW;i++)
		{
			
			pScanData->OccupePtr =Page[k].VRamOccup;
			pScanData->CouleurPtr =Page[k].VRamCoul;
			pScanData->PVRamNum =Page[k].VRamNum;
			memset(pScanData->PVRamNum,0,256*256*2);
			InitVRamMap();

			Page[k].Num=0;


			if(i!=NbPageW-1&&j!=NbPageH-1&&i!=0&&j!=0) //cas normal
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;

				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=256;
				Page[k].rc.h=256;
			}
			else
			if(NbPageW==1&&NbPageH==1)				  // 1 seul bloc
			{
				Page[k].rc.x=x+i*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=w;
				Page[k].rc.h=h;
				InitVramY(k,yd);
				InitVramX(k,xd);
				InitVramW(k,wd);
				InitVramH(k,hd);
			}
			else
			if(NbPageW==1&&j==0)						// une seule zone en haut
			{
				Page[k].rc.x=x+i*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=wd-xd;
				Page[k].rc.h=256-yd;				
				InitVramY(k,yd);
				InitVramX(k,xd);
				InitVramW(k,wd);
			}
			else
			if(NbPageH==1&&i==0)						// un seule zone a gauche
			{
				Page[k].rc.x=x+i*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=256-xd;
				Page[k].rc.h=hd-yd;
				InitVramY(k,yd);
				InitVramH(k,hd);
				InitVramX(k,xd);
			}
			else
			if(NbPageW==1&&j==NbPageH-1)				// une seule zone en bas
			{
				Page[k].rc.x=x+i*256;
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=wd-xd;
				Page[k].rc.h=((hd-1)&255)+1;				
				InitVramH(k,((hd-1)&255)+1);
				InitVramX(k,xd);
				InitVramW(k,wd);
			}
			else
			if(NbPageH==1&&i==NbPageW-1)				// un seule zone a droite
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				Page[k].rc.y=y+j*256;
				Page[k].rc.w=((wd-1)&255)+1;
				Page[k].rc.h=hd-yd;
				InitVramY(k,yd);
				InitVramH(k,hd);
				InitVramW(k,((wd-1)&255)+1);
			}
			else
			if(i==NbPageW-1&&j==NbPageH-1)			// bloc en bas a droite
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=((wd-1)&255)+1;
				Page[k].rc.h=((hd-1)&255)+1;
				InitVramW(k,((wd-1)&255)+1);
				InitVramH(k,((hd-1)&255)+1);
			}
			else
			if(i==0&&j==0)							// bloc en haut a gauche
			{
				Page[k].rc.x=x+i*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=256-xd;
				Page[k].rc.h=256-yd;
				InitVramX(k,xd);
				InitVramY(k,yd);
			}
			else
			if(i==0&&j==NbPageH-1)					//bloc en bas a gauche 
			{
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;
				Page[k].rc.x=x+i*256;

				Page[k].rc.w=256-xd;
				Page[k].rc.h=((hd-1)&255)+1;
				InitVramX(k,xd);
				InitVramH(k,((hd-1)&255)+1);
			}
			else
			if(i==NbPageW-1&&j==0)					//bloc en haut  a droite
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=((wd-1)&255)+1;
				Page[k].rc.h=256-yd;
				InitVramW(k,((wd-1)&255)+1);
				InitVramY(k,yd);
			}
			else
			if(NbPageW==1)							// une seule zone en haut/bas
			{
				Page[k].rc.x=x+i*256;
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=wd-xd;
				Page[k].rc.h=256;				
				InitVramX(k,xd);
				InitVramW(k,wd);
			}
			else
			if(NbPageH==1)							// un seule zone a gauche/droite
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=256;
				Page[k].rc.h=hd-yd;
				InitVramY(k,yd);
				InitVramH(k,hd);
			}
			else
			if(i==NbPageW-1)						// zone a droite
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=((wd-1)&255)+1;
				Page[k].rc.h=256;
				InitVramW(k,((wd-1)&255)+1);
			}
			else
			if(j==NbPageH-1)						// zone en bas
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;

				Page[k].rc.w=256;
				Page[k].rc.h=((hd-1)&255)+1;
				InitVramH(k,((hd-1)&255)+1);
			}
			else
			if(i==0)								// zone a gauche
			{
				if(yd==0)
					Page[k].rc.y=y+j*256;
				else
					Page[k].rc.y=y+yd+(j-1)*256;
				Page[k].rc.x=x+i*256;

				Page[k].rc.w=256-xd;
				Page[k].rc.h=256;
				InitVramX(k,xd);
			}
			else
			if(j==0)								// zone en haut
			{
				if(xd==0)
					Page[k].rc.x=x+i*256;
				else
					Page[k].rc.x=x+xd+(i-1)*256;
				Page[k].rc.y=y+j*256;

				Page[k].rc.w=256;
				Page[k].rc.h=256-yd;
				InitVramY(k,yd);
			}
			else
			{
			}
			s.Format(L"Page %2d %4d %4d %4d %4d\r\n",k,Page[k].rc.x,Page[k].rc.y,Page[k].rc.w,Page[k].rc.h);
			SPgs->AddDbgInfo(s);
			VRam->LoadPage(&Page[k].rc,Page[k].VRamCoul,Page[k].VRamOccup,Page[k].VRamNum);
			k++;
		}

	}

	return 1;
}

void CVramAutoPos::SetSprite(SPRITE *ListSprite,int Nb,int Nb16)
{
	int i;
	Sprites=ListSprite;
	NbSprites=Nb;
	NbSpr16=Nb16;

	for(i=0;i<Nb;i++)
	{
		if(Sprites[i].NCol>16)
		{
			memcpy(Pal,Sprites[i].pal,Sprites[i].NCol*3);
			break;
		}
	}
}

void CVramAutoPos::ToVram(CVram *VRam)
{
	int i=0;
	for(i=0;i<max_page;i++)
	{
		VRam->StorePage(&Page[i].rc,Page[i].VRamCoul,Page[i].VRamOccup,Page[i].VRamNum);
	}
}

void CVramAutoPos::Save(CVram *Vram,CDOut *Out,BOOL IsHI)
{
	int i,x,y;
	BLKPSX Bnk;
	CString DbP;

	for(i=0;i<NbSprites;i++)
	{
		if(Sprites[i].page==-1)
		{
			CString s;
			s.Format(L"No Room for block %s\r\n",Sprites[i].Name);
			SPgs->AddDbgInfo(s);

			std::string sprite= wstring_to_utf8(Sprites[i].Name);

			Out->AddData(sprite.c_str(),Bnk,Sprites[i].IsZoneFilter);
			continue;
		}

		x=Page[Sprites[i].page].rc.x&0xffff00;
		y=Page[Sprites[i].page].rc.y;
		
		if(Sprites[i].NCol>16)
		{

			Bnk.u0= uint8_t(Sprites[i].xpos&0x7f);
			Bnk.u1= uint8_t((Sprites[i].xpos&0x7f)+Sprites[i].l-1);
			Bnk.u2= uint8_t(Sprites[i].xpos&0x7f);
			Bnk.u3= uint8_t((Sprites[i].xpos&0x7f)+Sprites[i].l-1);

			Bnk.v0= uint8_t(Sprites[i].ypos);
			Bnk.v1= uint8_t(Sprites[i].ypos);
			Bnk.v2= uint8_t(Sprites[i].ypos+Sprites[i].h-1);
			Bnk.v3= uint8_t(Sprites[i].ypos+Sprites[i].h-1);

			//Bnk.tpage=getTPage(1,0,((((Sprites[i].page&0xfe)+(Sprites[i].xpos>>7))<<6)+256),(Sprites[i].page&1)*256);
			if(IsHI)
				Bnk.tpage=getTPagePC(1,(x+(Sprites[i].xpos&0x80))>>1,y);
			else
				Bnk.tpage=getTPage(1,0,(x+(Sprites[i].xpos&0x80))>>1,y);

			if((Bnk.clut=Vram->PutPal(Sprites[i].pal,Sprites[i].Trans,Sprites[i].NCol))==0xffff)
			{
				// Error
				SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
			}
			DbP.Format(L"Clut infos (%s) NbCol:%3d   X:%4d Y:%3d Clut:0x%04x\r\n",Sprites[i].Name,256,CLUT_X(Bnk.clut),CLUT_Y(Bnk.clut),Bnk.clut);
			SPgs->AddDbgInfo(DbP);

		}
		else
		{
			Bnk.u0= uint8_t((Sprites[i].xpos&0x7f)<<1);
			Bnk.u1= uint8_t(((Sprites[i].xpos&0x7f)<<1)+Sprites[i].l16-1);
			Bnk.u2= uint8_t((Sprites[i].xpos&0x7f)<<1);
			Bnk.u3= uint8_t(((Sprites[i].xpos&0x7f)<<1)+Sprites[i].l16-1);

			Bnk.v0=(uint8_t)Sprites[i].ypos;
			Bnk.v1=(uint8_t)Sprites[i].ypos;
			Bnk.v2=(uint8_t)(Sprites[i].ypos+Sprites[i].h-1);
			Bnk.v3=(uint8_t)(Sprites[i].ypos+Sprites[i].h-1);

			//Bnk.tpage=getTPage(0,0,((((Sprites[i].page&0xfe)+(Sprites[i].xpos>>7))<<6)+256),(Sprites[i].page&1)*256);
			if(IsHI)
				Bnk.tpage=getTPagePC(0,(x+(Sprites[i].xpos&0x80))>>1,y);
			else
				Bnk.tpage=getTPage(0,0,(x+(Sprites[i].xpos&0x80))>>1,y);

			// si la 1ere palette a moins de 16 couleurs avec plus d'une palette ca cause un pb =>modif!
			//if((Bnk.clut=Vram->PutPal(Sprites[i].pal,Sprites[i].Trans,Sprites[i].NCol*Sprites[i].NPal))==0xffff)
			//if(Sprites[i].NPal!=1)
			{
				
				FILE *fp;
				wchar_t Path[256];
				int Version=0x1000;
				int j;

				DbP.Format(L"Clut infos (%s) NbCol:%3dx%d =>\r\n",Sprites[i].Name,16,Sprites[i].NPal);
				SPgs->AddDbgInfo(DbP);

				swprintf(Path,L"%s\\%s_16c.clt",Sprites[i].Path,Sprites[i].Name);
				if((fp=_wfopen(Path,L"wb"))==NULL) 
				{
					DbP.Format(L"%s open Error \r\n",Path);
					SPgs->AddDbgInfo(Path);		
				}
				else
				{
					fwrite("CLUT",1,4,fp);
					fwrite(&Version,1,4,fp);
					int NPal=Sprites[i].NPal;
					fwrite(&NPal,1,sizeof(int),fp);
					for(j=0;j<Sprites[i].NPal;j++)
					{
						if((Bnk.clut=Vram->PutPal(Sprites[i].pal+j*16*3,Sprites[i].Trans+j*16,16))==0xffff)
						{
							// Error
							SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
						}
						fwrite(&Bnk.clut,1,sizeof(Bnk.clut),fp);

						DbP.Format(L"%2d : X:%4d Y:%3d Clut:0x%04x\r\n",j,CLUT_X(Bnk.clut),CLUT_Y(Bnk.clut),Bnk.clut);
						SPgs->AddDbgInfo(DbP);
					}
					fclose(fp);
				}
			}
			/*else
			{
				if((Bnk.clut=Vram->PutPal(Sprites[i].pal,Sprites[i].Trans,16))==0xffff)
				{
					// Error
					SPgs->AddDbgInfo("No Room for all palette\r\n");		
				}
			} */

		}

		SPgs->AddDbgInfo(L"-----------------------------------------------\r\n");		

		std::string sprite= wstring_to_utf8(Sprites[i].Name);

		Out->AddData(sprite.c_str(),Bnk,Sprites[i].IsZoneFilter);
	}
}

void CVramAutoPos::SaveFNT(CVram *Vram,CDOut *Out,int Type,int First,int Sp,int SpW,int SpH,BOOL IsHI)
{
	int i,x,y,h,p;
	FNTCLIST Fnt;
	

	Fnt.Hdr.First=First;
	Fnt.Hdr.Type=Type;
	Fnt.Hdr.NbC=NbSprites;
	if(Sprites[0].RotType==ROT_0||Sprites[0].RotType==ROT_180)
		h=Fnt.Hdr.h=(UCHAR)Sprites[0].h;
	else
		h=Fnt.Hdr.h=(UCHAR)Sprites[0].l16;
	p=Sprites[0].page;
	
	x=Page[p].rc.x;
	y=Page[p].rc.y;

	if(IsHI)
		Fnt.Hdr.TPage=getTPagePC(0,x>>1,y);
	else
		Fnt.Hdr.TPage=getTPage(0,0,x>>1,y);

	Fnt.Hdr.Sp=Sp;
	Fnt.Hdr.SpW=SpW;
	Fnt.Hdr.SpH=SpH;
	Fnt.Hdr.NbSwp=0;
	Fnt.Hdr.Mode=0;

	if((Fnt.Hdr.Clut=Vram->PutPalFnt(Sprites[0].pal,Sprites[0].Trans,Sprites[0].NCol))==0xffff)
	{
		// Error
		SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
	}

	CString DbP;
	DbP.Format(L"Clut :%3d %4d %3d(%4x)\r\n",16, CLUT_X(Fnt.Hdr.Clut), CLUT_Y(Fnt.Hdr.Clut),Fnt.Hdr.Clut);
	SPgs->AddDbgInfo(DbP);

	for(i=0;i<NbSprites;i++)
	{
		if(Sprites[i].page==-1)
		{
			CString s;
			s.Format(L"No Room for char %d\r\n",i+First);
			SPgs->AddDbgInfo(s);
			continue;
		}

		if(IsHI)
		{
			x=Page[Sprites[i].page].rc.x;
			y=Page[Sprites[i].page].rc.y;
			Fnt.TPagePC[i]=getTPagePC(0,((Sprites[i].xpos&0x80)+x)>>1,y);
		}
		else
		{
			if(Sprites[i].page!=p)
			{
				//error
				CString s;
				s.Format(L"char %d is non on the same page of other\r\n",i+First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}

		if(Sprites[i].RotType==ROT_0||Sprites[i].RotType==ROT_180)
		{
			if(h!=Sprites[i].h)
			{
				//error
				CString s;
				s.Format(L"char %d have not the same H of other\r\n",i+First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}
		else
		{
			if(h!=Sprites[i].l16)
			{
				//error
				CString s;
				s.Format(L"char %d have not the same H of other\r\n",i+First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}

		Fnt.Body[i].x=(UCHAR)((Sprites[i].xpos&0x7f)<<1);
		Fnt.Body[i].y=(UCHAR)Sprites[i].ypos;

		if(Sprites[i].RotType==ROT_0||Sprites[i].RotType==ROT_180)
		{
			Fnt.Body[i].w=(UCHAR)Sprites[i].l16;		
			Fnt.Body[i].h=(UCHAR)Sprites[i].h;
		}
		else
		{
			Fnt.Body[i].w=(UCHAR)Sprites[i].h;
			Fnt.Body[i].h=(UCHAR)Sprites[i].l16;
		}

	/*	CString s;
		s.Format("%4d %4d %c(%2x %2x) %4d %4d\r\n",x>>1,y,i+First,i+First,i,Fnt.Body[i].x,Fnt.Body[i].y);
		SPgs->AddDbgInfo(s);*/

	}
	std::string sprite= wstring_to_utf8(Sprites[0].Name);

	Out->AddData(sprite.c_str(),Fnt,Sprites[0].IsZoneFilter);
}

void CVramAutoPos::SaveFNT(CVram *Vram,CDOut *Out,FNTCLIST &Fnt,BOOL IsHI)
{
	int i,x,y,h,p;
	
	if(Sprites[0].RotType==ROT_0||Sprites[0].RotType==ROT_180)
		h=Fnt.Hdr.h=(UCHAR)Sprites[0].h;
	else
		h=Fnt.Hdr.h=(UCHAR)Sprites[0].l16;

	p=Sprites[0].page;
	
	x=Page[p].rc.x;
	y=Page[p].rc.y;

	if(IsHI)
		Fnt.Hdr.TPage=getTPagePC(0,x>>1,y);
	else
		Fnt.Hdr.TPage=getTPage(0,0,x>>1,y);

	if((Fnt.Hdr.Clut=Vram->PutPalFnt(Sprites[0].pal,Sprites[0].Trans,Sprites[0].NCol))==0xffff)
	{
		// Error
		SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
	}

	CString DbP;
	DbP.Format(L"Clut :%3d %4d %3d(%4x)\r\n",16, CLUT_X(Fnt.Hdr.Clut), CLUT_Y(Fnt.Hdr.Clut),Fnt.Hdr.Clut);
	SPgs->AddDbgInfo(DbP);

	for(i=0;i<NbSprites;i++)
	{
		if(Sprites[i].page==-1)
		{
			CString s;
			s.Format(L"No Room for char %d\r\n",i+Fnt.Hdr.First);
			SPgs->AddDbgInfo(s);
			continue;
		}

		if(IsHI)
		{
			x=Page[Sprites[i].page].rc.x;
			y=Page[Sprites[i].page].rc.y;
			Fnt.TPagePC[i]=getTPagePC(0,((Sprites[i].xpos&0x80)+x)>>1,y);
		}
		else
		{
			if(Sprites[i].page!=p)
			{
				//error
				CString s;
				s.Format(L"char %d is non on the same page of other\r\n",i+Fnt.Hdr.First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}

		if(Sprites[i].RotType==ROT_0||Sprites[i].RotType==ROT_180)
		{
			if(h!=Sprites[i].h)
			{
				//error
				CString s;
				s.Format(L"char %d have not the same H of other\r\n",i+Fnt.Hdr.First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}
		else
		{
			if(h!=Sprites[i].l16)
			{
				//error
				CString s;
				s.Format(L"char %d have not the same H of other\r\n",i+Fnt.Hdr.First);
				SPgs->AddDbgInfo(s);
				continue;
			}
		}

		Fnt.Body[i].x=(UCHAR)((Sprites[i].xpos&0x7f)<<1);
		Fnt.Body[i].y=(UCHAR)Sprites[i].ypos;

		if(Sprites[i].RotType==ROT_0||Sprites[i].RotType==ROT_180)
		{
			Fnt.Body[i].w=(UCHAR)Sprites[i].l16;		
			Fnt.Body[i].h=(UCHAR)Sprites[i].h;
		}
		else
		{
			Fnt.Body[i].w=(UCHAR)Sprites[i].h;
			Fnt.Body[i].h=(UCHAR)Sprites[i].l16;
		}

	/*	CString s;
		s.Format("%4d %4d %c(%2x %2x) %4d %4d\r\n",x>>1,y,i+First,i+First,i,Fnt.Body[i].x,Fnt.Body[i].y);
		SPgs->AddDbgInfo(s);*/

	}

	std::string sprite= wstring_to_utf8(Sprites[0].Name);

	Out->AddData(sprite.c_str(),Fnt,Sprites[0].IsZoneFilter);
}


void CVramAutoPos::Box(int x,int y,int w,int h)
{
	int i,c;
	c=0;
	ScanData* pScanData = GetScanData();

	for(i=x;i<x+w;i++)
	{
		*(pScanData->LogPtr+i+y*640)=c;
		*(pScanData->LogPtr +i+640*h+y*640)=c++;
	}
	c=0;
	for(i=y;i<y+h;i++)
	{
		*(pScanData->LogPtr +i*640+x)=c;
		*(pScanData->LogPtr +i*640+w+x)=c++;
	}
}

void CVramAutoPos::AfficheBlocActuel(LONG n)
{
	LONG xb,yb;
	LONG l,h;
	int i,j;
	UBYTE *PtrLog1,*PtrLog2,*PtrData;
	ULONG *Ptr;
	l=Sprites[n].l;
	h=Sprites[n].h;

	xb=offset_blocs_x;
	yb=offset_blocs_y;
	ScanData* pScanData = GetScanData();

	PtrLog1=PtrLog2= pScanData->LogPtr +yb*640+xb;
	PtrData=Sprites[n].data;
	Ptr=(ULONG *)pScanData->LogPtr;

	/*for(j=9;j<400;j++)
		for(i=0;i<256/4;i++)
	 		*(Ptr+i+j*160)=0;*/

	//Box(319,offset_blocs_y-1,257,257);
	//Box(xb-1,yb-1,l+1,h+1);

	for(i=0;i<h;i++)
	{
		for(j=0;j<l;j++)
		{
			*PtrLog1++=*PtrData++;
		}
		PtrLog1=PtrLog2+=640;
	}
	
/*	PtrLog1=PtrLog2=Log+320+640*offset_blocs_y;
	PtrData=Page[NumPage].VRamCoul;
	//PtrData=Page[NumPage].VRamOccup;
	for(i=0;i<256;i++)
	{
		for(j=0;j<256;j++)
		{
			*PtrLog1++=*PtrData++;
		}

		PtrLog1=PtrLog2+=640;
	}*/


	SVdd->ShowPageColor(Page[NumPage].VRamNum);

	pScanData ->OccupePtr=Page[NumPage].VRamOccup;
	i=CalculeEfficacite();
	if(Page[NumPage].VRamStat!=i)
	{
		VRamTotal+=i-Page[NumPage].VRamStat;
		Page[NumPage].VRamStat=i;
	}

	/*if(Sprites[n].NCol>16)
	{
		SVdd->ShowPage(Log,Sprites[n].pal);
	}
	else
		SVdd->ShowPage(Log,Pal);*/
}

void CVramAutoPos::AfficheBlocVRamColor(int n)
{
	int i,j;
	UBYTE *PtrData,c,*LPal;
	USHORT *PtrColor;

	PtrColor=Page[NumPage].VRamNum;
	PtrData=Sprites[n].data;

	PtrColor+=Sprites[n].ypos*256+Sprites[n].xpos;

	if(Sprites[n].NCol>16)
		LPal=Sprites[n].pal;
	else
		LPal=Pal;

	for(i=0;i<Sprites[n].h;i++)
	{
		for(j=0;j<Sprites[n].l;j++)
		{
			c=*PtrData++;
			*(PtrColor+i*256+j)=SVdd->Pal2DDMem(*(LPal+c*3+0),*(LPal+c*3+1),*(LPal+c*3+2));
		}
	}
}



int  CVramAutoPos::ExecAlgoMaxRecouvre(void)
{
	LONG n;
	LONG l,h;
	LONG xb,yb;
	
	int pmax;

	if(No16x256)
	{
		pmax=max_page;
	}
	else
	{
		float calc;
		calc=(float)NbSpr16/(float)NbSprites;

		pmax=(int)(max_page*calc);
		if(pmax==0) pmax=1;
	}


	SPgs->SetStatus(L"ExecAlgoMaxRecouvre ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	ScanData* pScanData = GetScanData();


	for (n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;
		Sprites[n].ok=FALSE;
	
		xb=offset_blocs_x;
		yb=offset_blocs_y;
		AfficheBlocActuel(n);
		if(Sprites[n].EquNo!=-1)
		{
			Sprites[n].ok=TRUE;
			Sprites[n].xpos=Sprites[Sprites[n].EquNo].xpos;
			Sprites[n].ypos=Sprites[Sprites[n].EquNo].ypos;
			Sprites[n].page=Sprites[Sprites[n].EquNo].page;
			Page[Sprites[n].page].Num++;
		}
		else
		{
			if(No16x256)
			{
				for (NumPage=0;NumPage<pmax;NumPage++)
				{
					pScanData ->OccupePtr=Page[NumPage].VRamOccup;
					pScanData -> CouleurPtr=Page[NumPage].VRamCoul;
					//if(Sprites[n].NCol>16)
						ScanneMapRecouvre(l,h,xb,yb);
					/*else
						ScanneMapRecouvre16(l,h,xb,yb);*/

					if (pScanData->BestRecouvre!=-1)   break;
				}
			}
			else
			{
				if(Sprites[n].NCol>16)
				{
					for (NumPage=pmax;NumPage<max_page;NumPage++)
					{
						pScanData->OccupePtr=Page[NumPage].VRamOccup;
						pScanData->CouleurPtr=Page[NumPage].VRamCoul;
						ScanneMapRecouvre(l,h,xb,yb);
					
						if (pScanData->BestRecouvre!=-1)   break;
					}

				}
				else
				{
					for (NumPage=0;NumPage<pmax;NumPage++)
					{
						pScanData->OccupePtr=Page[NumPage].VRamOccup;
						pScanData->CouleurPtr=Page[NumPage].VRamCoul;
						//ScanneMapRecouvre16(l,h,xb,yb);
						ScanneMapRecouvre(l,h,xb,yb);

						if (pScanData->BestRecouvre!=-1)   break;
					}
				}
			}
			if(NumPage>=max_page) NumPage=max_page-1;

			if (pScanData->BestRecouvre!=-1)
			{
				AfficheBlocVRam(l,h,xb,yb);


				Page[NumPage].Num++;

				Sprites[n].ok=TRUE;
				Sprites[n].xpos= pScanData->BestX;
				Sprites[n].ypos= pScanData->BestY;
				Sprites[n].page=NumPage;
				AfficheBlocVRamColor(n);
				if (NumPage>UsedPages)
					UsedPages=NumPage;
			}
			else
			{
				CString s;
				s.Format(L"Error No Room for block %d\r\n",n);
				SPgs->AddDbgInfo(s);
				Sprites[n].xpos=0;
				Sprites[n].ypos=0;
				Sprites[n].page=0;
			}
		}
	    SPgs->SetPos(n);
    }
	SPgs->SetStatus(L"ExecAlgoMaxRecouvre ...OK\r\n");
	return 1;
}


int CVramAutoPos::ExecAlgoOptimum(void)
{
	LONG n;
	LONG l,h;
	LONG xb,yb;
	int pmax;

	if(No16x256)
	{
		pmax=max_page;
	}
	else
	{
		float calc;
		calc=(float)NbSpr16/(float)NbSprites;

		pmax=(int)(max_page*calc);
		if(pmax==0) pmax=1;
	}

	SPgs->SetStatus(L"ExecAlgoOptimum ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	ScanData* pScanData = GetScanData();

	for (n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;
		Sprites[n].ok=FALSE;
	
		xb=offset_blocs_x;
		yb=offset_blocs_y;
		AfficheBlocActuel(n);
		if(Sprites[n].EquNo!=-1)
		{
			Sprites[n].ok=TRUE;
			Sprites[n].xpos=Sprites[Sprites[n].EquNo].xpos;
			Sprites[n].ypos=Sprites[Sprites[n].EquNo].ypos;
			Sprites[n].page=Sprites[Sprites[n].EquNo].page;
			Page[Sprites[n].page].Num++;
		}
		else
		{
			if(No16x256)
			{
				for (NumPage=0;NumPage<pmax;NumPage++)
				{
					pScanData->OccupePtr=Page[NumPage].VRamOccup;
					pScanData->CouleurPtr=Page[NumPage].VRamCoul;
					//PVRamNum=Page[NumPage].VRamNum;
					//if(Sprites[n].NCol>16)
						ScanneMapOptimum(l,h,xb,yb);
					/*else
						ScanneMapOptimum16(l,h,xb,yb);*/

					if (pScanData->BestVoisin>=0)  break;
				}
			}
			else
			{
				if(Sprites[n].NCol>16)
				{
					for (NumPage=pmax;NumPage<max_page;NumPage++)
					{
						pScanData->OccupePtr=Page[NumPage].VRamOccup;
						pScanData->CouleurPtr=Page[NumPage].VRamCoul;
						ScanneMapOptimum(l,h,xb,yb);

						if (pScanData->BestVoisin>=0)  break;
					}
				}
				else
				{
					for (NumPage=0;NumPage<pmax;NumPage++)
					{
						pScanData->OccupePtr=Page[NumPage].VRamOccup;
						pScanData->CouleurPtr=Page[NumPage].VRamCoul;
						//ScanneMapOptimum16(l,h,xb,yb);
						ScanneMapOptimum(l,h,xb,yb);

						if (pScanData->BestVoisin>=0)  break;
					}
				}

			}

			if(NumPage>=max_page) NumPage=max_page-1;

			if (pScanData->BestVoisin>=0)
			{
				AfficheBlocVRam(l,h,xb,yb);
				Page[NumPage].Num++;

				Sprites[n].ok=TRUE;
				Sprites[n].xpos= pScanData->BestX;
				Sprites[n].ypos= pScanData->BestY;
				Sprites[n].page=NumPage;
				AfficheBlocVRamColor(n);
				if (NumPage>UsedPages)
					UsedPages=NumPage;
			}
			else
			{
				CString s;
				s.Format(L"Error No Room for block %d\r\n",n);
				SPgs->AddDbgInfo(s);
				Sprites[n].xpos=0;
				Sprites[n].ypos=0;
				Sprites[n].page=0;
			}
		}
	    SPgs->SetPos(n);
    }
	SPgs->SetStatus(L"ExecAlgoOptimum ...OK\r\n");
	return 1;
}

int CVramAutoPos::ExecAlgoHorizontal(void)
{
	LONG n;
	LONG l,h;
	LONG xb,yb;
	int pmax;

	if(No16x256)
	{
		pmax=max_page;
	}
	else
	{
		float calc;
		calc=(float)NbSpr16/(float)NbSprites;

		pmax=(int)(max_page*calc);
		if(pmax==0) pmax=1;
	}

	SPgs->SetStatus(L"ExecAlgoHorizontal ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

/*	for(l=0;l<max_page;l++)
	{
		NumPage=l;
		AfficheBlocActuel(0);
		while(!SPgs->CheckCancelButton());
	}*/

	ScanData* pScanData = GetScanData();

	for (n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;
		Sprites[n].ok=FALSE;
	
		xb=offset_blocs_x;
		yb=offset_blocs_y;
		AfficheBlocActuel(n);
		if(Sprites[n].EquNo!=-1)
		{
			Sprites[n].ok=TRUE;
			Sprites[n].xpos=Sprites[Sprites[n].EquNo].xpos;
			Sprites[n].ypos=Sprites[Sprites[n].EquNo].ypos;
			Sprites[n].page=Sprites[Sprites[n].EquNo].page;
			Page[Sprites[n].page].Num++;
		}
		else
		{
			if(No16x256)
			{
				for (NumPage=0;NumPage<max_page;NumPage++)
				{
					pScanData->OccupePtr=Page[NumPage].VRamOccup;
					pScanData->CouleurPtr=Page[NumPage].VRamCoul;
					//PVRamNum=Page[NumPage].VRamNum;
					//if(Sprites[n].NCol>16)
						ScanneMapHorizontal(l,h,xb,yb);
					//else
						//ScanneMapHorizontal16(l,h,xb,yb);


					if (pScanData->BestPlace!=-1)  break;
				}
			}
			else
			{
				if(Sprites[n].NCol>16)
				{
					for (NumPage=pmax;NumPage<max_page;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;
						ScanneMapHorizontal(l,h,xb,yb);


						if (pScanData->BestPlace!=-1)  break;
					}
				}
				else
				{
					for (NumPage=0;NumPage<pmax;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;
						//ScanneMapHorizontal16(l,h,xb,yb);
						ScanneMapHorizontal(l,h,xb,yb);


						if (pScanData->BestPlace != -1)  break;
					}
				}

			}

			if(NumPage>=max_page) NumPage=max_page-1;

			if (pScanData->BestPlace!=-1)
			{
				AfficheBlocVRam(l,h,xb,yb);
				Page[NumPage].Num++;

				Sprites[n].ok=TRUE;
				Sprites[n].xpos= pScanData->BestX;
				Sprites[n].ypos= pScanData->BestY;
				Sprites[n].page=NumPage;
				AfficheBlocVRamColor(n);
				if (NumPage>UsedPages)
					UsedPages=NumPage;
			}
			else
			{
				CString s;
				s.Format(L"Error No Room for block %d\r\n",n);
				SPgs->AddDbgInfo(s);
				Sprites[n].xpos=0;
				Sprites[n].ypos=0;
				Sprites[n].page=0;
			}
		}
	    SPgs->SetPos(n);
    }
	SPgs->SetStatus(L"ExecAlgoHorizontal ...OK\r\n");
	return 1;
}


int CVramAutoPos::ExecAlgoVertical(void)
{
	LONG n;
	LONG l,h;
	LONG xb,yb;
	int pmax;

	if(No16x256)
	{
		pmax=max_page;
	}
	else
	{
		float calc;
		calc=(float)NbSpr16/(float)NbSprites;

		pmax=(int)(max_page*calc);
		if(pmax==0) pmax=1;
	}

	SPgs->SetStatus(L"ExecAlgoVertical ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	ScanData* pScanData = GetScanData();

	for (n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;
		Sprites[n].ok=FALSE;
	
		xb=offset_blocs_x;
		yb=offset_blocs_y;
		AfficheBlocActuel(n);
		if(Sprites[n].EquNo!=-1)
		{
			Sprites[n].ok=TRUE;
			Sprites[n].xpos=Sprites[Sprites[n].EquNo].xpos;
			Sprites[n].ypos=Sprites[Sprites[n].EquNo].ypos;
			Sprites[n].page=Sprites[Sprites[n].EquNo].page;
			Page[Sprites[n].page].Num++;
		}
		else
		{
			if(No16x256)
			{
				for (NumPage=0;NumPage<max_page;NumPage++)
				{
					pScanData->OccupePtr = Page[NumPage].VRamOccup;
					pScanData->CouleurPtr = Page[NumPage].VRamCoul;
					//PVRamNum=Page[NumPage].VRamNum;
					//if(Sprites[n].NCol>16)
						ScanneMapVertical(l,h,xb,yb);
					/*else
						ScanneMapVertical16(l,h,xb,yb);*/

					if (pScanData->BestPlace!=-1)  break;
				}
			}
			else
			{
				if(Sprites[n].NCol>16)
				{
					for (NumPage=pmax;NumPage<max_page;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;
						ScanneMapVertical(l,h,xb,yb);

						if (pScanData->BestPlace!=-1)  break;
					}

				}
				else
				{
					for (NumPage=0;NumPage<pmax;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;
						//ScanneMapVertical16(l,h,xb,yb);
						ScanneMapVertical(l,h,xb,yb);

						if (pScanData->BestPlace!=-1)  break;
					}
				}
			}

			if(NumPage>=max_page) NumPage=max_page-1;

			if (pScanData->BestPlace!=-1)
			{
				AfficheBlocVRam(l,h,xb,yb);
				Page[NumPage].Num++;

				Sprites[n].ok=TRUE;
				Sprites[n].xpos= pScanData->BestX;
				Sprites[n].ypos= pScanData->BestY;
				Sprites[n].page=NumPage;
				AfficheBlocVRamColor(n);
				if (NumPage>UsedPages)
					UsedPages=NumPage;
			}
			else
			{
				CString s;
				s.Format(L"Error No Room for block %d\r\n",n);
				SPgs->AddDbgInfo(s);
				Sprites[n].xpos=0;
				Sprites[n].ypos=0;
				Sprites[n].page=0;
			}
		}
	    SPgs->SetPos(n);
    }
	SPgs->SetStatus(L"ExecAlgoVertical ...OK\r\n");
	return 1;
}


int CVramAutoPos::ExecAlgoSelonForme(void)
{
	LONG n;
	LONG l,h;
	LONG xb,yb;
	int pmax;

	if(No16x256)
	{
		pmax=max_page;
	}
	else
	{
		float calc;
		calc=(float)NbSpr16/(float)NbSprites;

		pmax=(int)(max_page*calc);
		if(pmax==0) pmax=1;
	}

	SPgs->SetStatus(L"ExecAlgoSelonForme ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	ScanData* pScanData = GetScanData();

	for (n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;
		Sprites[n].ok=FALSE;
	
		xb=offset_blocs_x;
		yb=offset_blocs_y;
		AfficheBlocActuel(n);
		if(Sprites[n].EquNo!=-1)
		{
			Sprites[n].ok=TRUE;
			Sprites[n].xpos=Sprites[Sprites[n].EquNo].xpos;
			Sprites[n].ypos=Sprites[Sprites[n].EquNo].ypos;
			Sprites[n].page=Sprites[Sprites[n].EquNo].page;
			Page[Sprites[n].page].Num++;
		}
		else
		{
			if(No16x256)
			{
				for (NumPage=0;NumPage<max_page;NumPage++)
				{
					pScanData->OccupePtr = Page[NumPage].VRamOccup;
					pScanData->CouleurPtr = Page[NumPage].VRamCoul;
					//PVRamNum=Page[NumPage].VRamNum;

					//if(Sprites[n].NCol>16)
					{
						if (l>h)
   	            			ScanneMapHorizontal(l,h,xb,yb);
	      	   			else
         	      			ScanneMapVertical(l,h,xb,yb);
					}
					/*else
					{
	         			if (l>h)
   	            			ScanneMapHorizontal16(l,h,xb,yb);
		  	   			else
         	      			ScanneMapVertical16(l,h,xb,yb);
					} */

					if (pScanData->BestPlace!=-1)  break;
				}
			}
			else
			{
				if(Sprites[n].NCol>16)
				{
					for (NumPage=pmax;NumPage<max_page;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;
						if (l>h)
   	            			ScanneMapHorizontal(l,h,xb,yb);
	      	   			else
         	      			ScanneMapVertical(l,h,xb,yb);

						if (pScanData->BestPlace!=-1)  break;
					}
				}
				else
				{
					for (NumPage=0;NumPage<pmax;NumPage++)
					{
						pScanData->OccupePtr = Page[NumPage].VRamOccup;
						pScanData->CouleurPtr = Page[NumPage].VRamCoul;

	         			if (l>h)
   	            			//ScanneMapHorizontal16(l,h,xb,yb);
							ScanneMapHorizontal(l,h,xb,yb);
		  	   			else
         	      			//ScanneMapVertical16(l,h,xb,yb);
							ScanneMapVertical(l,h,xb,yb);

						if (pScanData->BestPlace!=-1)  break;
					}
				}

			}

			if(NumPage>=max_page) NumPage=max_page-1;

			if (pScanData->BestPlace!=-1)
			{
				AfficheBlocVRam(l,h,xb,yb);
				Page[NumPage].Num++;

				Sprites[n].ok=TRUE;
				Sprites[n].xpos= pScanData->BestX;
				Sprites[n].ypos= pScanData->BestY;
				Sprites[n].page=NumPage;
				AfficheBlocVRamColor(n);
				if (NumPage>UsedPages)
					UsedPages=NumPage;
			}
			else
			{
				CString s;
				s.Format(L"Error No Room for block %d\r\n",n);
				SPgs->AddDbgInfo(s);
				Sprites[n].xpos=0;
				Sprites[n].ypos=0;
				Sprites[n].page=0;
			}
		}
	    SPgs->SetPos(n);
    }
	SPgs->SetStatus(L"ExecAlgoSelonForme ...OK\r\n");
	return 1;
}


//****************************************************************

CVramSprites::CVramSprites(int NbS,CProgressDlg *Pgs)
{
	int i;
	Sprites=new SPRITE[NbS];
	SPgs=Pgs;
	if(Sprites==NULL)
	{
		SPgs->AddDbgInfo(L"No Memory for CVramSprites\r\n");
		SPgs->SaveDbgInfo();
	}
	NbSMax=NbS;
	NbSprites=0;
	Nb16=0;
	for(i=0;i<NbS;i++)
	{
		Sprites[i].EquNo=-1;
	}
	bNormalRemap16=FALSE;
}

CVramSprites::~CVramSprites()
{
	int i;
	for(i=0;i<NbSprites;i++)
	{
		free(Sprites[i].data);
		free(Sprites[i].pal);
	}
	delete [] Sprites;
}

int		CVramSprites::Add(pic_info_t *Pic,int _IsZoneFilter)
{
	if(NbSprites>=NbSMax) return 0;

	if(_IsZoneFilter)
	{
		UCHAR *NewPic,*OldPic;
		int i,j,k;
		NewPic=(UCHAR*)malloc((Pic->w+_IsZoneFilter*2)*(Pic->h+_IsZoneFilter*2));
		if(NewPic==NULL)
		{
			SPgs->AddDbgInfo(L"No Memory for CVramSprites::Add\r\n");
			SPgs->SaveDbgInfo();
		}
		OldPic=(UCHAR *)Pic->data;

		// Up 
		for(i=0;i<_IsZoneFilter;i++)
		{
			memset(NewPic+(Pic->w+_IsZoneFilter*2)*i,0,Pic->w+_IsZoneFilter*2);
		}
		//memset(NewPic+Pic->w+4,0,Pic->w+_IsZoneFilter*2);

		// Down
		for(i=0;i<_IsZoneFilter;i++)
		{
			memset(NewPic+(Pic->w+_IsZoneFilter*2)*Pic->h+(Pic->w+_IsZoneFilter*2)*(_IsZoneFilter+i),0,Pic->w+_IsZoneFilter*2);
		}

		k=0;
		for(j=0;j<Pic->h;j++)
		{
			for(i=0;i<_IsZoneFilter;i++)
			{
				// Left
				NewPic[j*(Pic->w+_IsZoneFilter*2)+(Pic->w+_IsZoneFilter*2)*_IsZoneFilter+i]=0;
				// Right
				NewPic[j*(Pic->w+_IsZoneFilter*2)+(Pic->w+_IsZoneFilter*2)*_IsZoneFilter+Pic->w+_IsZoneFilter+i]=0;
			}
			//NewPic[j*(Pic->w+4)+(Pic->w+4)*2+1]=0;
			//NewPic[j*(Pic->w+4)+(Pic->w+4)*2+Pic->w+2]=0;
			//NewPic[j*(Pic->w+4)+(Pic->w+4)*2+Pic->w+3]=0;

			for(i=_IsZoneFilter;i<Pic->w+_IsZoneFilter;i++)
			{
				NewPic[j*(Pic->w+_IsZoneFilter*2)+(Pic->w+_IsZoneFilter*2)*_IsZoneFilter+i]=OldPic[k++];
			}
		}

		Pic->w+=_IsZoneFilter*2;
		Pic->h+=_IsZoneFilter*2;
		Pic->data=NewPic;
		free(OldPic);
	}

	wcscpy(Sprites[NbSprites].Name,Pic->Name);
	wcscpy(Sprites[NbSprites].Path,Pic->Path);
	Sprites[NbSprites].num=NbSprites;
	Sprites[NbSprites].l=Pic->w;
	Sprites[NbSprites].h=Pic->h;
	Sprites[NbSprites].NCol=Pic->nb_colors;
	Sprites[NbSprites].NPal=Pic->GetNbPal();
	Sprites[NbSprites].data=(UCHAR *)Pic->data;
	Sprites[NbSprites].pal=(UCHAR *)Pic->palette;
	Sprites[NbSprites].Trans=(UCHAR *)Pic->Trans;
	Sprites[NbSprites].taille=Pic->w*Pic->h;
	Sprites[NbSprites].ox=0;
	Sprites[NbSprites].oy=0;
	Sprites[NbSprites].page=-1;
	Sprites[NbSprites].RotType=ROT_0;
	Sprites[NbSprites].IsZoneFilter=_IsZoneFilter;
	NbSprites++;
	return 1;
}

void CVramSprites::Rot(int Type)
{
	int n,i,j,t;
	UBYTE	   *NewData,*Data;

    if(Type!=ROT_0)
	{
		for (n=0;n<NbSprites;n++)
		{
		
			Data=Sprites[n].data;
			Sprites[n].data=NewData=(UCHAR *)malloc(Sprites[n].l*Sprites[n].h);
			switch(Type)
			{			
				case ROT_90:
					for(j=0;j<Sprites[n].h;j++)
					{
						for(i=0;i<Sprites[n].l;i++)
						{
							*(NewData+((Sprites[n].l-1)-i)*Sprites[n].h+j)=*(Data+j*Sprites[n].l+i);
						}
					}
					t=Sprites[n].h;
					Sprites[n].h=Sprites[n].l;
					Sprites[n].l=t;
					Sprites[n].RotType=ROT_90;
				break;
				case ROT_180:
					for(j=0;j<Sprites[n].h;j++)
					{
						for(i=0;i<Sprites[n].l;i++)
						{
							*(NewData+((Sprites[n].h-1)-j)*Sprites[n].l+((Sprites[n].l-1)-i))=*(Data+j*Sprites[n].h+i);
						}
					}
					Sprites[n].RotType=ROT_180;
				break;
				case ROT_270:
					for(j=0;j<Sprites[n].h;j++)
					{
						for(i=0;i<Sprites[n].l;i++)
						{
							*(NewData+i*Sprites[n].h+(Sprites[n].l-j))=*(Data+j*Sprites[n].l+i);
						}
					}
					t=Sprites[n].h;
					Sprites[n].h=Sprites[n].l;
					Sprites[n].l=t;
					Sprites[n].RotType=ROT_270;
				break;
			}	
			free(Data);
		}
	}
}


int CVramSprites::Sort(void)
{
    LONG i,j;
    SPRITE Temp;
	SPgs->SetStatus(L"Sort Sprites ...\r\n");
    SPgs->SetRange(0,NbSprites-2);
    SPgs->SetStep(1);
	SPgs->SetPos(0);
	
    for (i=0;i<NbSprites-1;i++)
    {
        for (j=i+1;j<NbSprites;j++)
        {
			if(SPgs->CheckCancelButton())
				return(-1);
            if (Sprites[i].taille<Sprites[j].taille)
            {
					Temp=Sprites[i];
					Sprites[i]=Sprites[j];
					Sprites[j]=Temp;
            }
        }
	    SPgs->SetPos(i);
    }
	SPgs->SetStatus(L"Sort Sprites ... Ok\r\n");
	return 1;
}

int CVramSprites::UnSort(void)
{
    LONG i,j;
	SPRITE Temp;
	SPgs->SetStatus(L"UnSort Sprites ...\r\n");
    SPgs->SetRange(0,NbSprites-2);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	for (i=0;i<NbSprites-1;i++)
	{

		for (j=i+1;j<NbSprites;j++)	
		{
			if(SPgs->CheckCancelButton())
				return(-1);
			if (Sprites[i].num>Sprites[j].num)
			{
				Temp=Sprites[i];
				Sprites[i]=Sprites[j];
				Sprites[j]=Temp;

			}
		}
		SPgs->SetPos(i);
	}
	SPgs->SetStatus(L"UnSort Sprites ... Ok\r\n");
	return 1;
}

int CVramSprites::Remap16(void)
{
	int i,l,h,j,k,n,m;
	char TabCol[256];
	UBYTE	   *Pal,*NewPal,*NewData,*Data;
	int 		CPal=1;
	char Trans[256];

	SPgs->SetStatus(L"Remap16 Sprites ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	for(n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;

		Pal=Sprites[n].pal;

		// Test 16 couleur
		memset(TabCol,0,256);

		// 1)regarder les couleurs utilisées
		for(i=0;i<l*h;i++)
		{
			TabCol[*(Sprites[n].data+i)]=1;
		}

		// 2)compter les couleurs utilisées
		for(j=i=0;i<256;i++)
			if(TabCol[i]) j++;

		if (!bNormalRemap16)
		{
			// Test Couleur Trans
			if(TabCol[0]==0)		// si l'index 0 n'est pas utilisé
				j++;				// ajouter une couleur
		}

		//Sprites[n].NCol=j;
		if(j>16)
			Sprites[n].NCol=256;
		else
			Sprites[n].NCol=16;

		if(j<=16)
		{
			// Remap 16 couleur
			
			CString s;
			s.Format(L"Remap16 %d(%s)\r\n",n,Sprites[n].Name);
			SPgs->AddDbgInfo(s);
			memcpy(Trans,Sprites[n].Trans,256);

			Nb16++;

			NewPal=(UCHAR *)malloc(16*3);
			//NewPal=(UCHAR *)malloc(256*3);

			// creer une palette avec les couleurs utilisées et la table de remappage
			if (!bNormalRemap16)
			{
				memset(NewPal,0,16*3);
				j=3;						// => offset couleur a 1

				if(TabCol[0])			// a quoi ca sert ca fufu ?
				{
					TabCol[0]=0;
				}
				else
				{
					TabCol[0]=1;
				}
				for(m=i=1;i<256;i++)	// skip l'index 0!
				{
					if(TabCol[i])
					{
						TabCol[i]=j/3;
						if(Pal[i*3]==0&&Pal[i*3+1]==0&&Pal[i*3+2]==0)	// force le noir en 8,8,8!
						{
							NewPal[j++]=8;
							NewPal[j++]=8;
							NewPal[j++]=8;
						}
						else
						{
							NewPal[j++]=Pal[i*3];
							NewPal[j++]=Pal[i*3+1];
							NewPal[j++]=Pal[i*3+2];
						}
						Sprites[n].Trans[m++]=Trans[i];
					}
				}
			}
			else
			{
				for (m=i=j=0; i<256; i++)
				{
					if (TabCol[i])
					{
						TabCol[i]=m;
						NewPal[j++]=Pal[i*3];
						NewPal[j++]=Pal[i*3+1];
						NewPal[j++]=Pal[i*3+2];
						Sprites[n].Trans[m++]=Trans[i];
					}
				}
			}
			
			memcpy(Sprites[n].Trans,Trans,16);
			if (Sprites[n].NPal==1)
			{
				free(Sprites[n].pal);
				Sprites[n].pal=NewPal;
			}
			else
			{
				free(NewPal);
			}

			Data=Sprites[n].data;

			k=((l+1)&0xffe)/2;

			Sprites[n].data=NewData=(UCHAR *)malloc(k*h);
			for(j=0;j<h;j++)
			{
				for(i=0;i<k-1;i++)
				{
					NewData[j*k+i]=TabCol[Data[j*l+i*2+1]]<<4;
					NewData[j*k+i]+=TabCol[Data[j*l+i*2]];
				}

				// si la largeur est impaire
				if(l&1)
				{
					NewData[j*k+i]=TabCol[Data[j*l+i*2]];
				}
				else
				{
					NewData[j*k+i]=TabCol[Data[j*l+i*2+1]]<<4;
					NewData[j*k+i]+=TabCol[Data[j*l+i*2]];
				}

			}
			free(Data);


			Sprites[n].l16=Sprites[n].l;
			Sprites[n].l=k;
			//Sprites[n].taille=l*h/2;
			Sprites[n].taille=k*h;
			//s.Format("Image \"%s\" converted to 16 colors\r\n",Sprites[n].Name);
			//SPgs->AddDbgInfo(s);
		}

		SPgs->SetPos(n);

		if(Sprites[n].l&1)
		{
			CString s;
			s.Format(L"Warning : horizontal size of Blk %d(%s) will be convert to even\r\n",n,Sprites[n].Name);
			SPgs->AddDbgInfo(s);
			Data=Sprites[n].data;
			Sprites[n].l++;			
			Sprites[n].data=NewData=(UCHAR *)malloc(Sprites[n].l*Sprites[n].h);
			for(j=0;j<Sprites[n].h;j++)
			{
				for(i=0;i<Sprites[n].l-1;i++)
				{
					NewData[i+j*Sprites[n].l]=Data[i+j*(Sprites[n].l-1)];
				}
				NewData[i+j*Sprites[n].l]=0;
			}
			
			free(Data);
		}
	}


	SPgs->SetStatus(L"Remap16 Sprites ... Ok\r\n");
	return 1;
}

int CVramSprites::PSX16(void)
{
	int i,l,h,j,k,n;
	char TabCol[256];
	UBYTE	   *Pal,*NewData,*Data;//,*NewPal;
	int 		CPal=1;

	SPgs->SetStatus(L"PSX16 Sprites ...\r\n");
    SPgs->SetRange(0,NbSprites-1);
    SPgs->SetStep(1);
	SPgs->SetPos(0);

	Pal=Sprites[0].pal;


	// Test 16 couleur
	memset(TabCol,0,256);
	for(n=0;n<NbSprites;n++)
	{
		l=Sprites[n].l;
		h=Sprites[n].h;
		for(i=0;i<l*h;i++)
		{
			TabCol[*(Sprites[n].data+i)]=1;
		}

		if(Sprites[0].NCol!=Sprites[n].NCol||memcmp(Pal,Sprites[n].pal,Sprites[0].NCol*3)!=0)
		{
			// Error 
			CString s;
			s.Format(L"Error : The palette of font %s,char %d is not same of other ...\r\n",Sprites[n].Name,i);
			SPgs->AddDbgInfo(s);
			return -1;
		}
	}

	j=0;
	for(i=0;i<256;i++)
		if(TabCol[i]) j++;

	// Test Couleur Trans
	if(TabCol[0]==0)
		j++;
	if(j>16)
		Sprites[0].NCol=256;
	else
		Sprites[0].NCol=16;


	if(j>16)
	{
		// Error
		CString s;
		s.Format(L"Error : The palette of file %s use %d colors not 16 colors\r\n",Sprites[0].Name,j);
		SPgs->AddDbgInfo(s);
		return -1;
	}

/*	if(TabCol[0])
	{
		NewPal=(UCHAR *)malloc(16*3);
		memset(NewPal,0,16*3);
		j=3;
		NewPal[0]=0;
		NewPal[1]=0;
		NewPal[2]=0;

		TabCol[0]=0;
	}
	else
	{
		NewPal=(UCHAR *)malloc(16*3);
		memset(NewPal,0,16*3);
		j=3;
		NewPal[0]=0;
		NewPal[1]=0;
		NewPal[2]=0;

		TabCol[0]=1;
	}

	for(i=1;i<256;i++)
	{
		if(TabCol[i])
		{
			TabCol[i]=j/3;
			if(Pal[i*3]==0&&Pal[i*3+1]==0&&Pal[i*3+2]==0)
			{
				NewPal[j++]=8;
				NewPal[j++]=8;
				NewPal[j++]=8;
			}
			else
			{
				NewPal[j++]=Pal[i*3];
				NewPal[j++]=Pal[i*3+1];
				NewPal[j++]=Pal[i*3+2];
			}
		}
	}

	free(Sprites[0].pal);

	Sprites[0].pal=NewPal;*/


	for(n=0;n<NbSprites;n++)
	{
		if(SPgs->CheckCancelButton())
			return(-1);
		l=Sprites[n].l;
		h=Sprites[n].h;


		if(n!=0)
		{
			Sprites[n].NCol=16;
			free(Sprites[n].pal);
			if((Sprites[n].pal=(UCHAR *)malloc(16*3))==NULL)
			{
				CString s;
				s.Format(L"Error : No Memory for font %s, char %d\r\n",Sprites[0].Name,n);
				SPgs->AddDbgInfo(s);
				return -1;
			}
			memcpy(Sprites[n].pal,Sprites[0].pal,16*3);
		}


		// Remap 16 couleur
		Nb16++;

		Data=Sprites[n].data;

		k=((l+1)&0xffe)/2;

		Sprites[n].data=NewData=(UCHAR *)malloc(k*h);
		for(j=0;j<h;j++)
		{
			for(i=0;i<k-1;i++)
			{
				/*NewData[j*k+i]=TabCol[Data[j*l+i*2+1]]<<4;
				NewData[j*k+i]+=TabCol[Data[j*l+i*2]];*/
				NewData[j*k+i]=Data[j*l+i*2+1]<<4;
				NewData[j*k+i]+=Data[j*l+i*2];

			}

			if(l&1)
			{
				//NewData[j*k+i]=TabCol[Data[j*l+i*2]];
				NewData[j*k+i]=Data[j*l+i*2];
			}
			else
			{
				//NewData[j*k+i]=TabCol[Data[j*l+i*2+1]]<<4;
				//NewData[j*k+i]+=TabCol[Data[j*l+i*2]];
				NewData[j*k+i]=Data[j*l+i*2+1]<<4;
				NewData[j*k+i]+=Data[j*l+i*2];
			}

		}

		free(Data);


		Sprites[n].l16=Sprites[n].l;
		Sprites[n].l=k;
		//Sprites[n].taille=l*h/2;
		Sprites[n].taille=k*h;

		SPgs->SetPos(n);

/*		if(Sprites[n].l&1)
		{
			CString s;
			s.Format("Warning : horizontal size of Blk %d(%s) will be convert to even\r\n",n,Sprites[n].Name);
			SPgs->AddDbgInfo(s);
			Data=Sprites[n].data;
			Sprites[n].l++;			
			Sprites[n].data=NewData=(UCHAR *)malloc(Sprites[n].l*Sprites[n].h);
			for(j=0;j<Sprites[n].h;j++)
			{
				for(i=0;i<Sprites[n].l-1;i++)
				{
					NewData[i+j*Sprites[n].l]=Data[i+j*(Sprites[n].l-1)];
				}
				NewData[i+j*Sprites[n].l]=0;
			}
			
			free(Data);
		}*/
	}

	SPgs->SetStatus(L"PSX16 Sprites ... Ok\r\n");
	return 1;
}


int CVramSprites::Diff(void)
{
	int i,j,k=0,l=0;
	CString s;
	SPgs->SetStatus(L"Diff Sprites ...\r\n");
  SPgs->SetRange(0,NbSprites-2);
  SPgs->SetStep(1);
	SPgs->SetPos(0);

	for(i=0;i<NbSprites-1;i++)
	{
		for(j=i+1;j<NbSprites;j++)
		{
			if(SPgs->CheckCancelButton())
				return(-1);

			if(Sprites[i].taille==Sprites[j].taille)
			{
				if(memcmp(Sprites[i].data,Sprites[j].data,Sprites[i].taille)==0)
				{
					if(Sprites[j].EquNo==-1)
					{
						//s.Format("Sprite %d (%s)(%3dx%3d-%5d) same as %d (%s)(%3dx%3d-%5d)\r\n",i,Sprites[i].Name,Sprites[i].l,Sprites[i].h,Sprites[i].taille,j,Sprites[j].Name,Sprites[i].l,Sprites[i].h,Sprites[i].taille);
						//SPgs->AddDbgInfo(s);
						Sprites[j].EquNo=i;
						k++;
						l+=Sprites[j].taille;
					}
				}
			}
		}
		SPgs->SetPos(i);
	}
	SPgs->SetStatus(L"Diff Sprites ... Ok\r\n");
	return 1;
}

//****************************************************************

CVram::CVram(CProgressDlg *Pgs,int x,int y,int w,int h)
{
	PalZone.x=0;
	PalZone.y=0;
	PalZone.w=0;
	PalZone.h=0;
	Zone.x=x&0xffc0;
	Zone.y=y;
	Zone.w=w&0xffc0;
	Zone.h=h;
	SPgs=Pgs;
	VRamData=new USHORT[Zone.w*Zone.h];
	VRamDataM=new USHORT[Zone.w*Zone.h];
	VRamDataC=new USHORT[Zone.w*Zone.h*2];
	if(VRamData==NULL||VRamDataM==NULL||VRamDataC==NULL)
	{
		SPgs->AddDbgInfo(L"No Memory for CVram\r\n");
		SPgs->SaveDbgInfo();
	}
	else
	{
		memset(VRamData,0,Zone.w*Zone.h*2);
		memset(VRamDataM,0,Zone.w*Zone.h*2);
		memset(VRamDataC,0,Zone.w*Zone.h*2*2);
	}
}

CVram::~CVram(void)
{
	delete [] VRamData;
	delete [] VRamDataM;
	delete [] VRamDataC;
}

void CVram::Show(CViewDx *Vdd)
{
	if(Zone.w<=1024&&Zone.h<=512)
	{
		SPgs->GetDlgItem(IDC_OUT)->SetWindowText(L"Quit");
		Vdd->NewSize(Zone.w,Zone.h);
		Vdd->ShowVRAM(VRamData,Zone.w,Zone.h);
		if(Vdd->FVCalc) while(!SPgs->CheckCancelButton());
	}
	

	/*Vdd->NewSize(Zone.w*2,Zone.h);
	Vdd->ShowVRAM(VRamDataC,Zone.w*2,Zone.h);
	while(!SPgs->CheckCancelButton());*/
}


psxRECT *CVram::GetZone(void)
{
	return(&Zone);
}


int CVram::SetZone(int x,int y,int w,int h)
{
	delete []VRamData;
	delete [] VRamDataM;
	delete [] VRamDataC;
	Zone.x=x&0xffc0;
	Zone.y=y;
	Zone.w=w&0xffc0;
	Zone.h=h;
	VRamData=new USHORT[Zone.w*Zone.h];
	VRamDataM=new USHORT[Zone.w*Zone.h];
	VRamDataC=new USHORT[Zone.w*Zone.h*2];
	if(VRamData==NULL||VRamDataM==NULL||VRamDataC==NULL)
	{

		//Error
		SPgs->AddDbgInfo(L"No Memory for AutoPlace\r\n");
		return 0;
	}
	memset(VRamData,0,Zone.w*Zone.h*2);
	memset(VRamDataM,0,Zone.w*Zone.h*2);
	memset(VRamDataC,0,Zone.w*Zone.h*2*2);
	return 1;
}

int CVram::SetPalZone(int x,int y,int w,int h)
{
	PalZone.x=x&0xfff0;
	PalZone.y=y;
	PalZone.w=w&0xfff0;
	PalZone.h=h;
	return 1;
}

UWORD *CVram::GetPal(USHORT clut)
{
	int x,y;
	x=CLUT_X(clut);
	y=CLUT_Y(clut);
	x-=Zone.x;
	y-=Zone.y;
	return(VRamData+x+y*Zone.w);
}


UWORD CVram::PutPal(UCHAR *Pal,UCHAR *Trans,int NbC)
{
	UWORD LPal[256],ZPal[256];
	int i,j;
	psxRECT rc;

	if(PalZone.x==0&&PalZone.y==0&&PalZone.w==0&&PalZone.h==0)	return 0xffff;
	if(PalZone.x<Zone.x||PalZone.y<Zone.y||PalZone.x+PalZone.w>Zone.x+Zone.w||PalZone.y+PalZone.h>Zone.y+Zone.h) return 0xffff;

	memset(LPal,0,512);
	memset(ZPal,0,512);


	if(Trans!=(UCHAR*)1)
	{
		for(i=0;i<NbC;i++)
		{
			LPal[i]=Trans_Pal(*(Pal+i*3),*(Pal+i*3+1),*(Pal+i*3+2));
			if(Trans!=NULL)
				LPal[i]|=Trans[i]<<15;
		}
	}
	else
	{
		memcpy(LPal,Pal,NbC*2);
	}

	if(NbC>16)
	{
		NbC=256;
	}

	if(NbC<=16)
	{
	
		NbC=16;
	}

	for(j=PalZone.y;j<PalZone.y+PalZone.h;j++)
	{
		for(i=PalZone.x;i<PalZone.x+PalZone.w;i+=16)
		{
			if((memcmp(LPal,VRamData+(j-Zone.y)*Zone.w+(i-Zone.x),NbC*2)==0||
			   memcmp(ZPal,VRamData+(j-Zone.y)*Zone.w+(i-Zone.x),NbC*2)==0)&&
			   (i+NbC<=PalZone.x+PalZone.w))
			{
				rc.x=i;
				rc.y=j;
				rc.w=NbC;
				rc.h=1;					
				StoreImage(&rc,(UCHAR *)LPal);
				return getClut(i,j);
			}
		}
	}	


	return 0xffff;
}

UWORD CVram::PutPalFnt(UCHAR *Pal,UCHAR *Trans,int NbC)
{
	UWORD LPal[256],ZPal[256];
	int i,j;
	psxRECT rc;

	if(PalZone.x==0&&PalZone.y==0&&PalZone.w==0&&PalZone.h==0)	return 0xffff;
	if(PalZone.x<Zone.x||PalZone.y<Zone.y||PalZone.x+PalZone.w>Zone.x+Zone.w||PalZone.y+PalZone.h>Zone.y+Zone.h) return 0xffff;

	memset(LPal,0,512);
	memset(ZPal,0,512);


	if(Trans!=(UCHAR*)1)
	{
		for(i=0;i<NbC;i++)
		{
			LPal[i]=Trans_Pal(*(Pal+i*3),*(Pal+i*3+1),*(Pal+i*3+2));
			if(Trans!=NULL)
				LPal[i]|=Trans[i]<<15;
		}
	}
	else
	{
		memcpy(LPal,Pal,NbC*2);
	}

	if(NbC>16)
	{
		NbC=256;
	}

	if(NbC<=16)
	{
	
		NbC=16;
	}

	for(j=PalZone.y;j<PalZone.y+PalZone.h;j++)
	{
		for(i=PalZone.x;i<PalZone.x+PalZone.w;i+=16)
		{
			if((memcmp(ZPal,VRamData+(j-Zone.y)*Zone.w+(i-Zone.x),NbC*2)==0)&&(i+NbC<=PalZone.x+PalZone.w))
			{
				rc.x=i;
				rc.y=j;
				rc.w=NbC;
				rc.h=1;					
				StoreImage(&rc,(UCHAR *)LPal);
				return getClut(i,j);
			}
		}
	}	


	return 0xffff;
}

int CVram::LoadPage(psxRECT *rcs,UBYTE *Data,UBYTE *DataM,USHORT *DataC)
{
	int i;
	int xl,yl;
	UBYTE *Src,*SrcM,*SrcC;	
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	Src=(UBYTE *)VRamData;
	SrcM=(UBYTE *)VRamDataM;
	SrcC=(UBYTE *)VRamDataC;

	xl=(rc->x&255);
	yl=(rc->y&255);

	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;

	Data+=yl*256+xl;
	DataM+=yl*256+xl;
	DataC+=yl*256+xl;

	Src+=rc->x+rc->y*Zone.w*2;
	SrcM+=rc->x+rc->y*Zone.w*2;
	SrcC+=rc->x+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		memcpy(Data+i*256,Src+i*Zone.w*2,rc->w);
		memcpy(DataM+i*256,SrcM+i*Zone.w*2,rc->w);
		memcpy(DataC+i*256,SrcC+i*Zone.w*2,rc->w*2);
	}

	return 1;
}

int CVram::StorePage(psxRECT *rcs,UBYTE *Data,UBYTE *DataM,USHORT *DataC)
{
	int i;
	
	int xl,yl;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	UBYTE *Src,*SrcM,*SrcC;	

	Src=(UBYTE *)VRamData;
	SrcM=(UBYTE *)VRamDataM;
	SrcC=(UBYTE *)VRamDataC;

	xl=rc->x&255;
	yl=rc->y&255;

	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	

	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;

	
	Data+=yl*256+xl;
	DataM+=yl*256+xl;
	DataC+=yl*256+xl;
	Src+=rc->x+rc->y*Zone.w*2;
	SrcM+=rc->x+rc->y*Zone.w*2;
	SrcC+=rc->x+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		memcpy(Src+i*Zone.w*2,Data+i*256,rc->w);
		memcpy(SrcM+i*Zone.w*2,DataM+i*256,rc->w);
		memcpy(SrcC+i*Zone.w*2,DataC+i*256,rc->w*2);
	}

	return 1;
}

int CVram::LoadImage(psxRECT *rcs,UBYTE *Data)
{
	int i;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));
	
	rc->x-=Zone.x;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w||rc->y+rc->h>Zone.h) return 0;
	
	for(i=0;i<rc->h;i++)
		memcpy(Data+i*rc->w*2,VRamData+rc->x+(rc->y+i)*Zone.w,rc->w*2);

	return 1;
}

int CVram::StoreImage(psxRECT *rcs,UBYTE *Data)
{
	int i,j;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	UCHAR *SrcM;

	rc->x-=Zone.x;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w||rc->y+rc->h>Zone.h) return 0;

	SrcM=(UCHAR *)VRamDataM+rc->x*2+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		memcpy(VRamData+rc->x+(rc->y+i)*Zone.w,Data+i*rc->w*2,rc->w*2);
		for(j=0;j<rc->w*2;j++)
		{
			*(SrcM+j)+=1;
		}
		SrcM+=Zone.w*2;
	}

	return 1;
}

int CVram::StoreImageT(psxRECT *rcs,UBYTE *Data)
{
	int i,j;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	UCHAR *SrcM,*Src;

	rc->x-=Zone.x;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w||rc->y+rc->h>Zone.h) return 0;

	SrcM=(UCHAR *)VRamDataM+rc->x*2+rc->y*Zone.w*2;
	Src=(UCHAR *)VRamData+rc->x*2+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		//memcpy(VRamData+rc->x+(rc->y+i)*Zone.w,Data+i*rc->w*2,rc->w*2);
		for(j=0;j<rc->w*2;j++)
		{
			if(*(Data+i*rc->w*2+j))
				*(Src+j)=*(Data+i*rc->w*2+j);
			*(SrcM+j)+=1;
		}
		SrcM+=Zone.w*2;
		Src+=Zone.w*2;
	}

	return 1;
}

int CVram::MoveImage(psxRECT *rcs,int x,int y)
{
	int i;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	rc->x-=Zone.x;
	rc->y-=Zone.y;
	x-=Zone.x;
	y-=Zone.y;
	
	
	if(x<0||y<0||x>=Zone.w||y>=Zone.h) return 0;
	if(x+rc->w<0||y+rc->h<0||x+rc->w>Zone.w||y+rc->h>Zone.h) return 0;
	if(rc->x<0||rc->y<0||rc->x>=Zone.w||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w||rc->y+rc->h>Zone.h) return 0;
	
	for(i=0;i<rc->h;i++)
		memcpy(VRamData+rc->x+(rc->y+i)*Zone.w,VRamData+x+(y+i)*Zone.w,rc->w*2);

	return 1;
}

int CVram::ClearImage(psxRECT *rcs,UBYTE r,UBYTE v,UBYTE b)
{
	int i;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	rc->x-=Zone.x;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w||rc->y+rc->h>Zone.h) return 0;

	for(i=0;i<rc->h;i++)
		memset(VRamData+rc->x+(rc->y+i)*Zone.w,Trans_Pal(r,v,b),rc->w*2);

	return 1;
}


int CVram::LoadImageB(psxRECT *rcs,UBYTE *Data)
{
	int i;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	UBYTE *Src;	
	Src=(UBYTE *)VRamData;
	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;
	
	for(i=0;i<rc->h;i++)
		memcpy(Data+i*rc->w,Src+rc->x+(rc->y+i)*Zone.w*2,rc->w);

	return 1;
}

int CVram::StoreImageB(psxRECT *rcs,UBYTE *Data)
{
	int i,j;
	UBYTE *Src,*SrcM;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));
	Src=(UBYTE *)VRamData;
	SrcM=(UBYTE *)VRamDataM;
	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;	
	
	SrcM+=rc->x+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		memcpy(Src+rc->x+(rc->y+i)*Zone.w*2,Data+i*rc->w,rc->w);
		for(j=0;j<rc->w;j++)
		{
			//if(*(Data+i*rc->w+j))
				*(SrcM+j)+=1;
		}
		SrcM+=Zone.w*2;
	}

	return 1;
}

int CVram::StoreImageBT(psxRECT *rcs,UBYTE *Data)
{
	int i,j;
	UBYTE *Src,*SrcM;
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));
	Src=(UBYTE *)VRamData;
	SrcM=(UBYTE *)VRamDataM;
	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	
	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;	
	
	SrcM+=rc->x+rc->y*Zone.w*2;
	Src+=rc->x+rc->y*Zone.w*2;

	for(i=0;i<rc->h;i++)
	{
		for(j=0;j<rc->w;j++)
		{
			if(*(Data+i*rc->w+j))
				*(Src+j)=*(Data+i*rc->w+j);
			*(SrcM+j)+=1;
		}
		SrcM+=Zone.w*2;
		Src+=Zone.w*2;
	}

	return 1;
}

int CVram::MoveImageB(psxRECT *rcs,int x,int y)
{
	int i;
	UBYTE *Src;	
	psxRECT *rc,drc;
	rc=&drc;
	memcpy(&drc,rcs,sizeof(psxRECT));

	Src=(UBYTE *)VRamData;
	rc->x-=Zone.x*2;
	rc->y-=Zone.y;
	x-=Zone.x*2;
	y-=Zone.y;
	
	if(x<0||y<0||x>=Zone.w*2||y>=Zone.h) return 0;
	if(x+rc->w<0||y+rc->h<0||x+rc->w>Zone.w*2||y+rc->h>Zone.h) return 0;
	if(rc->x<0||rc->y<0||rc->x>=Zone.w*2||rc->y>=Zone.h) return 0;
	if(rc->x+rc->w<0||rc->y+rc->h<0||rc->x+rc->w>Zone.w*2||rc->y+rc->h>Zone.h) return 0;	
	
	
	for(i=0;i<rc->h;i++)
		memcpy(Src+rc->x+(rc->y+i)*Zone.w*2,Src+x+(y+i)*Zone.w*2,rc->w);

	return 1;
}


int CVram::Load(wchar_t *filename)
{
	FILE *fp;
	if((fp=_wfopen(filename,L"rb"))==NULL) return 0;
	if(fread(&Zone,1,sizeof(psxRECT),fp)!=sizeof(psxRECT))
	{
		SPgs->AddDbgInfo(L"Error on load VRAM\r\n");
		fclose(fp);
		return 0;
	}
	if(fread(VRamData,1,Zone.w*Zone.h*2,fp)!=(size_t)(Zone.w*Zone.h*2))
	{
		SPgs->AddDbgInfo(L"Error on load VRAM\r\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

int CVram::Save(wchar_t *filename)
{
	FILE *fp;
	CString s;
	s=filename;
	s+=L".vrm";
	if((fp=_wfopen(s,L"ab"))==NULL) return 0;
	if(fwrite(&Zone,1,sizeof(psxRECT),fp)!=sizeof(psxRECT))
	{
		SPgs->AddDbgInfo(L"Error on save VRAM\r\n");
		fclose(fp);
		return 0;
	}
	if(fwrite(VRamData,1,Zone.w*Zone.h*2,fp)!=(size_t)(Zone.w*Zone.h*2))
	{
		SPgs->AddDbgInfo(L"Error on save VRAM\r\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	return 1;
}

int CVram::SaveRaw(wchar_t *filename)
{
	CString s;
	CImg Img(SPgs),IMask(SPgs);
	UCHAR *pData;
	int j;

	s=filename;
	s+=".raw";

	Img.Img.w=Zone.w;
	Img.Img.h=Zone.h;
	Img.Img.nb_colors=0;
	Img.Img.data=pData=new UCHAR[Zone.w*Zone.h*3];

	for(j=0;j<Zone.w*Zone.h;j++)
	{
		pData[j*3+0]=CLUT_R(VRamData[j]);
		pData[j*3+1]=CLUT_V(VRamData[j]);
		pData[j*3+2]=CLUT_B(VRamData[j]);
	}
	Img.save_raw(s.GetBuffer(),&Img.Img);

	delete[] pData;
	return 1;
}