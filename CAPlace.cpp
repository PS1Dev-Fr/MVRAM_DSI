
#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"

#include "CImg.h"
#include "cvram.h"
#include "cdcp.h"
#include "CAPlace.h"
#include "CDOut.h"
#include "stringconversion.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CAPlace::CAPlace(CVram *V,CDOut *O,CProgressDlg *Pgs,CViewDx *Vdd,BOOL _IsHI)
{
	Vram=V;
	Out=O;
	Algo=0;
	ListPic=NULL;
	SPgs=Pgs;
	VAPos=new CVramAutoPos(Pgs,Vdd);
	SVdd=Vdd;
	Is16x256=1;
	IsZoneFilter=0;
	IsHI=_IsHI;
	bNormalRemap16=FALSE;
}

CAPlace::~CAPlace(void)
{
	DelAllPic();
	delete VAPos;
}

	
int CAPlace::AutoPos(int x,int y,int w,int h)
{
	Zone.x=x;
	Zone.y=y;
	Zone.w=w;
	Zone.h=h;

	VAPos->SetZone(Zone,Vram);
	return 1;
}

int CAPlace::SetAlgo(int A)
{
	if(Algo!=0&&Algo!=A)
	{
		Algo=A;
		if(APGo()==-1) 
		{
			SPgs->AddDbgInfo(L"Aborded !!!\r\n");
			return -1;
		}
	}
	else
		Algo=A;
	return 1;
}

int CAPlace::Set16x256Mode(void)
{
	Is16x256=1;
	return 1;
}

int CAPlace::UnSet16x256Mode(void)
{
	Is16x256=0;
	return 0;
}

int CAPlace::SetZoneFilter(int Val)
{
	IsZoneFilter=Val;
	return 1;
}

int CAPlace::UnSetZoneFilter(void)
{
	IsZoneFilter=0;
	return 0;
}

int CAPlace::ImgPos(wchar_t *Name,int x,int y)
{
	CImg Img(SPgs);
	LISTPIC *TPic;
	if(Algo==0) 
	{
		SPgs->AddDbgInfo(L"You Can't do ImgPos because Algo is no set\r\n");		
		return 0;
	}
	if(Img.Load(Name)==0)
	{
		CString s;
		s.Format(L"Error to read %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}
	TPic=AddNewPic();
	TPic->x=x;
	TPic->y=y;
	TPic->Type=APBLOCK;
	TPic->Pic=Img.Img;
	return 1;
}

int CAPlace::ImgAdd(wchar_t*Name)
{
	CImg Img(SPgs);
	LISTPIC *TPic;
	CString s;
	if(Algo==0) 
	{
		SPgs->AddDbgInfo(L"You Can't do ImgADD because Algo is no set\r\n");
		return 0;
	}
	if(Img.Load(Name)==0)
	{
		CString s;
		s.Format(L"Error to read %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		//Img.free_pic();
		return 0;
	}


	if(Img.Img.w>256||Img.Img.h>256)
	{
		CString s;
		s.Format(L"ERROR: the image \"%s\" is too big (greater than 256x256)\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}
	TPic=AddNewPic();
	TPic->x=0;
	TPic->y=0;
	TPic->Type=APIMG;
	TPic->Pic=Img.Img;
	TPic->IsZoneFilter=IsZoneFilter;
	return 1;
}



int CAPlace::ImgDCP(wchar_t *Name,int BoxCol)
{
	CImg Img(SPgs);
	Cdcp dcp(SPgs);
	VGN InfoDcp;
	LISTPIC *TPic;
	FILE *fp;
	int i;
	CString s;

	if(Algo==0) 
	{
		SPgs->AddDbgInfo(L"You can't do ImgDCP because Algo is no set\r\n");		
		return 0;
	}

	if(Img.Load(Name)==0)
	{
		CString s;
		s.Format(L"Error to read %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		//Img.free_pic();
		return 0;
	}



	if(dcp.get_info_dcp((UCHAR *)Img.Img.data,Img.Img.w,Img.Img.h,BoxCol,-1,-1,&InfoDcp)==0)
	{
		CString s;
		s.Format(L"Error on dcp %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}	
	s=Name;
	wcscpy(Name,s.Left(s.ReverseFind(L'.')));
	wcscat(Name,L".DBN");

	fp=_wfopen(Name,L"r");
	for(i=0;i<InfoDcp.nb;i++)
	{
		InfoDcp.num[i].up++;
		InfoDcp.num[i].bottom--;
		InfoDcp.num[i].left++;
		InfoDcp.num[i].right--;

		TPic=AddNewPic();
		TPic->x=0;
		TPic->y=0;
		TPic->Type=APIMG;
		memset(&TPic->Pic,0,sizeof(pic_info_t));
		dcp.dcp_to_pic(&InfoDcp.num[i],&Img.Img,&TPic->Pic);
		TPic->IsZoneFilter=IsZoneFilter;
		if(fp==NULL)
		{
			swprintf(TPic->Pic.Name,L"%s_%04d",Img.Img.Name,i);
		}
		else
		{
			char s[512];
			if(fgets(s,64,fp)==NULL)
			{
				fclose(fp);
				fp=NULL;
			}

			utf8_to_wstring(s).copy(TPic->Pic.Name,512);			

		}
	}
	if(fp!=NULL)
		fclose(fp);
	return 1;
}

int CAPlace::EndAutoPos(void)
{
	if(Algo==0) 
	{
		SPgs->AddDbgInfo(L"You can't do an EndAutoPos because the Algo is no set\r\n");
		return 0;
	}
	if(APGo()==-1) 
	{
		SPgs->AddDbgInfo(L"Aborded !!!\r\n");
		return -1;
	}
	else
		VAPos->ToVram(Vram);
	return 1;
}

int CAPlace::APGo(void)
{
	int NbS=GetNbPic();
	LISTPIC *TPic;
	CString s;
	if(NbS==0) return 0;	
	if(Algo==0) return 0;

	CVramSprites *Sprite= new CVramSprites(NbS,SPgs);
	s.Format(L"AutoPlace %d Sprites\r\n",NbS);
	SPgs->AddDbgInfo(s);
	Sprite->SetRemap16Mode(bNormalRemap16);

	TPic=ListPic;
	do
	{
		if(TPic->Type==APBLOCK)
		{
			// To Do !!!
		}
		else
		{
			Sprite->Add(&TPic->Pic,TPic->IsZoneFilter);
		}				
		TPic=TPic->pLPic;
	}
	while(TPic!=NULL);

	if(Sprite->Remap16()==-1) return -1;
	if(Sprite->Sort()==-1)  return -1;
	if (Sprite->Diff()==-1) return -1;
	
	VAPos->SetSprite(Sprite->Sprites,NbS,Sprite->Nb16);

	if(Is16x256)
	{
		VAPos->Set16x256();
	}	
	else
	{
		VAPos->UnSet16x256();
	}

	switch(Algo)
	{
		case AMAXREC:
			if(VAPos->ExecAlgoMaxRecouvre()==-1) return -1;
		break;
		case AOPTIMUM:
			if(VAPos->ExecAlgoOptimum()==-1) return -1;
		break;
		case AHORIZONTAL:
			if(VAPos->ExecAlgoHorizontal()==-1) return -1;
		break;
		case AVERTICAL:
			if(VAPos->ExecAlgoVertical()==-1) return -1;
		break;
		case ASELONFORME:
			if(VAPos->ExecAlgoSelonForme()==-1) return -1;
		break;
		default :
			SPgs->AddDbgInfo(L"Error : Autoplace algo no exist\r\n");
		break;
	}

	if(Sprite->UnSort()==-1) return -1;
	VAPos->Save(Vram,Out,IsHI);
	DelAllPic();

	delete Sprite;
	return 1;
}

LISTPIC *CAPlace::AddNewPic(void)
{
	LISTPIC *TPic;
	if(ListPic!=NULL)
	{	
		
		TPic=ListPic;
		while(TPic->pLPic!=NULL)
		{
			TPic=TPic->pLPic;			
		}

		TPic->pLPic=new LISTPIC;
		if(TPic->pLPic==NULL) return NULL;
		TPic=TPic->pLPic;

	}
	else
	{
		TPic=ListPic=new LISTPIC;
		if(TPic==NULL) return NULL;
	}	

	TPic->pLPic=NULL;

	return TPic;
}

int CAPlace::DelAllPic(void)
{
	LISTPIC *TPic,*TPic2;
	CImg Img(SPgs);
	if(ListPic==NULL) return 0;
	TPic=ListPic;
	while(TPic!=NULL)
	{
		//Img.free_pic(&TPic->Pic);
		TPic2=TPic->pLPic;
		delete TPic;
		TPic=TPic2;
	}
	ListPic=NULL;
	return 1;
}

int CAPlace::GetNbPic(void)
{
	LISTPIC *TPic;
	int NbPic=0;
	if(ListPic==NULL) return 0;
	TPic=ListPic;

	while(TPic!=NULL)
	{
		TPic=TPic->pLPic;
		NbPic++;
	}

	return NbPic;
}
