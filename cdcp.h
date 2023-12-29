#ifndef _CDCP_
#define _CDCP_

#include "ProgressDlg.h"

#define MAX_VGN	1000

typedef struct
{
	//int pic_nb;
	int	up;
	int	bottom;
	int	right;
	int	left;
} dcpRECT ;

typedef struct
{
	int	nb;
	dcpRECT	num[MAX_VGN];
} VGN ;


class Cdcp
{
public:
	Cdcp::Cdcp(CProgressDlg *Pgs);
	Cdcp::~Cdcp(void);
	int	get_info_dcp(UCHAR *image,int w, int h, int back_color, int larg_dcp, int haut_dcp, VGN *info_dcp);
	int dcp_to_pic(dcpRECT *dcp, pic_info_t *src, pic_info_t *dst);


protected:
	int	pt_in_rect(dcpRECT	rectangle, int x, int y);
	int	ord[MAX_VGN];
	CProgressDlg *SPgs;
};
#endif