// [HACKATHON 3-9]
// TODO: Create scene_settings.h and scene_settings.c.
// No need to do anything for this part. We've already done it for
// you, so this 2 files is like the default scene template.
#include "scene_settings.h"
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
#include "utility.h"
#include "shared.h"

static void destroy();
static Button back, volume_bgm_up, volume_bgm_down, volume_effect_up, volume_effect_down;
static Button map_nthu, map_nnn;
extern ALLEGRO_SAMPLE_ID menuBGM;
static ALLEGRO_SAMPLE_ID test;
extern char* map_pos;
char mnthu[20] = "Assets/map_nthu.txt";
char mnnn[20] = "Assets/map_nnn.txt";
static enum mp {
	nthu,
	nnn
};
int mk = nthu;

static void init() {
	back = button_create(20, 20, 60, 60, "Assets/back.png", "Assets/back_touch.png");
	volume_bgm_up = button_create(700, 150, 60, 60, "Assets/sound_up.png", "Assets/sound_up_touch.png");
	volume_bgm_down = button_create(100, 150, 60, 60, "Assets/sound_down.png", "Assets/sound_down_touch.png");
	volume_effect_up = button_create(700, 300, 60, 60, "Assets/sound_up.png", "Assets/sound_up_touch.png");
	volume_effect_down = button_create(100, 300, 60, 60, "Assets/sound_down.png", "Assets/sound_down_touch.png");
	map_nthu = button_create(100, 420, 300, 250, "Assets/map_nthu_shot.png", "Assets/map_nthu_shot_touch.png");
	map_nnn = button_create(450, 420, 300, 250, "Assets/map_nnn_shot.png", "Assets/map_nnn_shot_touch.png");
	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);
}

static void draw(void ){
	al_clear_to_color(al_map_rgb(0, 45, 80));
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		400,
		120,
		ALLEGRO_ALIGN_CENTER,
		"BGM"
	);
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		400,
		270,
		ALLEGRO_ALIGN_CENTER,
		"EFFECT"
	);
	if (music_volume != 0)
		al_draw_textf(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			170,
			ALLEGRO_ALIGN_CENTER,
			"%g", music_volume * 100.0
		);
	else 
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			170,
			ALLEGRO_ALIGN_CENTER,
			"Mute"
		);
	if (effect_volume != 0)
		al_draw_textf(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			320,
			ALLEGRO_ALIGN_CENTER,
			"%g", effect_volume * 100.0
		);
	else
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			320,
			ALLEGRO_ALIGN_CENTER,
			"Mute"
		);
	if (mk == nthu) {
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			700,
			ALLEGRO_ALIGN_CENTER,
			"CURRENT MAP IS NTHU"
		);
	}
	else if (mk == nnn) {
		al_draw_text(
			menuFont,
			al_map_rgb(255, 255, 255),
			400,
			700,
			ALLEGRO_ALIGN_CENTER,
			"CURRENT MAP IS NNN"
		);
	}
	
	drawButton(volume_bgm_down);
	drawButton(volume_bgm_up);
	drawButton(volume_effect_down);
	drawButton(volume_effect_up);
	drawButton(back);
	drawButton(map_nnn);
	drawButton(map_nthu);
}
// The only function that is shared across files.
static void on_mouse_move(int a, int mouse_x, int mouse_y, int f) {
	volume_bgm_down.hovered = pnt_in_rect(mouse_x, mouse_y, volume_bgm_down.body);
	volume_bgm_up.hovered = pnt_in_rect(mouse_x, mouse_y, volume_bgm_up.body);
	volume_effect_down.hovered = pnt_in_rect(mouse_x, mouse_y, volume_effect_down.body);
	volume_effect_up.hovered = pnt_in_rect(mouse_x, mouse_y, volume_effect_up.body);
	back.hovered = pnt_in_rect(mouse_x, mouse_y, back.body);
	map_nnn.hovered = pnt_in_rect(mouse_x, mouse_y, map_nnn.body);
	map_nthu.hovered = pnt_in_rect(mouse_x, mouse_y, map_nthu.body);
}
static void on_mouse_down() {
	if (volume_bgm_down.hovered && music_volume > 0) {
		if (music_volume < 0.2f)
			music_volume = 0.0f;
		else
			music_volume -= 0.1f;
		stop_bgm(menuBGM);
		menuBGM = play_bgm(themeMusic, music_volume);
	}
	if (volume_bgm_up.hovered && music_volume < 1) {
		music_volume += 0.1f;
		stop_bgm(menuBGM);
		menuBGM = play_bgm(themeMusic, music_volume);
	}
	if (volume_effect_down.hovered && music_volume > 0) {
		if (effect_volume < 0.2f)
			effect_volume = 0.0f;
		else
			effect_volume -= 0.1f;
		stop_bgm(test, effect_volume);
		test = play_audio(PACMAN_DEATH_SOUND, effect_volume);
	}
	if (volume_effect_up.hovered && music_volume < 1) {
		effect_volume += 0.1f;
		stop_bgm(test, effect_volume);
		test = play_audio(PACMAN_DEATH_SOUND, effect_volume);
	}
	if (back.hovered) {
		stop_bgm(test);
		game_change_scene(scene_menu_create());
	}
	if (map_nnn.hovered) {
		mk = nnn;
		memset(map_pos, '\0', 50);
		for (int i = 0; i < strlen(mnnn); i++) {
			*(map_pos + i) = mnnn[i];
		}
	}
	if (map_nthu.hovered && strcmp(map_pos, mnthu)) {
		mk = nthu;
		memset(map_pos, '\0', 50);
		for (int i = 0; i < strlen(mnthu); i++) {
			*(map_pos + i) = mnthu[i];
		}
	}
}

static void destroy() {
	stop_bgm(menuBGM);
	stop_bgm(test);
	al_destroy_bitmap(volume_bgm_down.default_img);
	al_destroy_bitmap(volume_bgm_down.hovered_img);
	al_destroy_bitmap(volume_bgm_up.default_img);
	al_destroy_bitmap(volume_bgm_up.hovered_img);
	al_destroy_bitmap(volume_effect_down.default_img);
	al_destroy_bitmap(volume_effect_down.hovered_img);
	al_destroy_bitmap(volume_effect_up.default_img);
	al_destroy_bitmap(volume_effect_up.hovered_img);
	al_destroy_bitmap(map_nnn.default_img);
	al_destroy_bitmap(map_nnn.hovered_img);
	al_destroy_bitmap(map_nthu.default_img);
	al_destroy_bitmap(map_nthu.hovered_img);
	al_destroy_bitmap(back.default_img);
	al_destroy_bitmap(back.hovered_img);
	game_log("settings destroy");
}

Scene scene_settings_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Settings";
	scene.initialize = &init;
	scene.destroy = &destroy;
	scene.draw = &draw;
	scene.on_mouse_move = &on_mouse_move;
	scene.on_mouse_down = &on_mouse_down;
	game_log("Settings scene created");
	return scene;
}