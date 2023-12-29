#ifndef DIALOG_DD_INFO_H
	#define DIALOG_DD_INFO_H

    #if _EDITOR

        #include "Dd.h"

	    class CDialogDdInfo : public CDialog
	    {
		    // Construction
		    public:
			    CDialogDdInfo(Cdd *pDd,CWnd* pParent = NULL);   // standard constructor

                void InitVideoModes();
			    void Refresh();

                class Cdd *m_pDd;

		    // Dialog Data
			    //{{AFX_DATA(CDialogDdInfo)
	            enum { IDD = IDD_DD_INFO };
			    CString	m_text;
			    CString	m_textBlue;
			    CString	m_textGreen;
			    CString	m_textRed;
			    CString	m_textPixel;
			    BOOL	m_bAutoRefresh;
                BOOL    m_bGeneral;
                BOOL    m_bGeneral2;
                BOOL    m_bCKey;
                BOOL    m_bFX;
                BOOL    m_bFXAlpha;
                BOOL    m_bPal;
                BOOL    m_bSV;
                BOOL    m_bVideoMem;
                CComboBox m_combo;
	            //}}AFX_DATA

                int     m_nIndexCap;

		    // Overrides
			    // ClassWizard generated virtual function overrides
			    //{{AFX_VIRTUAL(CDialogDdInfo)
			    protected:
			    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			    //}}AFX_VIRTUAL

		    // Implementation
		    protected:

                void SetInfosMode(BOOL *pBoolMode);
                    
                // Generated message map functions
			    //{{AFX_MSG(CDialogDdInfo)
			    virtual BOOL OnInitDialog();
			    afx_msg void OnDdConnect();
			    afx_msg void OnDdInfoGetCaps();
			    afx_msg void OnDdInfoRefresh();
	            afx_msg void OnPrevCap();
	            afx_msg void OnNextCap();
	            afx_msg void OnCkey();
	            afx_msg void OnFx();
	            afx_msg void OnFxalpha();
	            afx_msg void OnGeneral();
	            afx_msg void OnGeneral2();
	            afx_msg void OnPal();
	            afx_msg void OnSv();
	            afx_msg void OnVideomem();
	            afx_msg void OnSelchangeCombo();
	            afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	            afx_msg void OnSurfaceCaps();
	            afx_msg void OnSelchangeComboSurfaces();
	            //}}AFX_MSG
			    DECLARE_MESSAGE_MAP()
	    };

    #endif // _EDITOR

#endif
