
#include "stdafx.h"
#include "resource.h"
#include "ProgressDlg.h"
#include "cimg.h"
#include "cdcp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

Cdcp::Cdcp(CProgressDlg *Pgs)
{
	SPgs=Pgs;
}
Cdcp::~Cdcp(void)
{

}



int	Cdcp::pt_in_rect(dcpRECT	rectangle, int x, int y)
{
	if ( (x>=rectangle.left) &&
	     (x<=rectangle.right) &&
	     (y<=rectangle.bottom) &&
	     (y>=rectangle.up) )
		return TRUE ;
	else
		return FALSE ;
}

/********************************************************************************/
/*		DETOURE LES VIGNETTES DES FICHIERS ANIMATIONS 	 		*/
/*	  									*/
/*------------------------------------------------------------------------------*/
/*------------------------------------------------------------------------------*/
/********************************************************************************/



int	Cdcp::get_info_dcp(UCHAR *image,int w, int h, int back_color, int larg_dcp, int haut_dcp, VGN *info_dcp)
{
	VGN		info_dcp_bis;
	int		x,x_bis,y,y_bis,i;
	int		fin;
	int		vgn_in_spr;
	int		num_dcp;
	int		nb_spr_x, nb_spr_y;
	int		xmax, xmin, ymax, ymin;
	int		num, in_rect,temp;
	int		touch_right, touch_left ;

	num = 0 ;

	for ( y=0 ; y<h ; y++ )
	{
		for ( x=0 ; x<w ; x++ )
		{

			if ( *(image + y*w + x) != back_color )
			{
			  in_rect = 0 ;

			  if (num > 0)
			    for ( i=0 ; i<num ; i++ )
			      in_rect += pt_in_rect(info_dcp_bis.num[i],x, y ) ;

			  if ( !in_rect )
			  {
				xmax = x;
				xmin = x;
				ymax = y;
				ymin = y;
				fin  = 0;
				touch_right = 0 ;
				touch_left  = 0 ;

				while(!fin)
				{
					fin = 1;
					for ( y_bis=y ; y_bis<=ymax ; y_bis++ )
						if ( ( *(image + w*y_bis + xmax) != back_color ) && ( !touch_right ) )
						{
							y_bis=y ;
							xmax++ ;
							if (xmax>=w) { touch_right = 1 ; xmax=w-1 ; }
							fin = 0;
						}

					for ( x_bis=xmin ; x_bis<=xmax ; x_bis++ )
						if ( *(image + w*ymax + x_bis) != back_color )
						{
							x_bis=xmin ;
							ymax++ ;
							fin = 0;
						}

					for ( y_bis=y ; y_bis<=ymax ; y_bis++ )
						if ( ( (*(image + w*y_bis + xmin) != back_color) && ( !touch_left ) )
						     && (xmin>=0) )
						{
							y_bis=y ;
							xmin-- ;
							if (xmin<0) { touch_left = 1 ; xmin=0 ; }
							fin = 0;
						}

				}
				info_dcp_bis.num[num].up     = y ;
				info_dcp_bis.num[num].bottom = ymax-1 ;
				info_dcp_bis.num[num].left   = touch_left  ? xmin : xmin+1 ;
				info_dcp_bis.num[num].right  = touch_right ? xmax : xmax-1 ;

				/**verif si 2 decoupes sont imbriquees **/
				if ( (larg_dcp==-1) || (haut_dcp==-1) )	/* si pas de parametres */
				{
				   for ( i=0 ; i<num ; i++ )
				   {
					if ((info_dcp_bis.num[i].left	<info_dcp_bis.num[num].right) &&
						(info_dcp_bis.num[i].right	>info_dcp_bis.num[num].left) &&
						(info_dcp_bis.num[i].up		<info_dcp_bis.num[num].bottom) &&
						(info_dcp_bis.num[i].bottom	>info_dcp_bis.num[num].up)	)
					{
						CString s;
						s.Format(L"Dcp error : give size of sprite  %d(%d,%d,%d,%d)-%d(%d,%d,%d,%d) for this file\r\n",i,info_dcp_bis.num[i].left,info_dcp_bis.num[i].up,info_dcp_bis.num[i].right,info_dcp_bis.num[i].bottom,num,info_dcp_bis.num[num].left,info_dcp_bis.num[num].up,info_dcp_bis.num[num].right,info_dcp_bis.num[num].bottom);
						SPgs->AddDbgInfo(s);		
						//return FALSE;
					}
				   }
				}
				num++;
			  }
			}
		}
	}
	if ( (larg_dcp==-1) || (haut_dcp==-1) )
	{
	/* Remise dans l'ordre "Ligne-Colonne" de la liste des vignettes */
	/*---------------------------------------------------------------*/
		for ( y=0 ; y<num ; y++ )
			ord[y] = y ;

		for ( y=0 ; y<(num-1) ; y++ )
		{
			for ( x=y ; x<(num-1) ; x++ )
			{
				if( (info_dcp_bis.num[ord[y]].right > info_dcp_bis.num[ord[x+1]].right ) &&
				    (info_dcp_bis.num[ord[y]].bottom> info_dcp_bis.num[ord[x+1]].up) )
				{
					temp     = ord[y]   ;
					ord[y]   = ord[x+1] ;
					ord[x+1] = temp     ;

				}
			}
		}

		for ( y=0 ; y<num ; y++ )
		{
			info_dcp->num[y].up	 = info_dcp_bis.num[ord[y]].up	;
			info_dcp->num[y].bottom  = info_dcp_bis.num[ord[y]].bottom	;
			info_dcp->num[y].right	 = info_dcp_bis.num[ord[y]].right	;
			info_dcp->num[y].left	 = info_dcp_bis.num[ord[y]].left	;
		}
		info_dcp->nb = num ;

	}
	else
	{
	/* Regroupement des vignettes selon les parametre larg et haut sprites */
	/*---------------------------------------------------------------------*/
		vgn_in_spr = 0 ;
		num_dcp	= 0 ;
		nb_spr_x = w / larg_dcp ;
		nb_spr_y = h / haut_dcp ;

		//printf("nb spr x = %d\n",nb_spr_x);
		//printf("nb spr y = %d\n",nb_spr_y);

		for ( y=0 ; y<nb_spr_y ; y++ )
			for ( x=0 ; x<nb_spr_x ; x++ )
			{
				for ( i=0 ; i<num ; i++ )
				{
					if ( 	(info_dcp_bis.num[i].left	<  ((x+1)*larg_dcp)) &&
						(info_dcp_bis.num[i].left	>= (  x  *larg_dcp)) &&
						(info_dcp_bis.num[i].up	<  ((y+1)*haut_dcp)) &&
						(info_dcp_bis.num[i].up	>= (  y  *haut_dcp)) )
					{

					  if (	(info_dcp_bis.num[i].right	< ((x+1)*larg_dcp)) &&
						(info_dcp_bis.num[i].right	> (  x  *larg_dcp)) &&
						(info_dcp_bis.num[i].bottom	< ((y+1)*haut_dcp)) &&
						(info_dcp_bis.num[i].bottom	> (  y  *haut_dcp)) )
					  {
					    if (vgn_in_spr)
					    {
/*	printf("	vignette num %d\n",i);
*/						info_dcp->num[num_dcp].left =
							(info_dcp->num[num_dcp].left<info_dcp_bis.num[i].left)?
							info_dcp->num[num_dcp].left :
							info_dcp_bis.num[i].left ;

						info_dcp->num[num_dcp].right =
							(info_dcp->num[num_dcp].right>info_dcp_bis.num[i].right)?
							info_dcp->num[num_dcp].right :
							info_dcp_bis.num[i].right ;

						info_dcp->num[num_dcp].up =
							(info_dcp->num[num_dcp].up<info_dcp_bis.num[i].up)?
							info_dcp->num[num_dcp].up :
							info_dcp_bis.num[i].up ;

						info_dcp->num[num_dcp].bottom =
							(info_dcp->num[num_dcp].bottom>info_dcp_bis.num[i].bottom)?
							info_dcp->num[num_dcp].bottom :
							info_dcp_bis.num[i].bottom ;
					    }
					    else
					    {
						vgn_in_spr = 1 ;
/*	printf("enregistrement num %d :	 spr_x num %d	spr_y num %d\n",num_dcp,x,y);
	printf("	vignette num %d\n",i);
*/						info_dcp->num[num_dcp].left 	= info_dcp_bis.num[i].left ;
						info_dcp->num[num_dcp].right	= info_dcp_bis.num[i].right;
						info_dcp->num[num_dcp].up	= info_dcp_bis.num[i].up ;
						info_dcp->num[num_dcp].bottom	= info_dcp_bis.num[i].bottom;

					    }
					  }
					  else
					  {
						SPgs->AddDbgInfo(L"Dcp error : taille sprite incorrecte\r\n");		
						return FALSE;
					  }
					}
				}
				if (vgn_in_spr) { num_dcp++ ; vgn_in_spr = 0 ; }
			}
			info_dcp->nb = num_dcp ;
	}

	return TRUE ;
}
/***************************************************************************************/


int Cdcp::dcp_to_pic(dcpRECT *dcp, pic_info_t *src, pic_info_t *dst)
{
	int x,y,w,h;
	UCHAR *ptr,*s1,*s2;
	int err;

	err = TRUE;

	w = dcp->right-dcp->left+1;
	h = dcp->bottom-dcp->up+1;

	if (w && h)
	{
		dst->nb_colors = src->nb_colors;
		dst->w = w;
		dst->h = h;

		if ((dst->palette =  malloc(dst->nb_colors*3))==NULL)
		{
			SPgs->AddDbgInfo(L"Not enought memory in dcp_to_pic\r\n");		
		}

		if ((dst->data = malloc(w*h))==NULL)
		{
			SPgs->AddDbgInfo(L"Not enought memory in dcp_to_pic\r\n");		
		}

		memcpy(dst->palette,src->palette,src->nb_colors*3);	// copy la palette
		ptr =(UCHAR *) dst->data;

		s1 = (UCHAR *)src->data;
		s1 += src->w * dcp->up + dcp->left;
		for (y=0; y<h; y++)
		{
			s2 = s1;
			for (x=0; x<w; x++)
			{
				*ptr++ = *s2++;
			}
			s1 += src->w;
		}
		err = FALSE;
	}

	return err;
}

