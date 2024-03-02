#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_native_dialog.h>
#include <math.h>
#include "scene_menu_object.h"
#include "scene_settings.h"
#include "scene_keyname.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "utility.h"
#include "shared.h"

static void destroy();
/* Internal Variables*/
static ALLEGRO_BITMAP* pacman = NULL;
static pacman_w, pacman_h;
char _name[50] = "";
char* name = _name;
int name_len = 0;
extern ALLEGRO_SAMPLE_ID menuBGM;

static void init() {
	//gameTitle = load_bitmap("Assets/title.png");
	//gameTitleW = al_get_bitmap_width(gameTitle);
	//gameTitleH = al_get_bitmap_height(gameTitle);
}
static void draw() {
	al_clear_to_color(al_map_rgb(0, 45, 80));
	/*al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h,
		gameTitleW * scale, gameTitleH * scale,
		0
	);*/
	al_draw_textf(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H / 2,
		ALLEGRO_ALIGN_CENTER,
		"YOUR NAME: %s",
		name
	);
}
static void destroy() {
	stop_bgm(menuBGM);
	/*al_destroy_bitmap(gameTitle);
	al_destroy_bitmap(btnSettings.default_img);
	al_destroy_bitmap(btnSettings.hovered_img);*/
	game_log("scene_menu destroy");
}
static void on_key_down(int keycode) {
	switch (keycode) {
	case ALLEGRO_KEY_ENTER:
		game_log("name = %s", name);
		game_change_scene(scene_main_create());
		break;
	case ALLEGRO_KEY_BACKSPACE:
		if (name_len > 0) name[--name_len] = ' ';
	default:
		break;
	}
	if (keycode >= 1 && keycode <= 26) {
		name[name_len++] = 64 + keycode;
	}
	if (keycode >= 27 && keycode <= 36) {
		name[name_len++] = 21 + keycode;
	}
}
Scene scene_keyname_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Menu";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	game_log("MKeyname scene created");
	return scene;
}