;
; Scanne.ASM
;

B_GRIS          equ 1
B_ROUGE         equ 4

BLEU            equ 9
VERT            equ 10
CYAN            equ 11
ROUGE           equ 12
MAGENTA         equ 13
JAUNE           equ 14
BLANC           equ 15


.386P
.model FLAT, SYSCALL



.DATA

extrn _PVRamOccup        :DWORD  ; Des pointeurs sur les buffers (selon la page!)
extrn _PVRamCoul         :DWORD
;extrn _PVRamNum          :DWORD
;extrn _VRamOccup         :DWORD  ; Des pointeurs sur les buffers (selon la page!)
;extrn _VRamCoul          :DWORD

extrn _Log               :DWORD
extrn _BestVoisin        :DWORD
extrn _BestRecouvre      :DWORD
extrn _BestPlace         :DWORD
extrn _BestX             :DWORD
extrn _BestY             :DWORD


xp              dd 0
yp              dd 0

OccupePtr       dd 0
CouleurPtr      dd 0
LogPtr          dd 0

Recouvre        dd 0
max_x           dd 0
max_y           dd 0
extreme         dd 0


.CODE


; public  NoLanguage      ScanneMapRecouvre
; public  NoLanguage



; void ScanneMapOptimum(l,h,xb,yb))

_ScanneMapOptimum proc uses esi edi ebx,largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     eax,_PVRamCoul
                mov     CouleurPtr,eax

                mov     eax,640
                mul     yb
                add     eax,_Log
                add     eax,xb
                mov     LogPtr,eax

                mov     eax,256
                mul     hauteur
                mov     extreme,eax         ; EBX=offset sur ligne du bas.

                mov     _BestRecouvre,-1
                mov     _BestVoisin,-1

                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:

                mov     esi,OccupePtr
                add     esi,ecx

                mov     edi,CouleurPtr
                add     edi,ecx

                mov     ebx,LogPtr

                mov     yp,edx
                mov     xp,ecx

                xor     eax,eax                 ; recouvre=0

                mov     edx,hauteur
loop_y:


                mov     ecx,largeur
loop_x:
                cmp     byte ptr [esi],0
                je      fin_scanne
                cmp     byte ptr [edi],0
                jne     termine
                cmp     byte ptr [ebx],0
                jne     termine
                inc     eax
fin_scanne:

                inc     esi
                inc     edi
                inc     ebx

                dec     ecx
                jne     loop_x

                add     esi,256
                sub     esi,largeur

                add     edi,256
                sub     edi,largeur

                add     ebx,640
                sub     ebx,largeur

                dec     edx
                jne     loop_y


                mov     recouvre,eax    ; Sauve le rÇsultat...

;//// fin recherche posible


                ;
                ; Scanne en X, en haut et en bas...
                ;
                xor     eax,eax         ; voisin=0

                mov     esi,OccupePtr
                add     esi,xp
                dec     esi             ; pointe sur ligne du haut (-1 en x)
                mov     edi,esi
                sub     esi,256
                add     edi,extreme     ; idem pour le bas

                mov     ecx,largeur
                add     ecx,2
loop_scanne_x:
                cmp     byte ptr [esi],0
                je      pas_voisin_haut
                inc     eax
pas_voisin_haut:
                cmp     byte ptr [edi],0
                je      pas_voisin_bas
                inc     eax
pas_voisin_bas:
                inc     esi
                inc     edi
                dec     ecx
                jne     loop_scanne_x




                mov     esi,OccupePtr
                add     esi,xp
                mov     edi,esi
                dec     esi             ; pointe sur colonne de gauche (-1 en x)
                add     edi,largeur     ; pointe sur colonne de droite (+1 en x)

                mov     ecx,hauteur
                add     edx,2
loop_scanne_y:
                cmp     byte ptr [esi],0
                je      pas_voisin_gauche
                inc     eax
pas_voisin_gauche:
                cmp     byte ptr [edi],0
                je      pas_voisin_droite
                inc     eax
pas_voisin_droite:
                add     esi,256
                add     edi,256
                dec     ecx
                jne     loop_scanne_y




;//// teste efficacitÇ 1
                cmp     eax,_BestVoisin
                jle     termine
                jg      confirme
                mov     ebx,recouvre
                cmp     ebx,_BestRecouvre
                jle     termine

confirme:
                mov     ecx,xp
                mov     edx,yp

                mov     _BestRecouvre,eax
                mov     _BestVoisin,ebx
                mov     _BestX,ecx

                mov     _BestY,edx

termine:

                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp

                add     OccupePtr,256
                add     CouleurPtr,256
;                add     LogPtr,640
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
fin_scanning:
                mov     _BestRecouvre,-1
                ret
_ScanneMapOptimum ENDP






; void ScanneMapHorizontal(l,h,xb,yb))

_ScanneMapHorizontal proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     _BestPlace,-1



                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:



                mov     edi,OccupePtr
                add     edi,ecx

                mov     yp,edx
                mov     xp,ecx


                xor     eax,eax         ; recherche sur Æ0Ø

                mov     edx,hauteur
loop_y:
                mov     ecx,largeur
                repe    scasb           ; VÇrifie si c'est libre...
                jne     emplacement_refuse
                add     edi,256
                sub     edi,largeur
                dec     edx
                jne     loop_y
valide_emplacement:
                mov     _BestPlace,1
                mov     ecx,xp
                mov     _BestX,ecx
                mov     edx,yp
                mov     _BestY,edx
                ret


emplacement_refuse:
                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp

                add     OccupePtr,256
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
_ScanneMapHorizontal  ENDP













; void ScanneMapVertical(l,h,xb,yb))

_ScanneMapVertical proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     _BestPlace,-1



                xor     ecx,ecx
loop_xp:
                mov     esi,OccupePtr
                xor     edx,edx
loop_yp:

                mov     eax,esi
                mov     edi,eax

                mov     yp,edx
                mov     xp,ecx


                xor     eax,eax         ; recherche sur Æ0Ø

                mov     edx,hauteur
loop_y:
                mov     ecx,largeur
                repe    scasb           ; VÇrifie si c'est libre...
                jne     emplacement_refuse
                add     edi,256
                sub     edi,largeur
                dec     edx
                jne     loop_y
valide_emplacement:
                mov     _BestPlace,1
                mov     ecx,xp
                mov     _BestX,ecx
                mov     edx,yp
                mov     _BestY,edx
                ret


emplacement_refuse:
                mov     ecx,xp
                mov     edx,yp

                add     esi,256
                inc     edx
                cmp     edx,max_y
                jne     loop_yp

                inc     OccupePtr
                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp
                ret
_ScanneMapVertical  ENDP















; void ScanneMapRecouvre(l,h,xb,yb))

_ScanneMapRecouvre proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,256
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,256
                sub     eax,hauteur
                mov     max_y,eax

                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     eax,_PVRamCoul
                mov     CouleurPtr,eax

                mov     eax,640
                mul     yb
                add     eax,_Log
                add     eax,xb
                mov     LogPtr,eax

                mov     _BestRecouvre,-1

                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:

                mov     esi,OccupePtr
                add     esi,ecx

                mov     edi,CouleurPtr
                add     edi,ecx

                mov     ebx,LogPtr

                mov     yp,edx
                mov     xp,ecx

                xor     eax,eax                 ; recouvre=0

                mov     edx,hauteur
loop_y:


                mov     ecx,largeur
loop_x:
                cmp     byte ptr [esi],0
                je      fin_scanne
                cmp     byte ptr [edi],0
                jne     termine
                cmp     byte ptr [ebx],0
                jne     termine
                inc     eax
fin_scanne:

                inc     esi
                inc     edi
                inc     ebx

                dec     ecx
                jne     loop_x

                add     esi,256
                sub     esi,largeur

                add     edi,256
                sub     edi,largeur

                add     ebx,640
                sub     ebx,largeur

                dec     edx
                jne     loop_y

                cmp     eax,_BestRecouvre
                jle     termine

                mov     ecx,xp
                mov     edx,yp

                mov     _BestRecouvre,eax
                mov     _BestX,ecx
                mov     _BestY,edx

termine:

                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp

                add     OccupePtr,256
                add     CouleurPtr,256
;                add     LogPtr,640
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
fin_scanning:
                mov     _BestRecouvre,-1
                ret
_ScanneMapRecouvre ENDP



; void ScanneMapOptimum16(l,h,xb,yb))

_ScanneMapOptimum16 proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     eax,_PVRamCoul
                mov     CouleurPtr,eax

                mov     eax,640
                mul     yb
                add     eax,_Log
                add     eax,xb
                mov     LogPtr,eax

                mov     eax,256
                mul     hauteur
                mov     extreme,eax         ; EBX=offset sur ligne du bas.

                mov     _BestRecouvre,-1
                mov     _BestVoisin,-1

                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:

                mov     esi,OccupePtr
                add     esi,ecx

                mov     edi,CouleurPtr
                add     edi,ecx

                mov     ebx,LogPtr

                mov     yp,edx
                mov     xp,ecx

                xor     eax,eax                 ; recouvre=0

                mov     edx,hauteur
loop_y:


                mov     ecx,largeur
loop_x:
                cmp     byte ptr [esi],0
                je      fin_scanne
                cmp     byte ptr [edi],0
                jne     termine
                cmp     byte ptr [ebx],0
                jne     termine
                inc     eax
fin_scanne:

                inc     esi
                inc     edi
                inc     ebx

                dec     ecx
                jne     loop_x

                add     esi,256
                sub     esi,largeur

                add     edi,256
                sub     edi,largeur

                add     ebx,640
                sub     ebx,largeur

                dec     edx
                jne     loop_y


                mov     recouvre,eax    ; Sauve le rÇsultat...

;//// fin recherche posible


                ;
                ; Scanne en X, en haut et en bas...
                ;
                xor     eax,eax         ; voisin=0

                mov     esi,OccupePtr
                add     esi,xp
                dec     esi             ; pointe sur ligne du haut (-1 en x)
                mov     edi,esi
                sub     esi,256
                add     edi,extreme     ; idem pour le bas

                mov     ecx,largeur
                add     ecx,2
loop_scanne_x:
                cmp     byte ptr [esi],0
                je      pas_voisin_haut
                inc     eax
pas_voisin_haut:
                cmp     byte ptr [edi],0
                je      pas_voisin_bas
                inc     eax
pas_voisin_bas:
                inc     esi
                inc     edi
                dec     ecx
                jne     loop_scanne_x




                mov     esi,OccupePtr
                add     esi,xp
                mov     edi,esi
                dec     esi             ; pointe sur colonne de gauche (-1 en x)
                add     edi,largeur     ; pointe sur colonne de droite (+1 en x)

                mov     ecx,hauteur
                add     edx,2
loop_scanne_y:
                cmp     byte ptr [esi],0
                je      pas_voisin_gauche
                inc     eax
pas_voisin_gauche:
                cmp     byte ptr [edi],0
                je      pas_voisin_droite
                inc     eax
pas_voisin_droite:
                add     esi,256
                add     edi,256
                dec     ecx
                jne     loop_scanne_y




;//// teste efficacitÇ 1
                cmp     eax,_BestVoisin
                jle     termine
                jg      confirme
                mov     ebx,recouvre
                cmp     ebx,_BestRecouvre
                jle     termine

confirme:       mov     ecx,xp

		cmp	ecx,127
		ja	NPs127
		add	ecx,largeur
		cmp	ecx,127
		ja	termine

NPs127:		mov     ecx,xp
                mov     edx,yp

                mov     _BestRecouvre,eax
                mov     _BestVoisin,ebx
                mov     _BestX,ecx

                mov     _BestY,edx

termine:

                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp

                add     OccupePtr,256
                add     CouleurPtr,256
;                add     LogPtr,640
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
fin_scanning:
                mov     _BestRecouvre,-1
                ret
_ScanneMapOptimum16 ENDP













; void ScanneMapHorizontal(l,h,xb,yb))

_ScanneMapHorizontal16 proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     _BestPlace,-1



                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:



                mov     edi,OccupePtr
                add     edi,ecx

                mov     yp,edx
                mov     xp,ecx


                xor     eax,eax         ; recherche sur Æ0Ø

                mov     edx,hauteur
loop_y:
                mov     ecx,largeur
                repe    scasb           ; VÇrifie si c'est libre...
                jne     emplacement_refuse
                add     edi,256
                sub     edi,largeur
                dec     edx
                jne     loop_y
valide_emplacement:
		mov     ecx,xp

		cmp	ecx,127
		ja	NPs127
		add	ecx,largeur
		cmp	ecx,127
		ja	emplacement_refuse

NPs127:

                mov     _BestPlace,1
                mov     ecx,xp
                mov     _BestX,ecx
                mov     edx,yp
                mov     _BestY,edx
                ret


emplacement_refuse:
                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp

                add     OccupePtr,256
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
_ScanneMapHorizontal16  ENDP













; void ScanneMapVertical(l,h,xb,yb))

_ScanneMapVertical16 proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,257
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,257
                sub     eax,hauteur
                mov     max_y,eax


                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     _BestPlace,-1



                xor     ecx,ecx
loop_xp:
                mov     esi,OccupePtr
                xor     edx,edx
loop_yp:

                mov     eax,esi
                mov     edi,eax

                mov     yp,edx
                mov     xp,ecx


                xor     eax,eax         ; recherche sur Æ0Ø

                mov     edx,hauteur
loop_y:
                mov     ecx,largeur
                repe    scasb           ; VÇrifie si c'est libre...
                jne     emplacement_refuse
                add     edi,256
                sub     edi,largeur
                dec     edx
                jne     loop_y
valide_emplacement:

                mov     ecx,xp

		cmp	ecx,127
		ja	NPs127
		add	ecx,largeur
		cmp	ecx,127
		ja	emplacement_refuse

NPs127:

                mov     _BestPlace,1
                mov     ecx,xp
                mov     _BestX,ecx
                mov     edx,yp
                mov     _BestY,edx
                ret


emplacement_refuse:
                mov     ecx,xp
                mov     edx,yp

                add     esi,256
                inc     edx
                cmp     edx,max_y
                jne     loop_yp

                inc     OccupePtr
                inc     ecx
                cmp     ecx,max_x
                jne     loop_xp
                ret
_ScanneMapVertical16  ENDP















; void ScanneMapRecouvre(l,h,xb,yb))

_ScanneMapRecouvre16 proc uses esi edi ebx,\
                       largeur:DWORD,hauteur:DWORD,xb:DWORD,yb:DWORD

                mov     eax,256
                sub     eax,largeur
                mov     max_x,eax

                mov     eax,256
                sub     eax,hauteur
                mov     max_y,eax

                mov     eax,_PVRamOccup
                mov     OccupePtr,eax

                mov     eax,_PVRamCoul
                mov     CouleurPtr,eax

                mov     eax,640
                mul     yb
                add     eax,_Log
                add     eax,xb
                mov     LogPtr,eax

                mov     _BestRecouvre,-1

                xor     edx,edx
loop_yp:

                xor     ecx,ecx
loop_xp:

                mov     esi,OccupePtr
                add     esi,ecx

                mov     edi,CouleurPtr
                add     edi,ecx

                mov     ebx,LogPtr

                mov     yp,edx
                mov     xp,ecx

                xor     eax,eax                 ; recouvre=0

                mov     edx,hauteur
loop_y:


                mov     ecx,largeur
loop_x:
                cmp     byte ptr [esi],0
                je      fin_scanne
                cmp     byte ptr [edi],0
                jne     termine
                cmp     byte ptr [ebx],0
                jne     termine
                inc     eax
fin_scanne:

                inc     esi
                inc     edi
                inc     ebx

                dec     ecx
                jne     loop_x

                add     esi,256
                sub     esi,largeur

                add     edi,256
                sub     edi,largeur

                add     ebx,640
                sub     ebx,largeur

                dec     edx
                jne     loop_y

                cmp     eax,_BestRecouvre
                jle     termine

                mov     ecx,xp
                mov     edx,yp

		cmp	ecx,127
		ja	NPs127
		add	ecx,largeur
		cmp	ecx,127
		ja	termine

NPs127:


                mov     ecx,xp
                mov     edx,yp


                mov     _BestRecouvre,eax
                mov     _BestX,ecx
                mov     _BestY,edx

termine:

                mov     ecx,xp
                mov     edx,yp

                inc     ecx
                inc     ecx
                cmp     ecx,max_x
                jb     loop_xp

                add     OccupePtr,256
                add     CouleurPtr,256
;                add     LogPtr,640
                inc     edx
                cmp     edx,max_y
                jne     loop_yp
                ret
fin_scanning:
                mov     _BestRecouvre,-1
                ret
_ScanneMapRecouvre16 ENDP









; AfficheBlocVRam(largeur,hauteur,xb,yb);
;
; ebx -> _VRamOccup
; esi -> _Log
; edi -> _VRamCoul
;
_AfficheBlocVRam proc uses esi edi ebx,\
                          largeur:DWORD,hauteur:DWORD,\
                          xb:DWORD,yb:DWORD

                mov     ebx,_PVRamOccup
                mov     edi,_PVRamCoul

                mov     eax,256
                mul     _BestY
                add     eax,_BestX
                add     ebx,eax
                add     edi,eax

                mov     eax,640
                mul     yb
                add     eax,xb
                mov     esi,eax
                add     esi,_Log

                mov     edx,hauteur
loop_y:
                mov     ecx,largeur
loop_x:
                inc     byte ptr [ebx]
                inc     ebx
                dec     ecx
                jne     loop_x

                mov     ecx,largeur
                rep     movsb

                add     esi,640
                sub     esi,largeur
                add     edi,256
                sub     edi,largeur
                add     ebx,256
                sub     ebx,largeur

                dec     edx
                jne     loop_y
                ret

_AfficheBlocVRam ENDP



; UWORD CalculeEfficacite();
;
_CalculeEfficacite proc uses esi edi ebx
                mov     esi,_PVRamOccup

                xor     eax,eax
                mov     edx,255
loop_y:
                mov     ecx,255
loop_x:
                cmp     byte ptr [esi],0
                je      pas_occupe
                inc     eax
pas_occupe:
                inc     esi
                dec     ecx
                jne     loop_x

                inc     esi

                dec     edx
                jne     loop_y

                ret
_CalculeEfficacite ENDP






; InitVRamMap();

_InitVRamMap proc uses esi edi ebx
                xor     eax,eax
                ;mov     edi,_PVRamNum
                ;mov     ecx,32768       ; 256x256 words
                ;rep     stosd           ; Efface toute la map

                mov     edi,_PVRamOccup
                mov     ecx,16384	;16320       ; 256x255 bytes
                rep     stosd           ; Efface les 255 premiäres lignes
                mov     eax,-1
                mov     ecx,64
                ;rep     stosd           ; Met Ö -1 la derniäre ligne

                mov     edi,_PVRamCoul
                mov     ecx,16384	;16320       ; 256x255 bytes
                rep     stosd           ; Efface les 255 premiäres lignes
                mov     eax,-1
                mov     ecx,64
                ;rep     stosd           ; Met Ö -1 la derniäre ligne
		ret

                mov     esi,_PVRamOccup  ; Met Ö -1 la colonne de droite
                add     esi,255
                mov     edi,_PVRamCoul
                add     edi,255

                mov     ecx,256
loop_y:
                mov     byte ptr [esi],255
                mov     byte ptr [edi],255
                add     esi,256
                add     edi,256
                dec     ecx
                jne     loop_y
                ret
_InitVRamMap ENDP






















 END




