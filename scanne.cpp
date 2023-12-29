#include "stdafx.h"
#include <windows.h>


#include "scanne.h"



ScanData g_state;

void ScanneMapOptimum( int largeur, int hauteur, int xb, int yb) 
{
    g_state.max_x = 257- largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t *pOccupeData = g_state.OccupePtr;
    uint8_t* pCouleurData = g_state.CouleurPtr;

    uint8_t* pLogData = g_state.LogPtr + 640 * yb + xb;

    g_state.extreme = hauteur*256;

    g_state.BestRecouvre = -1;
    g_state.BestVoisin = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) 
    {
        for (int gx = 0; gx < g_state.max_x; ++gx) 
        {
            BYTE* pOccupePtr = pOccupeData + gx;
            BYTE* pColorPtr = pCouleurData + gx;
            BYTE* pLogPtr = pLogData;

            g_state.yp = gy;
            g_state.xp = gx;

            int nNumberRecouvert= 0; // recouvre = 0

            for (int y = 0; y < hauteur; ++y) 
            {
                for (int x = 0; x < largeur; ++x) 
                {
                    if (*pOccupePtr != 0)
                    {
                        if (*pOccupePtr == 0 && *pLogPtr == 0)
                        {
                            nNumberRecouvert++;
                        }
                    }

                    pOccupePtr++;
                    pColorPtr++;
                    pLogPtr++;
                }

                pOccupePtr += 256 - largeur;
                pColorPtr += 256 - largeur;
                pLogPtr += 640 - largeur;
            }

            g_state.recouvre = nNumberRecouvert;

            // Scanning en X, en haut et en bas
            int nNumberRecouvertXY = 0; // voisin = 0
            pOccupePtr = pOccupeData + g_state.xp - 1;
            pColorPtr = pColorPtr - 256;
            BYTE* pOccupePtrX = pOccupePtr + g_state.extreme; // Pour la ligne du bas

            for (int x = 0; x < largeur + 2; ++x) 
            {
                nNumberRecouvertXY += (pColorPtr[x] != 0) + (pOccupePtrX[x] != 0);
            }

            // Scanning en Y, gauche et droite
            BYTE* pOccupePtrY = (pOccupeData + g_state.xp - 1) + largeur;

            for (int y = 0; y < hauteur + 2; ++y) 
            {
                nNumberRecouvertXY += (pOccupePtrY[y * 256] != 0) + (pOccupePtrY[y * 256] != 0);
            }

            // Test d'efficacité et mise à jour des meilleures valeurs
            if (nNumberRecouvertXY > g_state.BestVoisin ||
                (nNumberRecouvertXY == g_state.BestVoisin && g_state.recouvre > g_state.BestRecouvre))
            {
                g_state.BestRecouvre = nNumberRecouvertXY;
                g_state.BestVoisin = g_state.recouvre;
                g_state.BestX = g_state.xp;
                g_state.BestY = g_state.yp;
            }

            // Mise à jour des pointeurs pour la prochaine itération
            pOccupeData += 256;
            pCouleurData += 256;
            // pLogData += 640; // Ajustez selon la logique de votre implémentation
        }
    }
}

void ScanneMapHorizontal(int largeur, int hauteur, int xb, int yb) 
{
    g_state.max_x = 257 - largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    g_state.BestPlace = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) 
    {
        for (int gx = 0; gx < g_state.max_x; ++gx) 
        {
            BYTE* currentPtr = pOccupeData + gx;

            bool isValidPlace = true;
            for (int y = 0; y < hauteur; ++y) 
            {
                for (int x = 0; x < largeur; ++x) 
                {
                    if (currentPtr[x] != 0) {
                        isValidPlace = false;
                        break;
                    }
                }
                if (!isValidPlace) 
                {
                    break;
                }
                currentPtr += 256; // Move to next line
            }

            if (isValidPlace) 
            {
                g_state.BestPlace = 1;
                g_state.BestX = gx;
                g_state.BestY = gy;
                return; // Found a valid place, exit early
            }

            pOccupeData += 256; // Move to next line for next iteration
        }
    }
}


void ScanneMapVertical(int largeur, int hauteur, int xb, int yb) 
{
    g_state.max_x = 257 - largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    g_state.BestPlace = -1;

    for (int gx = 0; gx < g_state.max_x; ++gx) 
    {
        for (int gy = 0; gy < g_state.max_y; ++gy) 
        {
            BYTE* currentPtr = pOccupeData + gy * 256 + gx;

            bool isValidPlace = true;
            for (int y = 0; y < hauteur; ++y) 
            {
                for (int x = 0; x < largeur; ++x) 
                {
                    if (currentPtr[x * 256] != 0) 
                    {
                        isValidPlace = false;
                        break;
                    }
                }
                if (!isValidPlace) 
                {
                    break;
                }
            }

            if (isValidPlace) 
            {
                g_state.BestPlace = 1;
                g_state.BestX = gx;
                g_state.BestY = gy;
                return; // Found a valid place, exit early
            }
        }
    }
}



void ScanneMapRecouvre(int largeur, int hauteur, int xb, int yb) {
    g_state.max_x = 256 - largeur;
    g_state.max_y = 256 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    uint8_t* pCouleurData = g_state.CouleurPtr;
    uint8_t* pLogData = g_state.LogPtr + 640 * yb + xb;

    g_state.BestRecouvre = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) {
        for (int gx = 0; gx < g_state.max_x; ++gx) {
            BYTE* pOccupePtr = pOccupeData + gx;
            BYTE* pColorPtr = pCouleurData + gx;
            BYTE* pLogPtr = pLogData + gy * 640;

            int nNumberRecouvert = 0;

            for (int y = 0; y < hauteur; ++y) {
                for (int x = 0; x < largeur; ++x) {
                    if (*pOccupePtr != 0 && *pColorPtr == 0 && *pLogPtr == 0) {
                        nNumberRecouvert++;
                    }

                    pOccupePtr++;
                    pColorPtr++;
                    pLogPtr++;
                }

                pOccupePtr += 256 - largeur;
                pColorPtr += 256 - largeur;
                pLogPtr += 640 - largeur;
            }

            if (nNumberRecouvert > g_state.BestRecouvre) {
                g_state.BestRecouvre = nNumberRecouvert;
                g_state.BestX = gx;
                g_state.BestY = gy;
            }

            pOccupeData += 256;
            pCouleurData += 256;
            pLogData += 640;
        }
    }
}


void ScanneMapOptimum16(int largeur, int hauteur, int xb, int yb) {
    g_state.max_x = 257 - largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    uint8_t* pCouleurData = g_state.CouleurPtr;

    uint8_t* pLogData = g_state.LogPtr + 640 * yb + xb;

    g_state.extreme = hauteur * 256;

    g_state.BestRecouvre = -1;
    g_state.BestVoisin = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) {
        for (int gx = 0; gx < g_state.max_x; ++gx) {
            BYTE* pOccupePtr = pOccupeData + gx;
            BYTE* pColorPtr = pCouleurData + gx;
            BYTE* pLogPtr = pLogData + gy * 640;

            int nNumberRecouvert = 0;

            for (int y = 0; y < hauteur; ++y) {
                for (int x = 0; x < largeur; ++x) {
                    if (*pOccupePtr != 0 && *pColorPtr == 0 && *pLogPtr == 0) {
                        nNumberRecouvert++;
                    }

                    pOccupePtr++;
                    pColorPtr++;
                    pLogPtr++;
                }

                pOccupePtr += 256 - largeur;
                pColorPtr += 256 - largeur;
                pLogPtr += 640 - largeur;
            }

            int nNumberRecouvertXY = 0;
            BYTE* pOccupePtrX = pOccupePtr - 1;
            BYTE* pOccupePtrBottom = pOccupePtrX + g_state.extreme;

            for (int x = 0; x < largeur + 2; ++x) {
                nNumberRecouvertXY += (pOccupePtrX[x] != 0) + (pOccupePtrBottom[x] != 0);
            }

            BYTE* pOccupePtrY = pOccupePtrX + largeur;
            for (int y = 0; y < hauteur + 2; ++y) {
                nNumberRecouvertXY += (pOccupePtrY[y * 256] != 0) + (pOccupePtrY[y * 256] != 0);
            }

            if (nNumberRecouvertXY > g_state.BestVoisin ||
                (nNumberRecouvertXY == g_state.BestVoisin && nNumberRecouvert > g_state.BestRecouvre)) {
                if (gx <= 127 && gx + largeur <= 127) {
                    g_state.BestRecouvre = nNumberRecouvertXY;
                    g_state.BestVoisin = nNumberRecouvert;
                    g_state.BestX = gx;
                    g_state.BestY = gy;
                }
            }

            pOccupeData += 256;
            pCouleurData += 256;
            pLogData += 640;
        }
    }
}



void ScanneMapHorizontal16(int largeur, int hauteur, int xb, int yb) {
    g_state.max_x = 257 - largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    g_state.BestPlace = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) {
        for (int gx = 0; gx < g_state.max_x; ++gx) {
            BYTE* currentPtr = pOccupeData + gy * 256 + gx;

            bool isValidPlace = true;
            for (int y = 0; y < hauteur; ++y) {
                for (int x = 0; x < largeur; ++x) {
                    if (currentPtr[x] != 0) {
                        isValidPlace = false;
                        break;
                    }
                }
                if (!isValidPlace) {
                    break;
                }
                currentPtr += 256; // Move to next line
            }

            // Check for 16-bit constraint
            if (isValidPlace && gx <= 127 && (gx + largeur) <= 127) {
                g_state.BestPlace = 1;
                g_state.BestX = gx;
                g_state.BestY = gy;
                return; // Found a valid place, exit early
            }
        }
    }
}



void ScanneMapVertical16(int largeur, int hauteur, int xb, int yb) {
    g_state.max_x = 257 - largeur;
    g_state.max_y = 257 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    g_state.BestPlace = -1;

    for (int gx = 0; gx < g_state.max_x; ++gx) {
        for (int gy = 0; gy < g_state.max_y; ++gy) {
            BYTE* currentPtr = pOccupeData + gx;

            bool isValidPlace = true;
            for (int y = 0; y < hauteur; ++y) {
                if (currentPtr[y * 256] != 0) {
                    isValidPlace = false;
                    break;
                }
            }

            // Check for 16-bit constraint
            if (isValidPlace && gx <= 127 && (gx + largeur) <= 127) {
                g_state.BestPlace = 1;
                g_state.BestX = gx;
                g_state.BestY = gy;
                return; // Found a valid place, exit early
            }

            currentPtr += 256; // Move to next line for next iteration
        }
    }
}



void ScanneMapRecouvre16(int largeur, int hauteur, int xb, int yb) {
    g_state.max_x = 256 - largeur;
    g_state.max_y = 256 - hauteur;

    uint8_t* pOccupeData = g_state.OccupePtr;
    uint8_t* pCouleurData = g_state.CouleurPtr;

    uint8_t* pLogData = g_state.LogPtr + 640 * yb + xb;

    g_state.BestRecouvre = -1;

    for (int gy = 0; gy < g_state.max_y; ++gy) {
        for (int gx = 0; gx < g_state.max_x; ++gx) {
            BYTE* pOccupePtr = pOccupeData + gx;
            BYTE* pColorPtr = pCouleurData + gx;
            BYTE* pLogPtr = pLogData + gy * 640;

            int nNumberRecouvert = 0;

            for (int y = 0; y < hauteur; ++y) {
                for (int x = 0; x < largeur; ++x) {
                    if (*pOccupePtr != 0 && *pColorPtr == 0 && *pLogPtr == 0) {
                        nNumberRecouvert++;
                    }

                    pOccupePtr++;
                    pColorPtr++;
                    pLogPtr++;
                }

                pOccupePtr += 256 - largeur;
                pColorPtr += 256 - largeur;
                pLogPtr += 640 - largeur;
            }

            // Check for 16-bit constraint
            if (nNumberRecouvert > g_state.BestRecouvre && gx <= 127 && (gx + largeur) <= 127) {
                g_state.BestRecouvre = nNumberRecouvert;
                g_state.BestX = gx;
                g_state.BestY = gy;
            }

            pOccupeData += 256;
            pCouleurData += 256;
            pLogData += 640;
        }
    }
}


void AfficheBlocVRam(int largeur, int hauteur, int xb, int yb) 
{
    uint8_t* pOccupeData = g_state.OccupePtr + g_state.BestY * 256 + g_state.BestX;;
    uint8_t* pCouleurData = g_state.CouleurPtr + g_state.BestY * 256 + g_state.BestX;;

    uint8_t* pLogData = g_state.LogPtr + 640 * yb + xb;


    for (int y = 0; y < hauteur; ++y) {
        for (int x = 0; x < largeur; ++x) {
            pOccupeData[x] += 1; // Incrémente le compteur d'occupation
            pCouleurData[x] = pLogData[x]; // Copie la couleur
        }

        // Mise à jour des pointeurs pour la ligne suivante
        pOccupeData += 256;
        pCouleurData += 256;
        pLogData += 640;
    }
}


int CalculeEfficacite() {
    uint8_t* pOccupeData = g_state.OccupePtr;
    int compteurEfficacite = 0;

    for (int y = 0; y < 255; ++y) {
        for (int x = 0; x < 255; ++x) {
            if (pOccupeData[x] != 0) {
                compteurEfficacite++; // Incrémente le compteur si la case est occupée
            }
        }
        pOccupeData += 256; // Passe à la ligne suivante
    }

    return compteurEfficacite; // Retourne le compteur d'efficacité
}


void InitVRamMap() {
    // Réinitialiser _PVRamOccup
    memset(g_state.OccupePtr, 0, 256 * 256 * sizeof(BYTE));

    // Réinitialiser _PVRamCoul
    memset(g_state.CouleurPtr, 0, 256 * 256 * sizeof(BYTE));

    // Réinitialiser la dernière ligne et la dernière colonne
    for (int i = 0; i < 256; ++i) {
        g_state.OccupePtr[255 + i * 256] = 255; // Dernière colonne
        g_state.CouleurPtr[255 + i * 256] = 255;

        g_state.OccupePtr[i + 256 * 255] = 255; // Dernière ligne
        g_state.CouleurPtr[i + 256 * 255] = 255;
    }
}

ScanData* GetScanData()
{
	return &g_state;
}



