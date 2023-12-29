#include "stdafx.h"

#define PL_COLOR_C

#include "RgbHls.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static double max3(double a,double b,double c)
{
    double max_ab;

    max_ab = (a>b)?a:b;
    
    return( (max_ab>c)?max_ab:c );
}

static double min3(double a,double b,double c)
{
    double min_ab;

    min_ab = (a<b)?a:b;
    
    return( (min_ab<c)?min_ab:c );
}

void RgbToHlsDouble(double r,double g,double b,double *h,double *l,double *s)
{
    double max_rgb,min_rgb,delta;
    
    max_rgb=max3(r,g,b);
    min_rgb=min3(r,g,b);
    
    *l=(max_rgb+min_rgb)/2.0;
    
    if(max_rgb==min_rgb)
    {
        *s=0;
		*h=HLS_H_UNDEFINED; /* undefined */
    }
    else
    {
    	if(*l<=0.5)
		{
			*s=(max_rgb-min_rgb)/(max_rgb+min_rgb);
		}
		else
		{
			*s=(max_rgb-min_rgb)/(2.0-max_rgb-min_rgb);
		}
		
		delta = max_rgb-min_rgb;

		if( r==max_rgb )
		{
			*h=(g-b)/delta;
		}
		else
		if( g==max_rgb )
		{
			*h=2.0+(b-r)/delta;
		}
		else
		if( b==max_rgb )
		{
			*h=4.0+(r-g)/delta;
		}
		
		*h *= 60.0;
		
		if( *h<0 )
		{
			*h += 360.0;
		}
    }
    
}

void RgbToHls(RGB_MEMBER r,RGB_MEMBER g,RGB_MEMBER b,int *h,int *l,int *s)
{
    double r_double,g_double,b_double,h_double,l_double,s_double;

    r_double = r*1.0/MAX_SCREEN_RGB;
    g_double = g*1.0/MAX_SCREEN_RGB;
    b_double = b*1.0/MAX_SCREEN_RGB;
    
    RgbToHlsDouble(r_double,g_double,b_double,&h_double,&l_double,&s_double);
    
    if( h_double!=HLS_H_UNDEFINED )
    {
		*h = (int)(h_double*MAX_H/360.0);
    }
    else
    {
		*h = HLS_H_UNDEFINED;
    }

    *l = (int)(l_double*MAX_L);
    *s = (int)(s_double*MAX_S);
}

static double value(double n1,double n2,double hue)
{
    double val;

    if( hue>360.0 )
    {
        hue -= 360.0;
    }
    else
    if( hue<0 )
    {
        hue += 360.0;
    }
    
    if( hue<60.0 )
    {
		val = n1+(n2-n1)*hue/60;
    }
    else
    if( hue<180.0 )
    {
        val = n2;
    }
    else
    if( hue<240.0 )
    {
        val = n1 + (n2-n1)*(240.0-hue)/60.0;
    }
    else
    {
        val = n1;
    }
    
    return val;
}

void HlsToRgbDouble(double h,double l,double s,double *r,double *g,double *b)
{
    double m2,m1;
    
    if( l<0.5 )
    {
		m2 = l*(1+s);
    }
    else
    {
        m2 = l+s-l*s;
    }
    
    m1 = 2*l-m2;
    
    if( s==0 )
    {
        if( h==HLS_H_UNDEFINED )
		{
			(*r)=(*g)=(*b)=l;
		}
		else
		{
			(*r)=(*g)=(*b)=0; /* ERROR */
		}
    }
    else
    {
        *r = value(m1,m2,h+120.0);
        *g = value(m1,m2,h);
        *b = value(m1,m2,h-120.0);
    }
}

void HlsToRgb(int h,int l,int s,RGB_MEMBER *r,RGB_MEMBER *g,RGB_MEMBER *b)
{
    double h_double,l_double,s_double,r_double,g_double,b_double;

    h_double = 1.0*h/MAX_H*360.0;
    l_double = 1.0*l/MAX_L;
    s_double = 1.0*s/MAX_S;
    
    HlsToRgbDouble(h_double,l_double,s_double,&r_double,&g_double,&b_double);
    
    *r = (RGB_MEMBER)(r_double*MAX_SCREEN_RGB);
    *g = (RGB_MEMBER)(g_double*MAX_SCREEN_RGB);
    *b = (RGB_MEMBER)(b_double*MAX_SCREEN_RGB);
}

#undef PL_COLOR_C
