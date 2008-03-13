//Gfx converted using Mollusk's PAGfx Converter

//This file contains all the .c, for easier inclusion in a project

#ifdef __cplusplus
extern "C" {
#endif

#include "all_gfx.h"

// Background files : 
#include "background_image.c"

// Sprite files : 
#include "plane_image.c"
#include "black_image.c"
#include "grass0_image.c"
#include "grass2a_image.c"
#include "grass4a_image.c"
#include "grass6a_image.c"
#include "grass8a_image.c"
#include "grass10a_image.c"
#include "grass12a_image.c"
#include "grass14a_image.c"
#include "grass16a_image.c"
#include "grass18a_image.c"
#include "grass18b_image.c"
#include "grass20a_image.c"
#include "grass20b_image.c"
#include "grass22a_image.c"
#include "grass22b_image.c"
#include "grass24a_image.c"
#include "grass24b_image.c"
#include "runway_image.c"
#include "runway_tower_image.c"
#include "spitfire_image.c"
#include "plane_piece_particles_image.c"
#include "tower_allies_image.c"
#include "tower_german_image.c"
#include "tree1_image.c"
#include "tree2_image.c"
#include "tree3_image.c"
#include "tree4_image.c"
#include "tree_destroyed_image.c"
#include "tower_destroyed_image.c"
#include "explosion_image.c"
#include "muzzle_image.c"
#include "smoke_image.c"

// Palette files : 
#include "plane_image.pal.c"
#include "background_image.pal.c"
#include "grass_image.pal.c"
#include "runway_image.pal.c"
#include "spitfire_image.pal.c"
#include "particles_image.pal.c"
#include "landscape_allobjects.pal.c"
#include "sfx_allobjects.pal.c"

// Background Pointers :
PAGfx_struct background_image = {(void*)background_image_Map, 768, (void*)background_image_Tiles, 49152, (void*)background_image_Pal, (int*)background_image_Info };


#ifdef __cplusplus
}
#endif

