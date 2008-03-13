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
extern const unsigned char runway_image_Sprite[2048] __attribute__ ((aligned (4))) ;  // Pal : runway_image_Pal
extern const unsigned char runway_tower_image_Sprite[2048] __attribute__ ((aligned (4))) ;  // Pal : runway_image_Pal
extern const unsigned char spitfire_image_Sprite[13312] __attribute__ ((aligned (4))) ;  // Pal : spitfire_image_Pal
extern const unsigned char plane_piece_particles_image_Sprite[64] __attribute__ ((aligned (4))) ;  // Pal : particles_image_Pal
extern const unsigned char tower_allies_image_Sprite[2048] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tower_german_image_Sprite[2048] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tree1_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tree2_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tree3_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tree4_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tree_destroyed_image_Sprite[1024] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char tower_destroyed_image_Sprite[2048] __attribute__ ((aligned (4))) ;  // Pal : landscape_allobjects_Pal_Pal
extern const unsigned char explosion_image_Sprite[30720] __attribute__ ((aligned (4))) ;  // Pal : sfx_allobjects_Pal
extern const unsigned char muzzle_image_Sprite[128] __attribute__ ((aligned (4))) ;  // Pal : sfx_allobjects_Pal
extern const unsigned char smoke_image_Sprite[64] __attribute__ ((aligned (4))) ;  // Pal : sfx_allobjects_Pal

// Background files : 
extern const int background_image_Info[3]; // BgMode, Width, Height
extern const unsigned short background_image_Map[768] __attribute__ ((aligned (4))) ;  // Pal : background_image_Pal
extern const unsigned char background_image_Tiles[49152] __attribute__ ((aligned (4))) ;  // Pal : background_image_Pal
extern PAGfx_struct background_image; // background pointer

// Palette files : 
extern const unsigned short plane_image_Pal[138] __attribute__ ((aligned (4))) ;
extern const unsigned short background_image_Pal[66] __attribute__ ((aligned (4))) ;
extern const unsigned short grass_image_Pal[80] __attribute__ ((aligned (4))) ;
extern const unsigned short runway_image_Pal[93] __attribute__ ((aligned (4))) ;
extern const unsigned short spitfire_image_Pal[250] __attribute__ ((aligned (4))) ;
extern const unsigned short particles_image_Pal[2] __attribute__ ((aligned (4))) ;
extern const unsigned short landscape_allobjects_Pal[212] __attribute__ ((aligned (4)));
extern const unsigned short sfx_allobjects_Pal[182] __attribute__ ((aligned (4))) ;

#endif

