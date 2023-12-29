#pragma once
// Nom du fichier : ScanneMap.h
// Conversion en C++ du code ASM (NP) : il est possible qu'il y ai des bugs comme j'ai fait ca vite ...  

#ifndef SCANNEMAP_H
#define SCANNEMAP_H

#include <cstdint>
#include <stdint.h>

struct ScanData {
    uint8_t* OccupePtr;
    uint8_t* CouleurPtr;
    uint8_t* LogPtr;
    uint16_t* PVRamNum;
    int max_x, max_y, extreme, recouvre;
    int xp, yp;
    LONG BestVoisin, BestX, BestY;
    LONG BestRecouvre, BestPlace;
};

// Déclarations des fonctions
void ScanneMapOptimum( int largeur, int hauteur, int xb, int yb);
void ScanneMapHorizontal( int largeur, int hauteur, int xb, int yb);
void ScanneMapVertical( int largeur, int hauteur, int xb, int yb);
void ScanneMapRecouvre( int largeur, int hauteur, int xb, int yb);
void ScanneMapOptimum16( int largeur, int hauteur, int xb, int yb);
void ScanneMapHorizontal16( int largeur, int hauteur, int xb, int yb);
void ScanneMapVertical16( int largeur, int hauteur, int xb, int yb);
void ScanneMapRecouvre16( int largeur, int hauteur, int xb, int yb);
void AfficheBlocVRam( int largeur, int hauteur, int xb, int yb);
int CalculeEfficacite();
void InitVRamMap();

ScanData*GetScanData();


#endif // SCANNEMAP_H

