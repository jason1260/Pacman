// [shared.c]
// you should define the shared variable declared in the header here.

#include "shared.h"
#include "utility.h"
#include "game.h"
// #include "scene_menu.h"

ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_SAMPLE* themeMusic = NULL;
ALLEGRO_SAMPLE* PACMAN_MOVESOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND = NULL;
ALLEGRO_SAMPLE* PACMAN_SUPER = NULL;
ALLEGRO_SAMPLE* PACMAN_EATGHOST = NULL;
ALLEGRO_SAMPLE* CONGRADULATION = NULL;
ALLEGRO_FONT* menuFont = NULL;
ALLEGRO_FONT* endFont = NULL;
int fontSize = 30;
float music_volume = 0.5;
float effect_volume = 0.5;
bool gameDone = false;

/*
	A way to accelerate is load assets once.
	And delete them at the end.
	This method does provide you better management of memory.
	
*/
void shared_init(void) {
	menuFont = load_font("Assets/Minecraft.ttf", fontSize);
	endFont = load_font("Assets/Minecraft.ttf", 72);
	themeMusic = load_audio("Assets/Music/original_theme.ogg");
	PACMAN_MOVESOUND = load_audio("Assets/Music/pacman-chomp.ogg");
	PACMAN_DEATH_SOUND = load_audio("Assets/Music/pacman_death.ogg");
	PACMAN_SUPER = load_audio("Assets/Music/pacman_super.ogg");
	PACMAN_EATGHOST = load_audio("Assets/Music/pacman_eatghost.ogg");
	CONGRADULATION = load_audio("Assets/Music/congradulation.ogg");
}

void shared_destroy(void) {
	al_destroy_font(menuFont);
	al_destroy_font(endFont);
	al_destroy_font(font_pirulen_24);
	al_destroy_font(font_pirulen_32);
	al_destroy_sample(themeMusic);
	al_destroy_sample(PACMAN_MOVESOUND);
	al_destroy_sample(PACMAN_DEATH_SOUND);
	al_destroy_sample(PACMAN_SUPER);
	al_destroy_sample(PACMAN_EATGHOST);
	al_destroy_sample(CONGRADULATION);
}