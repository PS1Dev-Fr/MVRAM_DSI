#ifndef DSI_DD_CAPS_H
    #define DSI_DD_CAPS_H

    #ifdef DSI_DD_C
        #define DSI_DD_EXT
    #else
        #define DSI_DD_EXT extern
    #endif

    typedef struct
    {
        DWORD   nFlag;
        char   *sFlagName;
        char   *sLabel;
        DWORD   nVal;
    }   ddCapsType;

    #define ddGeneralCapsNbFlags 26
    DSI_DD_EXT ddCapsType ddGeneralCapsHEL[ddGeneralCapsNbFlags];
    DSI_DD_EXT ddCapsType ddGeneralCapsHardware[ddGeneralCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDCAPS_3D,"DDCAPS_3D","Display hardware has 3D acceleration.",FALSE},
        {DDCAPS_ALIGNBOUNDARYDEST,"DDCAPS_ALIGNBOUNDARYDEST","Indicates that DirectDraw will support only source rectangles whose X axis is aligned on DIRECTDRAWCAPS.dwAlignBoundaryDest boundaries of the surface, respectively.",FALSE},
        {DDCAPS_ALIGNSIZEDEST,"DDCAPS_ALIGNSIZEDEST","Indicates that DirectDraw will support only source rectangles whose X axis size in BYTEs are DIRECTDRAWCAPS.dwAlignSizeDest multiples, respectively. ",FALSE},
        {DDCAPS_ALIGNBOUNDARYSRC,"DDCAPS_ALIGNBOUNDARYSRC","Indicates that DirectDraw will support only source rectangles whose X axis is aligned on DIRECTDRAWCAPS.dwAlignBoundarySrc boundaries of the surface, respectively.",FALSE},
        {DDCAPS_ALIGNSIZESRC,"DDCAPS_ALIGNSIZESRC","Indicates that DirectDraw will support only source rectangles whose X axis size in BYTEs are DIRECTDRAWCAPS.dwAlignSizeSrc multiples, respectively.",FALSE},
        {DDCAPS_ALIGNSTRIDE,"DDCAPS_ALIGNSTRIDE","Indicates that DirectDraw will create video memory surfaces that have a stride alignment equal to DIRECTDRAWCAPS.dwAlignStrideAlign.",FALSE},
        {DDCAPS_BANKSWITCHED,"DDCAPS_BANKSWITCHED","Display hardware is bank switched, and potentially very slow at random access to VRAM.",FALSE},
        {DDCAPS_BLT,"DDCAPS_BLT","Display hardware is capable of blit operations.",FALSE},
        {DDCAPS_BLTCOLORFILL,"DDCAPS_BLTCOLORFILL","Display hardware is capable of color fill with bltter.",FALSE},
        {DDCAPS_BLTQUEUE,"DDCAPS_BLTQUEUE","Display hardware is capable of asynchronous blit operations.",FALSE},
        {DDCAPS_BLTFOURCC,"DDCAPS_BLTFOURCC","Display hardware is capable of color space conversions during the blit operations.",FALSE},
        {DDCAPS_BLTSTRETCH,"DDCAPS_BLTSTRETCH","Display hardware is capable of stretching during blit operations.",FALSE},
        {DDCAPS_GDI,"DDCAPS_GDI","Display hardware is shared with GDI.",FALSE},
        {DDCAPS_OVERLAY,"DDCAPS_OVERLAY","Display hardware can overlay.",FALSE},
        {DDCAPS_OVERLAYCANTCLIP,"DDCAPS_OVERLAYCANTCLIP","Set if display hardware supports overlays but can not clip them.",FALSE},
        {DDCAPS_OVERLAYFOURCC,"DDCAPS_OVERLAYFOURCC","Indicates that overlay hardware is capable of color space conversions during the overlay operation.",FALSE},
        {DDCAPS_OVERLAYSTRETCH,"DDCAPS_OVERLAYSTRETCH","Indicates that stretching can be done by the overlay hardware.",FALSE},
        {DDCAPS_PALETTE,"DDCAPS_PALETTE","Indicates that DirectDraw is capable of creating and supporting DIRECTDRAWPALETTE objects for more than the primary surface.",FALSE},
      //{DDCAPS_PALETTECANVSYNC,"DDCAPS_PALETTECANVSYNC","Indicates that DirectDraw is capable of updating the palette in sync with the veritcal refresh.",FALSE},
        {DDCAPS_READSCANLINE,"DDCAPS_READSCANLINE","Display hardware can return the current scan line.",FALSE},
        {DDCAPS_VBI,"DDCAPS_VBI","Display hardware is capable of generating a vertical blank interrupt.",FALSE},
        {DDCAPS_ZBLTS,"DDCAPS_ZBLTS","Supports the use of Z buffers with blit operations.",FALSE},
        {DDCAPS_ZOVERLAYS,"DDCAPS_ZOVERLAYS","Supports the use of OverlayZOrder as a z value for overlays to control their layering.",FALSE},
        {DDCAPS_COLORKEY,"DDCAPS_COLORKEY","Obsolete flag that represents colorkey capabilities in either overlay hardware or blit hardware.  Will be replaced with DDCAPS_OVERLAYCOLORKEY and DDCAPS_BLTCOLORKEY.",FALSE},
        {DDCAPS_ALPHA,"DDCAPS_ALPHA","Display hardware supports alpha channel during blit operations.",FALSE},
      //{DDCAPS_COLORKEY_HWASSIST,"DDCAPS_COLORKEY_HWASSIST","Colorkey is hardware assisted.",FALSE},
        {DDCAPS_NOHARDWARE,"DDCAPS_NOHARDWARE","No hardware support at all.",FALSE},
    }   
    #endif
    ;

    #define ddGeneralCaps2NbFlags 1
    DSI_DD_EXT ddCapsType ddGeneralCaps2HEL[ddGeneralCaps2NbFlags];
    DSI_DD_EXT ddCapsType ddGeneralCaps2Hardware[ddGeneralCaps2NbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDCAPS2_CERTIFIED,"DDCAPS2_CERTIFIED","Display hardware is certified.",FALSE},
    }
    #endif
    ;

    #define ddCKeyCapsNbFlags 18
    DSI_DD_EXT ddCapsType ddCKeyCapsHEL[ddCKeyCapsNbFlags];
    DSI_DD_EXT ddCapsType ddCKeyCapsHardware[ddCKeyCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDCKEYCAPS_DESTBLT,"DDCKEYCAPS_DESTBLT","Supports transparent blitting using a color key to identify the replaceable bits of the destination surface for RGB colors.",FALSE},
        {DDCKEYCAPS_DESTBLTCLRSPACE,"DDCKEYCAPS_DESTBLTCLRSPACE","Supports transparent blitting using a color space to identify the replaceable bits of the destination surface for RGB colors.",FALSE},
        {DDCKEYCAPS_DESTBLTCLRSPACEYUV,"DDCKEYCAPS_DESTBLTCLRSPACEYUV","Supports transparent blitting using a color space to identify the replaceable bits of the destination surface for YUV colors.",FALSE},
        {DDCKEYCAPS_DESTBLTYUV,"DDCKEYCAPS_DESTBLTYUV","Supports transparent blitting using a color key to identify the replaceable bits of the destination surface for YUV colors.",FALSE},
        {DDCKEYCAPS_DESTOVERLAY,"DDCKEYCAPS_DESTOVERLAY","Supports overlaying using colorkeying of the replaceable bits of the surface being overlayed for RGB colors.",FALSE},
        {DDCKEYCAPS_DESTOVERLAYCLRSPACE,"DDCKEYCAPS_DESTOVERLAYCLRSPACE","Supports a color space as the color key for the destination for RGB colors.",FALSE},
        {DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV,"DDCKEYCAPS_DESTOVERLAYCLRSPACEYUV","Supports a color space as the color key for the destination for YUV colors.",FALSE},
        {DDCKEYCAPS_DESTOVERLAYONEACTIVE,"DDCKEYCAPS_DESTOVERLAYONEACTIVE","Supports only one active destination color key value for visible overlay surfaces.",FALSE},
        {DDCKEYCAPS_DESTOVERLAYYUV,"DDCKEYCAPS_DESTOVERLAYYUV","Supports overlaying using colorkeying of the replaceable bits of the surface being overlayed for YUV colors.",FALSE},
        {DDCKEYCAPS_SRCBLT,"DDCKEYCAPS_SRCBLT","Supports transparent blitting using the color key for the source with this surface for RGB colors.",FALSE},
        {DDCKEYCAPS_SRCBLTCLRSPACE,"DDCKEYCAPS_SRCBLTCLRSPACE","Supports transparent blitting using a color space for the source with this surface for RGB colors.",FALSE},
        {DDCKEYCAPS_SRCBLTCLRSPACEYUV,"DDCKEYCAPS_SRCBLTCLRSPACEYUV","Supports transparent blitting using a color space for the source with this surface for YUV colors.",FALSE},
        {DDCKEYCAPS_SRCBLTYUV,"DDCKEYCAPS_SRCBLTYUV","Supports transparent blitting using the color key for the source with this surface for YUV colors.",FALSE},
        {DDCKEYCAPS_SRCOVERLAY,"DDCKEYCAPS_SRCOVERLAY","Supports overlays using the color key for the source with this overlay surface for RGB colors.",FALSE},
        {DDCKEYCAPS_SRCOVERLAYCLRSPACE,"DDCKEYCAPS_SRCOVERLAYCLRSPACE","Supports overlays using a color space as the source color key for the overlay surface for RGB colors.",FALSE},
        {DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV,"DDCKEYCAPS_SRCOVERLAYCLRSPACEYUV","Supports overlays using a color space as the source color key for the overlay surface for YUV colors.",FALSE},
        {DDCKEYCAPS_SRCOVERLAYONEACTIVE,"DDCKEYCAPS_SRCOVERLAYONEACTIVE","Supports only one active source color key value for visible overlay surfaces.",FALSE},
        {DDCKEYCAPS_SRCOVERLAYYUV,"DDCKEYCAPS_SRCOVERLAYYUV","Supports overlays using the color key for the source with this overlay surface for YUV colors.",FALSE},
    }
    #endif
    ;

    #define ddFXCapsNbFlags 26
    DSI_DD_EXT ddCapsType ddFXCapsHEL[ddFXCapsNbFlags];
    DSI_DD_EXT ddCapsType ddFXCapsHardware[ddFXCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDFXCAPS_BLTARITHSTRETCHY,"DDFXCAPS_BLTARITHSTRETCHY","Uses arithmetic operations to stretch and shrink surfaces during blit rather than pixel doubling techniques.  Along the Y axis.",FALSE},
        {DDFXCAPS_BLTARITHSTRETCHYN,"DDFXCAPS_BLTARITHSTRETCHYN","Uses arithmetic operations to stretch and shrink surfaces during blit rather than pixel doubling techniques.  Along the Y axis.  Only works for x1, x2, etc.",FALSE},
        {DDFXCAPS_BLTMIRRORLEFTRIGHT,"DDFXCAPS_BLTMIRRORLEFTRIGHT","Supports mirroring left to right in blit.",FALSE},
        {DDFXCAPS_BLTMIRRORUPDOWN,"DDFXCAPS_BLTMIRRORUPDOWN","Supports mirroring top to bottom in blit.",FALSE},
        {DDFXCAPS_BLTROTATION,"DDFXCAPS_BLTROTATION","Supports arbitrary rotation.",FALSE},
        {DDFXCAPS_BLTROTATION90,"DDFXCAPS_BLTROTATION90","Supports 90 degree rotations.",FALSE},
        {DDFXCAPS_BLTSHRINKX,"DDFXCAPS_BLTSHRINKX","Supports arbitrary shrinking of a surface along the x axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSHRINKXN,"DDFXCAPS_BLTSHRINKXN","Supports integer shrinking (1x,2x,) of a surface along the x axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSHRINKY,"DDFXCAPS_BLTSHRINKY","Supports arbitrary shrinking of a surface along the y axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSHRINKYN,"DDFXCAPS_BLTSHRINKYN","Supports integer shrinking (1x,2x,) of a surface along the y axis (vertical direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSTRETCHX,"DDFXCAPS_BLTSTRETCHX","Supports arbitrary stretching of a surface along the x axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSTRETCHXN,"DDFXCAPS_BLTSTRETCHXN","Supports integer stretching (1x,2x,) of a surface along the x axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSTRETCHY,"DDFXCAPS_BLTSTRETCHY","Supports arbitrary stretching of a surface along the y axis (horizontal direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_BLTSTRETCHYN,"DDFXCAPS_BLTSTRETCHYN","Supports integer stretching (1x,2x,) of a surface along the y axis (vertical direction).  This flag is only valid for blit operations.",FALSE},
        {DDFXCAPS_OVERLAYARITHSTRETCHY,"DDFXCAPS_OVERLAYARITHSTRETCHY","Uses arithmetic operations to stretch and shrink surfaces during overlay rather than pixel doubling techniques.  Along the Y axis.",FALSE},
        {DDFXCAPS_OVERLAYARITHSTRETCHYN,"DDFXCAPS_OVERLAYARITHSTRETCHYN","Uses arithmetic operations to stretch and shrink surfaces during overlay rather than pixel doubling techniques.  Along the Y axis.  Only works for x1, x2, etc.",FALSE},
        {DDFXCAPS_OVERLAYSHRINKX,"DDFXCAPS_OVERLAYSHRINKX","Supports arbitrary shrinking of a surface along the x axis (horizontal direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that shrinking is available.",FALSE},
        {DDFXCAPS_OVERLAYSHRINKXN,"DDFXCAPS_OVERLAYSHRINKXN","Supports integer shrinking (1x,2x,) of a surface along the x axis (horizontal direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that shrinking is available.",FALSE},
        {DDFXCAPS_OVERLAYSHRINKY,"DDFXCAPS_OVERLAYSHRINKY","Supports arbitrary shrinking of a surface along the y axis (vertical direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that shrinking is available.",FALSE},
        {DDFXCAPS_OVERLAYSHRINKYN,"DDFXCAPS_OVERLAYSHRINKYN","Supports integer shrinking (1x,2x,) of a surface along the y axis (vertical direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that shrinking is available.",FALSE},
        {DDFXCAPS_OVERLAYSTRETCHX,"DDFXCAPS_OVERLAYSTRETCHX","Supports arbitrary stretching of a surface along the x axis (horizontal direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that stretching is available.",FALSE},
        {DDFXCAPS_OVERLAYSTRETCHXN,"DDFXCAPS_OVERLAYSTRETCHXN","Supports integer stretching (1x,2x,) of a surface along the x axis (horizontal direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that stretching is available.",FALSE},
        {DDFXCAPS_OVERLAYSTRETCHY,"DDFXCAPS_OVERLAYSTRETCHY","Supports arbitrary stretching of a surface along the y axis (vertical direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that stretching is available.",FALSE},
        {DDFXCAPS_OVERLAYSTRETCHYN,"DDFXCAPS_OVERLAYSTRETCHYN","Supports integer stretching (1x,2x,) of a surface along the y axis (vertical direction).  This flag is only valid for DDSCAPS_OVERLAY surfaces.  This flag only indicates the capabilities of a surface.  It does not indicate that stretching is available.",FALSE},
        {DDFXCAPS_OVERLAYMIRRORLEFTRIGHT,"DDFXCAPS_OVERLAYMIRRORLEFTRIGHT","Supports mirroring of overlays across the vertical axis.",FALSE},
        {DDFXCAPS_OVERLAYMIRRORUPDOWN,"DDFXCAPS_OVERLAYMIRRORUPDOWN","Supports mirroring of overlays across the horizontal axis. ",FALSE},
    }
    #endif
    ;

    #define ddFXAlphaCapsNbFlags 10
    DSI_DD_EXT ddCapsType ddFXAlphaCapsHEL[ddFXAlphaCapsNbFlags];
    DSI_DD_EXT ddCapsType ddFXAlphaCapsHardware[ddFXAlphaCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDFXALPHACAPS_BLTALPHAEDGEBLEND,"DDFXALPHACAPS_BLTALPHAEDGEBLEND","Supports alpha blending around the edge of a source color keyed surface.  For Blt.",FALSE},
        {DDFXALPHACAPS_BLTALPHAPIXELS,"DDFXALPHACAPS_BLTALPHAPIXELS","Supports alpha information in the pixel format.  The bit depth of alpha  information in the pixel format can be 1,2,4, or 8.  The alpha value becomes more opaque as the alpha value increases.  (0 is transparent.) For Blt.",FALSE},
        {DDFXALPHACAPS_BLTALPHAPIXELSNEG,"DDFXALPHACAPS_BLTALPHAPIXELSNEG","Supports alpha information in the pixel format.  The bit depth of alpha information in the pixel format can be 1,2,4, or 8.  The alpha value becomes more transparent as the alpha value increases.  (0 is opaque.)  This flag can only be set if DDCAPS_ALPHA is set. For Blt.",FALSE},
        {DDFXALPHACAPS_BLTALPHASURFACES,"DDFXALPHACAPS_BLTALPHASURFACES","Supports alpha only surfaces.  The bit depth of an alpha only surface can be 1,2,4, or 8. The alpha value becomes more opaque as the alpha value increases. (0 is transparent.) For Blt.",FALSE},
        {DDFXALPHACAPS_BLTALPHASURFACESNEG,"DDFXALPHACAPS_BLTALPHASURFACESNEG","The depth of the alpha channel data can range can be 1,2,4, or 8.  The NEG suffix indicates that this alpha channel becomes more transparent as the alpha value increases. (0 is opaque.) This flag can only be set if DDFXCAPS_ALPHASURFACES is set. For Blt.  ",FALSE},
        {DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND,"DDFXALPHACAPS_OVERLAYALPHAEDGEBLEND","Supports alpha blending around the edge of a source color keyed surface.  For Overlays.",FALSE},
        {DDFXALPHACAPS_OVERLAYALPHAPIXELS,"DDFXALPHACAPS_OVERLAYALPHAPIXELS","Supports alpha information in the pixel format.  The bit depth of alpha information in the pixel format can be 1,2,4, or 8. The alpha value becomes more opaque as the alpha value increases.  (0 is transparent.)  For Overlays.",FALSE},
        {DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG,"DDFXALPHACAPS_OVERLAYALPHAPIXELSNEG","Supports alpha information in the pixel format.  The bit depth of alpha information in the pixel format can be 1,2,4, or 8. The alpha value becomes more transparent as the alpha value increases.  (0 is opaque.)  This flag can only be set if DDFXCAPS_ALPHAPIXELS is set.  For Overlays.",FALSE},
        {DDFXALPHACAPS_OVERLAYALPHASURFACES,"DDFXALPHACAPS_OVERLAYALPHASURFACES","Supports alpha only surfaces.  The bit depth of an alpha only surface can be 1,2,4, or 8. The alpha value becomes more opaque as the alpha value increases.  (0 is transparent.)  For Overlays.",FALSE},
        {DDFXALPHACAPS_OVERLAYALPHASURFACESNEG,"DDFXALPHACAPS_OVERLAYALPHASURFACESNEG","The depth of the alpha channel data can range can be 1,2,4, or 8.  The NEG suffix indicates that this alpha channel becomes more transparent as the alpha value increases. (0 is opaque.)  This flag can only be set if DDFXCAPS_ALPHASURFACES is set.  For Overlays.",FALSE},
    }
    #endif
    ;

    #define ddPalCapsNbFlags 8
    DSI_DD_EXT ddCapsType ddPalCapsHEL[ddPalCapsNbFlags];
    DSI_DD_EXT ddCapsType ddPalCapsHardware[ddPalCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDPCAPS_4BIT,"DDPCAPS_4BIT","Index is 4 bits.  There are sixteen color entries in the palette table.",FALSE},
        {DDPCAPS_8BITENTRIES,"DDPCAPS_8BITENTRIES","Index is onto an 8 bit color index.  This field is only valid with the DDPCAPS_4BIT capability and the target surface is in 8bpp.  Each color entry is one byte long and is an index into destination surface's 8bpp palette.",FALSE},
        {DDPCAPS_8BIT,"DDPCAPS_8BIT","Index is 8 bits.  There are 256 color entries in the palette table.",FALSE},
        {DDPCAPS_ALLOW256,"DDPCAPS_ALLOW256","This palette can have all 256 entries defined.",FALSE},
        {DDPCAPS_INITIALIZE,"DDPCAPS_INITIALIZE","Indicates that this DIRECTDRAWPALETTE should use the palette color array passed into the lpDDColorArray parameter to initialize the DIRECTDRAWPALETTE object.",FALSE},
        {DDPCAPS_PRIMARYSURFACE,"DDPCAPS_PRIMARYSURFACE","This palette is the one attached to the primary surface.  Changing this table has immediate effect on the display unless DDPAL_VSYNC is specified and supported.",FALSE},
        {DDPCAPS_PRIMARYSURFACELEFT,"DDPCAPS_PRIMARYSURFACELEFT","This palette is the one attached to the primary surface left.  Changing this table has immediate effect on the display unless DDPAL_VSYNC is specified and supported.",FALSE},
        {DDPCAPS_VSYNC,"DDPCAPS_VSYNC","This palette can have modifications to it synced with the monitors refresh rate.",FALSE},
    }
    #endif
    ;

    #define ddSVCapsNbFlags 4
    DSI_DD_EXT ddCapsType ddSVCapsHEL[ddSVCapsNbFlags];
    DSI_DD_EXT ddCapsType ddSVCapsHardware[ddSVCapsNbFlags];

    enum
    {
        DDOTHERCAPS_TOTALMEM=0,
        DDOTHERCAPS_FREEMEM,
        ddOtherCapsNbFlags
    };

    DSI_DD_EXT ddCapsType ddOtherCapsHEL[ddOtherCapsNbFlags];
    DSI_DD_EXT ddCapsType ddOtherCapsHardware[ddOtherCapsNbFlags]
    #ifdef DSI_DD_C
    =
    {
        {DDOTHERCAPS_TOTALMEM,"Total memory","Total amount of video memory.",0},
        {DDOTHERCAPS_FREEMEM,"Free memory","Amount of free video memory.",0},
    }
    #endif
    ;

    #undef DSI_DD_EXT

#endif
