
#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include <io.h>
#include <fcntl.h>

#include "CImg.h"
#include "cvram.h"
#include "cdcp.h"
#include "CAPlace.h"
#include "cscript.h"
#include "Font.h"
#include "FileText.h"
#include "stringconversion.h"
										
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CScript::CScript(CProgressDlg *Pgs,CViewDx *Vdd,BOOL _SaveRaw)
{
	CPath[0]=0;
	CPathH[0]=0;
	SPgs=Pgs;
	Vram=new CVram(Pgs);
	Out=new CDOut(Pgs,Vdd);
	SVdd=Vdd;
	IsZoneFilter=0;
	IsHI=0;
	FontNbPageW=1;
	SaveRaw=_SaveRaw;
	bNormalRemap16=FALSE;
	pPlace=NULL;
}

CScript::~CScript(void)
{
	delete Vram;
	delete Out;
}
void CScript::Filtre(char *Line)
{
	char *Txt=Line,c;
	unsigned int i,l;
	l=(int)strlen(Txt);
	for(i=0;i<l;i++)
	{
		c=*Txt;
		if(c==','||c=='('||c==')') *Txt=' ';
		Txt++;
	}
}

int CScript::ParseCommonCommand(char *Cmd)
{
	int res=1;
	if (strcmp(Cmd,"NORMALREMAP16")==0)
	{
		bNormalRemap16=TRUE;
	}
	else if (strcmp(Cmd,"ORIGINALREMAP16")==0)
	{
		bNormalRemap16=FALSE;
	}
	else
		res=0;
	if (pPlace)
		pPlace->SetRemap16Mode(bNormalRemap16);
	return res;
}

int CScript::SetZoneFilter(int Val)
{
	IsZoneFilter=Val;
	return 1;
}

int CScript::UnSetZoneFilter(void)
{
	IsZoneFilter=0;
	return 0;
}

int CScript::AutoPos(int x,int y,int w,int h)
{
	int retCode;

	pPlace=new CAPlace(Vram,Out,SPgs,SVdd,IsHI);

	pPlace->AutoPos(x,y,w,h);
	if(IsZoneFilter)
		pPlace->SetZoneFilter(IsZoneFilter);
	pPlace->SetRemap16Mode(bNormalRemap16);	// bp
	retCode=DoAutoPos(x,y,w,h);

	delete pPlace;
	pPlace=NULL;

	return retCode;
}


int CScript::DoAutoPos(int x,int y,int w,int h)
{
	int r=0;
	int exitCode=0;

	try
	{
		while(1)
		{
			memset(Ligne,0,256);
			if(fgets(Ligne,256,fp)==NULL)
				break;
			memset(Cmd,0,sizeof(Cmd));
			Filtre(Ligne);
			NbParam=sscanf(Ligne,"%s %s %s %d %d %d %d\n",&Cmd,&TmpStr,&TmpStr2,&P2,&P3,&P4,&P5);
			NoLine++;

			_strupr(Cmd);
			_strupr(TmpStr);

			if(Cmd[0]=='#'||Cmd[0]==';'||NbParam<=0)
			{
				// do nothing on comment 
			}
			else if (ParseCommonCommand(Cmd))
			{
				// nothing to do
			}
			else
			if (strcmp(Cmd,"INCLUDE")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				CString s;
				s.Format(L">>>>>> Processing include \"%s\" (line %d)\r\n",TmpStr,NoLine);
				SPgs->AddDbgInfo(s);

				{	// Empile quelques variables pour autoriser la récursivité
					int saveNoLine=NoLine;
					FILE *saveFile=fp;

					std::wstring wstr = utf8_to_wstring(TmpStr);
					wchar_t* wstr2 = new wchar_t[wcslen(wstr.c_str()) + 1];
					wcscpy(wstr2, wstr.c_str());
					OpenScript(wstr2);
					delete[] wstr2;
					DoAutoPos(x,y,w,h);
					CloseScript();
					// Restaure les variables pour la récursivité
					NoLine=saveNoLine;
					fp=saveFile;
				}
				s.Format(L"<<<<<< End of include\r\n");
				SPgs->AddDbgInfo(s);
			}
			else
			if(strcmp(Cmd,"ADDZONE")==0)
			{
				//to do!!
				CString s;
				s.Format(L"Error Line :%4d, AddZone not implemented \r\n",NoLine);
				SPgs->AddDbgInfo(s);
			}
			else
			if(strcmp(Cmd,"SET16X256")==0)
			{
				pPlace->Set16x256Mode();
			}
			else
			if(strcmp(Cmd,"UNSET16X256")==0)
			{
				pPlace->UnSet16x256Mode();
			}
			else
			if(strcmp(Cmd,"SETZONEFILTER")==0)
			{

				if(NbParam==2)
				{
					pPlace->SetZoneFilter(atoi(TmpStr));
				}
				else
				{
					CString s;
					s.Format(L"Error Line :%4d, incorrect param number for SetZoneFilter\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"UNSETZONEFILTER")==0)
			{
				pPlace->UnSetZoneFilter();
			}
			else
			if(strcmp(Cmd,"ALGO")==0)
			{
				if(strcmp(TmpStr,"MAXREC")==0)
					r=pPlace->SetAlgo(AMAXREC);
				else
				if(strcmp(TmpStr,"OPTIMUM")==0)
					r=pPlace->SetAlgo(AOPTIMUM);
				else
				if(strcmp(TmpStr,"HORIZONTAL")==0)
					r=pPlace->SetAlgo(AHORIZONTAL);
				else
				if(strcmp(TmpStr,"VERTICAL")==0)
					r=pPlace->SetAlgo(AVERTICAL);
				else
				if(strcmp(TmpStr,"SELONFORME")==0)
					r=pPlace->SetAlgo(ASELONFORME);
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Algo  %s not implemented\r\n",NoLine,Cmd);
					SPgs->AddDbgInfo(s);
					throw 0;
				}
			}
			else
			if(strcmp(Cmd,"IMGPOS")==0)
			{
				// to do !!!		
				/*
				if(CPath[0]!=0)
					sprintf(Name,"%s\\%s",CPath,TmpStr);
				else
					strcpy(Name,TmpStr);
				pPlace->ImgPos(Name,P1,P2);*/
				CString s;
				s.Format(L"Error Line :%4d, ImgPos not implemented\r\n",NoLine);
				SPgs->AddDbgInfo(s);
			}
			else
			if(strcmp(Cmd,"IMGDCP")==0)
			{
				if(NbParam<=2) 
					strcpy(TmpStr2,"0");

				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				std::wstring wstr = utf8_to_wstring(TmpStr);
					
				if(CPath[0]!=0)
					swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
				else
					swprintf(Name, L"%s", wstr.c_str());

				pPlace->ImgDCP(Name,atoi(TmpStr2));
			}
			else
			if(strcmp(Cmd,"IMGADD")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				std::wstring wstr = utf8_to_wstring(TmpStr);

				if (CPath[0] != 0)
					swprintf(Name, L"%s\\%s", CPath, wstr.c_str());
				else
					swprintf(Name, L"%s", wstr.c_str());

				pPlace->ImgAdd(Name);
			}
			else
			if(strcmp(Cmd,"ENDAUTOPOS")==0)
			{
				r=pPlace->EndAutoPos();
				throw 1;
			}
			else
			if(strcmp(Cmd,"PATH")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
				else
				{
					std::wstring wstr = utf8_to_wstring(TmpStr);
					swprintf(CPath, L"%s", wstr.c_str());
				}
			}
			else
			{
				// command don't exist
				CString s;
				s.Format(L"Error Line :%4d, i don't Don't knows command %s \r\n",NoLine,Cmd);
				SPgs->AddDbgInfo(s);
				throw 0;
			}

			if(r==-1)
			{
				throw -1;
			}
		}
	}
	catch (int code)
	{
		exitCode=code;
	}

	return exitCode;
}

int CScript::LoadTim(wchar_t *Path,int Mode,int t)
{
	struct _wfinddata_t c_file;
	intptr_t hFile;
	int Nbfile=0;
	CImg Img(SPgs);
	CString s,s2,s3;
	BLKPSX Bnk;
	short x,y,w,h;
	wchar_t Path2[256];
	char STmp[256];
	FILE *fp;

	s=Path;
	s2=s.Left(s.ReverseFind(L'\\'));
	s2+=L"\\";
	s=s2;

	if( (hFile = _wfindfirst(Path, &c_file )) == -1L )
	{
		CString s;
		s.Format(L"can't find file %s\r\n",Path);
		SPgs->AddDbgInfo(s);
		return 0;
	}
	else
	{
		do
		{
			s2=s;
			s2+=c_file.name;
			if(Img.LoadTim(s2.GetBuffer())==0)
			{
				CString s;
				s.Format(L"Error to read %s file\r\n",s2);
				SPgs->AddDbgInfo(s);
				return 0;
			}
			else
			{
				Vram->StoreImageT(&Img.Img.TData,(UCHAR *)Img.Img.data);
				//Vram->StoreImage(&Img.Img.TClut,(UCHAR *)Img.Img.palette);
				
				s3=s2;
				wcscpy(Path2,s3.Left(s3.ReverseFind(L'.')));
				wcscat(Path2,L".T");

				fp=_wfopen(Path2,L"r");
				if(fp!=NULL)
				{
					CString s;
					UWORD *Pal; 
					Pal=(UWORD*)Img.Img.palette;
					if(Img.Img.nb_colors<=256)
					{
						s.Format(L"Use trans file %s\r\n",Path2);
						SPgs->AddDbgInfo(s);
						do
						{
							fgets(STmp,256,fp);
							t=atoi(STmp);
							if(t>=0||t<256)
							{
								Img.Img.Trans[t]=1;
								Pal[t]|=0x8000;
								s.Format(L"For Pal Index %d\r\n",t);
							}
						}
						while(!feof(fp));
						fclose(fp);
					}
					else
						s.Format(L"warning Found trans file %s but >=32768\r\n",Path2);
				}

				if(Img.Img.nb_colors<=16)
				{
					x=Img.Img.TData.x*4;
					y=Img.Img.TData.y;
					w=Img.Img.w;
					h=Img.Img.h;

					Bnk.u0=(x&0x7f);
					Bnk.u1=(x&0x7f)+w-1;
					Bnk.u2=(x&0x7f);
					Bnk.u3=(x&0x7f)+w-1;

					Bnk.v0=(UCHAR)y;
					Bnk.v1=(UCHAR)y;
					Bnk.v2=(UCHAR)y+h-1;
					Bnk.v3=(UCHAR)y+h-1;

					Bnk.tpage=getTPage(0,t,x>>2,y);

					if(!Mode)
					{
						psxRECT rc;
						Bnk.clut=getClut(Img.Img.TClut.x,Img.Img.TClut.y);
						rc.x=Img.Img.TClut.x;
						rc.y=Img.Img.TClut.y;
						rc.w=16;
						rc.h=1;					
						Vram->StoreImage(&rc,(UCHAR *)Img.Img.palette);
					}
					else
					{
						if((Bnk.clut=Vram->PutPal((UCHAR *)Img.Img.palette,(UCHAR*)1,16))==0xffff)
						{
							// Error
							SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
						}
					}
					CString DbP;
					DbP.Format(L"Clut :%3d %4d %3d(%4x)\r\n",16, CLUT_X(Bnk.clut), CLUT_Y(Bnk.clut),Bnk.clut);
					SPgs->AddDbgInfo(DbP);

				}
				else
				if(Img.Img.nb_colors<=256)
				{
					x=Img.Img.TData.x*2;
					y=Img.Img.TData.y;
					w=Img.Img.w;
					h=Img.Img.h;

					Bnk.u0=(x&0x7f);
					Bnk.u1=(x&0x7f)+w-1;
					Bnk.u2=(x&0x7f);
					Bnk.u3=(x&0x7f)+w-1;

					Bnk.v0=(UCHAR)y;
					Bnk.v1=(UCHAR)y;
					Bnk.v2=(UCHAR)y+h-1;
					Bnk.v3=(UCHAR)y+h-1;

					Bnk.tpage=getTPage(1,t,x>>1,y);
					if(!Mode)
					{
						psxRECT rc;
						Bnk.clut=getClut(Img.Img.TClut.x,Img.Img.TClut.y);
						rc.x=Img.Img.TClut.x;
						rc.y=Img.Img.TClut.y;
						rc.w=256;
						rc.h=1;					
						Vram->StoreImage(&rc,(UCHAR *)Img.Img.palette);
					}
					else
					{
						if((Bnk.clut=Vram->PutPal((UCHAR *)Img.Img.palette,(UCHAR*)1,256))==0xffff)
						{
							// Error
							SPgs->AddDbgInfo(L"No Room for all palette\r\n");		
						}
					}
					CString DbP;
					DbP.Format(L"Clut :%3d %4d %3d(%4x)\r\n",256, CLUT_X(Bnk.clut), CLUT_Y(Bnk.clut),Bnk.clut);
					SPgs->AddDbgInfo(DbP);


				}
				else
				if(Img.Img.nb_colors<=32768)
				{
					x=Img.Img.TData.x;
					y=Img.Img.TData.y;
					w=Img.Img.w;
					h=Img.Img.h;

					Bnk.u0=(x&0x3f);
					Bnk.u1=(x&0x3f)+w-1;
					Bnk.u2=(x&0x3f);
					Bnk.u3=(x&0x3f)+w-1;

					Bnk.v0=(UCHAR)y;
					Bnk.v1=(UCHAR)y;
					Bnk.v2=(UCHAR)y+h-1;
					Bnk.v3=(UCHAR)y+h-1;
					Bnk.tpage=getTPage(2,t,x,y);
					Bnk.clut=0;
				}


				CString s;
				s.Format(L"Load Tim %s ,Clut : 0x%04x Tpage 0x%04x\r\n",Path,Bnk.clut,Bnk.tpage);
				SPgs->AddDbgInfo(s);

				std::string str = wstring_to_utf8(Img.Img.Name);

				Out->AddData(str.c_str(),Bnk,FALSE);
				Img.free_pic();
			}
		}
		while( _wfindnext( hFile, &c_file ) == 0 );

		_findclose( hFile );

	}
	return 1;
}

int CScript::Fonts(wchar_t *Name,char *sType,int Spd,int Rot,int x,int y,int h,int First,int Sp,int SpW,int SpH,int FntNPSize)
{
	CVramAutoPos *VAPos=new CVramAutoPos(SPgs,SVdd);
	psxRECT Zone;
	CImg Img(SPgs);
	Cdcp dcp(SPgs);
	VGN InfoDcp;
	int i,Type;
	CString s;
	pic_info_t *Pic;

	Zone.x=(x*2)&(~127);
	Zone.y=y;
	Zone.w=128*FontNbPageW;
	Zone.h=h;

	if(!IsHI)
	if((y&255)+h>256)
	{
		//Error
		s.Format(L"The font(%s) Zone is more big\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}
	
	if(strcmp(sType,"PAL")==0)
	{
		Type=FT_PAL;
	}
	else if(strcmp(sType,"INTER")==0)
	{
		Type=FT_INTER;
	}
	else if(strcmp(sType,"MOTO")==0)
	{
		Type=FT_MOTO;
	}
	else if(strcmp(sType,"ALIAS")==0)
	{
		Type=FT_ALIAS;
	}
	else
	{
		s.Format(L"The font(%s) Type's not exist\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}

	VAPos->SetZone(Zone,Vram);

	if(Img.Load(Name)==0)
	{
		s.Format(L"Error to read %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}

	if(dcp.get_info_dcp((UCHAR *)Img.Img.data,Img.Img.w,Img.Img.h,0,-1,-1,&InfoDcp)==0)
	{
		s.Format(L"Error on dcp %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}	

	if((Pic=new pic_info_t[InfoDcp.nb])==NULL)
	{
		s.Format(L"Error No Memory for autopos fonts %s \r\n",Img.Img.Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}

	memset(Pic,0,sizeof(pic_info_t)*InfoDcp.nb);

	CVramSprites *Sprite= new CVramSprites(InfoDcp.nb,SPgs);
	s.Format(L"AutoPlace %d Char for font %s \r\n",InfoDcp.nb,Img.Img.Name);
	SPgs->AddDbgInfo(s);


	for(i=0;i<InfoDcp.nb;i++)
	{
		InfoDcp.num[i].up++;
		InfoDcp.num[i].bottom--;
		InfoDcp.num[i].left++;
		InfoDcp.num[i].right--;
		dcp.dcp_to_pic(&InfoDcp.num[i],&Img.Img,&Pic[i]);
		//sprintf(Pic[i].Name,"%s Char 0x%02x(%c)",Img.Img.Name,i+First,i+First);
		if(FntNPSize!=-1)
		{
			int w,h;
			UBYTE *pDataSrc;
			UBYTE *pDataDest;
			w=Pic[i].w;
			h=Pic[i].h;
			pDataSrc=(UBYTE*)Pic[i].data;
			if(w>FntNPSize)
			{
				s.Format(L"Warning : in The font(%s) the char %d is > FntNPSize\r\n",Name,i);
				SPgs->AddDbgInfo(s);		
			}
			else
			if(w!=FntNPSize)
			{
				UBYTE *pData;				
				int i,j;
				pDataDest=(UBYTE *)malloc(FntNPSize*h);
				memset(pDataDest,0,FntNPSize*h);
				pData=pDataSrc;
				for(j=0;j<h;j++)
				{
					for(i=(FntNPSize-w)/2;i<(FntNPSize+w)/2;i++)
					{
						pDataDest[i+j*FntNPSize]=*pData++;
					}
				}
				free(pDataSrc);
				pDataSrc=pDataDest;
			}
			Pic[i].w=FntNPSize;
			Pic[i].data=pDataSrc;
		}

		wcscpy(Pic[i].Name,Img.Img.Name);
		Sprite->Add(&Pic[i],IsZoneFilter);
	}

	switch(Rot)
	{
		case ROT_90:
		Sprite->Rot(Rot);
		Type|=FT_90;
		break;
		case ROT_180:
		Sprite->Rot(Rot);
		Type|=FT_180;
		break;
		case ROT_270:
		Sprite->Rot(Rot);
		Type|=FT_270;
		break;
	}

	if(Sprite->PSX16()==-1) return -1;
	if(Sprite->Sort()==-1)  return -1;
	if(Sprite->Diff()==-1) return -1; 
	
	VAPos->SetSprite(Sprite->Sprites,InfoDcp.nb,InfoDcp.nb);

	if(Spd)
	{
		if(VAPos->ExecAlgoOptimum()==-1) return -1;
	}
	else
	{
		if(VAPos->ExecAlgoHorizontal()==-1) return -1;
	}
	
	if(Sprite->UnSort()==-1) return -1;

	VAPos->ToVram(Vram);

	VAPos->SaveFNT(Vram,Out,Type,First,Sp,SpW,SpH,IsHI);

	delete Sprite;
	delete [] Pic;
	delete VAPos;
	return 1;
}

int CScript::FontsI(wchar_t *Name,wchar_t *NameFti,int Spd,int x,int y,int h,int Mode)
{
	CVramAutoPos *VAPos=new CVramAutoPos(SPgs,SVdd);
	psxRECT Zone;
	CImg Img(SPgs);
	Cdcp dcp(SPgs);
	VGN InfoDcp;
	int i,handle,size,tl;
	CString s;
	pic_info_t *Pic;

	FNTCLIST Fnt;

	Zone.x=(x*2)&(~127);
	Zone.y=y;
	Zone.w=128*FontNbPageW;
	Zone.h=h;
	Fnt.PCalcSwp=NULL;
	
	if(!IsHI)
	if((y&255)+h>256)
	{
		//Error
		s.Format(L"The font(%s) Zone is more big\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}
	
	VAPos->SetZone(Zone,Vram);
	if((handle=_wopen(NameFti,O_RDONLY|O_BINARY))==-1)
	{
		s.Format(L"Error to read %s file\r\n",NameFti);
		SPgs->AddDbgInfo(s);
		return(0);
	}

	size=_filelength(handle);
	_read(handle,&Fnt.Hdr,sizeof(FNTHDRS));
	_read(handle,&Fnt.SwpList[0],size-sizeof(FNTHDRS));
	_close(handle);

	if(Mode==0)
	{
		Fnt.Hdr.NbSwp=Fnt.Hdr.Mode;
	}
	else
	{
		tl=((Fnt.Hdr.NbC*Fnt.Hdr.NbC)&0xfffffffc)+4;
		Fnt.PCalcSwp=new UCHAR[tl];
		memset(Fnt.PCalcSwp,0,tl);
		Fnt.Hdr.NbSwp=Fnt.Hdr.Mode;
		s.Format(L"NbSwp %d\r\n",tl);
		SPgs->AddDbgInfo(s);
		for(i=0;i<Fnt.Hdr.NbSwp;i++)
		{
			Fnt.PCalcSwp[(Fnt.SwpList[i].L1-Fnt.Hdr.First)+Fnt.Hdr.NbC*(Fnt.SwpList[i].L2-Fnt.Hdr.First)]=(char)Fnt.SwpList[i].Sw;
			//s.Format(L"Swp %2d(%4d) (%c(%2d),%c(%2d))=%+4d\r\n",i,(Fnt.SwpList[i].L1-Fnt.Hdr.First)+Fnt.Hdr.NbC*(Fnt.SwpList[i].L2-Fnt.Hdr.First),Fnt.SwpList[i].L1,Fnt.SwpList[i].L1-Fnt.Hdr.First,Fnt.SwpList[i].L2,Fnt.SwpList[i].L2-Fnt.Hdr.First,Fnt.SwpList[i].Sw);
			//SPgs->AddDbgInfo(s);
		}
	}
	Fnt.Hdr.Mode=Mode;


	if(Img.Load(Name)==0)
	{
		s.Format(L"Error to read %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}

	if(dcp.get_info_dcp((UCHAR *)Img.Img.data,Img.Img.w,Img.Img.h,0,-1,-1,&InfoDcp)==0)
	{
		s.Format(L"Error on dcp %s file\r\n",Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}	

	if((Pic=new pic_info_t[InfoDcp.nb])==NULL)
	{
		s.Format(L"Error No Memory for autopos fonts %s \r\n",Img.Img.Name);
		SPgs->AddDbgInfo(s);		
		Img.free_pic();
		return 0;
	}

	memset(Pic,0,sizeof(pic_info_t)*InfoDcp.nb);

	CVramSprites *Sprite= new CVramSprites(InfoDcp.nb,SPgs);
	s.Format(L"AutoPlace %d Char for font %s \r\n",InfoDcp.nb,Img.Img.Name);
	SPgs->AddDbgInfo(s);


	for(i=0;i<InfoDcp.nb;i++)
	{
		InfoDcp.num[i].up++;
		InfoDcp.num[i].bottom--;
		InfoDcp.num[i].left++;
		InfoDcp.num[i].right--;
		dcp.dcp_to_pic(&InfoDcp.num[i],&Img.Img,&Pic[i]);
		//sprintf(Pic[i].Name,"%s Char 0x%02x(%c)",Img.Img.Name,i+Fnt.Hdr.First,i+Fnt.Hdr.First);
		wcscpy(Pic[i].Name,Img.Img.Name);
		Sprite->Add(&Pic[i],IsZoneFilter);
	}

	switch(Fnt.Hdr.Type&0xf0)
	{
		case FT_90:
		Sprite->Rot(ROT_90);
		break;
		case FT_180:
		Sprite->Rot(ROT_180);
		break;
		case FT_270:
		Sprite->Rot(ROT_270);
		break;
	}

	if(Sprite->PSX16()==-1) return -1;
	if(Sprite->Sort()==-1)  return -1;
	if(Sprite->Diff()==-1) return -1; 
	
	VAPos->SetSprite(Sprite->Sprites,InfoDcp.nb,InfoDcp.nb);

	if(Spd)
	{
		if(VAPos->ExecAlgoOptimum()==-1) return -1;
	}
	else
	{
		if(VAPos->ExecAlgoHorizontal()==-1) return -1;
	}
	
	if(Sprite->UnSort()==-1) return -1;

	VAPos->ToVram(Vram);

	VAPos->SaveFNT(Vram,Out,Fnt,IsHI);

	delete Sprite;
	delete [] Pic;
	delete VAPos;
	return 1;
}

int CScript::StrEnvVar(char *Str)
{
	char C,Env[512],Val[512],*PVal;
	int i=0,ValLen,TLen,RLen;
	int Pos1=-1;
	//printf("Deb %s\n",Str);
	do
	{
		C=*(Str+i);
		if(C=='%')
		{
			if(Pos1==-1)
			{
				Pos1=i;
			}
			else
			{
				TLen=(i-1)-Pos1;
				strncpy(Env,Str+Pos1+1,TLen);
				Env[TLen]=0;
				_strupr(Env);
				PVal=getenv(Env);
		 		if(PVal==NULL)
				{
    				//fprintf(fperr,"Var Env %s No Found\n",Env);
						return(0);
				}

				strcpy(Val,PVal);
				ValLen=(int)strlen(Val);
				RLen=(int)strlen(Str+i+1);

				memmove(Str+i-1+(ValLen-TLen),Str+i+1,RLen+1);
				strncpy(Str+Pos1,Val,ValLen);

				//printf("Res %s\n",Str);
				i=i-2+(ValLen-TLen);
				Pos1=-1;
			}
		}
		i++;
	}while(C);

	//printf("Fin %s\n",Str);
	return 1;
}	

FILE *CScript::OpenScript(wchar_t *Path)
{
	NoLine=0;
	if((fp=_wfopen(Path,L"r"))==NULL)
	{
		CString s;
		s.Format(L"ERROR:Script \"%s\" not found\r\n",Path);
		AfxMessageBox(s);
		SPgs->AddDbgInfo(s);
	}
	return fp;
}

void CScript::CloseScript(void)
{
	fclose(fp);
}

void CScript::InitExec(void)
{
	InitMode=1;
	PalInit=0;
	fp=NULL;
}

int CScript::Exec(wchar_t *Path)
{
	InitExec();
	return DoExec(Path);
}

// Attention la gestion des erreurs n'est pas top dans cette fonction (voir les return...)
int CScript::DoExec(wchar_t *Path)
{
	int exitCode;
	int r=0;
	FILE *saveFile;
	int saveNoLine;

	// Empile quelques variables pour autoriser la récursivité
	saveNoLine=NoLine;
	saveFile=fp;
	exitCode=1;

	try
	{
		if (OpenScript(Path)==NULL)
			throw 0;

		while(!feof(fp))
		{
			memset(Ligne,0,256);
			if(fgets(Ligne,256,fp)==NULL)
				break;
			memset(Cmd,0,sizeof(Cmd));
			Filtre(Ligne);
			NbParam=sscanf(Ligne,"%s %s %s %s %d %d %d %d %d %d %d %d %d\n",&Cmd,&TmpStr,&TmpStr2,&TmpStr3,&P2,&P3,&P4,&P5,&P6,&P7,&P8,&P9,&P10);
			NoLine++;

			_strupr(Cmd);
			_strupr(TmpStr);
			_strupr(TmpStr2);
			if(Cmd[0]=='#'||Cmd[0]==';'||NbParam<=0)
			{
				// do nothing on comment 
			}
			else if (ParseCommonCommand(Cmd))
			{
				// nothing to do
			}
			else
			if (strcmp(Cmd,"INCLUDE")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

			#if 0
				if(CPath[0]!=0)
					sprintf(Name,"%s\\%s",CPath,TmpStr);
				else
					strcpy(Name,TmpStr);
			#endif
				CString s;
				s.Format(L">>>>>> Processing include \"%s\" (line %d)\r\n",TmpStr,NoLine);
				SPgs->AddDbgInfo(s);

				std::wstring wstr = utf8_to_wstring(TmpStr);
				wchar_t* wstr2 = new wchar_t[wcslen(wstr.c_str()) + 1];
				wcscpy(wstr2, wstr.c_str());
				DoExec(wstr2);
				delete[] wstr2;
				s.Format(L"<<<<<< End of include\r\n");
				SPgs->AddDbgInfo(s);
			}
			else
			if(strcmp(Cmd,"DEFPALETTE")==0)
			{
				Vram->SetPalZone(atoi(TmpStr),atoi(TmpStr2),atoi(TmpStr3),P2);
				PalInit=1;
			}
			else
			if(strcmp(Cmd,"DEFVRAM")==0)
			{
				if(InitMode)
				{
					Vram->SetZone(atoi(TmpStr),atoi(TmpStr2),atoi(TmpStr3),P2);
				}
				else
				{
					//Error
					CString s;
					s.Format(L"Error Line :%4d, You can't change Vram Pos to this\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"SETVRAMHI")==0)
			{
				IsHI=1;
			}
			else
			if(strcmp(Cmd,"SETVRAMLO")==0)
			{
				IsHI=0;
			}
			else
			if(strcmp(Cmd,"SETNBPAGEFONTW")==0)
			{
				if(NbParam==2)
				{
					FontNbPageW=atoi(TmpStr);
				}
				else
				{
					CString s;
					s.Format(L"Error Line :%4d, incorrect param number for SetNbPageFontW\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

			}
			else
			if(strcmp(Cmd,"AUTOPOS")==0)
			{

				if(PalInit)
				{			
					r=AutoPos(atoi(TmpStr),atoi(TmpStr2),atoi(TmpStr3),P2);
					InitMode=0;
				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for AutoPos\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"SETZONEFILTER")==0)
			{
				if(NbParam==2)
				{
					SetZoneFilter(atoi(TmpStr));
				}
				else
				{
					CString s;
					s.Format(L"Error Line :%4d, incorrect param number for SetZoneFilter\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

			}
			else
			if(strcmp(Cmd,"UNSETZONEFILTER")==0)
			{
				UnSetZoneFilter();
			}
			else
			if(strcmp(Cmd,"LOADTIM")==0)
			{
				if(PalInit)
				{	
					if(!StrEnvVar(TmpStr))
					{
						CString s;
						s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}

					std::wstring wstr = utf8_to_wstring(TmpStr);

					if(CPath[0]!=0)
						swprintf(Name,L"%s\\%s",CPath, wstr.c_str());
					else
						wcscpy(Name, wstr.c_str());

					if(NbParam==2)
					{
						LoadTim(Name,0,1);
					}
					else
					if(NbParam==3)
					{
						LoadTim(Name,atoi(TmpStr2),1);
					}
					else
					if(NbParam==4)
					{
						LoadTim(Name,atoi(TmpStr2),atoi(TmpStr3));
					}
					else
					{
						CString s;
						s.Format(L"Error Line :%4d, incorrect param number for LoadTim\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for AutoPos\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}			
			}
			else
			if(strcmp(Cmd,"FONTS")==0)
			{
				if(PalInit)
				{			
					if(NbParam==12||NbParam==13)
					{
						int FntNPSize;
						if(NbParam==12)
							FntNPSize=-1;
						else
							FntNPSize=P10;
						if(!StrEnvVar(TmpStr))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}

						std::wstring wstr = utf8_to_wstring(TmpStr);

						if(CPath[0]!=0)
							swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
						else							
							wcscpy(Name,wstr.c_str());

						r=Fonts(Name,TmpStr2,atoi(TmpStr3),P2,P3,P4,P5,P6,P7,P8,P9,FntNPSize);
						InitMode=0;
					}
					else
					{
						CString s;
						s.Format(L"Error Line :%4d, incorrect param number for Fonts\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Fonts\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"FONTSI")==0)
			{
				if(PalInit)
				{			
					if(NbParam==8)
					{
						if(!StrEnvVar(TmpStr))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}

						if(!StrEnvVar(TmpStr2))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}

						std::wstring wstr = utf8_to_wstring(TmpStr);

						if(CPath[0]!=0)
							swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
						else
							wcscpy(Name,wstr.c_str());

						std::wstring wstr2 = utf8_to_wstring(TmpStr2);

						if(CPath[0]!=0)
							swprintf(Name2,L"%s\\%s",CPath,wstr2.c_str());
						else
							wcscpy(Name2,wstr2.c_str());
						r=FontsI(Name,Name2,atoi(TmpStr3),P2,P3,P4,P5);
						InitMode=0;
					}
					else
					{
						CString s;
						s.Format(L"Error Line :%4d, incorrect param number for Fonts\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Fonts\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"LOADBS")==0)
			{
				if(PalInit)
				{			
					
					if(NbParam==4)
					{
						MDECINFOD Mdec;
						int handle;


						if(!StrEnvVar(TmpStr))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}
						if(!StrEnvVar(TmpStr2))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}
						if(!StrEnvVar(TmpStr3))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}

						Mdec.Mdec.w=atoi(TmpStr2);
						Mdec.Mdec.h=atoi(TmpStr3);

						std::wstring wstr = utf8_to_wstring(TmpStr);

						if(CPath[0]!=0)
							swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
						else
							wcscpy(Name,wstr.c_str());
					
						CString s;
						s.Format(L"LoadBS Files %s (%d,%d)\r\n",Name,Mdec.Mdec.w,Mdec.Mdec.h);
						SPgs->AddDbgInfo(s);

						if((handle=_wopen(Name,O_RDONLY|O_BINARY))==-1)
						{
							CString s;
							s.Format(L"Error Line :%4d, Can't open %s file \r\n",NoLine,Name);
							SPgs->AddDbgInfo(s);
						}
						else
						{
							Mdec.Mdec.Size=_filelength(handle);
							if((Mdec.Data=(UBYTE *)malloc(Mdec.Mdec.Size))==NULL)
							{
								CString s;
								s.Format(L"Error Line :%4d, No memory for read %s file\r\n",NoLine,Name);
								SPgs->AddDbgInfo(s);
							}
							else
							if(_read(handle,Mdec.Data,Mdec.Mdec.Size)!=Mdec.Mdec.Size)
							{
								CString s;
								s.Format(L"Error Line :%4d,Can't read %s file\r\n",NoLine,Name);
								SPgs->AddDbgInfo(s);
							}
							else
							{					
								CString s2,s;
								s=Name;
								s2=s.Right(s.GetLength()-s.ReverseFind('\\')-1);
								s=s2.Left(s2.ReverseFind('.'));
								Out->AddData((char *)(LPCTSTR)s,Mdec);
							}
							_close(handle);

							InitMode=0;
						}
					}
					else
					{
						CString s;
						s.Format(L"Error Line :%4d, incorrect param number for LoadBS\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}

				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Fonts\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"LOADBIN")==0)
			{
				if(PalInit)
				{			
					
					if(NbParam==2)
					{
						DATABININFO DBin;
						int handle;

						if(!StrEnvVar(TmpStr))
						{
							CString s;
							s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
							SPgs->AddDbgInfo(s);
						}

						std::wstring wstr = utf8_to_wstring(TmpStr);

						if(CPath[0]!=0)
							swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
						else
							wcscpy(Name,wstr.c_str());

						if((handle=_wopen(Name,O_RDONLY|O_BINARY))==-1)
						{
							CString s;
							s.Format(L"Error Line :%4d, Can't open %s file \r\n",NoLine,Name);
							SPgs->AddDbgInfo(s);
						}
						else
						{
							DBin.Size=_filelength(handle);
							if((DBin.Data=(UBYTE *)malloc(DBin.Size))==NULL)
							{
								CString s;
								s.Format(L"Error Line :%4d, No memory for read %s file\r\n",NoLine,Name);
								SPgs->AddDbgInfo(s);
							}
							else
							if(_read(handle,DBin.Data,DBin.Size)!=DBin.Size)
							{
								CString s;
								s.Format(L"Error Line :%4d,Can't read %s file\r\n",NoLine,Name);
								SPgs->AddDbgInfo(s);
							}
							else
							{					
								CString s2,s;
								s=Name;
								s2=s.Right(s.GetLength()-s.ReverseFind('\\')-1);
								s=s2.Left(s2.ReverseFind('.'));
								Out->AddData((char *)(LPCTSTR)s,DBin);
							}
							_close(handle);

							InitMode=0;
						}
					}
					else
					{
						CString s;
						s.Format(L"Error Line :%4d, incorrect param number for LoadBIN\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}

				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Fonts\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"STATICTXT")==0)
			{
				
				Out->AddData(TmpStr,atoi(TmpStr3),TmpStr2);

			}
			else
			if(strcmp(Cmd,"PAGE")==0)
			{
				USHORT clut,tpage;

				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				std::wstring wstr = utf8_to_wstring(TmpStr);

				if(CPath[0]!=0)
					swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
				else
					wcscpy(Name,wstr.c_str());

				if(PalInit)
				{			
					psxRECT rc;
					CImg Img(SPgs);
					InitMode=0;
					if(Img.Load(Name)==0) 
					{
						CString s;
						s.Format(L"Error to read %s file\r\n",Name);
						SPgs->AddDbgInfo(s);		
						Img.free_pic();
						throw 0;
					}

					rc.x=atoi(TmpStr2);
					rc.y=atoi(TmpStr3);
					rc.w=Img.Img.w;
					rc.h=Img.Img.h;
					if(rc.w&1||rc.h&1) 
					{
						CString s;
						s.Format(L"Error size of Page %s is not divisible by two\r\n",Name);
						SPgs->AddDbgInfo(s);		
					}
					else
					{
						rc.w/=2;
						//rc.h/=2;
						if(Vram->StoreImage(&rc,(UCHAR*)Img.Img.data)==0)
						{
							CString s;
							s.Format(L"Error on StoreImage for Page %s\r\n",Name);
							SPgs->AddDbgInfo(s);
						} 
					}			

					if(Img.Img.nb_colors<=16)
						SPgs->AddDbgInfo(L"16 color page not implemanted\r\n");
					else
						tpage=getTPage(1,0,rc.x,rc.y);

					if((clut=Vram->PutPal((UCHAR *)Img.Img.palette,Img.Img.Trans,Img.Img.nb_colors))==0xffff)
					{
						// Error
						SPgs->AddDbgInfo(L"No Room for all palette\r\n");
					}

					std::string ImgNameStr = wstring_to_utf8(Img.Img.Name);
					Out->AddData(ImgNameStr.c_str(),rc,tpage,clut);

					Img.free_pic();
				}
				else
				{
					// Error
					CString s;
					s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Pages\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"ADDBLK")==0)
			{
				if(NbParam==12)
				{
					BLKPSX Bnk;
					Bnk.u0=atoi(TmpStr2);
					Bnk.v0=atoi(TmpStr3);
					Bnk.u1=P2;
					Bnk.v1=P3;
					Bnk.u2=P4;
					Bnk.v2=P5;
					Bnk.u3=P6;
					Bnk.v3=P7;
					Bnk.tpage=P8;					
					Bnk.clut=P9;
					Out->AddData(TmpStr,Bnk);
				}
				else
				{
					CString s;
					s.Format(L"Error Line :%4d, incorrect param number for AddBlk\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"LOADPAL")==0)
			{
				DATAPALRAM Ram;
				DATAPALVRAM VRam;
				CString s;



				if(NbParam==3)
				{
					if(!StrEnvVar(TmpStr))
					{
						CString s;
						s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}

					std::wstring wstr = utf8_to_wstring(TmpStr);

					if(CPath[0]!=0)
						swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
					else
						wcscpy(Name,wstr.c_str());

					if(PalInit)
					{			
						CImg Img(SPgs);
						InitMode=0;
						if(Img.Load(Name)==0) 
						{
							s.Format(L"Error to read %s file\r\n",Name);
							SPgs->AddDbgInfo(s);		
							Img.free_pic();
							throw 0;
						}

						if(strcmp(TmpStr2,"RAMCLUT")==0)
						{
							UCHAR *Pal;
							int i;
							CString s;
							Pal=(UCHAR *)Img.Img.palette;
							Ram.NbCol=Img.Img.nb_colors;
							//s.Format(L"Pal for %s\r\n",Img.Img.Name);
							//SPgs->AddDbgInfo(s);		
							for(i=0;i<Img.Img.nb_colors;i++)
							{
								*(((USHORT *)(Ram.Pal))+i)=Trans_Pal(*(Pal+i*3),*(Pal+i*3+1),*(Pal+i*3+2));
								*(((USHORT *)(Ram.Pal))+i)|=Img.Img.Trans[i]<<15;
								//s.Format(L"%3d TRGB(%02x,%02x,%02x,%02x)=>CLUT(%4x)\r\n",i,Img.Img.Trans[i],*(Pal+i*3),*(Pal+i*3+1),*(Pal+i*3+2),*(((USHORT *)(Ram.Pal))+i));
								//SPgs->AddDbgInfo(s);		
							}
							Ram.Mode=PALRAMCLUT;

							std::string ImgNameStr = wstring_to_utf8(Img.Img.Name);

							Out->AddData(ImgNameStr.c_str(),Ram);
						}
						else
						if(strcmp(TmpStr2,"RAMPAL")==0)
						{
							UCHAR *Pal;
							int i;
							Ram.NbCol=Img.Img.nb_colors;
							Ram.Mode=PALRAMRGB;
							Pal=(UCHAR *)Img.Img.palette;
							for(i=0;i<Img.Img.nb_colors;i++)
							{
								*(Ram.Pal+i*4+0)=*(Pal+i*3+0);
								*(Ram.Pal+i*4+1)=*(Pal+i*3+1);
								*(Ram.Pal+i*4+2)=*(Pal+i*3+2);
								*(Ram.Pal+i*4+3)=Img.Img.Trans[i];
							}

							std::string ImgNameStr = wstring_to_utf8(Img.Img.Name);

							Out->AddData(ImgNameStr.c_str(),Ram);
						}
						else
						if(strcmp(TmpStr2,"VRAM")==0)
						{
							VRam.NbCol=Img.Img.nb_colors;						
							VRam.Mode=PALVRAM;
							if((VRam.Clut=Vram->PutPal((UCHAR *)Img.Img.palette,Img.Img.Trans,Img.Img.nb_colors))==0xffff)
							{
								// Error
								SPgs->AddDbgInfo(L"No Room for all palette\r\n");
							}

							std::string ImgNameStr = wstring_to_utf8(Img.Img.Name);

							Out->AddData(ImgNameStr.c_str(),VRam);
						}
						else
						{
							// Error
							s.Format(L"Error Line :%4d, unknows %s mode for LoadPAL line \r\n",NoLine,TmpStr2);
							SPgs->AddDbgInfo(s);
						}


						Img.free_pic();
					}
					else
					{
						// Error
						s.Format(L"Error Line :%4d, Palette Zone Not Initalized for Pages\r\n",NoLine);
						SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					s.Format(L"Error Line :%4d, incorrect param number for LoadPAL\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
			}
			else
			if(strcmp(Cmd,"PATH")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
				else
				{
					std::wstring wstr = utf8_to_wstring(TmpStr);
					wcscpy(CPath, wstr.c_str());
				}
			}
			else
			if(strcmp(Cmd,"PATHH")==0)
			{
				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
				else
				{
					std::wstring wstr = utf8_to_wstring(TmpStr);
					wcscpy(CPath, wstr.c_str());
				}
			}
			else
			if(strcmp(Cmd,"OUT")==0)
			{
				CString s;
				Vram->Show(SVdd);

				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				s.Format(L"Out File Name : %s\r\n",TmpStr);
				SPgs->AddDbgInfo(s);

				std::wstring wstr = utf8_to_wstring(TmpStr);

				if(CPath[0]!=0)
					swprintf(Name,L"%s\\%s",CPath, wstr.c_str());
				else
					wcscpy(Name,wstr.c_str());

				if(CPathH[0]!=0)
					swprintf(NameH,L"%s\\%s",CPathH,wstr.c_str());
				else
					wcscpy(NameH,Name);

				s.Format(L"Out File Path : %s\r\n",Name);
				SPgs->AddDbgInfo(s);
				s.Format(L"Out File PathH : %s\r\n",NameH);
				SPgs->AddDbgInfo(s);

				if(SaveRaw)
				{
					Vram->SaveRaw(Name);
				}

				if(strcmp(TmpStr2,"VRAM")==0)
				{
					Out->SaveVRM(Name,NameH);
					Vram->Save(Name);
				}
				else
				if(strcmp(TmpStr2,"VRAM2")==0)
				{
					Out->SaveVRM2(Name,NameH);
					Vram->Save(Name);
				}
				else
				if(strcmp(TmpStr2,"TIM")==0)
				{
					Out->SaveTIM(Name,Vram);
				}
				else
				if(strcmp(TmpStr2,"PC")==0)
				{
					Out->SavePC(Name,NameH,Vram);
				}
				else
				{
					CString s;
					s.Format(L"Error Line :%4d, Bad out file type\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}
				OutName=Name;
			}
			else
			if(strcmp(Cmd,"OUTBLK")==0)
			{
				psxRECT rc;
				FILE *fpb;
				WORD *Ptr;

				if(!StrEnvVar(TmpStr))
				{
					CString s;
					s.Format(L"Error Line :%4d, VarEnv For Path\r\n",NoLine);
					SPgs->AddDbgInfo(s);
				}

				std::wstring wstr = utf8_to_wstring(TmpStr);

				if(CPath[0]!=0)
					swprintf(Name,L"%s\\%s",CPath,wstr.c_str());
				else
					wcscpy(Name,wstr.c_str());

				if((fpb=_wfopen(Name,L"wb"))==NULL)
				{
					CString s;
					s.Format(L"Error to write %s file\r\n",Name);
					SPgs->AddDbgInfo(s);		
					throw 0;
				}
				rc.x=atoi(TmpStr2);
				rc.y=atoi(TmpStr3);
				rc.w=P2;
				rc.h=P3;
				Ptr=new WORD[P2*P3];
				Vram->LoadImage(&rc,(UBYTE*)Ptr);
				fwrite(&rc,1,sizeof(psxRECT),fpb);
				fwrite(Ptr,1,P2*P3*2,fpb);
				fclose(fpb);			
				delete[] Ptr;

			}
			else
			{
				// command don't exist
				CString s;
				s.Format(L"Error Line :%4d, Don't know command %s \r\n",NoLine,Cmd);
				SPgs->AddDbgInfo(s);
			}

			if(r==-1)
				throw -1;
		}
	}
	catch(int code)
	{
		exitCode=code;
	}

	CloseScript();

	// Restaure les variables pour la récursivité
	NoLine=saveNoLine;
	fp=saveFile;
	return exitCode;
}
