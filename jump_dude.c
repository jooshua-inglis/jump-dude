#include "jump_dude.h"


void setup(
            bool *treasure_stop, bool *air_born, int *score, int *lives,
            int *time, timer_id *treasure_timer, timer_id *game_timer, 
            sprite_id *hero, sprite_id *treasure, sprite_id *prev_colided,
            game_sprite blocks[30][30]
            ) {
    // Calls all the functions needed to set up the game and set the global variables. 
    *air_born = false;
    *score = 0;
    *lives = 10;
    *time = 0;
    *treasure_stop = false;
    *hero = sprite_create( 2, row_height(0) - 3.5 , 3, 3, hero_stat);
    *treasure = sprite_create( 2,row_height(rows()-1) -3,3,3, treasure_closed);
    *treasure_timer = create_timer(500);
    *game_timer = create_timer(1000);
    make_blocks( blocks );
    accelerate_blocks( blocks );
    sprite_turn_to( *treasure, 0.2, 0 );
    *prev_colided = blocks[0][0].sprite;
}


void process( bool treasure_stop, bool air_born, int score, int lives,
            int time, timer_id treasure_timer, timer_id game_timer,
            sprite_id hero, sprite_id treasure, sprite_id prev_colided,
            game_sprite blocks[30][30] ){ 

    while ( !game_over ) {
        
        bool down_colide;
        game_sprite colided_block;

        colision( hero, treasure, &lives, &score, &air_born, &down_colide, blocks, &colided_block );
        edge_detect( hero, treasure, blocks, &lives );
        gravity( hero, down_colide );
        if ( !air_born ) controls( &treasure_stop, hero, &air_born, colided_block );
        else  {
            sprite_set_image( hero, hero_falling);
            clear_input_buffer(); 
            }
        sprite_step( hero );
        if (!treasure_stop) sprite_step( treasure );
        blocks_step( blocks);

        if (timer_expired(game_timer)) time ++;
        if (lives < 0) show_game_over(score, time);

        draw_game(score, lives, time, hero, treasure, blocks);   
        timer_pause( DELAY ); 
	}
}

int main( void ) {
    srand(get_current_time());
	setup_screen(); 

    bool treasure_stop, air_born;
    int score, lives, time;
    timer_id treasure_timer, game_timer;
    sprite_id hero, treasure, prev_colided;
    game_sprite blocks[30][30];

    while ( !game_quit ){
        game_over = false;
        setup( 
            &treasure_stop, &air_born, &score, &lives,
            &time, &treasure_timer, &game_timer, &hero,
            &treasure, &prev_colided, blocks
            );

        show_screen();
        process( 
                treasure_stop, air_born, score, lives,
                time, treasure_timer, game_timer, hero,
                treasure, prev_colided, blocks );
        
        cleanup( hero, treasure, blocks, &game_timer, &treasure_timer);
    }
	return 0;
}