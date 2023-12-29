#include "stdafx.h"

#if _EDITOR

#include "resource.h"
#include "DialogDdInfo.h"
#include "Dd.h"
#include "DdCaps.h"

#ifdef _DEBUG
	#define new DEBUG_NEW
	#undef THIS_FILE
	static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CDialogDdInfo dialog


CDialogDdInfo::CDialogDdInfo(Cdd *pDd,CWnd* pParent /*=NULL*/) : CDialog(CDialogDdInfo::IDD, pParent)
{
    m_pDd = pDd;

	//{{AFX_DATA_INIT(CDialogDdInfo)
	m_text			= _T("");
	m_textBlue		= _T("");
	m_textGreen		= _T("");
	m_textRed		= _T("");
	m_textPixel		= _T("");
	m_bAutoRefresh	= FALSE;
	m_bAutoRefresh  = FALSE;
    m_bGeneral      = FALSE;
    m_bGeneral2     = FALSE;
    m_bCKey         = FALSE;
    m_bFX           = FALSE;
    m_bFXAlpha      = FALSE;
    m_bPal          = FALSE;
    m_bSV           = FALSE;
    m_bVideoMem     = FALSE;
	//}}AFX_DATA_INIT
}


void CDialogDdInfo::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDialogDdInfo)
	DDX_Text(pDX, IDC_DD_INFO, m_text);
	DDX_Text(pDX, IDC_BLUE_INFOS, m_textBlue);
	DDX_Text(pDX, IDC_GREEN_INFOS, m_textGreen);
	DDX_Text(pDX, IDC_RED_INFOS, m_textRed);
	DDX_Text(pDX, IDC_PIXEL_INFOS, m_textPixel);
	DDX_Check(pDX, IDC_DD_CONNECT, m_bAutoRefresh);
    DDX_Check(pDX, IDC_GENERAL,m_bGeneral);
    DDX_Check(pDX, IDC_GENERAL2,m_bGeneral2);
    DDX_Check(pDX, IDC_CKEY,m_bCKey);
    DDX_Check(pDX, IDC_FX,m_bFX);
    DDX_Check(pDX, IDC_FXALPHA,m_bFXAlpha);
    DDX_Check(pDX, IDC_PAL,m_bPal);
    DDX_Check(pDX, IDC_SV,m_bSV);
    DDX_Check(pDX, IDC_VIDEOMEM,m_bVideoMem);
    DDX_Control(pDX, IDC_COMBO,m_combo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDialogDdInfo, CDialog)
	//{{AFX_MSG_MAP(CDialogDdInfo)
	ON_BN_CLICKED(IDC_DD_CONNECT, OnDdConnect)
	ON_BN_CLICKED(IDC_DD_INFO_GET_CAPS, OnDdInfoGetCaps)
	ON_BN_CLICKED(IDC_DD_INFO_REFRESH, OnDdInfoRefresh)
	ON_BN_CLICKED(IDC_PREV_CAP, OnPrevCap)
	ON_BN_CLICKED(IDC_NEXT_CAP, OnNextCap)
	ON_BN_CLICKED(IDC_CKEY, OnCkey)
	ON_BN_CLICKED(IDC_FX, OnFx)
	ON_BN_CLICKED(IDC_FXALPHA, OnFxalpha)
	ON_BN_CLICKED(IDC_GENERAL, OnGeneral)
	ON_BN_CLICKED(IDC_GENERAL2, OnGeneral2)
	ON_BN_CLICKED(IDC_PAL, OnPal)
	ON_BN_CLICKED(IDC_SV, OnSv)
	ON_BN_CLICKED(IDC_VIDEOMEM, OnVideomem)
	ON_CBN_SELCHANGE(IDC_COMBO, OnSelchangeCombo)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDialogDdInfo message handlers


void CDialogDdInfo::InitVideoModes()
{
    UpdateData();

	m_combo.ResetContent();

    int i,index;
    CString s;

    for( i=0 ; i<Cdd::nNbAvailableVideoModes ; i++ )
    {
        s.Format( "%4dx%4d (%2d bits)",
                  Cdd::tabVideoMode[i].w,  
                  Cdd::tabVideoMode[i].h,  
                  Cdd::tabVideoMode[i].nbpp  
                );   
        
        index=m_combo.AddString(s);
        m_combo.SetItemData(index,(DWORD)i);

    }

    index=m_combo.AddString("This session");
    m_combo.SetItemData(index,0xFFFF);

    m_combo.SetCurSel(index);

    UpdateData(FALSE);
}


BOOL CDialogDdInfo::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    OnDdInfoRefresh();

	m_bAutoRefresh = TRUE;
    m_nIndexCap    = 0;

    SetInfosMode(&m_bGeneral);

	Refresh();

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CDialogDdInfo::OnDdInfoRefresh() 
{
    Cdd::getCaps();
    Cdd::nNbAvailableVideoModes=0;
    Cdd::enumVideoModes();
    InitVideoModes();
	Refresh();
}

void CDialogDdInfo::Refresh() 
{
    static char *sBool[2] = {"No","Yes"};

	// Lecture des infos inclues dans Cdd
	m_textBlue	= "Not available";
	m_textGreen	= "Not available";
	m_textRed	= "Not available";
	m_textPixel	= "Not available";

    int nComboIndex = m_combo.GetCurSel();
    int nVideoMode  = m_combo.GetItemData(nComboIndex);

    if( (nComboIndex!=CB_ERR) && (nVideoMode!=CB_ERR) )
    {
        if( nVideoMode==0xFFFF )
        {
	        if( Cdd::bPixelCapsAvailable )
	        {
		        m_textPixel.Format  ("%d bits per pixel",Cdd::nbBitsPerPixel);
		        m_textRed.Format    ("%2d bpp   shift %3d   mask 0x%04x",Cdd::nbBitsR,Cdd::shiftR,Cdd::maskR);
		        m_textGreen.Format  ("%2d bpp   shift %3d   mask 0x%04x",Cdd::nbBitsG,Cdd::shiftG,Cdd::maskG);
		        m_textBlue.Format   ("%2d bpp   shift %3d   mask 0x%04x",Cdd::nbBitsB,Cdd::shiftB,Cdd::maskB);
	        }
        }
        else
        {
		    m_textPixel.Format  ("%d bits per pixel",Cdd::tabVideoMode[nVideoMode].nbpp);
		    m_textRed.Format    ("%mask 0x%04x",Cdd::tabVideoMode[nVideoMode].maskR);
		    m_textGreen.Format  ("%mask 0x%04x",Cdd::tabVideoMode[nVideoMode].maskG);
		    m_textBlue.Format   ("%mask 0x%04x",Cdd::tabVideoMode[nVideoMode].maskB);
        }
    }

	// Infos générales

    if( Cdd::bCapsAvailable )
    {
        BOOL bBooleanValues=TRUE;
        ddCapsType *pHard=NULL,*pHEL=NULL;
        int nTabLen=0;

        if( m_bGeneral )
        {
            pHard   = ddGeneralCapsHardware;
            pHEL    = ddGeneralCapsHEL;
            nTabLen = ddGeneralCapsNbFlags;
        }
        else
        if( m_bGeneral2 )
        {
            pHard   = ddGeneralCaps2Hardware;
            pHEL    = ddGeneralCaps2HEL;
            nTabLen = ddGeneralCaps2NbFlags;
        }
        else
        if( m_bCKey )
        {
            pHard   = ddCKeyCapsHardware;
            pHEL    = ddCKeyCapsHEL;
            nTabLen = ddCKeyCapsNbFlags;
        }
        else
        if( m_bFX )
        {
            pHard   = ddFXCapsHardware;
            pHEL    = ddFXCapsHEL;
            nTabLen = ddFXCapsNbFlags;
        }
        else
        if( m_bFXAlpha )
        {
            pHard   = ddFXAlphaCapsHardware;
            pHEL    = ddFXAlphaCapsHEL;
            nTabLen = ddFXAlphaCapsNbFlags;
        }
        else
        if( m_bPal )
        {
            pHard   = ddPalCapsHardware;
            pHEL    = ddPalCapsHEL;
            nTabLen = ddPalCapsNbFlags;
        }
        else
        if( m_bSV )
        {
            pHard   = ddSVCapsHardware;
            pHEL    = ddSVCapsHEL;
            nTabLen = ddSVCapsNbFlags;
        }
        else
        if( m_bVideoMem )
        {
            pHard   = ddOtherCapsHardware;
            pHEL    = ddOtherCapsHEL;
            nTabLen = ddOtherCapsNbFlags;

            bBooleanValues = FALSE;
        }

        if( (nTabLen==0) || ((nTabLen>0) && pHard && pHEL) )
        {
            if( nTabLen>0 )
            {
                if( m_nIndexCap <  0       ) m_nIndexCap=0;
                if( m_nIndexCap >= nTabLen ) m_nIndexCap=nTabLen-1;

                m_text  = pHard[m_nIndexCap].sFlagName;
                m_text += ":\n\n";

                if( bBooleanValues )
                {
                    m_text += "Hardware : ";
                    m_text += sBool[(int)pHard[m_nIndexCap].nVal];
                    m_text += "\n";
                    m_text += "Emulation: ";
                    m_text += sBool[(int)pHEL[m_nIndexCap].nVal];
                }
                else
                {
                    CString s;
                    s.Format("Hardware : %d\nEmulation: %d",pHard[m_nIndexCap].nVal,pHEL[m_nIndexCap].nVal);
                    m_text += s;
                }

                m_text += "\n\n";
                m_text += pHard[m_nIndexCap].sLabel;
            }
            else
            {
            }
        }
    }
    else
    {
        m_text = "Capabilities informations not available";
    }

	UpdateData(FALSE);
}

void CDialogDdInfo::OnDdInfoGetCaps() 
{
    m_pDd->InitCapsVariables();
	OnDdInfoRefresh();
}

void CDialogDdInfo::OnDdConnect()
{
	UpdateData();
}

// Gestion des boutons de mode d'infos

void CDialogDdInfo::OnPrevCap() 
{
    m_nIndexCap--;
    Refresh();
}

void CDialogDdInfo::OnNextCap() 
{
    m_nIndexCap++;
    Refresh();
}

void CDialogDdInfo::SetInfosMode(BOOL *pBoolMode)
{
    m_bGeneral      = FALSE;
    m_bGeneral2     = FALSE;
    m_bCKey         = FALSE;
    m_bFX           = FALSE;
    m_bFXAlpha      = FALSE;
    m_bPal          = FALSE;
    m_bSV           = FALSE;
    m_bVideoMem     = FALSE;

    *pBoolMode      = TRUE;

    Refresh();
}

void CDialogDdInfo::OnCkey() 
{
    SetInfosMode(&m_bCKey);
}

void CDialogDdInfo::OnFx() 
{
    SetInfosMode(&m_bFX);
}

void CDialogDdInfo::OnFxalpha() 
{
    SetInfosMode(&m_bFXAlpha);
}

void CDialogDdInfo::OnGeneral() 
{
    SetInfosMode(&m_bGeneral);
}

void CDialogDdInfo::OnGeneral2() 
{
    SetInfosMode(&m_bGeneral2);
}                

void CDialogDdInfo::OnPal() 
{
    SetInfosMode(&m_bPal);
}

void CDialogDdInfo::OnSv() 
{
    SetInfosMode(&m_bSV);
}

void CDialogDdInfo::OnVideomem() 
{
    SetInfosMode(&m_bVideoMem);
}

void CDialogDdInfo::OnSelchangeCombo() 
{
    Refresh();
}

void CDialogDdInfo::OnSurfaceCaps()
{
    Refresh();
}

void CDialogDdInfo::OnSelchangeComboSurfaces()
{
    Refresh();
}

void CDialogDdInfo::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	// TODO: Add your message handler code here
	
	if( nStatus==SW_PARENTOPENING )
    {
        Refresh();
    }
}

#endif // _EDITOR

