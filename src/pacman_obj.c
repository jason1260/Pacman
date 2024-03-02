#include <allegro5/allegro_primitives.h>
#include "pacman_obj.h"
#include "map.h"
#include "shared.h"
/* Static variables */
static const int start_grid_x = 25, start_grid_y = 25;		// where to put pacman at the beginning
static const int fix_draw_pixel_offset_x = -3, fix_draw_pixel_offset_y = -3;  // draw offset 
static const int draw_region = 30;							// pacman bitmap draw region
ALLEGRO_SAMPLE_ID PACMAN_MOVESOUND_ID;
ALLEGRO_SAMPLE_ID PACMAN_SUPER_ID;
static const int basic_speed = 2;

/* Shared variables */
extern ALLEGRO_SAMPLE* PACMAN_SUPER;
extern ALLEGRO_SAMPLE* PACMAN_MOVESOUND;
extern ALLEGRO_SAMPLE* PACMAN_DEATH_SOUND;
extern ALLEGRO_TIMER* end_tick_timer;
extern uint32_t GAME_TICK;
extern uint32_t GAME_TICK_CD;
extern bool game_over;
extern bool game_pass;
extern float effect_volume;

/* Declare static function */
static bool pacman_movable(Pacman* pacman, Map* M, Directions targetDirec) {
	int tmp_x = pacman->objData.Coord.x, tmp_y = pacman->objData.Coord.y;
	switch (targetDirec)
	{
	case UP:
		tmp_y -= 1;
		break;
	case DOWN:
		tmp_y += 1;
		break;
	case LEFT:
		tmp_x -= 1;
		break;
	case RIGHT:
		tmp_x += 1;
		break;
	default:
		// for none UP, DOWN, LEFT, RIGHT direction u should return false.
		return false;
	}
	if (is_wall_block(M, tmp_x, tmp_y) || is_room_block(M, tmp_x, tmp_y))
		return false;
	return true;
}

Pacman* pacman_create() {
	Pacman* pman = (Pacman*)malloc(sizeof(Pacman));
	if (!pman)
		return NULL;
	/* Init pman data */
	/* set starting point, Size, */
	/* TODO? */
	/* hint / just put it */
	pman->objData.Coord.x = 24;
	pman->objData.Coord.y = 24;
	pman->objData.Size.x = block_width;
	pman->objData.Size.y = block_height;

	pman->objData.preMove = NONE;
	pman->objData.nextTryMove = NONE;
	pman->objData.moveCD = 0;
	pman->objData.facing = 1;
	pman->speed = basic_speed;
	pman->death_anim_counter = al_create_timer(1.0f / 64);
	pman->powerUp = false;
	/* load sprites */
	pman->move_sprite = load_bitmap("Assets/pacman_move.png");
	pman->die_sprite = load_bitmap("Assets/pacman_die.png");
	return pman;

}

void pacman_destroy(Pacman* pman) {
	al_destroy_bitmap(pman->die_sprite);
	al_destroy_bitmap(pman->move_sprite);
	al_destroy_timer(pman->death_anim_counter);
	free(pman);
	game_log("pacman destroy");
}


void pacman_draw(Pacman* pman) {
	RecArea drawArea = getDrawArea(pman->objData, GAME_TICK_CD);
	int offset = 0;
	if (game_over) {
		al_draw_scaled_bitmap(pman->die_sprite, (al_get_timer_count(pman->death_anim_counter) / 16) * 16, 0
			, 16, 16, drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
			draw_region, draw_region, 0
		);
		if (al_get_timer_count(pman->death_anim_counter) >= 192 && !((al_get_timer_count(pman->death_anim_counter) / 32) & 1)) {
			al_draw_text(
				endFont,
				al_map_rgb(255, 0, 187),
				SCREEN_W / 2,
				SCREEN_H / 2 - 150,
				ALLEGRO_ALIGN_CENTER,
				"GAME OVER!"
			);
		}
	}
	else if (game_pass) {
		al_draw_scaled_bitmap(pman->move_sprite, (al_get_timer_count(end_tick_timer) & 1) * 32, 0
			, 16, 16, drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
			draw_region, draw_region, 0
		);
		if (!(al_get_timer_count(end_tick_timer) & 1))
			al_draw_text(
				endFont,
				al_map_rgb(255, 0, 187),
				SCREEN_W / 2,
				SCREEN_H / 2 - 150,
				ALLEGRO_ALIGN_CENTER,
				"CONGRADUATION!"
			);
	}
	else {
		if ((pman->objData.moveCD / 32)) {
			al_draw_scaled_bitmap(pman->move_sprite, 0 + (pman->objData.facing - 1) * 32, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
		else {
			al_draw_scaled_bitmap(pman->move_sprite, 16 + (pman->objData.facing - 1) * 32, 0,
				16, 16,
				drawArea.x + fix_draw_pixel_offset_x, drawArea.y + fix_draw_pixel_offset_y,
				draw_region, draw_region, 0
			);
		}
	}
}
void pacman_move(Pacman* pacman, Map* M) {
	if (!movetime(pacman->speed))
		return;
	if (game_over || game_pass)
		return;

	int probe_x = pacman->objData.Coord.x, probe_y = pacman->objData.Coord.y;
	if (pacman_movable(pacman, M, pacman->objData.nextTryMove))
		pacman->objData.preMove = pacman->objData.nextTryMove;
	else if (!pacman_movable(pacman, M, pacman->objData.preMove))
		return;

	switch (pacman->objData.preMove)
	{
	case UP:
		pacman->objData.Coord.y -= 1;
		pacman->objData.preMove = UP;
		break;
	case DOWN:
		pacman->objData.Coord.y += 1;
		pacman->objData.preMove = DOWN;
		break;
	case LEFT:
		pacman->objData.Coord.x -= 1;
		pacman->objData.preMove = LEFT;
		break;
	case RIGHT:
		pacman->objData.Coord.x += 1;
		pacman->objData.preMove = RIGHT;
		break;
	default:
		break;
	}
	pacman->objData.facing = pacman->objData.preMove;
	pacman->objData.moveCD = GAME_TICK_CD;
}
void pacman_eatItem(Pacman* pacman, const char Item) {
	switch (Item) {
		case '.':
			stop_bgm(PACMAN_MOVESOUND_ID);
			PACMAN_MOVESOUND_ID = play_audio(PACMAN_MOVESOUND, effect_volume);
			break;
		case 'P':
			stop_bgm(PACMAN_SUPER_ID);
			PACMAN_SUPER_ID = play_audio(PACMAN_SUPER, effect_volume);
			break;
		default:
			break;
	}
}

void pacman_NextMove(Pacman* pacman, Directions next) {
	pacman->objData.nextTryMove = next;
}

void pacman_die() {
	stop_bgm(PACMAN_MOVESOUND_ID);
	PACMAN_MOVESOUND_ID = play_audio(PACMAN_DEATH_SOUND, effect_volume);
}