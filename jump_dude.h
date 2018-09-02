#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <math.h>
#include "images.h"
#include "debug.h"

#define DELAY (10) 
#define GAME_HEIGHT (6)
#define BLOCK_SPACING (2)


//Defining the structs
typedef struct game_sprite
{
    sprite_id sprite;
    int type;
} game_sprite;


// Defining global variables
bool game_over = false;
bool game_quit = false;

void clear_input_buffer() {
    while (get_char() >= 0) {}
}

// Defining game functions
int columns() {
    return screen_width()/12;
}


int rows() {
    return (screen_height()-6)/10;
}


char * get_type_image( int type ){
    // Returns the character coresponding to the interger type.
    // 0 is no block, 1 is safe_block and 2 is danger_block.

    // In: the type id
    // Out: The pointer to the character array corespoinging to the inputed type id.
    if ( type == 1 )        return safe_block;
    else if ( type == 2)    return danger_block;
    else                    return no_block;
}


int row_height( int row ){
    return  GAME_HEIGHT + 8 * ( row + 1 ) + 2 * row;
}


void make_column( int column, int *safe_blocks, int *danger_blocks, game_sprite blocks[30][30] ) {
    // Function to create an individual column, return the number of danger blocks created in the column
    int block_type, row_safe_blocks, x, width, danger_blocks = 0;
    do {
        row_safe_blocks = 0;
        for (int row = 0; row < rows() ; row ++){
            if (danger_blocks > 40) {
                if (safe_blocks > 160) return danger_blocks;
                block_type = 1;
            }
            else if (safe_blocks > 160) block_type = 2;
            else block_type = rand() % 3;
            width = rand() % 6 + 5;

            if (block_type == 1) row_safe_blocks ++;
            else if (block_type == 2) danger_blocks ++;
            if (column == 0) x = 0;
            else x = 1 +  sprite_x(blocks[column-1][row].sprite) + sprite_width(blocks[column-1][row].sprite);

            blocks[column][row].type = block_type;
            blocks[column][row].sprite = sprite_create( x, row_height(row), width, 2, get_type_image(block_type));
        }
    } while(row_safe_blocks < 1);
    safe_blocks += row_safe_blocks;
}


void make_blocks( game_sprite blocks[30][30] )  {
    // Populates the blocks array with game_sprite structs to be used as the platforms
    // that the hero jumps on.
    int danger_blocks = 0, safe_blocks = 0;
    while (danger_blocks < 2) {
        for (int column = 0; column < columns(); column ++){
            make_column( column, &safe_blocks, &danger_blocks, blocks );
        }
    }
    sprite_set_image ( blocks[0][0].sprite , "sssssssssss" "sssssssssss");
    blocks[0][0].type = 1;
}


void accelerate_blocks( game_sprite blocks[30][30] ) {
    int direction = 2 * ( rand() % 2 ) - 1;; 
    double speed;
    for ( int row = 1; row < rows() -1; row ++) {
        speed = ( rand() % 3 + 1 )/10.0;
        direction *= -1;
        for (int column = 0; column < columns(); column ++) {
            sprite_turn_to(blocks[column][row].sprite, direction * speed, 0); //debug
        }
    }
}

void respawn_animation( sprite_id hero ) {
    draw_string(hero->x, hero->y + 3, "/ \\");
    show_screen();
    timer_pause(100);
    
    draw_string(hero->x, hero->y + 2, "[|]");
    show_screen();
    timer_pause(100);
    
    draw_string(hero->x, hero->y + 1, " 0 ");
    show_screen();
    timer_pause(100);
    sprite_set_image(hero, hero_stat);
}


void respawn(sprite_id hero, game_sprite blocks[30][30] ) {
    int spawn_column;
    game_sprite spawn_block;

    do {
        spawn_column = rand() % (columns()  + 1);
        spawn_block = blocks[spawn_column][0];
    } while (spawn_block.type != 1);

    for (int i = 0; i < 4; i++) draw_string(hero->x, hero->y + i, "     ");
    show_screen();
    
    timer_pause(150);

    sprite_move_to( hero, spawn_block.sprite->x + 1 , row_height(0) - 3.5);
    respawn_animation( hero );
    sprite_turn_to( hero, 0, 0 );
    clear_input_buffer();
}


void the_time_pls(char message[100], int time) {
    // Takes the time in seconds and stores it as a character array in the 
    // Form of mm:ss

    // In: The character array to store the time message; The time in seconds from 
    //     the beginning of the game

    int seconds = time % 60;
    int minutes = time / 60;
    sprintf(message,"Time: %d%d:%d%d", minutes/10, minutes%10, seconds/10, seconds%10);
}


void gravity( sprite_id hero, bool on_block) {
    if (on_block) hero->dy = 0;
    else hero->dy += 0.01;
}

void wait_for_responce(){
    char key;
    while ( true ) {
        timer_pause(10);
        key = get_char();
        if (key == 'q'){
            game_quit = true;
            return;
        }
        else if (key == 'r') {
            game_quit = false; 
            return;
        }
    }

}

void show_game_over( int score, int time ) {
    game_over = true;
    char time_message[100];
    the_time_pls(time_message, time);
    clear_screen();
    draw_formatted((screen_width()-34)/2,screen_height()/2-1,"So long an thanks for all the fish");
    draw_formatted((screen_width()-34)/2,screen_height()/2+1,"Score: %d", score);
    draw_formatted((screen_width()-34)/2,screen_height()/2+2,time_message);
    show_screen();
    wait_for_responce();
}

void face_direction( sprite_id hero, game_sprite colided_block ) {
    if (round(hero->dx) == 0) sprite_set_image( hero, hero_stat);
    if ( hero->dx - colided_block.sprite->dx > 0) sprite_set_image( hero, hero_right );
    else if (hero->dx - colided_block.sprite->dx < 0) sprite_set_image( hero, hero_left );
}


void controls(bool *treausre_stop, sprite_id hero, bool *air_born, game_sprite colided_block) {
    char key = 0;
    key = get_char();
    if ( key == 'w') {
        sprite_turn_to( hero, sprite_dx(hero ), -0.32);
        sprite_step( hero );
    }
    if ( key == 'a' ){
        sprite_turn_to( hero , sprite_dx(hero) -0.2, sprite_dy(hero) );
    }  if ( key == 'd' ){
        sprite_turn_to( hero, sprite_dx(hero) + 0.2, sprite_dy(hero) );
    }  if ( key == 't' ){
        *treausre_stop = !*treausre_stop;
    }

    if ( key ) {
        face_direction( hero, colided_block );
    }  
}

void draw_grid( game_sprite blocks[30][30] ) {
    for (int column = 0; column < columns(); column ++ ) {
        for (int row = 0; row < rows(); row ++) {
            sprite_draw(blocks[column][row].sprite);
        }
    }
}

void draw_game( int score, int lives, int time, sprite_id hero, sprite_id treasure, game_sprite blocks[30][30] ){
    clear_screen();
    char time_message[100];
    the_time_pls(time_message, time);
    draw_line(0,5, screen_width()-1,5, '-');
    draw_formatted( ( screen_width() - 10 ) * 1/5 , 2 , "n9989773" );
    draw_formatted( ( screen_width() - 10 ) * 2/5 , 2 , "Score: %d",score );
    draw_formatted( ( screen_width() - 10 ) * 3/5 , 2 ,  time_message);
    draw_formatted( ( screen_width() - 10 ) * 4/5 , 2 , "Lives: %d", lives );
    draw_grid( blocks );
    sprite_draw( hero );
    sprite_draw( treasure );
    show_screen();
}


bool horizontal_edge_detect ( sprite_id sprite, int offset ) {
    return (sprite->x < -offset || sprite->x + sprite->width > screen_width() + offset );
}

bool top_detect ( sprite_id sprite, int offset ) {
    return sprite->y < offset + GAME_HEIGHT;
}

bool bottom_detect( sprite_id sprite, int offset ) {
    return sprite->y + sprite->height > screen_width() + offset;
}

void edge_detect( sprite_id hero, sprite_id treasure, game_sprite blocks[30][30], int *lives ) {
    if ( bottom_detect(hero, 0)){
        *lives -= 1;
        respawn ( hero, blocks );
    }
    if ( horizontal_edge_detect( hero , hero->width )) {
        *lives -= 1;
        respawn ( hero, blocks );
    }
    
    if ( horizontal_edge_detect( treasure , -1  ) ) {
        treasure->dx = -treasure->dx;
    }


}

void blocks_step( game_sprite blocks[30][30]) {
    for (int column = 0; column < columns(); column ++) {
        for ( int row = 1; row < rows() -1; row ++) {
            sprite_id sprite = blocks[column][row].sprite;
            if (sprite->x < - 8) sprite->x = screen_width();
            else if (sprite->x > screen_width()) sprite->x = -8;
            sprite_step(sprite);
        }
    }
}


bool to_the_side( sprite_id sprite_1, sprite_id sprite_2 ) {
    if ( sprite_1->y <= sprite_2->y + sprite_2->height  &&
         sprite_1->y + sprite_1->height >= sprite_2->y  
         ) {
            return ( sprite_1->x + sprite_1->width >= sprite_2->x &&
            sprite_1->x <= sprite_2->x + sprite_2->width);
         }
    else return false;
}

bool on_top_of( sprite_id sprite_1, sprite_id sprite_2, int correction) {
    return ( round(sprite_1->y + sprite_1->height + correction ) == round(sprite_2->y) &&
             sprite_1->x + sprite_1->width > sprite_2->x && 
             sprite_1->x < sprite_2->x + sprite_2->width - 1);        
}


void treasure_colide( int *lives, sprite_id hero, sprite_id treasure, game_sprite blocks[30][30]) {
    sprite_set_image( treasure, treasure_open );
    for (int i = 0; i < 3; i++) draw_string( treasure->x, treasure->y + i, "   ");

    sprite_draw(treasure);
    show_screen();
    timer_pause( 1000 );
    sprite_hide(treasure);
    sprite_move_to(treasure,0,0);
    *lives += 2;
    respawn ( hero, blocks );
    
    
}


void colision( sprite_id hero, sprite_id treasure,  int *lives, int *score, bool *air_born, bool *down_colide, game_sprite blocks[30][30], game_sprite *colided_block ) {
    bool up_colide = false;
    bool horizontal_colide = false;
    static sprite_id prev_colided;
    *down_colide = false;

    if (to_the_side(treasure, hero)) treasure_colide ( lives, hero, treasure, blocks );

    for (int c = 0; c < columns(); c++) { for (int r = 0; r < rows(); r++) {
        if (on_top_of( hero, blocks[c][r].sprite, 0 ) && blocks[c][r].type != 0 ){
            *down_colide = true;
            *colided_block = blocks[c][r];
        }
        if ( ( top_detect( hero, 0 ) || on_top_of( blocks[c][r].sprite, hero, 0 ) )  && blocks[c][r].type != 0 && hero->dy < 0 ) {
            up_colide = true;
            *colided_block = blocks[c][r];
            }

        if ( to_the_side( hero, blocks[c][r].sprite ) && blocks[c][r].type != 0 ){
            horizontal_colide = true;
            *colided_block = blocks[c][r];
            }
    }}
    if (!*down_colide) *air_born = true;

    if ( *down_colide ) {
        if (colided_block->type == 2) {
           *lives -= 1;
            respawn( hero, blocks);            
        } else {
            hero->dy = 0;
            if (*air_born) {
                if (!sprites_equal(colided_block->sprite, prev_colided)) *score += 1;
                hero->dx = colided_block->sprite->dx;
                *air_born = false;
                prev_colided = colided_block->sprite;
            }
        }
    }

    if ( up_colide ) hero->dy = 0;
    if ( horizontal_colide ) hero->dx = colided_block->sprite->dx;
}

void cleanup( sprite_id hero, sprite_id treasure, game_sprite blocks[30][30], timer_id *game_timer, timer_id *treasure_timer ) {
    sprite_destroy( hero );
    sprite_destroy( treasure );
    for (int c = 0; c < columns(); c++){
        for (int r = 0; r < rows(); r++){
            sprite_destroy( blocks[c][r].sprite );
        }
    }
    destroy_timer( *game_timer );
    destroy_timer( *treasure_timer );
}
