//Gfx converted using Mollusk's PAGfx Converter

//This file contains all the .h, for easier inclusion in a project

#ifndef ALL_GFX_H
#define ALL_GFX_H

#ifndef PAGfx_struct
    typedef struct{
    void *Map;
    int MapSize;
    void *Tiles;
    int TileSize;
    void *Palette;
    int *Info;
} PAGfx_struct;
#endif


// Sprite files : 
extern const unsigned char plane_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : plane_image_Pal
extern const unsigned char black_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass0_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass2a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass4a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass6a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass8a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass10a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass12a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass14a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass16a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass18a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass18b_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass20a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass20b_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass22a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass22b_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass24a_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal
extern const unsigned char grass24b_image_Sprite[512] __attribute__ ((aligned (4))) ;  // Pal : grass_Pal

// Background files : 
extern const int background_image_Info[3]; // BgMode, Width, Height
extern const unsigned short background_image_Map[768] __attribute__ ((aligned (4))) ;  // Pal : background_image_Pal
extern const unsigned char background_image_Tiles[49152] __attribute__ ((aligned (4))) ;  // Pal : background_image_Pal
extern PAGfx_struct background_image; // background pointer

// Palette files : 
extern const unsigned short plane_image_Pal[138] __attribute__ ((aligned (4))) ;
extern const unsigned short background_image_Pal[66] __attribute__ ((aligned (4))) ;
extern const unsigned short grass_image_Pal[80] __attribute__ ((aligned (4))) ;

#endif

