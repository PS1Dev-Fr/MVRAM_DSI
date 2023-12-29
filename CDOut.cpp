
#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "ViewDx.h"
#include "cvram.h"
#include "CDOut.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

char TabHex[16]={'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

CDOut::CDOut(CProgressDlg *Pgs,CViewDx *Vdd)
{
	List=NULL;
	SPgs=Pgs;
	SVdd=Vdd;
}

CDOut::~CDOut(void)
{
	DelAllData();
}

int CDOut::AddData(const char *Name,DATAPALVRAM DPal)
{
	ODPALLIST *Ch;

	if(List!=NULL)
	{
		Ch=(ODPALLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new ODPALLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new ODPALLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CDPAL;
	Ch->pSuiv=NULL;
	Ch->Data.VRam=DPal;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,DATAPALRAM DPal)
{
	ODPALLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(ODPALLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new ODPALLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new ODPALLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CDPAL;
	Ch->pSuiv=NULL;
	Ch->Data.Ram=DPal;
	strcpy(Ch->Name,Name);
	return 1;
}


int CDOut::AddData(const char *Name,DATABININFO DBin)
{
	ODBINLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(ODBINLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new ODBINLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new ODBINLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CDBIN;
	Ch->pSuiv=NULL;
	Ch->DBin=DBin;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,MDECINFOD Mdec)
{
	OMDECLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(OMDECLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new OMDECLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new OMDECLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CMDEC;
	Ch->pSuiv=NULL;
	Ch->Mdec=Mdec;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,FNTCLIST Fnt,BOOL _IsZoneFilter)
{
	OFNTLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(OFNTLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new OFNTLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new OFNTLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CFONT;
	Ch->pSuiv=NULL;
	Ch->FCList=Fnt;
	Ch->IsZoneFilter=_IsZoneFilter;
	strcpy(Ch->Name,Name);
	return 1;
}


int CDOut::AddData(const char *Name,BLKPSX Blk,BOOL _IsZoneFilter)
{
	ODPSXLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(ODPSXLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new ODPSXLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new ODPSXLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CBLK;
	Ch->pSuiv=NULL;
	Ch->Blk=Blk;
	Ch->IsZoneFilter=_IsZoneFilter;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,BLKPSX Blk)
{
	ODPSXLIST *Ch;

	if(List!=NULL)
	{	
		Ch=(ODPSXLIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new ODPSXLIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new ODPSXLIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CBLK2;
	Ch->pSuiv=NULL;
	Ch->Blk=Blk;
	Ch->IsZoneFilter=0;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,psxRECT &rc,USHORT tpage,USHORT clut)
{
	OPAGELIST *Ch;

	if(List!=NULL)
	{	
		Ch=(OPAGELIST *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new OPAGELIST;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;
	}
	else
	{
		Ch=new OPAGELIST;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}


	Ch->Type=CPAGE;
	Ch->pSuiv=NULL;

	Ch->Blk.clut=clut;
	Ch->Blk.tpage=tpage;
	Ch->Blk.u0=(UCHAR)rc.x;
	Ch->Blk.v0=(UCHAR)rc.y;
	Ch->Blk.u1=(UCHAR)(rc.x+rc.w-1);
	Ch->Blk.v1=(UCHAR)rc.y;
	Ch->Blk.u2=(UCHAR)rc.x;
	Ch->Blk.v2=(UCHAR)(rc.y+rc.h-1);
	Ch->Blk.u3=(UCHAR)(rc.x+rc.w-1);
	Ch->Blk.v3=(UCHAR)(rc.y+rc.h-1);

	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::AddData(const char *Name,int SType,CString Txt)
{
	OSTATICTXT *Ch;

	if(List!=NULL)
	{	
		Ch=(OSTATICTXT *)List;
		while(Ch->pSuiv!=NULL)
		{
			Ch=Ch->pSuiv;		
		}

		Ch->pSuiv=new OSTATICTXT;
		if(Ch->pSuiv==NULL) return 0;
		Ch=Ch->pSuiv;

	}
	else
	{
		Ch=new OSTATICTXT;
		List=(CHUNKLIST *)Ch;
		if(Ch==NULL) return 0;
	}

	Ch->Type=CSTxx;
	Ch->pSuiv=NULL;
	Ch->SType=SType;
	Ch->Txt=Txt;
	strcpy(Ch->Name,Name);
	return 1;
}

int CDOut::DelAllData(void)
{
	CHUNKLIST *Ch1,*Ch2;

	if(List==NULL) return 0;

	Ch1=List;
	while(Ch1->pSuiv!=NULL)
	{
		Ch2=Ch1->pSuiv;
		delete Ch1;
		Ch1=Ch2;
	}
	List=NULL;
	return 1;
}

int CDOut::SaveVRM(wchar_t*Path,wchar_t *PathH)
{
	FILE *fpb,*fpt;
	int Nb=GetNbPic(),Pos,T1,T2,T3,Pos2,Len;
	CString s,s2,s3;
	CTime t=CTime::GetCurrentTime();

	ODPSXLIST *Ch;
	OFNTLIST *Chf;
	OMDECLIST *Chm;
	ODBINLIST *Chd;
	ODPALLIST *Chp;
	OSTATICTXT *Chst;
	DATAST Dst;
	char cDummy[4];

	Ch=(ODPSXLIST *)List;
	if(Ch==NULL) return 0;

	s=Path;
	s+=L".vrm";

	if((fpb=_wfopen(s,L"wb"))==NULL)
	{
		CString err;
		err.Format(L"Error: can't write file \"%s\"\r\n",s);
		SPgs->AddDbgInfo(err);
		return 0;
	}

	s2=PathH;
	//s2=s.Left(s.ReverseFind('.'));
	s2+=".H";

	if((fpt= _wfopen(s2,L"wt"))==NULL)
	{
		CString err;
		err.Format(L"Error: can't write file \"%s\"\r\n",s2);
		SPgs->AddDbgInfo(err);
		return 0;
	}

	{
		int len,SPos;
		s3=PathH;
		len=s3. GetLength();
		SPos=s3.ReverseFind('\\');

		s3=s3.Right(len-SPos-1);
		s3.MakeUpper();

	}

	fprintf(fpt,"// File %ls generated by mvram (c)1996 DSI, NP\n",s2.GetBuffer());
	fprintf(fpt,"// Date : %02d/%02d/%04d %02d:%02d:%02d\n\n\n",t.GetDay(),t.GetMonth(),t.GetYear(),t.GetHour(),t.GetMinute(),t.GetSecond());
	fprintf(fpt,"#ifndef __MVRAM_%ls_H\n#define __MVRAM_%ls_H\n\n\n",s3.GetBuffer(),s3.GetBuffer());

	fwrite(&Nb,1,sizeof(int),fpb);
	fwrite(&Nb,1,sizeof(int),fpb);

	Pos=0;
	Pos2=0;
	SPgs->AddDbgInfo(L"\r\n\r\nWrite VRM File\r\n------------------------------\r\n");

	do
	{
		switch(Ch->Type)
		{
			case CDPAL:
				Chp=(ODPALLIST*)Ch;
				_strupr(Ch->Name);
				fprintf(fpt,"#define P_%s\t%d\n",Ch->Name,Pos);
				switch(Chp->Data.Mode)
				{
					case PALRAMRGB:
						Pos2+=(int)fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4;
					break;
					case PALRAMCLUT:
						Pos2+= (int)fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2;
					break;
					case PALVRAM:
						Pos2+= (int)fwrite(&Chp->Data.VRam,1,sizeof(DATAPALVRAM),fpb);
						Pos+=sizeof(DATAPALVRAM);
					break;
				}
				s.Format(L"Pal %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CBLK2:
			case CPAGE:
			case CBLK:
				Pos2+= (int)fwrite(&Ch->Blk,1,sizeof(BLKPSX),fpb);
				_strupr(Ch->Name);
				fprintf(fpt,"#define B_%s\t%d\t//Clut:%04x TPage:%04x ((%3d,%3d)(%3d,%3d)(%3d,%3d)(%3d,%3d))\n",Ch->Name,Pos,Ch->Blk.clut,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.v0,Ch->Blk.u1,Ch->Blk.v1,Ch->Blk.u2,Ch->Blk.v2,Ch->Blk.u3,Ch->Blk.v3);
				Pos+=sizeof(BLKPSX);
				s.Format(L"Blk %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CFONT:
				Chf=(OFNTLIST*)Ch;
				Pos2+=T1= (int)fwrite(&Chf->FCList.Hdr,1,sizeof(FNTHDR),fpb);
				if(Chf->FCList.Hdr.NbSwp)
				{
					if(Chf->FCList.Hdr.Mode)
					{
						Pos2+=T2= (int)fwrite(Chf->FCList.PCalcSwp,1,((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4,fpb);
						delete [] Chf->FCList.PCalcSwp;
					}
					else
					{
						Pos2+=T2= (int)fwrite(Chf->FCList.SwpList,1,sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp,fpb);
					}
				}
				Pos2+=T3= (int)fwrite(Chf->FCList.Body,1,sizeof(FNTBODY)*Chf->FCList.Hdr.NbC,fpb);
				_strupr(Chf->Name);
				switch(Chf->FCList.Hdr.Type&0xc0)
				{
					case FT_0:
						fprintf(fpt,"#define F_%s\t%d\n",Chf->Name,Pos);
					break;
					case FT_90:
						fprintf(fpt,"#define F_%s_90\t%d\n",Chf->Name,Pos);
					break;
					case FT_180:
						fprintf(fpt,"#define F_%s_180\t%d\n",Chf->Name,Pos);
					break;
					case FT_270:
						fprintf(fpt,"#define F_%s_270\t%d\n",Chf->Name,Pos);
					break;
				}
				if(Chf->FCList.Hdr.NbSwp&&Chf->FCList.Hdr.Mode)
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4;
				else
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp;
				s.Format(L"Font %s Pos : %d/%d\r\n",Chf->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);

			break;
			case CMDEC:
				Chm=(OMDECLIST*)Ch;
				Pos2+= (int)fwrite(&Chm->Mdec.Mdec,1,sizeof(MDECINFO),fpb);
				Pos2+= (int)fwrite(Chm->Mdec.Data,1,Chm->Mdec.Mdec.Size,fpb);
				_strupr(Chm->Name);
				fprintf(fpt,"#define M_%s\t%d\n",Chm->Name,Pos);
				Pos+=sizeof(MDECINFO)+Chm->Mdec.Mdec.Size;
				free(Chm->Mdec.Data);
				s.Format(L"Mdec %s Pos : %d/%d\r\n",Chm->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CDBIN:
				Chd=(ODBINLIST*)Ch;
				Pos2+= (int)fwrite(Chd->DBin.Data,1,Chd->DBin.Size,fpb);
				_strupr(Chd->Name);
				fprintf(fpt,"#define D_%s\t%d\n",Chd->Name,Pos);
				Pos+=Chd->DBin.Size;
				free(Chd->DBin.Data);
				s.Format(L"Bin %s Pos : %d/%d\r\n",Chd->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CSTxx:
				Chst=(OSTATICTXT*)Ch;
				_strupr(Chst->Name);
				fprintf(fpt,"#define S%02d_%s\t%d\n",Chst->SType,Chst->Name,Pos);
				Len=Chst->Txt.GetLength();
				Dst.SType=Chst->SType;
				Dst.TxtSize=Len;
				Pos2+= (int)fwrite(&Dst,1,sizeof(DATAST),fpb);
				Pos2+= (int)fwrite(Chst->Txt,1,Len,fpb);
				Pos2+= (int)fwrite(cDummy,1,4-(Len&3),fpb);
				Pos+=sizeof(DATAST)+(Len&0xfffffffc)+4;
				s.Format(L"STxx %s Pos : %d/%d\r\n",Chst->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			default:
				char Txt[5];
				CString s;
				*(int*)Txt=Ch->Type;
				Txt[4]=0;
				s.Format(L"Out %s type error\r\n",Txt);
				SPgs->AddDbgInfo(s);
			break;
		}
		Ch=Ch->pSuiv;
	}
	while(Ch!=NULL);
	fprintf(fpt,"#endif\n");

	fseek(fpb,4,SEEK_SET);
	fwrite(&Pos,1,sizeof(int),fpb);
	fclose(fpb);
	fclose(fpt);
	return 1;
}

int CDOut::SaveVRM2(wchar_t*Path,wchar_t *PathH)
{
	FILE *fpb,*fpt;
	int Nb=GetNbPic(),Pos;
	CString s,s2,s3;
	CTime t=CTime::GetCurrentTime();

	ODPSXLIST *Ch;
	OFNTLIST *Chf;
	OMDECLIST *Chm;
	ODBINLIST *Chd;
	ODPALLIST *Chp;

	Ch=(ODPSXLIST *)List;
	if(Ch==NULL) return 0;

	s=Path;
	s+=L".vrm";

	if((fpb=_wfopen(s,L"wb"))==NULL)
	{
		s.Format(L"Error to write %s file\r\n",s);
		SPgs->AddDbgInfo(s);	
		return 0;
	}

/*	s=Path;
	Pos=s.ReverseFind('\\');
	s2=s.Right(s.GetLength()-Pos-1);
	s3=s2.Left(s2.ReverseFind('.'));
	s3.MakeUpper();
	s2=s.Left(s.ReverseFind('.'));
	s2+=".H";*/

	s=PathH;
	s2+=L".h";

	if((fpt=_wfopen(s2,L"wt"))==NULL)
	{
		s.Format(L"Error to write %s file\r\n",s2);
		SPgs->AddDbgInfo(s);	
		return 0;
	}

	{
		int len,SPos;
		s3=PathH;
		len=s3. GetLength();
		SPos=s3.ReverseFind('\\');

		s3=s3.Right(len-SPos-1);
		s3.MakeUpper();

	}

	fprintf(fpt,"// File %ls generated by mvram (c)1996 DSI, NP\n",s2.GetBuffer());
	fprintf(fpt,"// Date : %02d/%02d/%04d %02d:%02d:%02d\n\n\n",t.GetDay(),t.GetMonth(),t.GetYear(),t.GetHour(),t.GetMinute(),t.GetSecond());
	fprintf(fpt,"#ifndef __MVRAM_%ls_H\n#define __MVRAM_%ls_H\n\n\n",s3.GetBuffer(),s3.GetBuffer());


	fwrite(&Nb,1,sizeof(int),fpb);
	fwrite(&Nb,1,sizeof(int),fpb);


	Pos=0;
	fprintf(fpt,"enum{\n");
	do
	{
		switch(Ch->Type)
		{
			case CDPAL:
				Chp=(ODPALLIST*)Ch;
				fprintf(fpt,"\tP_%s\t=%d,\n",Ch->Name,Pos);
			break;
			case CBLK2:
			case CPAGE:
			case CBLK:
				fprintf(fpt,"\tB_%s\t=%d,\t//Clut:%04x TPage:%04x ((%3d,%3d)(%3d,%3d)(%3d,%3d)(%3d,%3d))\n",Ch->Name,Pos,Ch->Blk.clut,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.v0,Ch->Blk.u1,Ch->Blk.v1,Ch->Blk.u2,Ch->Blk.v2,Ch->Blk.u3,Ch->Blk.v3);
			break;
			case CFONT:
				Chf=(OFNTLIST*)Ch;
				switch(Chf->FCList.Hdr.Type&0xc0)
				{
					case FT_0:
						fprintf(fpt,"\tF_%s\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_90:
						fprintf(fpt,"\tF_%s_90\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_180:
						fprintf(fpt,"\tF_%s_180\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_270:
						fprintf(fpt,"\tF_%s_270\t=%d,\n",Chf->Name,Pos);
					break;
				}
			break;
			case CMDEC:
				Chm=(OMDECLIST*)Ch;
				fprintf(fpt,"\tM_%s\t=%d,\n",Chm->Name,Pos);
			break;
			case CDBIN:
				Chd=(ODBINLIST*)Ch;
				fprintf(fpt,"\tD_%s\t=%d,\n",Chd->Name,Pos);
			break;
		}
		Ch=Ch->pSuiv;
		Pos++;
	}
	while(Ch!=NULL);
	fprintf(fpt,"};\n\n #define %ls {",s3.GetBuffer());
	Pos=0;
	Ch=(ODPSXLIST *)List;
	do
	{
		switch(Ch->Type)
		{
			case CDPAL:
				Chp=(ODPALLIST*)Ch;
				_strupr(Ch->Name);
				fprintf(fpt,"%d,",Pos);
				switch(Chp->Data.Mode)
				{
					case PALRAMRGB:
						fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4;
					break;
					case PALRAMCLUT:
						fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2;
					break;
					case PALVRAM:
						fwrite(&Chp->Data.VRam,1,sizeof(DATAPALVRAM),fpb);
						Pos+=sizeof(DATAPALVRAM);
					break;
				}
			break;
			case CBLK2:
			case CPAGE:
			case CBLK:
				fwrite(&Ch->Blk,1,sizeof(BLKPSX),fpb);
				_strupr(Ch->Name);
				fprintf(fpt,"%d,",Pos);
				Pos+=sizeof(BLKPSX);
			break;
			case CFONT:
				Chf=(OFNTLIST*)Ch;
				fwrite(&Chf->FCList.Hdr,1,sizeof(FNTHDR),fpb);
				if(Chf->FCList.Hdr.NbSwp)
				{
					if(Chf->FCList.Hdr.Mode)
					{
						fwrite(Chf->FCList.PCalcSwp,1,((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4,fpb);
					}
					else
					{
						fwrite(Chf->FCList.SwpList,1,sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp,fpb);
					}
				}
				fwrite(Chf->FCList.Body,1,sizeof(FNTBODY)*Chf->FCList.Hdr.NbC,fpb);
				_strupr(Chf->Name);
				fprintf(fpt,"%d,",Pos);
				if(Chf->FCList.Hdr.Mode&&Chf->FCList.Hdr.Mode)
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4;
				else
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp;
			break;
			case CMDEC:
				Chm=(OMDECLIST*)Ch;
				fwrite(&Chm->Mdec.Mdec,1,sizeof(MDECINFO),fpb);
				fwrite(Chm->Mdec.Data,1,Chm->Mdec.Mdec.Size,fpb);
				_strupr(Chm->Name);
				fprintf(fpt,"%d,",Pos);
				Pos+=sizeof(MDECINFO)+Chm->Mdec.Mdec.Size;
				free(Chm->Mdec.Data);
			break;
			case CDBIN:
				Chd=(ODBINLIST*)Ch;
				fwrite(Chd->DBin.Data,1,Chd->DBin.Size,fpb);
				_strupr(Chd->Name);
				fprintf(fpt,"%d,",Pos);
				Pos+=Chd->DBin.Size;
				free(Chd->DBin.Data);
			break;
			default:
				char Txt[5];
				CString s;
				*(int*)Txt=Ch->Type;
				Txt[4]=0;
				s.Format(L"Out %s type error\r\n",Txt);
				SPgs->AddDbgInfo(s);
			break;
		}
		Ch=Ch->pSuiv;
	}
	while(Ch!=NULL);
	fprintf(fpt,"-1};\n");
	fprintf(fpt,"#endif\n");

	fseek(fpb,4,SEEK_SET);
	fwrite(&Pos,1,sizeof(int),fpb);
	fclose(fpb);
	fclose(fpt);
	return 1;
}

int CDOut::SaveTIM(wchar_t *Path,CVram *Vram)
{
	int Nb=GetNbPic(),tp;
	CString s;
	CImg Img(SPgs);
	ODPSXLIST *Ch;
	BLKPSX *Blk;
	USHORT *Data;

	Ch=(ODPSXLIST *)List;
	if(Ch==NULL) return 0;



	if(GetFileAttributes(Path)!=FILE_ATTRIBUTE_DIRECTORY)
	{
		CreateDirectory(Path,NULL);
	}


	do
	{
		if(Ch->Type==CPAGE||Ch->Type==CBLK)
		{
			Blk=&Ch->Blk;
		
			s=Path;

			s+=L"\\";
			s+=Ch->Name;
			s+=L".tim";

			Img.Img.TData.y=Blk->v0+TPAGE_Y(Blk->tpage);
			Img.Img.TData.w=(Blk->u1-Blk->u0)+1;
			Img.Img.TData.h=(Blk->v2-Blk->v0)+1;
			Img.Img.TClut.x=CLUT_X(Blk->clut);
			Img.Img.TClut.y=CLUT_Y(Blk->clut);
			tp=TPAGE_TP(Blk->tpage);

			if(tp==CLUT4)
			{
				Img.Img.TData.x=((Blk->u0)/4)+TPAGE_X(Blk->tpage);
				Img.Img.TClut.w=16;
				Img.Img.nb_colors=16;
				Img.Img.TData.w+=3;
				Img.Img.TData.w/=4;
			}
			else if(tp==CLUT8)
			{
				Img.Img.TData.x=((Blk->u0)/2)+TPAGE_X(Blk->tpage);
				Img.Img.TClut.w=256;
				Img.Img.nb_colors=256;
				Img.Img.TData.w+=1;
				Img.Img.TData.w/=2;
			}
			else
			{
				CString s;
				s.Format(L"Error : 16 Bits clut not supported on file %s\r\n",s);
				SPgs->AddDbgInfo(s);
			}

			Img.Img.TClut.h=1;
			Img.Img.palette=Vram->GetPal(Blk->clut);

			if((Data=(USHORT*)malloc(Img.Img.TData.w*Img.Img.TData.h*2))==NULL)
			{
				CString s;
				s.Format(L"Memory error for save %s\r\n",s);
				SPgs->AddDbgInfo(s);
			}

			Vram->LoadImage(&Img.Img.TData,(UCHAR *)Data);

			Img.Img.data=Data;
			Img.SaveTim(s.GetBuffer(),&Img.Img);

			free(Data);
		}
		else
		{
			char Txt[5];
			CString s;
			*(int*)Txt=Ch->Type;
			Txt[4]=0;
			s.Format(L"Skip %s type\r\n",Txt);
			SPgs->AddDbgInfo(s);
		}
		Ch=Ch->pSuiv;
	}
	while(Ch!=NULL);


	return 1;
}


int CDOut::SavePC(wchar_t*Path,wchar_t *PathH,CVram *Vram)
{
	CImg Img(SPgs);
	ODPSXLIST *Ch;
	FILE *fpb,*fpt;
	int Nb=GetNbPic(),Pos,Pos2,NbBnk=0,NbBmp=0,NoBnk,Len;
	CString s,s2,s3;
	CTime t=CTime::GetCurrentTime();
	OFNTLIST *Chf;
	OMDECLIST *Chm;
	ODBINLIST *Chd;
	ODPALLIST *Chp;
	OSTATICTXT *Chst;
	DATAST Dst;
	OUTHEAD hd;
	char cDummy[4];
	void*	pDummy=NULL;

	SVdd->NewSize(256,256);

	Ch=(ODPSXLIST *)List;
	if(Ch==NULL) return 0;


	if(GetFileAttributes(Path)!=FILE_ATTRIBUTE_DIRECTORY)
	{
	
		CreateDirectory(Path,NULL);
		s.Format(L"%s\\bnk??.raw",Path);
		DeleteFile(s);
		s.Format(L"%s\\bnk??.msk",Path);
		DeleteFile(s);
	}


	s=Path;
	s+=L".vrm";

	if((fpb=_wfopen(s,L"wb"))==NULL)
	{
		s.Format(L"Error to write %s file\r\n",s);
		SPgs->AddDbgInfo(s);	
		return 0;
	}

	s2=PathH;
	s2+=L".H";


	if((fpt=_wfopen(s2,L"wt"))==NULL)
	{
		s.Format(L"Error to write %s file\r\n",s2);
		SPgs->AddDbgInfo(s);	
		return 0;
	}


	{
		int len,SPos;
		s3=PathH;
		len=s3. GetLength();
		SPos=s3.ReverseFind('\\');

		s3=s3.Right(len-SPos-1);
		s3.MakeUpper();
	}

	time_t now;
	time(&now);

	fprintf(fpt,"// File %ls generated by mvram (c)1996 DSI, NP\n",s2.GetBuffer());
	fprintf(fpt,"// Date : %02d/%02d/%04d %02d:%02d:%02d\n\n\n",t.GetDay(),t.GetMonth(),t.GetYear(),t.GetHour(),t.GetMinute(),t.GetSecond());
	fprintf(fpt,"#ifndef __MVRAM_%ls_H\n#define __MVRAM_%ls_H\n\n\n",s3.GetBuffer(),s3.GetBuffer());

	fprintf(fpt,"#define CODEVRAM_%ls 0x%llx\n",s3.GetBuffer(),(uint64_t)now);

	fwrite(&Nb,1,sizeof(int),fpb);
	fwrite(&Nb,1,sizeof(int),fpb);
	fwrite(&Nb,1,sizeof(int),fpb);
	fwrite(&now,1,sizeof(int),fpb);


	Pos=0;
	Pos2=0;
	fprintf(fpt,"enum{\n");
	do
	{
		switch(Ch->Type)
		{
			case CDPAL:
				Chp=(ODPALLIST*)Ch;
				hd.Pos=Pos2;
				hd.Type=CDPAL;
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);

				switch(Chp->Data.Mode)
				{
					case PALRAMRGB:
						Pos2+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4;
					break;
					case PALRAMCLUT:
						Pos2+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2;
					break;
					case PALVRAM:
						Pos2+=sizeof(DATAPALVRAM);
					break;
				}
			break;
			case CBLK2:
			case CPAGE:
			case CBLK:
				NbBmp++;
				hd.Pos=Pos2;
				hd.Type=CBLK;
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);
				Pos2+=sizeof(BLKPSX);
			break;
			case CFONT:
				Chf=(OFNTLIST*)Ch;
				NbBmp++;
				hd.Pos=Pos2;
				hd.Type=CFONT;
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);
				if(Chf->FCList.Hdr.NbSwp&&Chf->FCList.Hdr.Mode)
					Pos2+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4+sizeof(void *);
				else
					Pos2+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp+sizeof(void *);
			break;
			case CMDEC:
				Chm=(OMDECLIST*)Ch;
				hd.Pos=Pos2;
				hd.Type=CMDEC;
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);
				Pos2+=sizeof(MDECINFO)+Chm->Mdec.Mdec.Size;
			break;
			case CDBIN:
				Chd=(ODBINLIST*)Ch;
				hd.Pos=Pos2;
				hd.Type=CDBIN;
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);
				Pos2+=Chd->DBin.Size;
			break;
			case CSTxx:
				Chst=(OSTATICTXT*)Ch;
				hd.Pos=Pos2;
				hd.Type=(CSTxx&0xffff)|(TabHex[Chst->SType&7]<<24)|(TabHex[(Chst->SType>>4)&7]<<16);
				fwrite(&hd,1,sizeof(OUTHEAD),fpb);
				Pos2+=sizeof(DATAST)+(Chst->Txt.GetLength()&0xfffffffc)+4;
			break;
		}
		Ch=Ch->pSuiv;
		Pos++;
	}
	while(Ch!=NULL);
	int *TPageList=new int[NbBmp];

	Pos=0;
	Ch=(ODPSXLIST *)List;
	do
	{
		int i,find=0;
		switch(Ch->Type)
		{
			case CPAGE:
			case CBLK:
				for(i=0;i<NbBnk;i++)
				{
					int Mask=0xffff-(TPAGEPC_TP(Ch->Blk.tpage)&1);
					if(TPageList[i]==(Ch->Blk.tpage&Mask))
					{
						find=1;
						break;
					}		
				}
				if(find!=1)
				{
					int Mask=0xffff-(TPAGEPC_TP(Ch->Blk.tpage)&1);
					TPageList[NbBnk]=(Ch->Blk.tpage&Mask);
					NbBnk++;
				}
			break;
			case CFONT:
			{
				int j;
				Chf=(OFNTLIST*)Ch;
				for(j=0;j<Chf->FCList.Hdr.NbC;j++)
				{
					find=0;
					int Mask=0xffff-(TPAGEPC_TP(Chf->FCList.TPagePC[j])&1);

					for(i=0;i<NbBnk;i++)
					{
						if(TPageList[i]==(Chf->FCList.TPagePC[j]&Mask))
						{
							find=1;
							break;
						}		
					}
					if(find!=1)
					{
						TPageList[NbBnk]=(Chf->FCList.TPagePC[j]&Mask);
						NbBnk++;
					}
				}
			}
			break;
		}
		Ch=Ch->pSuiv;
		Pos++;
	}
	while(Ch!=NULL);

	CPage24 P24(SPgs,SVdd,NbBnk);

	s.Format(L"NbBnk %d\r\n",NbBnk);
	SPgs->AddDbgInfo(s);

	Pos=0;
	Pos2=0;
	Ch=(ODPSXLIST *)List;

	do
	{
		int i,j;
		switch(Ch->Type)
		{
			case CDPAL:
				Chp=(ODPALLIST*)Ch;
				_strupr(Ch->Name);
				fprintf(fpt,"\tP_%s\t=%d,\n",Ch->Name,Pos);
				switch(Chp->Data.Mode)
				{
					case PALRAMRGB:
						Pos2+=(int)fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*4;

					break;
					case PALRAMCLUT:
						Pos2+= (int)fwrite(&Chp->Data.Ram,1,sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2,fpb);
						Pos+=sizeof(DATAPALRAMS)+Chp->Data.Ram.NbCol*2;
					break;
					case PALVRAM:
						Pos2+= (int)fwrite(&Chp->Data.VRam,1,sizeof(DATAPALVRAM),fpb);
						Pos+=sizeof(DATAPALVRAM);
					break;
				}
				s.Format(L"Pal %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);

			break;
			case CBLK2:
				Pos2+= (int)fwrite(&Ch->Blk,1,sizeof(BLKPSX),fpb);
				_strupr(Ch->Name);
				fprintf(fpt,"\tB_%s\t=%d,\t//Clut:%04x TPage:%04x ((%3d,%3d)(%3d,%3d)(%3d,%3d)(%3d,%3d))\n",Ch->Name,Pos,Ch->Blk.clut,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.v0,Ch->Blk.u1,Ch->Blk.v1,Ch->Blk.u2,Ch->Blk.v2,Ch->Blk.u3,Ch->Blk.v3);
				Pos+=sizeof(BLKPSX);
				s.Format(L"Blk %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CPAGE:
			case CBLK:
				int Mask;
				for(i=0;i<NbBnk;i++)
				{
					Mask=0xffff-(TPAGEPC_TP(Ch->Blk.tpage)&1);
					if(TPageList[i]==(Ch->Blk.tpage&Mask))
					{
						NoBnk=i;

						break;
					}
				}

				P24.AddBlk(NoBnk,Vram,&Ch->Blk,Ch->IsZoneFilter,FALSE);
			
				if(Mask==0xfffe)
				{
					if(Ch->Blk.tpage&1)
					{
						//s.Format("Blk %s (*) TPage:%04x (%03d-%03d-%03d-%03d)->(%03d-%03d-%03d-%03d)\r\n",Ch->Name,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.u1,Ch->Blk.u2,Ch->Blk.u3,Ch->Blk.u0+128,Ch->Blk.u1+128,Ch->Blk.u2+128,Ch->Blk.u3+128);
						//SPgs->AddDbgInfo(s);

						Ch->Blk.u0+=128;
						Ch->Blk.u1+=128;
						Ch->Blk.u2+=128;
						Ch->Blk.u3+=128;
					}
					else
					{
						//s.Format("Blk %s (+) TPage:%04x (%03d-%03d-%03d-%03d)->(%03d-%03d-%03d-%03d)\r\n",Ch->Name,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.u1,Ch->Blk.u2,Ch->Blk.u3,Ch->Blk.u0+128,Ch->Blk.u1+128,Ch->Blk.u2+128,Ch->Blk.u3+128);
						//SPgs->AddDbgInfo(s);
					}
				}
				else
				{
					//s.Format("Blk %s (-) TPage:%04x (%03d-%03d-%03d-%03d)->(%03d-%03d-%03d-%03d)\r\n",Ch->Name,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.u1,Ch->Blk.u2,Ch->Blk.u3,Ch->Blk.u0+128,Ch->Blk.u1+128,Ch->Blk.u2+128,Ch->Blk.u3+128);
					//SPgs->AddDbgInfo(s);
				}


				Ch->Blk.tpage=NoBnk;								

				Pos2+= (int)fwrite(&Ch->Blk,1,sizeof(BLKPSX),fpb);
				_strupr(Ch->Name);
				fprintf(fpt,"\tB_%s\t=%d,\t//Clut:%04x TPage:%04x ((%3d,%3d)(%3d,%3d)(%3d,%3d)(%3d,%3d))\n",Ch->Name,Pos,Ch->Blk.clut,Ch->Blk.tpage,Ch->Blk.u0,Ch->Blk.v0,Ch->Blk.u1,Ch->Blk.v1,Ch->Blk.u2,Ch->Blk.v2,Ch->Blk.u3,Ch->Blk.v3);
				Pos+=sizeof(BLKPSX);
				s.Format(L"Blk %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			case CFONT:
				Chf=(OFNTLIST*)Ch;
				int Find;
				for(i=0;i<Chf->FCList.Hdr.NbC;i++)
				{
					BLKPSX Blk;

					//s.Format("Fnt %s \'%c\' (%d) TPage:%04x(%d,%d,%d)\r\n",Ch->Name,i+Chf->FCList.Hdr.First,i,Chf->FCList.TPagePC[i],TPAGEPC_TP(Chf->FCList.TPagePC[i]),TPAGEPC_X(Chf->FCList.TPagePC[i]),TPAGEPC_Y(Chf->FCList.TPagePC[i]));
					//SPgs->AddDbgInfo(s);

					if((Chf->FCList.Hdr.Type&FT_90)||(Chf->FCList.Hdr.Type&FT_270))
					{
						Blk.u0=Chf->FCList.Body[i].x;
						Blk.u1=Chf->FCList.Body[i].x+Chf->FCList.Body[i].h-1;
						Blk.u2=Chf->FCList.Body[i].x;
						Blk.u3=Chf->FCList.Body[i].x+Chf->FCList.Body[i].h-1;

						Blk.v0=Chf->FCList.Body[i].y;
						Blk.v1=Chf->FCList.Body[i].y;
						Blk.v2=Chf->FCList.Body[i].y+Chf->FCList.Body[i].w-1;
						Blk.v3=Chf->FCList.Body[i].y+Chf->FCList.Body[i].w-1;
					}
					else
					{
						Blk.u0=Chf->FCList.Body[i].x;
						Blk.u1=Chf->FCList.Body[i].x+Chf->FCList.Body[i].w-1;
						Blk.u2=Chf->FCList.Body[i].x;
						Blk.u3=Chf->FCList.Body[i].x+Chf->FCList.Body[i].w-1;

						Blk.v0=Chf->FCList.Body[i].y;
						Blk.v1=Chf->FCList.Body[i].y;
						Blk.v2=Chf->FCList.Body[i].y+Chf->FCList.Body[i].h-1;
						Blk.v3=Chf->FCList.Body[i].y+Chf->FCList.Body[i].h-1;
					}
				

					Blk.clut=Chf->FCList.Hdr.Clut;

					Blk.tpage=Chf->FCList.TPagePC[i];
					Find=0;
					for(j=0;j<NbBnk;j++)
					{
						int Mask=0xffff-(TPAGEPC_TP(Blk.tpage)&1);
						if(TPageList[j]==(Blk.tpage&Mask))
						{
							Chf->FCList.Body[i].h=j;
							Chf->FCList.Hdr.TPage=j;
							NoBnk=j;
							Find=1;
							break;
						}
					}

					if(Find==0)
					{
						s.Format(L"Fnt %s \'%c\' ERROR NOT FIND TPAGE!!!\r\n",Ch->Name,i+Chf->FCList.Hdr.First,i,Chf->FCList.Body[i].h);
						SPgs->AddDbgInfo(s);
					}

					//s.Format("Fnt %s \'%c\'(%d) NewTPage:%0d\r\n",Ch->Name,i+Chf->FCList.Hdr.First,i,Chf->FCList.Body[i].h);

					//SPgs->AddDbgInfo(s);

					P24.AddBlk(NoBnk,Vram,&Blk,Ch->IsZoneFilter,TRUE);

					if(Ch->IsZoneFilter)
					{
						Chf->FCList.Body[i].x+=Ch->IsZoneFilter;
						Chf->FCList.Body[i].y+=Ch->IsZoneFilter;
						Chf->FCList.Body[i].w-=Ch->IsZoneFilter*2;
					}
				}

				if(Ch->IsZoneFilter)
				{
					Chf->FCList.Hdr.h-=Ch->IsZoneFilter*2;
				}

				Pos2+= (int)fwrite(&Chf->FCList.Hdr,1,sizeof(FNTHDR),fpb);
				Pos2+= (int)fwrite(&pDummy,1,sizeof(void *),fpb);

				if(Chf->FCList.Hdr.NbSwp)
				{
					if(Chf->FCList.Hdr.Mode)
					{
						Pos2+= (int)fwrite(Chf->FCList.PCalcSwp,1,((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4,fpb);
					}
					else
					{
						Pos2+= (int)fwrite(Chf->FCList.SwpList,1,sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp,fpb);
					}
				}
				Pos2+= (int)fwrite(Chf->FCList.Body,1,sizeof(FNTBODY)*Chf->FCList.Hdr.NbC,fpb);
				_strupr(Chf->Name);
				switch(Chf->FCList.Hdr.Type&0xc0)
				{
					case FT_0:
						fprintf(fpt,"\tF_%s\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_90:
						fprintf(fpt,"\tF_%s_90\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_180:
						fprintf(fpt,"\tF_%s_180\t=%d,\n",Chf->Name,Pos);
					break;
					case FT_270:
						fprintf(fpt,"\tF_%s_270\t=%d,\n",Chf->Name,Pos);
					break;
				}
				if(Chf->FCList.Hdr.NbSwp&&Chf->FCList.Hdr.Mode)
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+((Chf->FCList.Hdr.NbC*Chf->FCList.Hdr.NbC)&0xfffffffc)+4+sizeof(void *);
				else
					Pos+=sizeof(FNTHDR)+sizeof(FNTBODY)*Chf->FCList.Hdr.NbC+sizeof(SWPLIST)*Chf->FCList.Hdr.NbSwp+sizeof(void *);
				s.Format(L"Fnt %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);

			break;
			case CMDEC:
				Chm=(OMDECLIST*)Ch;
				Pos2+= (int)fwrite(&Chm->Mdec.Mdec,1,sizeof(MDECINFO),fpb);
				Pos2+= (int)fwrite(Chm->Mdec.Data,1,Chm->Mdec.Mdec.Size,fpb);
				_strupr(Chm->Name);
				fprintf(fpt,"\tM_%s\t=%d,\n",Chm->Name,Pos);
				Pos+=sizeof(MDECINFO)+Chm->Mdec.Mdec.Size;
				s.Format(L"Mdec %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
				free(Chm->Mdec.Data);
			break;
			case CDBIN:
				Chd=(ODBINLIST*)Ch;
				Pos2+= (int)fwrite(Chd->DBin.Data,1,Chd->DBin.Size,fpb);
				_strupr(Chd->Name);
				fprintf(fpt,"\tD_%s\t=%d,\n",Chd->Name,Pos);
				Pos+=Chd->DBin.Size;
				s.Format(L"Bin %s Pos : %d/%d\r\n",Ch->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
				free(Chd->DBin.Data);
			break;
			case CSTxx:
				Chst=(OSTATICTXT*)Ch;
				_strupr(Chst->Name);
				fprintf(fpt,"\tS%02d_%s\t=%d,\n",Chst->SType,Chst->Name,Pos);
				Len=Chst->Txt.GetLength();
				Dst.SType=Chst->SType;
				Dst.TxtSize=Len;
				Pos2+= (int)fwrite(&Dst,1,sizeof(DATAST),fpb);
				Pos2+= (int)fwrite(Chst->Txt,1,Len,fpb);
				Pos2+= (int)fwrite(cDummy,1,4-(Len&3),fpb);
				Pos+=sizeof(DATAST)+(Len&0xfffffffc)+4;
				s.Format(L"STxx %s Pos : %d/%d\r\n",Chst->Name,Pos2,Pos);
				SPgs->AddDbgInfo(s);
			break;
			default:
				char Txt[5];
				CString s;
				*(int*)Txt=Ch->Type;
				Txt[4]=0;
				s.Format(L"Out %s type error\r\n",Txt);
				SPgs->AddDbgInfo(s);
			break;
		}
		Ch=Ch->pSuiv;
	}
	while(Ch!=NULL);

	fprintf(fpt,"};\n");
	fprintf(fpt,"#endif\n");

	fseek(fpb,4,SEEK_SET);
	fwrite(&Pos,1,sizeof(int),fpb);
	fwrite(&NbBnk,1,sizeof(int),fpb);

	fclose(fpb);
	fclose(fpt);

	P24.SavePage24(Path);

	delete[] TPageList;
	return 1;
}


int CDOut::GetNbPic(void)
{
	CHUNKLIST *Ch;
	int Nb=0;
	if(List==NULL) return 0;
	Ch=List;

	while(Ch!=NULL)
	{
		Ch=Ch->pSuiv;
		Nb++;
	}

	return Nb;
}


CPage24::CPage24(CProgressDlg *Pgs,CViewDx *Vdd,int NbPage)
{
	int i;
	Page=new PAGE24B[NbPage];
	SPgs=Pgs;
	SVdd=Vdd;
	for(i=0;i<NbPage;i++)
	{
		Page[i].Mode=0;
		memset(&Page[i].Pix,0,256*256*3);
		memset(&Page[i].Alpha,0,256*256);
	}
	NbPages=NbPage;
}

CPage24::~CPage24(void)
{
	delete[] Page;
}

void PutPalPos2Page(int Pos,int PalPos,RGB24DEF *Pal,RGB24DEF *Pix,UCHAR *Alpha,BOOL IsFont)
{
	Pix[Pos].r=Pal[PalPos].r;
	Pix[Pos].g=Pal[PalPos].g;
	Pix[Pos].b=Pal[PalPos].b;
	if(IsFont)
	{
#if 1
		if(PalPos==0)
			Alpha[Pos]=0;
		else
			if(PalPos>7)					
				Alpha[Pos]=(Pal[PalPos].r+Pal[PalPos].g+Pal[PalPos].b)/3;
			else
				Alpha[Pos]=255;
#else
		if(PalPos==0)
			Alpha[Pos]=255;
		else
			if(PalPos>7)					
				Alpha[Pos]=256-((Pal[PalPos].r+Pal[PalPos].g+Pal[PalPos].b)/3);
			else
				Alpha[Pos]=0;
#endif
	}
}
void CPage24::AddBlk(int Pg,class CVram *Vram,BLKPSX *Blk,BOOL IsZoneFilter,BOOL IsFont)
{
	CImg Img(SPgs);
	int tp,i,j,Pos,LPos,OPos,LPos16,Pos16Bits;
	RGB24DEF *Pal;
	USHORT *Clut;
	UCHAR *Data;
	UCHAR *Data16;
	int u0,du;
	int w,h;
	RGB24DEF *Pix=nullptr;
	UCHAR *Alpha;

	Img.Img.TData.y=Blk->v0+TPAGEPC_Y(Blk->tpage);
	Img.Img.TData.w=(Blk->u1-Blk->u0)+1;
	Img.Img.TData.h=(Blk->v2-Blk->v0)+1;
	Img.Img.TClut.x=CLUT_X(Blk->clut);
	Img.Img.TClut.y=CLUT_Y(Blk->clut);
	tp=TPAGEPC_TP(Blk->tpage);
	Page[Pg].IsFont=IsFont;
	if(tp==CLUT4)
	{
		u0=Blk->u0&0xfc;
		du=Blk->u0-u0;
	
		Img.Img.TData.x=((u0)/4)+TPAGEPC_X(Blk->tpage);
		Img.Img.TClut.w=16;
		Img.Img.nb_colors=16;
		Img.Img.TData.w+=du;
		Img.Img.TData.w+=3;
		Img.Img.TData.w/=4;
		if(Page[Pg].Mode==0)
			Page[Pg].Mode=CLUT4;
		else
			if(Page[Pg].Mode!=CLUT4)
			{
				CString s;
				s.Format(L"Error Page %d don't have only CLUT4\r\n",Page);
				SPgs->AddDbgInfo(s);			
			}

	}
	else if(tp==CLUT8)
	{
		Img.Img.TData.x=((Blk->u0)/2)+TPAGEPC_X(Blk->tpage);
		Img.Img.TClut.w=256;
		Img.Img.nb_colors=256;
		Img.Img.TData.w+=1;
		Img.Img.TData.w/=2;
		if(Page[Pg].Mode==0)
			Page[Pg].Mode=CLUT8;
		else
			if(Page[Pg].Mode!=CLUT8)
			{
				CString s;
				s.Format(L"Error Page %d don't have only CLUT8\r\n",Page);
				SPgs->AddDbgInfo(s);			
			}
	}
	else
	{
		CString s;
		s.Format(L"Error : 16 Bits clut not supported on file %s\r\n",s);
		SPgs->AddDbgInfo(s);
	}

	Img.Img.TClut.h=1;
	if((Pal=new RGB24DEF[Img.Img.nb_colors])==NULL)
	{
		CString s;
		s.Format(L"Memory error for AddBlk %s\r\n");
		SPgs->AddDbgInfo(s);
	}
	Clut=Vram->GetPal(Blk->clut);
	for(i=0;i<Img.Img.nb_colors;i++)
	{
		Pal[i].r=CLUT_R(Clut[i])<<3;
		Pal[i].g=CLUT_V(Clut[i])<<3;
		Pal[i].b=CLUT_B(Clut[i])<<3;
	}


	if((Data=new UCHAR[Img.Img.TData.w*Img.Img.TData.h*2])==NULL)
	{
		CString s;
		s.Format(L"Memory error for AddBlk %s\r\n");
		SPgs->AddDbgInfo(s);
	}

	Vram->LoadImage(&Img.Img.TData,(UCHAR *)Data);


	if(tp==CLUT4)
	{
		int PalPos;
		LPos=0;
		Pos16Bits=OPos=Pos=Blk->v0*256+Blk->u0;

		w=(Blk->u1-Blk->u0)+1;
		h=(Blk->v2-Blk->v0)+1;
		Pix=Page[Pg].Pix;
		Alpha=Page[Pg].Alpha;

		if((Data16=new UCHAR[Img.Img.TData.w*Img.Img.TData.h*4])==NULL)
		{
			CString s;
			s.Format(L"Memory error for AddBlk %s\r\n");
			SPgs->AddDbgInfo(s);
		}

		LPos16=0;
		for(j=0;j<Img.Img.TData.h;j++)
		{
			for(i=du/2;i<((w+3)/4)*2+du/2;i++)
			{
				Data16[LPos16++]=Data[i+j*Img.Img.TData.w*2];
			}
		}

		delete[] (Data);
		Data=Data16;

		for(j=0;j<h;j++)
		{
			for(i=0;i<w/2;i++)
			{
				PalPos=Data[LPos]&0xf;
				PutPalPos2Page(Pos,PalPos,Pal,Pix,Alpha,IsFont);
				Pos++;
				PalPos=Data[LPos]>>4;
				PutPalPos2Page(Pos,PalPos,Pal,Pix,Alpha,IsFont);
				LPos++;
				Pos++;
			}
			switch(w&3)
			{
				case 1:
					PalPos=Data[LPos]&0xf;
					PutPalPos2Page(Pos,PalPos,Pal,Pix,Alpha,IsFont);
					Pos++;

					//Pix[Pos].r=Pal[Data[LPos]>>4].r;
					//Pix[Pos].g=Pal[Data[LPos]>>4].g;
					//Pix[Pos].b=Pal[Data[LPos]>>4].b;

					LPos+=2;
					Pos++;
				break;
				case 2:
					LPos++;
				break;
				case 3:
					PalPos=Data[LPos]&0xf;
					PutPalPos2Page(Pos,PalPos,Pal,Pix,Alpha,IsFont);
					Pos++;

					//Pix[Pos].r=Pal[Data[LPos]>>4].r;
					//Pix[Pos].g=Pal[Data[LPos]>>4].g;
					//Pix[Pos].b=Pal[Data[LPos]>>4].b;

					LPos++;
					Pos++;

				break;
			}

          	OPos+=256;
			Pos=OPos;
		}
	}

	if(tp==CLUT8)
	{
		LPos=0;
		Pos16Bits=OPos=Pos=Blk->v0*256+Blk->u0+128*(Blk->tpage&1);

		w=(Blk->u1-Blk->u0)+1;
		h=(Blk->v2-Blk->v0)+1;
		Pix=Page[Pg].Pix;

		for(j=0;j<h;j++)
		{
			for(i=0;i<w;i++)
			{
				Pix[Pos].r=Pal[Data[LPos]].r;
				Pix[Pos].g=Pal[Data[LPos]].g;
				Pix[Pos].b=Pal[Data[LPos]].b;
				LPos++;
				Pos++;
			}
			OPos+=256;
			Pos=OPos;
		}
	}

	if(IsZoneFilter)
	{
	
		// Add 0xff00ff color to border of blk for filter

		OPos=Pos16Bits;
		OPos+=256*(IsZoneFilter-1);
		Pos=OPos;
		for(j=0;j<IsZoneFilter;j++)
		{
			for(i=0;i<w;i++)
			{
				Pix[Pos].r=Pix[Pos+256].r;
				Pix[Pos].g=Pix[Pos+256].g;
				Pix[Pos].b=Pix[Pos+256].b;
				LPos++;
				Pos++;
			}
			OPos-=256;
			Pos=OPos;
		}

		OPos=Pos=Pos16Bits+256*(h-IsZoneFilter);
	
		for(j=0;j<IsZoneFilter;j++)
		{
			for(i=0;i<w;i++)
			{
				Pix[Pos].r=Pix[Pos-256].r;
				Pix[Pos].g=Pix[Pos-256].g;
				Pix[Pos].b=Pix[Pos-256].b;
				LPos++;
				Pos++;
			}
			OPos+=256;
			Pos=OPos;
		}

		OPos=Pos16Bits;
		OPos+=(IsZoneFilter-1);
		Pos=OPos;
		for(j=0;j<h;j++)
		{
			for(i=0;i<IsZoneFilter;i++)
			{
				Pix[Pos].r=Pix[Pos+1].r;
				Pix[Pos].g=Pix[Pos+1].g;
				Pix[Pos].b=Pix[Pos+1].b;
				LPos++;
				Pos--;
			}
			OPos+=256;
			Pos=OPos;
		}

		OPos=Pos=Pos16Bits+w-IsZoneFilter;
	
		for(j=0;j<h;j++)
		{
			for(i=0;i<IsZoneFilter;i++)
			{
				Pix[Pos].r=Pix[Pos-1].r;
				Pix[Pos].g=Pix[Pos-1].g;
				Pix[Pos].b=Pix[Pos-1].b;
				LPos++;
				Pos++;
			}
			OPos+=256;
			Pos=OPos;
		}
	
		Blk->u0+=IsZoneFilter;
		Blk->u1-=IsZoneFilter;
		Blk->u2+=IsZoneFilter;
		Blk->u3-=IsZoneFilter;
		Blk->v0+=IsZoneFilter;
		Blk->v1+=IsZoneFilter;
		Blk->v2-=IsZoneFilter;
		Blk->v3-=IsZoneFilter;
	}
/*
// For PC only (windows rect format)
	Blk->u1++;
	Blk->u3++;
	Blk->v2++;
	Blk->v3++;
*/
	SVdd->ShowPageColor24((UCHAR *)Page[Pg].Pix);

	delete[] (Data);
	delete[] (Pal);
}

int CPage24::SavePage24(wchar_t *Path)
{
	int i,j;
	CString s;
	CImg Img(SPgs),IMask(SPgs);
	UCHAR *pMask;
	Img.Img.w=256;
	Img.Img.h=256;
	Img.Img.nb_colors=0;
	IMask.Img.w=256;
	IMask.Img.h=256;
	IMask.Img.nb_colors=0;
	IMask.Img.data=pMask=new UCHAR[256*256*3];

	for(i=0;i<NbPages;i++)
	{
		s.Format(L"%s\\BNK%02d.raw",Path,i);
		Img.Img.data=Page[i].Pix;
		Img.save_raw(s.GetBuffer(),&Img.Img);

		if(Page[i].IsFont)
		{
			s.Format(L"%s\\BNK%02d.msk",Path,i);

			for(j=0;j<256*256;j++)
			{
				pMask[j*3+0]=Page[i].Alpha[j];
				pMask[j*3+1]=Page[i].Alpha[j];
				pMask[j*3+2]=Page[i].Alpha[j];
			}
			IMask.save_raw(s.GetBuffer(),&IMask.Img);
		}

	}
	delete [] IMask.Img.data;
	return 1;
}
