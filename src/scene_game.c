#include <allegro5/allegro_image.h>
#include <allegro5/allegro_primitives.h>
#include <string.h>
#include "game.h"
#include "shared.h"
#include "utility.h"
#include "scene_game.h"
#include "scene_menu.h"
#include "scene_score.h"
#include "pacman_obj.h"
#include "ghost.h"
#include "map.h"


// [HACKATHON 2-0]
// Just modify the GHOST_NUM to 1
#define GHOST_NUM 4
/* global variables*/
extern const uint32_t GAME_TICK_CD;
extern uint32_t GAME_TICK;
extern ALLEGRO_TIMER* game_tick_timer;
extern ALLEGRO_SAMPLE* CONGRADULATION;
extern ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
extern ALLEGRO_SAMPLE_ID PACMAN_SUPER_ID;
extern const int cage_grid_x, cage_grid_y;
int game_main_Score = 0;
int pacman_life = 3;
bool game_over = false;
bool game_pass = false;
bool effect_flag = false;
ALLEGRO_TIMER* end_tick_timer;
ALLEGRO_TIMER* power_up_timer;
ALLEGRO_SAMPLE_ID CONGRADULATION_ID;
extern ALLEGRO_SAMPLE_ID PACMAN_SUPER_ID;
static const int power_up_duration = 10;
static Pacman* pman;
static Map* basic_map;
static Ghost** ghosts;
char _map_pos[50] = "Assets/map_nthu.txt";
char* map_pos = _map_pos;
bool debug_mode = false;
bool cheat_mode = false;

/* Declare static function prototypes */
static void init(void);
static void step(void);
static void checkItem(void);
static void status_update(void);
static void update(void);
static void draw(void);
static void printinfo(void);
static void destroy(void);
static void on_key_down(int key_code);
static void on_mouse_down(void);
static void render_init_screen(void);
static void draw_hitboxes(void);

static void init(void) {
	game_over = false;
	game_main_Score = 0;
	pacman_life = 3;
	// create map
	basic_map = create_map(map_pos);
	// [TODO]
	// Create map from .txt file and design your own map !!
	// basic_map = create_map("Assets/map_nthu.txt");
	if (!basic_map) {
		game_abort("error on creating map");
	}	
	// create pacman
	pman = pacman_create();
	if (!pman) {
		game_abort("error on creating pacamn\n");
	}
	
	// allocate ghost memory
	// [HACKATHON 2-1]
	// TODO: Allocate dynamic memory for ghosts array.
	
	ghosts = (Ghost**)malloc(sizeof(Ghost*) * GHOST_NUM);
	if (!ghosts) game_abort("ghost create failed");
	
	else {
		// [HACKATHON 2-2]
		// TODO: create a ghost.
		// Try to look the definition of ghost_create and figure out what should be placed here.
		for (int i = 0; i < GHOST_NUM; i++) {
			game_log("creating ghost %d\n", i);
			ghosts[i] = ghost_create(i);  
			if (!ghosts[i])
				game_abort("error creating ghost\n");
			//printf("ghost[%d] = %d\n", i, ghosts[i]);
		}
	}
	GAME_TICK = 0;

	render_init_screen();
	power_up_timer = al_create_timer(1.0f); // 1 tick / sec
	end_tick_timer = al_create_timer(1.0f);
	if (!power_up_timer)
		game_abort("Error on create timer\n");
	return ;
}

static void step(void) {
	if (pman->objData.moveCD > 0)
		pman->objData.moveCD -= pman->speed;
	for (int i = 0; i < GHOST_NUM; i++) {
		// important for movement
		if (ghosts[i]->objData.moveCD > 0)
			ghosts[i]->objData.moveCD -= ghosts[i]->speed;
	}
}
static void checkItem(void) {
	int Grid_x = pman->objData.Coord.x, Grid_y = pman->objData.Coord.y;
	if (Grid_y >= basic_map->row_num - 1 || Grid_y <= 0 || Grid_x >= basic_map->col_num - 1 || Grid_x <= 0)
		return;
	// [HACKATHON 1-3]
	// TODO: check which item you are going to eat and use `pacman_eatItem` to deal with it.
	if (basic_map->beansNum <= 0) {
		game_pass = true;
		return;
	}
	switch (basic_map->map[Grid_y][Grid_x]) {
		case '.':
			pacman_eatItem(pman, '.');
			game_main_Score += 10;
			basic_map->beansNum -= 1;
			break;
		case 'P':
			pacman_eatItem(pman, 'P');
			pman->powerUp = true;
			game_main_Score += 50;
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], true);
			}
			al_start_timer(power_up_timer);
			al_set_timer_count(power_up_timer, 0);
			break;
		case 'A':
			pacman_eatItem(pman, 'P');
			basic_map->map[10][11] = ' ';
			basic_map->map[10][12] = ' ';
			basic_map->map[10][13] = ' ';
			pman->powerUp = true;
			game_main_Score += 50;
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], true);
			}
			al_start_timer(power_up_timer);
			al_set_timer_count(power_up_timer, 5);
			break;
		case '!':
			pacman_eatItem(pman, 'P');
			basic_map->map[6][1] = ' ';
			pman->powerUp = true;
			game_main_Score += 50;
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], true);
			}
			al_start_timer(power_up_timer);
			al_set_timer_count(power_up_timer, 5);
			break;
		default:
			break;
	}
	// [HACKATHON 1-4]
	// erase the item you eat from map
	// becareful no erasing the wall block.
	if (basic_map->map[Grid_y][Grid_x] == '.' || basic_map->map[Grid_y][Grid_x] == 'P'
		|| basic_map->map[Grid_y][Grid_x] == 'A' || basic_map->map[Grid_y][Grid_x] == '!')
		basic_map->map[Grid_y][Grid_x] = ' ';
	
}
static void status_update(void) {
	for (int i = 0; i < GHOST_NUM; i++) {
		if (ghosts[i]->status == GO_IN)
			continue;
		// [TODO]
		// use `getDrawArea(..., GAME_TICK_CD)` and `RecAreaOverlap(..., GAME_TICK_CD)` functions to detect
		// if pacman and ghosts collide with each other.
		// And perform corresponding operations.
		// [NOTE]
		// You should have some branch here if you want to implement power bean mode.
		// Uncomment Following Code
		if(!cheat_mode && 
			RecAreaOverlap(getDrawArea(pman->objData, GAME_TICK_CD), getDrawArea(ghosts[i]->objData, GAME_TICK_CD)) &&
			!(ghosts[i]->status == FLEE)) {
			game_log("collide with ghost\n");
			al_rest(1.0);
			pacman_die();
			game_over = true;
			if (pacman_life > 0) {
				pacman_life -= 1;
			}
			break;
		}
		else if (!cheat_mode &&
			RecAreaOverlap(getDrawArea(pman->objData, GAME_TICK_CD), getDrawArea(ghosts[i]->objData, GAME_TICK_CD))) {
			ghost_collided(ghosts[i]);
		}
		else if (al_get_timer_count(power_up_timer) >= power_up_duration) {
			al_stop_timer(power_up_timer);
			al_set_timer_count(power_up_timer, 0);
			stop_bgm(PACMAN_SUPER_ID);
			for (int i = 0; i < GHOST_NUM; i++) {
				ghost_toggle_FLEE(ghosts[i], false);
			}
			pman->powerUp = false;
		}
	}
}

static void update(void) {

	if (game_over) {
		al_start_timer(pman->death_anim_counter);
		if (al_get_timer_count(pman->death_anim_counter) >= 190 && pacman_life > 0) {
			game_over = false;
			pman->objData.Coord.x = 24;
			pman->objData.Coord.y = 24;
			pman->objData.preMove = NONE;
			pman->objData.nextTryMove = NONE;
			pman->objData.moveCD = 0;
			pman->objData.facing = 1;
			for (int i = 0; i < GHOST_NUM; i++) {
				switch (ghosts[i]->typeFlag) {
				case Blinky:
					ghosts[i]->objData.Coord.x = cage_grid_x - 1;
					ghosts[i]->objData.Coord.y = cage_grid_y;
					break;
				case Pinky:
					ghosts[i]->objData.Coord.x = cage_grid_x + 1;
					ghosts[i]->objData.Coord.y = cage_grid_y;
					break;
				case Inky:
					ghosts[i]->objData.Coord.x = cage_grid_x - 1;
					ghosts[i]->objData.Coord.y = cage_grid_y + 1;
					break;
				case Clyde:
					ghosts[i]->objData.Coord.x = cage_grid_x + 1;
					ghosts[i]->objData.Coord.y = cage_grid_y + 1;
					break;
				default:
					ghosts[i]->objData.Coord.x = cage_grid_x - 1;
					ghosts[i]->objData.Coord.y = cage_grid_y;
					break;
				}
				ghosts[i]->status = BLOCKED;
				ghosts[i]->objData.nextTryMove = NONE;
				ghosts[i]->objData.moveCD = 0;
				ghosts[i]->objData.facing = UP;
			}
			al_stop_timer(pman->death_anim_counter);
			al_set_timer_count(pman->death_anim_counter, 0);
			game_restart();
		}
		if (al_get_timer_count(pman->death_anim_counter) >= 448) {
			game_change_scene(scene_score_create());
		}
		return;
	}
	if (game_pass) {
		if (!effect_flag) {
			stop_bgm(PACMAN_MOVESOUND_ID);
			stop_bgm(PACMAN_SUPER_ID);
			CONGRADULATION_ID = play_audio(CONGRADULATION, effect_volume);
			effect_flag = true;
		}
		al_start_timer(end_tick_timer);
		if (al_get_timer_count(end_tick_timer) >= 7) {
			stop_bgm(CONGRADULATION_ID);
			al_stop_timer(end_tick_timer);
			al_set_timer_count(end_tick_timer, 0);			
			game_pass = false;
			game_change_scene(scene_score_create());
		}
		return;
	}

	step();
	checkItem();
	status_update();
	pacman_move(pman, basic_map);
	for (int i = 0; i < GHOST_NUM; i++) 
		ghosts[i]->move_script(ghosts[i], basic_map, pman);
}

static void draw(void) {
	al_clear_to_color(al_map_rgb(0, 45, 80));
	//	[TODO]
	//	Draw scoreboard, something your may need is sprinf();
	al_draw_textf(menuFont, al_map_rgb(255, 255, 255), 
		40, 700, ALLEGRO_ALIGN_LEFT, "score: %d", game_main_Score);
	al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
		700, 700, ALLEGRO_ALIGN_LEFT, "life: %d", pacman_life);
	
	draw_map(basic_map);

	pacman_draw(pman);
	if (game_over || game_pass)
		return;
	// no drawing below when game over
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}
	//debugging mode
	if (debug_mode) {
		draw_hitboxes();
	}
}

static void draw_hitboxes(void) {
	RecArea pmanHB = getDrawArea(pman->objData, GAME_TICK_CD);
	al_draw_rectangle(
		pmanHB.x, pmanHB.y,
		pmanHB.x + pmanHB.w, pmanHB.y + pmanHB.h,
		al_map_rgb_f(1.0, 0.0, 0.0), 2
	);

	for (int i = 0; i < GHOST_NUM; i++) {
		RecArea ghostHB = getDrawArea(ghosts[i]->objData, GAME_TICK_CD);
		al_draw_rectangle(
			ghostHB.x, ghostHB.y,
			ghostHB.x + ghostHB.w, ghostHB.y + ghostHB.h,
			al_map_rgb_f(1.0, 0.0, 0.0), 2
		);
	}

}

static void printinfo(void) {
	game_log("pacman:\n");
	game_log("coord: %d, %d\n", pman->objData.Coord.x, pman->objData.Coord.y);
	game_log("PreMove: %d\n", pman->objData.preMove);
	game_log("NextTryMove: %d\n", pman->objData.nextTryMove);
	game_log("Speed: %f\n", pman->speed);
}


static void destroy(void) {
	stop_bgm(PACMAN_SUPER_ID);
	pacman_destroy(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_destroy(ghosts[i]);
	}
	al_destroy_timer(end_tick_timer);
	al_destroy_timer(power_up_timer);
	free(ghosts);
	delete_map(basic_map);
	game_log("scene_game destroy");
}

static void on_key_down(int key_code) {
	int x = pman->objData.Coord.x, y = pman->objData.Coord.y;
	switch (key_code)
	{
		case ALLEGRO_KEY_W:
			pacman_NextMove(pman, UP);
			break;
		case ALLEGRO_KEY_A:
			pacman_NextMove(pman, LEFT);
			break;
		case ALLEGRO_KEY_S:
			pacman_NextMove(pman, DOWN);
			break;
		case ALLEGRO_KEY_D:
			pacman_NextMove(pman, RIGHT);
			break;
		case ALLEGRO_KEY_C:
			cheat_mode = !cheat_mode;
			if (cheat_mode)
				printf("cheat mode on\n");
			else 
				printf("cheat mode off\n");
			break;
		case ALLEGRO_KEY_G:
			debug_mode = !debug_mode;
			break;		
		default:
			break;
	}

}

static void on_mouse_down(void) {
	// nothing here
}

static void render_init_screen(void) {
	al_clear_to_color(al_map_rgb(0, 45, 80));

	draw_map(basic_map);
	pacman_draw(pman);
	for (int i = 0; i < GHOST_NUM; i++) {
		ghost_draw(ghosts[i]);
	}

	al_draw_text(
		menuFont,
		al_map_rgb(255, 255, 0),
		400, 400,
		ALLEGRO_ALIGN_CENTER,
		"READY!"
	);

	al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
		40, 700, ALLEGRO_ALIGN_LEFT, "score: %d", game_main_Score);
	al_draw_textf(menuFont, al_map_rgb(255, 255, 255),
		700, 700, ALLEGRO_ALIGN_LEFT, "life: %d", pacman_life);

	al_flip_display();
	al_rest(2.0);

}
// Functions without 'static', 'extern' prefixes is just a normal
// function, they can be accessed by other files using 'extern'.
// Define your normal function prototypes below.

// The only function that is shared across files.
Scene scene_main_create(void) {
	Scene scene;
	memset(&scene, 0, sizeof(Scene));
	scene.name = "Start";
	scene.initialize = &init;
	scene.update = &update;
	scene.draw = &draw;
	scene.destroy = &destroy;
	scene.on_key_down = &on_key_down;
	scene.on_mouse_down = &on_mouse_down;
	// TODO: Register more event callback functions such as keyboard, mouse, ...
	game_log("Start scene created");
	return scene;
}
