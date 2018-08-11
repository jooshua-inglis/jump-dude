#include <stdlib.h>
#include <cab202_graphics.h>
#include <cab202_sprites.h>
#include <cab202_timers.h>
#include <math.h>


// Include additional headers here

// Constants used to configure game
#define DELAY (10) /* Millisecond delay between game updates */
#define GAME_HEIGHT (6)
#define BLOCK_SPACING (2)

// Add additional constants here

// Variables used to store the game’s dynamic state.
bool game_over = false; /* Set this to true when game is over */

// Define additional global variables here.
int score, lives, time;
int hero_column, hero_row;
int treasure_column, treasure_row, treasure_dx;
bool treausre_stop = false;
int grid[30][30], columns, rows;


char * hero_image = " 0  "
                    " |//"
                    " /\\  ";

char * treasure_image = "##T##"
                        "#####";

sprite_id hero;
sprite_id treasure;
timer_id treasure_timer, game_timer;
// Setup game.
void draw_block(int column, int row, char image) {

    int x = (7 + BLOCK_SPACING)* column;
    int y = (row + 1) * 6 + GAME_HEIGHT + row * 2;

    int w = 7;
    int h = 2;

    draw_line(x, y, x+w-1, y, image);
    draw_line(x, y, x, y+h-1, image);
    draw_line(x+w-1, y, x+w-1, y+h-1, image);
    draw_line(x, y+h-1, x+w-1, y+h-1, image);  
}    

void move_hero(int column, int row){
    hero_column = column, hero_row = row;
    int x = (7 + BLOCK_SPACING) * column + 1;
    int y = (row + 1) * 6 + GAME_HEIGHT + row * 2 - 3;
    sprite_move_to(hero,x,y);
}

void move_treasure(int column, int row){
    treasure_column = column, treasure_row = row;
    int x = (7 + BLOCK_SPACING) * column + 1;
    int y = (row + 1) * 6 + GAME_HEIGHT + row * 2 - 2;
    sprite_move_to(treasure,x,y);
}

void the_time_pls(char message[100]) {
    int seconds = time % 60;
    int minutes = time / 60;
    sprintf(message,"Time: %d%d:%d%d", minutes/10, minutes%10, seconds/10, seconds%10);
}


void update_grid(int column, int row, int type) {
    grid[column][row] = type;
}

char get_type_image(int type){
        if (type == 2 || type == 3) {
            return 'x';
        }
        else {
            return '=';
        }
}

void draw_grid() {
    for (int column = 0; column < columns; column++) {
        for (int row = 0; row < 3; row++) {
            if (grid[column][row]) {
                draw_block(column, row, get_type_image(grid[column][row]));
            }
        }
    }
}

void make_column(int column) {
    int rows = 3;
    int block_type;
    int safe_blocks;
    do{
        safe_blocks = 0;
        for (int row = 0; row < rows ; row ++){
            if (grid[column][row] != 3){
                block_type  = rand() % 3;
                if (block_type == 1){
                    safe_blocks ++;
                }
                update_grid(column, row, block_type);
            }
        }
    } while(safe_blocks < 0);
}

void draw_game(){
    char time_message[100];
    the_time_pls(time_message);
    draw_line(0,5, screen_width()-1,5, '-');
    draw_formatted( ( screen_width() - 10 ) * 1/5 , 2 , "n9989773" );
    draw_formatted( ( screen_width() - 10 ) * 2/5 , 2 , "Score: %d",score );
    draw_formatted( ( screen_width() - 10 ) * 3/5 , 2 ,  time_message);
    draw_formatted( ( screen_width() - 10 ) * 4/5 , 2 , "Lives: %d", lives );
    draw_grid();
    sprite_draw(hero);
    sprite_draw(treasure);
}

void setup( void ) {
    //HUD Section =========================================================
    columns = round(screen_width() / 9.0) ;
    rows = 3;
    score = 0;
    lives = 10;
    time = 0;
    game_timer = create_timer(1000);
    
    int danger_x_1, danger_x_2, danger_y_1, danger_y_2;
    danger_x_1 = rand()%columns;
    danger_x_2 = rand()%columns;    
    do {
        danger_y_1 = rand()%3;
        danger_y_2 = rand()%3;

    } while (danger_y_1 == danger_y_2);

    update_grid(danger_x_1, danger_y_1, 3);
    update_grid(danger_x_2, danger_y_2, 3);
    
    for (int column = 0; column < columns; column ++){
        make_column(column);
    }
    update_grid(0,0,1);
    hero = sprite_create(0,0,4,3,hero_image);
    treasure = sprite_create(0,0,5,2, treasure_image);
    treasure_dx = 1;
    treasure_timer = create_timer(500);
    move_hero(0,0);
    move_treasure(0,2);
      
}




void block_checker() {
    if (grid[hero_column][hero_row] == 1) score ++;
    else if (grid[hero_column][hero_row] >= 2) {
        lives --;
        move_hero(0,0);
    }

    else {
        hero_row ++;
        move_hero(hero_column, hero_row);
        block_checker();
    }
}

void show_game_over() {
    bool game_over_screen = true;
    char key;
    char time_message[100];
    the_time_pls(time_message);
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

// Play one turn of game.
void process( void ) {
    char key = get_char();
    if ( key == 'a' && hero_column !=0 ){
        move_hero(hero_column - 1, hero_row);
        block_checker();
    } else if ( key == 'd' && hero_column != columns-1 ){
        move_hero(hero_column + 1, hero_row);
        block_checker();
    } else if ( key == 't' ){
        treausre_stop = !treausre_stop;
    }
    
    if (timer_expired(treasure_timer) && !treausre_stop) {
        
        move_treasure(treasure_column + treasure_dx, 2);
        if (treasure_column == 0 || treasure_column == columns-1) treasure_dx = - treasure_dx;
    }

    if (treasure_column == hero_column && treasure_row == hero_row) {
        lives += 2;
        move_hero(0,0);
    }

    if (lives < 0 || key == 'y') {
        show_game_over();
    }
    if (timer_expired(game_timer)) time ++;

    draw_game();
}


// Clean up at end of game.
void cleanup( void ) {
	// Insert statements to release resources such as files, memory.
}

// Program entry point.
int main( void ) {
    srand(get_current_time());
	setup_screen();  // Call ZDK function to prepare screen.
	setup();         // Call our setup function to initialise game state.
	show_screen();   // Display the screen at the start of play.

	// Following block will repeat until game_over becomes true.
	while ( ! game_over ) {
        clear_screen();
		process();            // Execute one time step of game.
		show_screen();        // Show user the results.
		timer_pause( DELAY ); // Let CPU rest a short time.
	}

	// Game over is now true, so clean up and exit.
	cleanup();

	return 0;
}