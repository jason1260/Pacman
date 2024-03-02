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
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_keyname.h"
#include "utility.h"
#include "shared.h"

static void destroy();
/* Internal Variables*/
static ALLEGRO_BITMAP* gameTitle = NULL;
ALLEGRO_SAMPLE_ID menuBGM;
static int gameTitleW ;
static int gameTitleH ;
static Button btnSettings;

static void init() {
	btnSettings = button_create(730, 20, 50, 50, "Assets/settings.png", "Assets/settings2.png");
	gameTitle = load_bitmap("Assets/title.png");
	gameTitleW = al_get_bitmap_width(gameTitle);
	gameTitleH = al_get_bitmap_height(gameTitle);
	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);
}
static void draw() {
	al_clear_to_color(al_map_rgb(0, 45, 80));
	const float scale = 0.7;
	const float offset_w = (SCREEN_W >> 1) - 0.5 * scale * gameTitleW;
	const float offset_h = (SCREEN_H >> 1) - 0.5 * scale * gameTitleH;
	al_draw_scaled_bitmap(
		gameTitle,
		0, 0,
		gameTitleW, gameTitleH,
		offset_w, offset_h,
		gameTitleW * scale, gameTitleH * scale,
		0
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W/2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\""
	);
	drawButton(btnSettings);
}
static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	btnSettings.hovered = pnt_in_rect(mouse_x, mouse_y, btnSettings.body);
}
static void on_mouse_down() {
	if (btnSettings.hovered)
		game_change_scene(scene_settings_create());
}
static void destroy() {
	//stop_bgm(menuBGM);
	al_destroy_bitmap(gameTitle);
	al_destroy_bitmap(btnSettings.default_img);
	al_destroy_bitmap(btnSettings.hovered_img);
	game_log("scene_menu destroy");
}
static void on_key_down(int keycode) {
	switch (keycode) {
		case ALLEGRO_KEY_ENTER:			
			game_change_scene(scene_keyname_create());
			break;
		default:
			break;
	}
}
Scene scene_menu_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Menu";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	game_log("Menu scene created");
	return scene;
}