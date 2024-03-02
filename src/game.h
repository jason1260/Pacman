// [game.h]
#ifndef GAME_H
#define GAME_H
#define _CRT_SECURE_NO_DEPRECATE
#include <allegro5/allegro.h>
#include <allegro5/allegro_font.h>	
typedef struct Ghost Ghost;
typedef struct Map Map;
typedef struct Pacman Pacman;
#define LOG_ENABLED
; // Question
// Types of function pointers.
static ALLEGRO_EVENT_QUEUE* game_event_queue;
typedef void(*func_ptr)(void);
typedef void(*script_func_ptr)(Ghost* ghost, Map*M, Pacman* pacman);
typedef void(*func_ptr_keyboard)(int keycode);
typedef void(*func_ptr_mouse)(int btn, int x, int y, int dz);
// TODO: More function pointer typedefs for other events.
typedef struct {
	char* name;
	func_ptr initialize;
	func_ptr update;
	func_ptr draw;
	func_ptr destroy;
	func_ptr_keyboard on_key_down;
	func_ptr_keyboard on_key_up;
	func_ptr_mouse on_mouse_down;
	func_ptr_mouse on_mouse_move;
	func_ptr_mouse on_mouse_up;
	func_ptr_mouse on_mouse_scroll;
	// TODO: More event callbacks such as timer tick, video finished, ...
} Scene;
extern const int FPS;
extern const int SCREEN_W;
extern const int SCREEN_H;
extern const int RESERVE_SAMPLES;
extern Scene active_scene;
extern bool key_state[ALLEGRO_KEY_MAX];
extern bool* mouse_state;
extern int mouse_x, mouse_y;
// TODO: More variables to store input states such as joysticks, ...
void game_create(void);
void game_change_scene(Scene next_scene);
void game_restart();
void game_abort(const char* format, ...);
void game_log(const char* format, ...);
extern void shared_init(void);
extern void shared_destroy(void);
#endif
