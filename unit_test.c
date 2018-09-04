#include <math.h>
#include "jump_dude.h"
#include <string.h>
#define PAUSE (0)

void clear_blocks(game_sprite in[20][10]) {
    for (int c = 0; c < 20; c++) { for (int r = 0; r < 10; r++){
        in[c][r].sprite =  sprite_create(0,0,1,1,"");
        in[c][r].type =  0;
    }}
}

bool max_block_test(game_sprite test1[20][10]) {
    // Tests if there are no more than 160 safe blocks and 40 forbiden blocks
    int forbinden_blocks = 0, safe_blocks = 0;
    for (int column = 0; column < 20; column ++) { 
        for (int row = 0; row < 10; row ++) {
            if (test1[column][row].type == 1) safe_blocks ++;
            else if (test1[column][row].type == 2) forbinden_blocks ++;
        }
    }
    printf("There are %d safe and %d forb blocks\n", safe_blocks, forbinden_blocks);
    if (safe_blocks <= 160 && forbinden_blocks <= 40) return true;

    else return false;
}

bool min_block_test(game_sprite test1[20][10]) {
    // Tests if there are no more than 160 safe blocks and 40 forbiden blocks
    int forbinden_blocks = 0, safe_blocks;
    for (int column = 0; column < 20; column ++) { 
        safe_blocks = 0;
        for (int row = 0; row < 10; row ++) {
            if (test1[column][row].type == 1) safe_blocks ++;
            else if (test1[column][row].type == 2) forbinden_blocks ++;
        }
        if (safe_blocks < 1) {
            printf("Insufficient safe Blocks");
            return false;
        }
    }
    if (forbinden_blocks < 2) {
        printf("wrong number of forb blocks");
        return false;
    }
    else return true;
}

bool width_test(game_sprite test1[20][10]) {
    // tests if the max width is no less than 5 and no more than 10
    int max_width = 4, min_width = 11;
    for (int column = 0; column < 20; column ++) { 
        for (int row = 0; row < 10; row ++) {
            int width = test1[column][row].sprite->width;
            if (width > max_width) max_width = width;
            else if (width < min_width) min_width = width;
        }
    }
    if (min_width <= 5 && max_width <= 10)  {
        printf("Min width: %d, max width: %d\n", min_width, max_width);
        return true;
    }
    else  return false;
}

bool accelerate_blocks_test(game_sprite test1[20][10]) {
    // Tests if the speed of consecutive rows are opposite direction
    for (int row = 1; row < 8; row ++) {
        double diff = (test1[1][row].sprite->dx) / (test1[1][row+1].sprite->dx);
        if (diff  > 0 && isfinite(diff)) return false ;
    }
    
    return true;
}


bool respawn_test(game_sprite control[20][10]) {
    // Allows tester to see if the respawn is acting appropriatly
    sprite_id hero = sprite_create(10,10,3,3,hero_stat);
    draw_formatted(screen_width()/2, 2,"press any key to call the respawn function");
    draw_formatted(screen_width()/2, 2,"Press 'p' if passed, else press 'f'");
    while (true) {
        respawn(hero, control);
        draw_grid(control);
        sprite_draw(hero);
        show_screen();
        char a = wait_char();
        if ( a == 'p')  return true;
        else if (a == 'f') return false;
    }
}

bool time_test(int input, char *expected) {
    // Tests if time makes the right output
    char time_message[100];
    the_time_pls(time_message, input);
    if (strcmp(time_message,expected) == 0) return true;
    else return false;
}

bool show_game_over_test() {
    // Allows tester to observe game over screen
    show_game_over(39,50);
    if (game_quit) draw_formatted(5,5,"Game over True; Pass [p] or fail [f]");
    else draw_formatted(5,5,"Game over false;  Pass [p] or fail [f]");
    show_screen();
    while (true){
        char a = wait_char();
        if ( a == 'p') return true;
        else if (a == 'f') return false;
    }
    return false;
}

bool on_the_top_test(int side, int x1, int x2) {
    // Tests colsion by moving one block on top of another
    sprite_id test1 = sprite_create( x1, 5, 3,3,"ttttttttttt");
    sprite_id test2 = sprite_create( x2, 15,3,3,"bbbbbbbbbbb");
    sprite_draw(test1);
    sprite_draw(test2);
    show_screen();
    if (side == 0) test1->dy = 0.5;
    else test2->dy = -0.5;
    int count = 0;
    while (!on_top_of(test1,test2,0) && count < 20) {
        clear_screen();
        sprite_step(test1);
        sprite_step(test2);
        sprite_draw(test1);
        sprite_draw(test2);
        show_screen();
        timer_pause(1);
        count ++;
    }
    timer_pause(PAUSE);
    if (side == 0 && round(test1->y) == 15-3) return true;
    if (side == 1 && round(test2->y) == 5+3) return true;
    return false;
}

bool to_the_side_test(int side, int y1, int y2) {
    // Tests collision by moving one block beside another
    
    sprite_id test1 = sprite_create( 5, y1, 3,3,"lllllllll");
    sprite_id test2 = sprite_create( 15, y2,3,3,"rrrrrrrrr");
    sprite_draw(test1);
    sprite_draw(test2);
    show_screen();
    if (side == 0) test1->dx = 0.5;
    else test2->dx = -0.5;
    int count = 0;
    while (!to_the_side(test1,test2) && count < 20) {
        clear_screen();
        sprite_step(test1);
        sprite_step(test2);
        sprite_draw(test1);
        sprite_draw(test2);
        show_screen();
        timer_pause(1);
        count ++;
    }
    
    timer_pause(PAUSE);
    if (side == 0 && round(test1->x) == 15-3) return true;
    if (side == 1 && round(test2->x) == 5+3) return true;

    return false;
}

bool draw_game_test(game_sprite control[20][10]) {
    draw_game(10, 2, 89, sprite_create(10,10,3,3,"###########"), sprite_create(13,10,3,3,"*********"), control );
    draw_formatted(5,6,"Pass [p] or fail [f]");
    show_screen();
    while (true){
        char a = wait_char();
        if ( a == 'p') return true;
        else if (a == 'f') return false;
    }
    return false;

}



int main() {
    srand(get_current_time());
    game_sprite test1[20][10];
    clear_blocks(test1);
    game_sprite control[20][10] = {NULL};
    for( int c = 0; c < 20;c++) {
        for( int r = 0; r < 10;r++){
            int block_type = (c+r + 1) % 3;
            control[c][r].sprite = sprite_create(c * 11, row_height(r), 10,2,get_type_image(block_type));
            control[c][r].type = block_type;
        }
    }

    bool time_res = time_test(86, "Time: 01:26");
    bool max_res = true, mk_blc_res = true, min_block_res = true, acc_blc_res = true;

    for (int i = 0; i < 100; i++) {
        make_blocks(test1,20,10);
        max_res = max_res && width_test(test1);


        clear_blocks(test1);
        make_blocks(test1,20,10);
        mk_blc_res = mk_blc_res && max_block_test(test1);
        min_block_res = min_block_res && min_block_test(test1);

        setup_screen();
        
        clear_blocks(test1);
        make_blocks(test1,20,10);
        accelerate_blocks(test1, 10, 20);
        cleanup_screen();
        acc_blc_res = acc_blc_res && accelerate_blocks_test(test1);

    }
    setup_screen();
    
    bool top_res = !on_the_top_test(0,5,8) && on_the_top_test(0,5,7) && !on_the_top_test(0,8,5) && on_the_top_test(0,7,5);
    bool bot_res = !on_the_top_test(1,5,8) && on_the_top_test(1,5,7) && !on_the_top_test(1,8,5) && on_the_top_test(1,7,5);

    bool left_res = !to_the_side_test(0, 5,8) && to_the_side_test(0, 5,7) && !to_the_side_test(0, 8,5) && to_the_side_test(0, 7,5);
    bool right_res = !to_the_side_test(1, 5,8) && to_the_side_test(1, 5,7) && !to_the_side_test(1, 8,5) && to_the_side_test(1, 7,5);


    clear_screen();

    bool res_res = respawn_test(control);
    bool game_ovr_res = show_game_over_test();


    bool draw_game_res = draw_game_test(control);


    cleanup_screen();

    printf("\n");

    if (time_res) printf("Time: PASSED\n");
    else printf("Time: FAILED\n");

    if (max_res) printf("Min max: PASSED\n");
    else printf("Min max: FAILED\n");

    if (mk_blc_res) printf("Make blocks test: PASSED\n");
    else printf("Make blocks test: FAILED\n");

    if (top_res) printf("Top: PASSED\n");
    else printf("Top: FAILED\n");

    if (bot_res) printf("Bot: PASSED\n");
    else printf("Bot: FAILED\n");

    if (left_res) printf("Left: PASSED\n");
    else printf("Left: FAILED\n");

    if (right_res) printf("Right: PASSED\n");
    else printf("Right: FAILED\n");

    if (acc_blc_res) printf("Accelerate Blocks: PASSED\n");
    else printf("Accel blocks: FAILED\n");

    if (game_ovr_res) printf("Game Over: PASSED\n");
    else printf("Game Over: FAILED\n");

    if (res_res) printf("Respawn: PASSED\n");
    else printf("Respawn: FAILED\n");

    if (draw_game_res) printf("Draw Game: PASSED\n");
    else printf("Draw Game: FAILED\n");

    if (min_block_res) printf("Min Block Test: PASSED\n");
    else printf("Min Block Test: FAILED\n");


    return 0;
}