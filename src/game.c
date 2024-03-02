// [game.c]
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include "game.h"
#include "scene_game.h" 
#include "scene_menu.h"
const int FPS = 60;
const int SCREEN_W = 800;
const int SCREEN_H = 800;
const int RESERVE_SAMPLES = 4;
Scene active_scene;
bool key_state[ALLEGRO_KEY_MAX];
bool* mouse_state;
int mouse_x, mouse_y;
uint32_t GAME_TICK = 0;
const uint32_t GAME_TICK_CD = 64;
ALLEGRO_TIMER* game_tick_timer;
extern bool gameDone;
static ALLEGRO_DISPLAY* game_display;
static ALLEGRO_TIMER* game_update_timer;
//static const char* game_title = "I2P(I)_2021 Final Project Template";
static const char* game_title = "Hackaman";
static void allegro5_init(void);
static void game_start_event_loop(void);
static void game_update(void);
static void game_draw(void);
static void game_destroy(void);
static void game_vlog(const char* format, va_list arg);

void game_create() {
	srand(time(NULL));
	allegro5_init();
	game_log("Allegro5 initialized");
	game_log("Game begin");
	shared_init();
	game_log("Game initialized");
	game_change_scene(scene_menu_create());
	game_draw();
	game_log("Game start event loop");
	game_start_event_loop();
	game_log("Game end");
	if (active_scene.destroy)
		(*active_scene.destroy)();
	game_destroy();
}

static void allegro5_init(void) {
	if (!al_init())
		game_abort("failed to initialize allegro");
	if (!al_init_primitives_addon())
		game_abort("failed to initialize primitives add-on");
	if (!al_init_font_addon())
		game_abort("failed to initialize font add-on");
	if (!al_init_ttf_addon())
		game_abort("failed to initialize ttf add-on");
	if (!al_init_image_addon())
		game_abort("failed to initialize image add-on");
	if (!al_install_audio())
		game_abort("failed to initialize audio add-on");
	if (!al_init_acodec_addon())
		game_abort("failed to initialize audio codec add-on");
	if (!al_reserve_samples(RESERVE_SAMPLES))
		game_abort("failed to reserve samples");
	if (!al_install_keyboard())
		game_abort("failed to install keyboard");
	if (!al_install_mouse())
		game_abort("failed to install mouse");
	// TODO: Initialize other addons such as video, ...
	game_display = al_create_display(SCREEN_W, SCREEN_H);
	if (!game_display)
		game_abort("failed to create display");
	al_set_window_title(game_display, game_title);
	game_update_timer = al_create_timer(1.0f / FPS);
	if (!game_update_timer)
		game_abort("failed to create timer");
	game_tick_timer = al_create_timer(1.0f / GAME_TICK_CD / 2);
	if (!game_tick_timer)
		game_abort("faild to create tick timer");
	game_event_queue = al_create_event_queue();
	if (!game_event_queue)
		game_abort("failed to create event queue");
	const unsigned m_buttons = al_get_mouse_num_buttons();
	game_log("There are total %u supported mouse buttons", m_buttons);
	mouse_state = malloc((m_buttons + 1) * sizeof(bool));
	if (mouse_state != NULL) {
		memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));
	}
	al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
	al_register_event_source(game_event_queue, al_get_timer_event_source(game_tick_timer));
	al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
	al_register_event_source(game_event_queue, al_get_keyboard_event_source());
	al_register_event_source(game_event_queue, al_get_mouse_event_source());
	// TODO: Register other event sources such as timer, video, ...
	al_start_timer(game_update_timer);
}

static void game_start_event_loop(void) {
	ALLEGRO_EVENT event;
	int redraws = 0;
	srand(time(NULL));
	while (!gameDone) {
		al_wait_for_event(game_event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			game_log("Window close button clicked");
			gameDone = true;
		}
		else if (event.type == ALLEGRO_EVENT_TIMER) {
			if (event.timer.source == game_update_timer) redraws++;
			else if(event.timer.source == game_tick_timer){
				GAME_TICK++;
				if (GAME_TICK >= GAME_TICK_CD) {
					GAME_TICK = 0;
				}
				game_update();
			}
		}
		else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			game_log("Key with keycode %d down", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = true;
			if (event.keyboard.keycode == ALLEGRO_KEY_ESCAPE && active_scene.name == "Menu") {
				game_log("Escape clicked");
				gameDone = true;
				continue;
			}
			if (active_scene.on_key_down)
				(*active_scene.on_key_down)(event.keyboard.keycode);
		}
		else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			//game_log("Key with keycode %d up", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = false;
			if (active_scene.on_key_up)
				(*active_scene.on_key_up)(event.keyboard.keycode);
		}
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			//game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = true;
			if (active_scene.on_mouse_down)
				(*active_scene.on_mouse_down)(event.mouse.button, event.mouse.x, event.mouse.y, 0);
		}
		else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			//game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = false;
			if (active_scene.on_mouse_up)
				(*active_scene.on_mouse_up)(event.mouse.button, event.mouse.x, event.mouse.y, 0);
		}
		else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.mouse.dx != 0 || event.mouse.dy != 0) {
				//game_log("Mouse move_script to (%d, %d)", event.mouse.x, event.mouse.y);
				mouse_x = event.mouse.x;
				mouse_y = event.mouse.y;
				if (active_scene.on_mouse_move)
					(*active_scene.on_mouse_move)(0, event.mouse.x, event.mouse.y, 0);
			}
			else if (event.mouse.dz != 0) {
				//game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
				if (active_scene.on_mouse_scroll)
					(*active_scene.on_mouse_scroll)(0, event.mouse.x, event.mouse.y, event.mouse.dz);
			}
		}
		// TODO: Process more events and call callbacks by adding more
		// entries inside Scene.
		if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
			// if (redraws > 1)
			// 	game_log("%d frame(s) dropped", redraws - 1);
			game_draw();
			redraws = 0;
		}
	}
}

static void game_update(void) {
	if (active_scene.update)
		(*active_scene.update)();
}

static void game_draw(void) {
	al_clear_to_color(al_map_rgb(0, 45, 80));
	if (active_scene.draw)
		(*active_scene.draw)();
	al_flip_display();
}

static void game_destroy(void) {
	shared_destroy();
	game_log("shared destroy!");
	al_destroy_timer(game_tick_timer);
	game_log("game tick timer destroy!");
	al_destroy_timer(game_update_timer);
	game_log("game update timer destroy!");
	al_destroy_event_queue(game_event_queue);
	game_log("game event que destroy!");
	al_destroy_display(game_display);
	game_log("game display destroy!");
	free(mouse_state);
	game_log("mouse destroy!");
}

void game_change_scene(Scene next_scene) {
	game_log("Change scene from %s to %s",
		active_scene.name ? active_scene.name : "(unnamed)",
		next_scene.name ? next_scene.name : "(unnamed)");
	if (active_scene.destroy)
		(*active_scene.destroy)();
	if(game_tick_timer == NULL)
		game_abort("NULL game tick timer!!!");
	al_stop_timer(game_tick_timer);
	active_scene = next_scene;
	if (active_scene.initialize)
		active_scene.initialize();
	if (game_tick_timer == NULL) {
		game_abort("NULL game tick timer!!!");
	}
	al_set_timer_count(game_tick_timer, 0);
	al_start_timer(game_tick_timer);
}

void game_restart() {
	if (game_tick_timer == NULL)
		game_abort("NULL game tick timer!!!");
	al_stop_timer(game_tick_timer);
	al_set_timer_count(game_tick_timer, 0);
	al_start_timer(game_tick_timer);
}

// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
	fprintf(stderr, "error occured, exiting after 2 secs");
	// Wait 2 secs before exiting.
	al_rest(2);
	// Force exit program.
	exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
#endif
}

static void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
	static bool clear_file = true;
	vprintf(format, arg);
	printf("\n");
	// Write log to file for later debugging.
	FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
	if (pFile) {
		vfprintf(pFile, format, arg);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	clear_file = false;
#endif
}

