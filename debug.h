bool flying = false;

void fly( sprite_id * hero ) {
    // This is used for debugging perposes, if this is still here, please ignore.

    char key = get_char();
    if (key == 'w')sprite_turn_to( *hero, sprite_dx(*hero),sprite_dy(*hero) -0.08);
    else if (key == 'd') sprite_turn_to( *hero, sprite_dx(*hero) +0.1,sprite_dy(*hero));
    else if (key == 'a') sprite_turn_to( *hero, sprite_dx(*hero) -0.1,sprite_dy(*hero));
    else if (key == 's') sprite_turn_to( *hero, sprite_dx(*hero),sprite_dy(*hero) +0.08);
}