#include <graphics.h>
#include <math.h>
#include <sprites.h>
#include <stdlib.h>
#include <string.h>
#include <timers.h>

#define DELAY (10)
#define GAME_HEIGHT (6)
#define MAX_WIDTH (10)

// Defining the structs
typedef struct game_sprite {
    sprite_id sprite;
    int type;
} game_sprite;

// Defining global variables

char* jump_dude_right =
    " 0 "
    "/|/"
    " /\\";

char* jump_dude_left =
    " 0 "
    "\\|\\"
    "/\\ ";

char* jump_dude_stat =
    " 0 "
    "[|]"
    "/ \\";

char* jump_dude_falling =
    "\\0/"
    " | "
    "/ \\";

char* treasure_closed =
    "   "
    "#T#"
    "###";

char* treasure_open =
    "#T#"
    "+2 "
    "###";

char* safe_block =
    "=========="
    "==========";

char* danger_block =
    "xxxxxxxxxx"
    "xxxxxxxxxx";

char* no_block =
    "          "
    "          ";

bool game_over = false;
bool game_quit = false;
bool testing = false;
int columns, rows;

void clear_input_buffer() {
    while (get_char() >= 0) {
    }
}

// Defining game functions
int get_columns() {
    if (screen_width() / (11) < 50)
        return screen_width() / 11;
    else
        return 50;
}

int get_rows() {
    if ((screen_height() - 6) / 10 < 10)
        return (screen_height() - 6) / 10;
    else
        return 10;
}

char* get_type_image(int type) {
    // Returns the character coresponding to the interger type.
    // 0 is no block, 1 is safe_block and 2 is danger_block.
    if (type == 1)
        return safe_block;
    else if (type == 2)
        return danger_block;
    else
        return no_block;
}

int row_height(int row) { return GAME_HEIGHT + 8 * (row + 1) + 2 * row; }

int rand_type() {
    int block_type = rand() % 101;
    if (block_type <= 30)
        return 0;
    else if (block_type > 30 && block_type <= 45)
        return 2;
    else
        return 1;
}

bool make_column(int column, int rows, int* safe_blocks, int* danger_blocks, game_sprite blocks[columns][rows]) {
    // Function to create an individual column, return the number of danger
    // blocks created in the column
    int block_type, row_safe_blocks, row_danger_blocks, x, width;
    do {
        row_safe_blocks = 0;
        row_danger_blocks = 0;
        for (int row = 0; row < rows; row++) {
            if (*danger_blocks + row_danger_blocks > 40) {
                if (*safe_blocks + row_safe_blocks > 160) return true;
                block_type = 1;
            } else if (*safe_blocks + row_safe_blocks > 160)
                block_type = 2;
            else
                block_type = rand_type();

            width = rand() % 6 + 5;

            if (block_type == 1)
                row_safe_blocks++;
            else if (block_type == 2)
                row_danger_blocks += 1;
            if (column == 0)
                x = 11 * column;
            else
                x = 1 + sprite_x(blocks[column - 1][row].sprite) + sprite_width(blocks[column - 1][row].sprite);
            blocks[column][row].type = block_type;
            blocks[column][row].sprite = sprite_create(x, row_height(row), width, 2, get_type_image(block_type));
        }
    } while (row_safe_blocks < 1);
    *safe_blocks += row_safe_blocks;
    *danger_blocks += row_danger_blocks;
    return false;
}

void make_blocks(game_sprite blocks[columns][rows], int columns, int rows) {
    // Populates the blocks array with game_sprite structs to be used as the
    // platforms that the jump_dude jumps on.
    int danger_blocks = 0, safe_blocks = 0;
    bool block_limit = false;
    while (danger_blocks < 2) {
        for (int column = 0; column < columns && !block_limit; column++) {
            block_limit = make_column(column, rows, &safe_blocks, &danger_blocks, blocks);
        }
    }
    sprite_set_image(blocks[0][0].sprite,
                     "sssssssssss"
                     "sssssssssss");
    blocks[0][0].type = 1;
}

void accelerate_blocks(game_sprite blocks[columns][rows], int rows, int columns) {
    // Sets the speed of each row
    int direction = 2 * (rand() % 2) - 1;
    direction = 2;
    double speed;
    for (int row = 1; row < rows - 1; row++) {
        speed = (rand() % 2 + 1) / 15.0;
        direction *= -1;
        for (int column = 0; column < columns; column++) {
            sprite_turn_to(blocks[column][row].sprite, direction * speed,
                           0);  // debug
        }
    }
}

void respawn_animation(sprite_id jump_dude) {
    // A nice animation when jump dude respawns
    draw_string(jump_dude->x, jump_dude->y + 3, "/ \\");
    show_screen();
    timer_pause(100);

    draw_string(jump_dude->x, jump_dude->y + 2, "[|]");
    show_screen();
    timer_pause(100);

    draw_string(jump_dude->x, jump_dude->y + 1, " 0 ");
    show_screen();
    timer_pause(100);
    sprite_set_image(jump_dude, jump_dude_stat);
}

void respawn(sprite_id jump_dude, game_sprite blocks[columns][rows]) {
    int spawn_column;
    game_sprite spawn_block;

    do {
        spawn_column = rand() % (columns + 1);
        spawn_block = blocks[spawn_column][0];
    } while (spawn_block.type != 1);

    for (int i = 0; i < 4; i++) draw_string(jump_dude->x, jump_dude->y + i, "     ");
    show_screen();

    timer_pause(150);

    sprite_move_to(jump_dude, spawn_block.sprite->x + 1, row_height(0) - 3.5);
    respawn_animation(jump_dude);
    sprite_turn_to(jump_dude, 0, 0);
    clear_input_buffer();
}

void get_time_message(char message[100], int time) {
    // Takes the time in seconds and stores it as a character array in the
    // Form of mm:ss
    int seconds = time % 60;
    int minutes = time / 60;
    sprintf(message, "Time: %d%d:%d%d", minutes / 10, minutes % 10, seconds / 10, seconds % 10);
}

void gravity(sprite_id jump_dude, bool on_block) {
    if (on_block)
        jump_dude->dy = 0;
    else
        jump_dude->dy += 0.01;
}

void wait_for_responce() {
    char key;
    while (true) {
        key = wait_char();
        if (key == 'q') {
            game_quit = true;
            return;
        } else if (key == 'r') {
            game_quit = false;
            return;
        }
    }
}

void show_game_over(int score, int time) {
    game_over = true;
    char time_message[100];
    get_time_message(time_message, time);
    clear_screen();
    draw_formatted((screen_width() - 34) / 2, screen_height() / 2 - 1, "So long an thanks for all the fish");
    draw_formatted((screen_width() - 34) / 2, screen_height() / 2 + 1, "Score: %d", score);
    draw_formatted((screen_width() - 34) / 2, screen_height() / 2 + 2, time_message);
    draw_formatted((screen_width() - 34) / 2, screen_height() / 2 + 4, "[R]: Restart   [Q]: Quit");
    show_screen();
    wait_for_responce();
}

void face_direction(sprite_id jump_dude, game_sprite colided_block) {
    if (round(jump_dude->dx) == round(colided_block.sprite->dx)) sprite_set_image(jump_dude, jump_dude_stat);
    if (jump_dude->dx - colided_block.sprite->dx > 0)
        sprite_set_image(jump_dude, jump_dude_right);
    else if (jump_dude->dx - colided_block.sprite->dx < 0)
        sprite_set_image(jump_dude, jump_dude_left);
}

void controls(bool* treausre_stop, sprite_id jump_dude, bool* air_born, game_sprite colided_block) {
    char key = 0;
    key = get_char();
    if (key == 'w') {
        jump_dude->dy = -0.32;
        sprite_step(jump_dude);
    }
    if (key == 'a' && jump_dude->dx - colided_block.sprite->dx > -0.2) jump_dude->dx -= 0.3;
    if (key == 'd' && jump_dude->dx - colided_block.sprite->dx < 0.2) jump_dude->dx += 0.3;
    if (key == 't') *treausre_stop = !*treausre_stop;
    if (key) face_direction(jump_dude, colided_block);
}

void draw_grid(game_sprite blocks[columns][rows]) {
    for (int column = 0; column < columns; column++) {
        for (int row = 0; row < rows; row++) {
            sprite_draw(blocks[column][row].sprite);
        }
    }
}

void draw_game(int score, int lives, int time, sprite_id jump_dude, sprite_id treasure,
               game_sprite blocks[columns][rows]) {
    clear_screen();
    char time_message[100];
    get_time_message(time_message, time);
    draw_line(0, 5, screen_width() - 1, 5, '-');
    draw_formatted((screen_width() - 10) * 1 / 5, 2, "n9989773");
    draw_formatted((screen_width() - 10) * 2 / 5, 2, "Score: %d", score);
    draw_formatted((screen_width() - 10) * 3 / 5, 2, time_message);
    draw_formatted((screen_width() - 10) * 4 / 5, 2, "Lives: %d", lives);
    draw_grid(blocks);
    sprite_draw(jump_dude);
    sprite_draw(treasure);
    show_screen();
}

bool horizontal_edge_detect(sprite_id sprite, int offset) {
    return (sprite->x < -offset || sprite->x + sprite->width > screen_width() + offset);
}

bool top_detect(sprite_id sprite, int offset) { return sprite->y < offset + GAME_HEIGHT; }

bool bottom_detect(sprite_id sprite, int offset) { return sprite->y + sprite->height > screen_height() + offset; }

// Handles all edge detection of treasure and sprite
void edge_detect(sprite_id jump_dude, sprite_id treasure, game_sprite blocks[columns][rows], int* lives) {
    if (bottom_detect(jump_dude, 0)) {
        *lives -= 1;
        respawn(jump_dude, blocks);
    }
    if (horizontal_edge_detect(jump_dude, jump_dude->width)) {
        *lives -= 1;
        respawn(jump_dude, blocks);
    }

    if (horizontal_edge_detect(treasure, -1)) {
        treasure->dx = -treasure->dx;
    }
}

void blocks_step(game_sprite blocks[columns][rows]) {
    for (int column = 0; column < columns; column++) {
        for (int row = 1; row < rows - 1; row++) {
            sprite_id sprite = blocks[column][row].sprite;
            if (sprite->x < 0 && sprite->dx < 0)
                sprite->x = screen_width();
            else if (sprite->x + sprite->width > screen_width() && sprite->dx > 0)
                sprite->x = -sprite->width + 8;
            sprite_step(sprite);
        }
    }
}

bool to_the_side(sprite_id sprite_1, sprite_id sprite_2) {
    if (sprite_1->y < sprite_2->y + sprite_2->height && sprite_1->y + sprite_1->height > sprite_2->y) {
        return (sprite_1->x + sprite_1->width >= sprite_2->x && sprite_1->x <= sprite_2->x + sprite_2->width);
    } else
        return false;
}

bool on_top_of(sprite_id sprite_1, sprite_id sprite_2, int correction) {
    return (round(sprite_1->y + sprite_1->height) == round(sprite_2->y) &&
            sprite_1->x + sprite_1->width + correction > sprite_2->x &&
            sprite_1->x < sprite_2->x + sprite_2->width + correction);
}

void open_treasure(int* lives, sprite_id jump_dude, sprite_id treasure, game_sprite blocks[columns][rows]) {
    sprite_set_image(treasure, treasure_open);
    for (int i = 0; i < 3; i++) draw_string(treasure->x, treasure->y + i, "   ");

    sprite_draw(treasure);
    show_screen();
    timer_pause(1000);
    sprite_hide(treasure);
    sprite_move_to(treasure, 0, 0);
    *lives += 2;
    respawn(jump_dude, blocks);
}

game_sprite get_collisions(bool* down_colide, bool* up_colide, bool* horizontal_colide, sprite_id jump_dude,
                           game_sprite blocks[columns][rows]) {
    for (int c = 0; c < columns; c++) {
        for (int r = 0; r < rows; r++) {
            if (on_top_of(jump_dude, blocks[c][r].sprite, 0) && blocks[c][r].type != 0) {
                *down_colide = true;
                return blocks[c][r];
            }
            if ((top_detect(jump_dude, 0) || on_top_of(blocks[c][r].sprite, jump_dude, -1)) && blocks[c][r].type != 0 &&
                jump_dude->dy < 0) {
                *up_colide = true;
                return blocks[c][r];
            }

            if (to_the_side(jump_dude, blocks[c][r].sprite) && blocks[c][r].type != 0) {
                *horizontal_colide = true;
                return blocks[c][r];
            }
        }
    }
    return blocks[0][0];
}

sprite_id land(int* score, sprite_id jump_dude, sprite_id prev_colided, game_sprite* colided_block) {
    if (!sprites_equal(colided_block->sprite, prev_colided)) *score += 1;
    jump_dude->dx = colided_block->sprite->dx;
    sprite_set_image(jump_dude, jump_dude_stat);
    return colided_block->sprite;
}

void colision(sprite_id jump_dude, sprite_id treasure, int* lives, int* score, bool* air_born, bool* down_colide,
              game_sprite blocks[columns][rows], game_sprite* colided_block) {
    bool up_colide = false, horizontal_colide = false;
    static sprite_id prev_colided;
    *down_colide = false;
    *colided_block = get_collisions(down_colide, &up_colide, &horizontal_colide, jump_dude, blocks);
    if (*down_colide) {
        if (colided_block->type == 2 || horizontal_edge_detect(colided_block->sprite, 0)) {
            *lives -= 1;
            respawn(jump_dude, blocks);
        } else {
            jump_dude->dy = 0;
            if (*air_born) {
                prev_colided = land(score, jump_dude, prev_colided, colided_block);
                *air_born = false;
            }
        }
    } else
        *air_born = true;
    if (to_the_side(treasure, jump_dude)) open_treasure(lives, jump_dude, treasure, blocks);
    if (up_colide) jump_dude->dy = 0;
    if (horizontal_colide) jump_dude->dx = 0;
}

void cleanup(sprite_id jump_dude, sprite_id treasure, game_sprite blocks[columns][rows], timer_id* game_timer,
             timer_id* treasure_timer) {
    sprite_destroy(jump_dude);
    sprite_destroy(treasure);
    for (int c = 0; c < columns; c++) {
        for (int r = 0; r < rows; r++) {
            sprite_destroy(blocks[c][r].sprite);
        }
    }
    destroy_timer(*game_timer);
    destroy_timer(*treasure_timer);
}

//-------------------------------------------------------------------------------------------------
void setup(bool* treasure_stop, bool* air_born, int* score, int* lives, int* time, timer_id* treasure_timer,
           timer_id* game_timer, sprite_id* jump_dude, sprite_id* treasure, sprite_id* prev_colided,
           game_sprite blocks[columns][rows]) {
    // Calls all the functions needed to set up the game and set the global
    // variables.
    *air_born = false;
    *score = 0;
    *lives = 10;
    *time = 0;
    *treasure_stop = false;
    *jump_dude = sprite_create(2, row_height(0) - 3.5, 3, 3, jump_dude_stat);
    *treasure = sprite_create(2, row_height(rows - 1) + 2, 3, 3, treasure_closed);
    *treasure_timer = create_timer(500);
    *game_timer = create_timer(1000);
    make_blocks(blocks, columns, rows);
    accelerate_blocks(blocks, rows, columns);
    sprite_turn_to(*treasure, 0.2, 0);
    *prev_colided = blocks[0][0].sprite;
}

void process(bool treasure_stop, bool air_born, int score, int lives, int time, timer_id treasure_timer,
             timer_id game_timer, sprite_id jump_dude, sprite_id treasure, sprite_id prev_colided,
             game_sprite blocks[columns][rows]) {
    while (!game_over) {
        bool down_colide;
        game_sprite colided_block;

        colision(jump_dude, treasure, &lives, &score, &air_born, &down_colide, blocks, &colided_block);
        edge_detect(jump_dude, treasure, blocks, &lives);
        gravity(jump_dude, down_colide);
        if (!air_born)
            controls(&treasure_stop, jump_dude, &air_born, colided_block);
        else {
            sprite_set_image(jump_dude, jump_dude_falling);
            clear_input_buffer();
        }
        sprite_step(jump_dude);
        if (!treasure_stop) sprite_step(treasure);
        blocks_step(blocks);

        if (timer_expired(game_timer)) time++;
        if (lives < 0) show_game_over(score, time);

        draw_game(score, lives, time, jump_dude, treasure, blocks);
        timer_pause(DELAY);
    }
}

int main(int argc, char const* argv[]) {
    srand(get_current_time());
    setup_screen();
    columns = get_columns(), rows = get_rows();
    bool treasure_stop, air_born;
    int score, lives, time;
    timer_id treasure_timer, game_timer;
    sprite_id jump_dude, treasure, prev_colided;
    game_sprite blocks[columns][rows];

    while (!game_quit) {
        game_over = false;
        setup(&treasure_stop, &air_born, &score, &lives, &time, &treasure_timer, &game_timer, &jump_dude, &treasure,
              &prev_colided, blocks);

        show_screen();
        process(treasure_stop, air_born, score, lives, time, treasure_timer, game_timer, jump_dude, treasure,
                prev_colided, blocks);

        cleanup(jump_dude, treasure, blocks, &game_timer, &treasure_timer);
    }
}
