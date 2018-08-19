#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <math.h>

#define DELAY (10) 
#define GAME_HEIGHT (6)
#define BLOCK_SPACING (2)

//Defining the structs
typedef struct {
    sprite_id sprite;
    int type;
} game_sprite;


// Defining global variables
bool game_over = false;
bool flying = false;


char * hero_right = " 0 "
                    "/|/"
                    " /\\";

char * hero_left = " 0 "
                   "\\|\\"
                   "/\\ ";


char * hero_stat = " 0 "
                   "[|]"
                   "/ \\";

char * hero_falling = "\\0/"
                      " | "
                      "/ \\";


char * treasure_closed = "     "
                         "##T##"
                         "#####";

char * treasure_open = "##T##"
                       " +2  "
                       "#####";

char * safe_block = "=========="
                    "==========";
char * danger_block = "xxxxxxxxxx"
                      "xxxxxxxxxx";
char * no_block = "          "
                  "          ";


int columns() {
    return screen_width()/12;
}

int rows() {
    return (screen_height()-6)/10;
}

char * get_type_image(int type);


void accelerate_blocks( game_sprite blocks[30][30] ) {
    int direction; 
    double speed;
    for ( int row = 1; row < rows() -1; row ++)
     {
        direction = 2 * ( rand() % 2 ) - 1; // randomly returns 1 or -1.
        speed = ( rand() % 3 + 1 )/10.0;
        for (int column = 0; column < columns(); column ++) {
            sprite_turn_to(blocks[column][row].sprite, direction * speed, 0); //debug
        }
    }
}

int vertical_height( int row ){
    return  GAME_HEIGHT + 8 * ( row + 1 ) + 2 * row;
}

int make_column( int column, game_sprite blocks[30][30] ) {
    // Function to create an individual column, return the number of danger blocks 
    // created in the column

    // In: the nth column from the left.
    // Out: The number of danger blocks created in the column.

    int block_type, safe_blocks, x, width, danger_blocks = 0;
    do {
        safe_blocks = 0;
        for (int row = 0; row < rows() ; row ++){
            block_type = rand() % 3;
            width = rand() % 6 + 5;

            if (block_type == 1) safe_blocks ++;
            else if (block_type == 2) danger_blocks ++;
            if (column == 0) x = 0;
            else x = 1 +  sprite_x(blocks[column-1][row].sprite) + sprite_width(blocks[column-1][row].sprite);


            blocks[column][row].type = block_type;
            blocks[column][row].sprite = sprite_create( x, vertical_height(row), width, 2, get_type_image(block_type));

        }
    } while(safe_blocks < 1);
    return danger_blocks;
}

void make_blocks( game_sprite blocks[30][30] )  {
    // Populates the blocks array with game_sprite structs to be used as the platforms
    // that the hero jumps on.

    // In: The total number of rows and columns.
    int danger_blocks = 0;
    while (danger_blocks < 2) {
        for (int column = 0; column < columns(); column ++){
            danger_blocks += make_column( column, blocks );
            danger_blocks = 3;
        }
    }
    sprite_set_image ( blocks[0][0].sprite , "sssssssssss" "sssssssssss");
    blocks[0][0].type = 1;
    
}

void respawn(sprite_id *hero, game_sprite blocks[30][30]);

void setup(
            bool *treasure_stop, bool *air_born, int *score, int *lives,
            int *time, timer_id *treasure_timer, timer_id *game_timer, 
            sprite_id *hero, sprite_id *treasure, game_sprite blocks[30][30]
            ) {
    // Calls all the functions needed to set up the game and set the global variables. 
    *air_born = false;
    *score = 0;
    *lives = 10;
    *time = 0;
    *treasure_stop = false;
    *hero = sprite_create( 0, 0, 3, 3, hero_stat);
    *treasure = sprite_create( 2,vertical_height(rows()-1) -3,5,3, treasure_closed);
    *treasure_timer = create_timer(500);
    *game_timer = create_timer(1000);
    sprite_turn_to( *treasure, 0.2, 0 );
    make_blocks( blocks );
    respawn( hero, blocks);
    accelerate_blocks( blocks );
}


char * get_type_image( int type ){
    // Returns the character coresponding to the interger type.
    // 0 is no block, 1 is safe_block and 2 is danger_block.

    // In: the type id
    // Out: The pointer to the character array corespoinging to the inputed type id.
    if ( type == 1 ){
        return safe_block;
    } else if ( type == 2) {
        return danger_block;
    } else {
        return no_block;
    }
}


void fly( sprite_id * hero ) {
    // This is used for debugging perposes, if this is still here, please ignore.

    char key = get_char();
    if (key == 'w')sprite_turn_to( *hero, sprite_dx(*hero),sprite_dy(*hero) -0.08);
    else if (key == 'd') sprite_turn_to( *hero, sprite_dx(*hero) +0.1,sprite_dy(*hero));
    else if (key == 'a') sprite_turn_to( *hero, sprite_dx(*hero) -0.1,sprite_dy(*hero));
    else if (key == 's') sprite_turn_to( *hero, sprite_dx(*hero),sprite_dy(*hero) +0.08);
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


void gravity( sprite_id *hero, bool on_block) {
    if (on_block) sprite_turn_to( *hero, sprite_dx( *hero ), 0 );
    else sprite_turn_to( *hero, sprite_dx( *hero ), sprite_dy( *hero ) + 0.01);
}


void show_game_over( int score, int time ) {
    bool game_over_screen = true;
    char key;
    char time_message[100];
    the_time_pls(time_message, time);
    clear_screen();
    draw_formatted((screen_width()-34)/2,screen_height()/2-1,"So long an thanks for all the fish");
    draw_formatted((screen_width()-34)/2,screen_height()/2+1,"Score: %d", score);
    draw_formatted((screen_width()-34)/2,screen_height()/2+2,time_message);
    show_screen();
    while ( game_over_screen ) {
        key = get_char();
        if (key == 'q'){
            game_over = true;
            return;
        }
        else if (key == 'r') {
            clear_screen();
            //setup();
            return;
        }
    }
}

void face_direction( sprite_id *hero, game_sprite colided_block ) {
    if ( sprite_dx( *hero ) - sprite_dx( colided_block.sprite ) > 0) sprite_set_image( *hero, hero_right );
    else if (sprite_dx( *hero ) - sprite_dx( colided_block.sprite ) < 0) sprite_set_image( *hero, hero_left );
    else sprite_set_image( *hero, hero_stat);
}


void controls(bool *treausre_stop, sprite_id *hero, bool *air_born, game_sprite colided_block) {
    char key = 0;
    key = get_char();
    if ( key == 'w') {
        sprite_turn_to( *hero, sprite_dx(*hero ), -0.32);
        sprite_step( *hero );
    }
    if ( key == 'a' ){
        sprite_turn_to( *hero , sprite_dx(*hero) -0.2, sprite_dy(*hero) );
    }  if ( key == 'd' ){
        sprite_turn_to( *hero, sprite_dx(*hero) + 0.2, sprite_dy(*hero) );
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



bool get_horizontal_colide( sprite_id *hero, game_sprite blocks[30][30] ) {
    int b;
    bool vertical = false;
    for ( int row = 0; row < rows(); row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) ) > round(sprite_y( *hero ) - 2) &&
             round(sprite_y(blocks[0][row].sprite)) < round(sprite_y( *hero ) + 3)) {
                 vertical = true;
                 b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns(); column ++) {
            game_sprite block = blocks[column][b];
            if ( x >= sprite_x(block.sprite) - 5 && 
                x <= sprite_x(block.sprite) + sprite_width(block.sprite) - 1 &&
                block.type != 0) {
                return true;
            } 
        }
    }
    return false;
}


bool get_down_colide( sprite_id *hero, game_sprite *colided_block, game_sprite blocks[30][30], bool *air_born ) {
    bool vertical = false;
    int b;
    for ( int row = 0; row < rows(); row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) - 3) == round(sprite_y( *hero ))) {
            vertical = true;
            b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns(); column ++) {
            game_sprite block = blocks[column][b];
            if ( x > sprite_x(block.sprite) - 3 && 
                x < sprite_x(block.sprite) + sprite_width(block.sprite) - 1 &&
                block.type != 0) {
                    *colided_block = blocks[column][b];
                    return true;
            } 
        }
    }
    if (!*air_born){
        *air_born = true;
        sprite_set_image ( *hero, hero_falling );
    }

    return false;
}


bool get_up_colide( sprite_id *hero, game_sprite blocks[30][30] ) {
    if ( sprite_dy( *hero ) >= 0 ) return false;
    if ( sprite_y( *hero ) <= 6 ) return true;
    bool vertical = false;
    int b;
    for ( int row = 0; row < rows(); row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) +2) == round(sprite_y( *hero ))) {
            vertical = true;
            b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns(); column ++) {
            game_sprite block = blocks[column][b];
            if ( x > sprite_x(block.sprite) - 4 && 
                x < sprite_x(block.sprite) + sprite_width(block.sprite) &&
                block.type != 0) {
                return true;
            } 
        }
    }
    return false;
}


void respawn(sprite_id *hero, game_sprite blocks[30][30] ) {
    int spawn_column;
    game_sprite spawn_block;

    do {
        spawn_column = rand() % (columns()  + 1);
        spawn_block = blocks[spawn_column][0];
    } while (spawn_block.type != 1);

    draw_string(sprite_x(*hero), sprite_y(*hero) + 0, "   ");
    draw_string(sprite_x(*hero), sprite_y(*hero) + 1, "   ");
    draw_string(sprite_x(*hero), sprite_y(*hero) + 2, "   ");
    draw_string(sprite_x(*hero), sprite_y(*hero) + 3, "   ");
    
    show_screen();
    //draw_game(score, lives, time, hero, treasure, blocks);

    draw_string(sprite_x(spawn_block.sprite), vertical_height(0) - 1, "/ \\");
    show_screen();
    timer_pause(100);
    
    draw_string(sprite_x(spawn_block.sprite), vertical_height(0) - 2, "[|]");
    show_screen();
    timer_pause(100);
    
    draw_string(sprite_x(spawn_block.sprite), vertical_height(0) - 3, " 0 ");
    show_screen();
    timer_pause(100);

    sprite_move_to( *hero, sprite_x(spawn_block.sprite) + 1 , vertical_height(0) - 3.5);
    sprite_turn_to( *hero, 0, 0);
}


bool get_treasure_colide( int *lives, sprite_id *hero, sprite_id *treasure, game_sprite blocks[30][30]) {
    if ( 
        sprite_x( *hero ) < sprite_x( *treasure ) + sprite_width( *treasure ) &&
        sprite_x( *hero ) + sprite_width( *hero ) > sprite_x( *treasure )  &&
        round(sprite_y( *hero )) == round(sprite_y( *treasure ))
        ) {
            sprite_set_image( *treasure, treasure_open );
            draw_string(sprite_x(*treasure), sprite_y(*treasure) + 0, "      ");
            draw_string(sprite_x(*treasure), sprite_y(*treasure) + 1, "      ");
            draw_string(sprite_x(*treasure), sprite_y(*treasure) + 2, "      ");

            sprite_draw(*treasure);
            show_screen();
            timer_pause( 1000 );

            *lives += 2;
            respawn ( hero, blocks );
            sprite_set_image( *treasure, treasure_closed );
        }
    return true;
}


void edge_detect( int *lives, sprite_id * sprite, game_sprite blocks[30][30] ) {
    if (sprite_x(*sprite) < - 3 && sprite_dx(*sprite) < 0) respawn( sprite, blocks );
    else if (sprite_x(*sprite) > screen_width() - sprite_width(*sprite) + 1 && sprite_dx(*sprite) > 0) respawn( sprite, blocks );

    if (sprite_y(*sprite) > screen_height()) {
        *lives -= 1;
        respawn( sprite, blocks );
    }
}

void treasure_edge_detect( sprite_id *treasure ) {
    if ( 
        sprite_x(*treasure) < 0 ||
        sprite_x(*treasure) > screen_width() - sprite_width(*treasure) - 2
        ) sprite_turn_to( *treasure, -sprite_dx( *treasure ), 0);
}

void blocks_step( game_sprite blocks[30][30]) {
    for (int column = 0; column < columns(); column ++) {
        for ( int row = 1; row < rows() -1; row ++) {
            sprite_id *sprite = &blocks[column][row].sprite;
            if (sprite_x(*sprite) < - 8) { sprite_move_to( *sprite, screen_width(), sprite_y(*sprite));}
            else if (sprite_x(*sprite) > screen_width()) { sprite_move_to( *sprite, -8 , sprite_y(*sprite));}
            sprite_step(*sprite);
        }
    }
}



void colision( sprite_id *hero, int *lives, int *score , bool h, bool d, bool u, bool *air_born, game_sprite blocks[30][30], game_sprite colided_block, sprite_id *prev_colided ) {

    edge_detect( lives, hero, blocks );

    if ( d ) {
        if (colided_block.type == 2) {
           *lives -= 1;
            respawn( hero, blocks);            
        } else {
            sprite_turn_to(*hero, sprite_dx( *hero ), 0 );
            if (*air_born) {
                if (!sprites_equal(colided_block.sprite, *prev_colided)) *score += 1;
                
                sprite_turn_to( *hero, sprite_dx(colided_block.sprite), 0 );
                *air_born = false;
                *prev_colided = colided_block.sprite;
            }
        }
    }

    if ( u ) sprite_turn_to(*hero, sprite_dx( *hero ), 0 );
    if ( h ) sprite_turn_to( *hero, sprite_dx( colided_block.sprite ), sprite_dy( *hero ));
}

void process( bool treasure_stop, bool air_born, int score, int lives,
            int time, timer_id treasure_timer, timer_id game_timer,
            sprite_id hero, sprite_id treasure, game_sprite blocks[30][30] ){ // Call our setup function to initialise game state.

        sprite_id prev_colided = blocks[0][0].sprite;

    while ( ! game_over ) {
        clear_screen();

        bool horizontal_colide, down_colide, up_colide;
        draw_game(score, lives, time, hero, treasure, blocks);
        game_sprite colided_block;
        horizontal_colide = get_horizontal_colide( &hero, blocks );
        down_colide = get_down_colide( &hero, &colided_block, blocks, &air_born );
        up_colide = get_up_colide( &hero, blocks);
        get_treasure_colide( &lives, &hero, &treasure , blocks);
        colision( &hero, &lives, &score, horizontal_colide, down_colide, up_colide, &air_born,blocks , colided_block, &prev_colided );
        treasure_edge_detect( &treasure );

        if (!flying) gravity( &hero, down_colide );
        if (!flying && !air_born ) controls( &treasure_stop, &hero, &air_born, colided_block );  
        else fly( &hero);

        sprite_step( hero );
        sprite_step( treasure );
        blocks_step(blocks);

        if (timer_expired(game_timer)) time ++;
        if (lives < 0) show_game_over(score, time);


        show_screen();       
        timer_pause( DELAY ); 
	}
}


void cleanup( void ) {
}


int main( void ) {
    srand(get_current_time());
	setup_screen(); 

    bool treasure_stop, air_born;
    
    int score, lives, time;
    timer_id treasure_timer, game_timer;
    sprite_id hero, treasure;
    game_sprite blocks[30][30];

	
    setup( &treasure_stop, &air_born, &score, &lives,
           &time, &treasure_timer, &game_timer, &hero,
           &treasure, blocks );

	show_screen();
    process( 
            treasure_stop, air_born, score, lives,
            time, treasure_timer, game_timer, hero,
            treasure, blocks );
	
	cleanup();

	return 0;
}