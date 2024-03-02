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
/* Internal Variables*/
extern ALLEGRO_SAMPLE_ID menuBGM;
static FILE* SCORE_LIST = NULL;
extern int game_main_Score;
extern char *name;
static int num, temp[10];
static char name_set[10][50];
static int record, count, index;

static void init() {
	record = -1;
	count = 9;
	index = 0;
	stop_bgm(menuBGM);
	menuBGM = play_bgm(themeMusic, music_volume);
	SCORE_LIST = fopen("Assets/score.txt", "r+");
	if (!SCORE_LIST) game_abort("scorelist open failed!");
	memset(temp, -1, 10);
	for (int i = 0; i < 10; i++) {
		memset(name_set[i], 0, 50);
	}
	for (int i = 0; i < 10; i++) {
		if (fscanf(SCORE_LIST, "%d: %s %d\n", &num, name_set[i], &temp[i]) != 3 || temp[i] <= 10) {
			count = i;
			if (record == -1)
				record = count;
			break;
		}
		printf("%d %s %d\n", num, name_set[i], temp[i]);
		printf("temp = %d, score = %d\n", temp[i], game_main_Score);
		if (temp[i] < game_main_Score && record == -1)
			record = i;
	}
	fclose(SCORE_LIST);
	printf("record = %d\n", record);
	SCORE_LIST = fopen("Assets/score.txt", "w");
	if (!SCORE_LIST) game_abort("scorelist open failed");
	for (int i = 0; i < 10; i++) {
		if (record == i) fprintf(SCORE_LIST, "%d: %s %d\n", i + 1, name, game_main_Score);
		else if (i <= count) {
			fprintf(SCORE_LIST, "%d: %s %d\n", i + 1, name_set[index - 1], temp[index++]);
		}
		else fprintf(SCORE_LIST, "%d: \n", i + 1);
	}
	fclose(SCORE_LIST);
}
static void draw() {
	index = 0;
	al_clear_to_color(al_map_rgb(0, 45, 80));
	for (int i = 0; i < 10; i++) {
		if (record == i) {
			al_draw_textf(
				menuFont,
				al_map_rgb(255, 255, 255),
				200,
				100 + 50 * i,
				ALLEGRO_ALIGN_LEFT,
				"%d: %s %d",
				i + 1, name, game_main_Score
			);
		}
		else if (i <= count) {
			al_draw_textf(
				menuFont,
				al_map_rgb(255, 255, 255),
				200,
				100 + 50 * i,
				ALLEGRO_ALIGN_LEFT,
				"%d: %s %d",
				i + 1, name_set[index - 1], temp[index++]
			);
		}
		else {
			al_draw_textf(
				menuFont,
				al_map_rgb(255, 255, 255),
				200,
				100 + 50 * i,
				ALLEGRO_ALIGN_LEFT,
				"%d: ",
				i + 1
			);
		}
	}
	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 255),
		SCREEN_W / 2,
		SCREEN_H - 150,
		ALLEGRO_ALIGN_CENTER,
		"PRESS \"ENTER\" TO CONTINUE..."
	);
}
static void destroy() {
	stop_bgm(menuBGM);
	//fclose(SCORE_LIST);
	game_log("scene_menu destroy");
}
static void on_key_down(int keycode) {
	switch (keycode) {
	case ALLEGRO_KEY_ENTER:
		game_change_scene(scene_menu_create());
		break;
	default:
		break;
	}
}
Scene scene_score_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Score";
	scene.initialize = &init;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	game_log("Score scene created");
	return scene;
}