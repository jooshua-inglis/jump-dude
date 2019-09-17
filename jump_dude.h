#ifndef _JUMP_DUDE_H_

#define _JUMP_DUDE_H_

#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <math.h>
#include <string.h>


#define DELAY (10) 
#define GAME_HEIGHT (6)
#define MAX_WIDTH (10)

typedef struct game_sprite game_sprite;

void clear_input_buffer();

int get_columns();

int get_rows();

char * get_type_image( int type );

int row_height( int row );

int rand_type();

bool make_column( int column, int rows, int *safe_blocks, int *danger_blocks, game_sprite **blocks );

void make_blocks( game_sprite **blocks, int columns, int rows )  ;

void accelerate_blocks( game_sprite **blocks, int rows, int columns ) ;

void respawn_animation( sprite_id jump_dude ) ;

void respawn(sprite_id jump_dude, game_sprite **blocks ) ;

void get_time_message(char message[100], int time) ;

void gravity( sprite_id jump_dude, bool on_block) ;

void wait_for_responce();

void show_game_over( int score, int time ) ;

void face_direction( sprite_id jump_dude, game_sprite colided_block ) ;

void controls(bool *treausre_stop, sprite_id jump_dude, bool *air_born, game_sprite colided_block) ;

void draw_grid( game_sprite **blocks ) ;

void draw_game( int score, int lives, int time, sprite_id jump_dude, sprite_id treasure, game_sprite **blocks );

bool horizontal_edge_detect ( sprite_id sprite, int offset ) ;

bool top_detect ( sprite_id sprite, int offset ) ;

bool bottom_detect( sprite_id sprite, int offset ) ;

void edge_detect( sprite_id jump_dude, sprite_id treasure, game_sprite **blocks, int *lives ) ;

void blocks_step( game_sprite **blocks ) ;

bool to_the_side( sprite_id sprite_1, sprite_id sprite_2 ) ;

bool on_top_of( sprite_id sprite_1, sprite_id sprite_2, int correction) ;

void open_treasure( int *lives, sprite_id jump_dude, sprite_id treasure, game_sprite **blocks ) ;

game_sprite get_collisions(bool *down_colide, bool *up_colide, bool *horizontal_colide, sprite_id jump_dude, game_sprite **blocks) ;

sprite_id land(int *score, sprite_id jump_dude, sprite_id prev_colided, game_sprite *colided_block) ;

void colision( sprite_id jump_dude, sprite_id treasure,  int *lives, int *score, bool *air_born, bool *down_colide, game_sprite **blocks, game_sprite *colided_block ) ;

void cleanup( sprite_id jump_dude, sprite_id treasure, game_sprite **blocks, timer_id *game_timer, timer_id *treasure_timer ) ;

int main( int argc, char const *argv[] ) ;

#endif