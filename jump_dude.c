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
bool game_over = false, first = false;
bool treasure_stop, air_born;
bool flying = false;
int score, lives, time, columns, rows;
timer_id treasure_timer, game_timer;
sprite_id hero, treasure;
game_sprite blocks[30][30];


char * hero_image = " 0  "
                    " |//"
                    "/ | ";

char * treasure_image = "##T##"
                        "#####";
char * safe_block = "=========="
                    "==========";
char * danger_block = "xxxxxxxxxx"
                      "xxxxxxxxxx";
char * no_block = "          "
                  "          ";

int make_column( int column);
void make_blocks( int columns, int rows);
void accelerate_blocks ( int columns, int rows );

void setup( void ) {
    // Calls all the functions needed to set up the game and set the global variables. 
    columns = screen_width()/12;
    air_born = false;
    rows = 4;
    score = 0;
    lives = 10;
    time = 0;
    treasure_stop = false;
    hero = sprite_create(2, GAME_HEIGHT + 2 , 4, 3, hero_image);
    treasure = sprite_create(2,GAME_HEIGHT + 17,5,2, treasure_image);
    treasure_timer = create_timer(500);
    game_timer = create_timer(1000);
    make_blocks(columns, rows);
    accelerate_blocks( columns, rows );
}

void accelerate_blocks( int columns, int rows ) {
    int direction; 
    double speed;
    for ( int row = 1; row < rows -1; row ++)
     {
        direction = 2 * ( rand() % 2 ) - 1; // randomly returns 1 or -1.
        speed = (rand() % 3 + 1)/10.0;
        for (int column = 0; column < columns; column ++) {
            sprite_turn_to(blocks[column][row].sprite, direction * speed, 0); //debug
        }
    }
}

void make_blocks( int columns, int rows) {
    // Populates the blocks array with game_sprite structs to be used as the platforms
    // that the hero jumps on.

    // In: The total number of rows and columns.
    int danger_blocks = 0;
    blocks[0][0].sprite = sprite_create( 0, GAME_HEIGHT + 5, 9, 2, "sssssssss" "sssssssss" );   
    blocks[0][0].type = 1;
    while (danger_blocks < 2) {
        for (int column = 0; column < columns; column ++){
            danger_blocks += make_column( column );
            danger_blocks = 3;
        }
    }
    
}

char * get_type_image(int type);

int make_column( int column ) {
    // Function to create an individual column, return the number of danger blocks 
    // created in the column

    // In: the nth column from the left.
    // Out: The number of danger blocks created in the column.

    int block_type, safe_blocks, x, y, width, danger_blocks = 0;
    do {
        safe_blocks = 0;
        for (int row = 0; row < rows ; row ++){
            if (column + row == 0) row ++;
            block_type = rand() % 3;
            width = rand() % 6 + 5;

            if (block_type == 1) safe_blocks ++;
            else if (block_type == 2) danger_blocks ++;
            if (column == 0) x = 0;
            else x = 5 +  sprite_x(blocks[column-1][row].sprite) + sprite_width(blocks[column-1][row].sprite);

            y = GAME_HEIGHT + 7 + row * 5 + (row-1)*2 ;

            blocks[column][row].type = block_type;
            blocks[column][row].sprite = sprite_create( x, y, width, 2, get_type_image(block_type));

        }
    } while(safe_blocks < 1);
    return danger_blocks;
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
            setup();
            return;
        }
    }
}


void controls(bool *treausre_stop, sprite_id *hero, bool *air_born) {
    char key = get_char();
    if ( key == 'w') {
        sprite_turn_to( *hero, sprite_dx(*hero ), -0.32);
        sprite_step( *hero );
        *air_born = true;
    }
    if ( key == 'a' ){
        sprite_turn_to( *hero , sprite_dx(*hero) -0.2, sprite_dy(*hero) );
    }  if ( key == 'd' ){
        sprite_turn_to( *hero, sprite_dx(*hero) + 0.2, sprite_dy(*hero) );
    }  if ( key == 't' ){
        *treausre_stop = !*treausre_stop;
    }

    // DEBUGGING
    else if ( key == 'y')  {
        show_game_over(score, time);
    }
    
}

void draw_grid( void );

void draw_game( int score, int lives, int time, int columns, int rows, sprite_id hero, sprite_id treasure ){
    char time_message[100];
    the_time_pls(time_message, time);
    draw_line(0,5, screen_width()-1,5, '-');
    draw_formatted( ( screen_width() - 10 ) * 1/5 , 2 , "n9989773" );
    draw_formatted( ( screen_width() - 10 ) * 2/5 , 2 , "Score: %d",score );
    draw_formatted( ( screen_width() - 10 ) * 3/5 , 2 ,  time_message);
    draw_formatted( ( screen_width() - 10 ) * 4/5 , 2 , "Lives: %d", lives );
    draw_grid();
    sprite_draw(hero);
    sprite_draw(treasure);
    show_screen();
}


void draw_grid( void ) {
    for (int column = 0; column < columns; column ++ ) {
        for (int row = 0; row < rows; row ++) {
            sprite_draw(blocks[column][row].sprite);
        }
    }
}



bool get_horizontal_colide( sprite_id *hero, int columns, int rows, game_sprite *colided_block ) {
    int b;
    bool vertical = false;
    for ( int row = 0; row < rows; row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) ) > round(sprite_y( *hero ) - 2) &&
             round(sprite_y(blocks[0][row].sprite)) < round(sprite_y( *hero ) + 3)) {
                 vertical = true;
                 b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns; column ++) {
            game_sprite block = blocks[column][b];
            if ( x >= sprite_x(block.sprite) - 5 && 
                x <= sprite_x(block.sprite) + sprite_width(block.sprite) - 1 &&
                block.type != 0) {
                *colided_block = blocks[column][b];
                return true;
            } 
        }
    }
    return false;
}

bool get_down_colide( sprite_id *hero, int columns, int row, game_sprite *colided_block, bool *airborn ) {
    bool vertical = false;
    int b;
    for ( int row = 0; row < rows; row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) - 3) == round(sprite_y( *hero ))) {
            vertical = true;
            b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns; column ++) {
            game_sprite block = blocks[column][b];
            if ( x > sprite_x(block.sprite) - 4 && 
                x < sprite_x(block.sprite) + sprite_width(block.sprite) - 1 &&
                block.type != 0) {
                    *colided_block = blocks[column][b];
                    return true;
            } 
        }
    }
    *airborn = true;
    return false;
}

bool get_up_colide( sprite_id *hero, int columns, int row ) {
    if ( sprite_dy( *hero ) >= 0 ) return false;
    if ( sprite_y( *hero ) <= 6 ) return true;
    bool vertical = false;
    int b;
    for ( int row = 0; row < rows; row ++) {
        if ( round(sprite_y(blocks[0][row].sprite) +2) == round(sprite_y( *hero ))) {
            vertical = true;
            b = row;
        }
    }
    if ( vertical ){
        int x = sprite_x( *hero );

        for (int column = 0; column < columns; column ++) {
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

void respawn( sprite_id *hero) {
    int spawn_column;
    game_sprite spawn_block;

    do {
        spawn_column = rand() % (columns  + 1);
        spawn_block = blocks[spawn_column][0];
    } while (spawn_block.type != 1);

    sprite_move_to( *hero, sprite_x(spawn_block.sprite) + 1 , GAME_HEIGHT + 1 );
}

void edge_detect( sprite_id * sprite ) {
    if (sprite_x(*sprite) < 0 && sprite_dx(*sprite) < 0) sprite_turn_to(*sprite, 0, 0 );
    else if (sprite_x(*sprite) > screen_width() - sprite_width(*sprite) - 2 && sprite_dx(*sprite) > 0) sprite_turn_to(*sprite, 0, 0 );

    if (sprite_y(*sprite) > screen_height()) {
        respawn( sprite );
    }
}

void blocks_step( int columns, int rows ) {
    for (int column = 0; column < columns; column ++) {
        for ( int row = 1; row < rows -1; row ++) {
            sprite_id *sprite = &blocks[column][row].sprite;
            if (sprite_x(*sprite) < - 8) { sprite_move_to( *sprite, screen_width(), sprite_y(*sprite));}
            else if (sprite_x(*sprite) > screen_width()) { sprite_move_to( *sprite, -8 , sprite_y(*sprite));}
            sprite_step(*sprite);
        }
    }
}


void colision( sprite_id *hero, bool h, bool d, bool u, bool *air_born, game_sprite colided_block ) {

    edge_detect( hero );

    if ( d ) {
        sprite_turn_to(*hero, sprite_dx( *hero ), 0 );
        if (*air_born) {
            sprite_turn_to( *hero, sprite_dx(colided_block.sprite), 0 );
            *air_born = false;
        }
    }
    if ( u ) sprite_turn_to(*hero, sprite_dx( *hero ), 0 );
    if ( h ) sprite_turn_to( *hero, sprite_dx( colided_block.sprite ), sprite_dy( *hero ));
}

void process( void ){ // Call our setup function to initialise game state.
    draw_game(score, lives, time, columns, rows, hero, treasure);
    bool horizontal_colide, down_colide, up_colide;
    game_sprite colided_block;
    horizontal_colide = get_horizontal_colide( &hero, columns, rows, &colided_block );
    down_colide = get_down_colide( &hero, columns, rows, &colided_block, &air_born );
    up_colide = get_up_colide( &hero, columns, rows );
    colision( &hero, horizontal_colide, down_colide, up_colide, &air_born, colided_block );

    if (!flying) gravity( &hero, down_colide );
    if (!flying && !air_born ) controls( &treasure_stop, &hero, &air_born );  
    else fly( &hero);

    sprite_step( hero );
    sprite_step( treasure );
    blocks_step( columns, rows );

    if (timer_expired(game_timer)) time ++;
    if (lives < 0) show_game_over(score, time);
}




// Clean up at end of game.
void cleanup( void ) {
	// Insert statements to release resources such as files, memory.
}

// Program entry point.
int main( void ) {
    srand(get_current_time());
	setup_screen();  // Call ZDK function to prepare screen.
	

    
    setup(); // Call our setup function to initialise game state.
	show_screen();   // Display the screen at the start of play.

	// Following block will repeat until game_over becomes true.
	while ( ! game_over ) {
        clear_screen();
		process(); // Call our setup function to initialise game state.            // Execute one time step of game.
		show_screen();        // Show user the results.
		timer_pause( DELAY ); // Let CPU rest a short time.
	}

	// Game over is now true, so clean up and exit.
	cleanup();

	return 0;
}