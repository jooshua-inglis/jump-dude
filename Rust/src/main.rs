extern crate ncurses;

use log;
use rand::Rng;
use std::ptr;
use std::time;

mod graphics;
mod sprites;

static IMAGE: &str = "&&&&&&&&&";
const SAFE_BLOCK: &str = "======================";
const DANGER_BLOCK: &str = "xxxxxxxxxxxxxxxxxxxx";
const START_BLOCK: &str = "ssssssssssssssssssss";
const NO_BLOCK: &str = "                    ";
const TREASURE_CLOSED: &str = "   #T####";
const TREASURE_OPEN: &str = "#T#+2 ###";

const GAME_HEIGHT: usize = 6;
const BLOCK_SPEED: f64 = 0.1;

const MOVE_SPEED: f64 = 0.3;
const JUMP_SPEED: f64 = 0.32;
static BLOCK_HEIGHT: usize = 2;

struct Block<'l> {
    sprite: sprites::Sprite<'l>,
    kind: BlockKind,
}

struct Game<'l> {
    screen: graphics::Screen,
    character: Character<'l>,
    treasure: sprites::Sprite<'l>,
    score: u32,
    lives: u32,
    time_instant: time::Instant,
    blocks: Vec<Vec<Block<'l>>>,
    prev_colided_block: (usize, usize),
}

struct Character<'l> {
    pub sprite: sprites::Sprite<'l>,
    pub airborne: bool,
}

enum BlockKind {
    Empty,
    Danger,
    Safe,
    Start,
}

enum Direction {
    Up,
    Down,
    Side,
    None,
}

fn create_character<'l>() -> Character<'l> {
    return Character {
        sprite: sprites::create_sprite(0.0, row_height(0) as f64 - 3.5, 3, 3, IMAGE),
        airborne: false,
    };
}

fn get_columns() -> usize {
    log::info!("{}", graphics::screen_width());
    let cols = graphics::screen_width() / 11;
    return std::cmp::min(cols, 50);
}

fn get_rows() -> usize {
    log::info!("{}", graphics::screen_height());
    let rows = (graphics::screen_height() - 6) / 10;
    return std::cmp::min(rows, 10);
}

fn rand_type() -> BlockKind {
    let mut rng = rand::thread_rng();
    let y: f64 = rng.gen();

    let t = (y * 2.0).round() as i32;

    if t == 0 {
        return BlockKind::Empty;
    }
    if t == 1 {
        return BlockKind::Safe;
    } else {
        return BlockKind::Danger;
    }
}

fn sprite_for_block_kind<'a>(kind: &BlockKind, width: usize) -> &'a str {
    let bitmap: &str;
    if matches!(kind, BlockKind::Empty) {
        bitmap = NO_BLOCK;
    } else if matches!(kind, BlockKind::Safe) {
        bitmap = SAFE_BLOCK;
    } else if matches!(kind, BlockKind::Danger) {
        bitmap = DANGER_BLOCK;
    } else if matches!(kind, BlockKind::Start) {
        bitmap = START_BLOCK;
    } else {
        std::panic!("Invliad block kind");
    }

    return &bitmap[0..width * 2];
}

fn rand_int(min: i32, max: i32) -> i32 {
    let mut rng = rand::thread_rng();
    let y: f64 = rng.gen();

    return min + (y * (max - min) as f64) as i32;
}

fn row_height(row: usize) -> i32 {
    return (GAME_HEIGHT + 8 * (row + 1) + 2 * row) as i32;
}

fn to_the_side(sprite_1: &sprites::Sprite, sprite_2: &sprites::Sprite) -> bool {
    let vertical_rectangles_intersect = sprite_1.y < sprite_2.y + sprite_2.height as f64
        && sprite_1.y + sprite_1.height as f64 > sprite_2.y;
    let horrizontal_rectangles_intersect = sprite_1.x + sprite_1.width as f64 >= sprite_2.x
        && sprite_1.x <= sprite_2.x + sprite_2.width as f64;

    return vertical_rectangles_intersect && horrizontal_rectangles_intersect;
}

fn on_top_of(sprite_1: &sprites::Sprite, sprite_2: &sprites::Sprite, correction: f64) -> bool {
    return sprite_1.y.round() + sprite_1.height as f64 == sprite_2.y.round()
        && sprite_1.x + sprite_1.width as f64 + correction > sprite_2.x
        && sprite_1.x < sprite_2.x + sprite_2.width as f64 + correction;
}

fn top_detect(sprite: &sprites::Sprite, offset: usize) -> bool {
    return sprite.y < (offset + GAME_HEIGHT) as f64;
}

fn bottom_detect(sprite: &sprites::Sprite, offset: i32) -> bool {
    return sprite.y + sprite.height as f64 > (graphics::screen_height() as i32 + offset) as f64;
}

fn horizontal_edge_detect(sprite: &sprites::Sprite, offset: i32) -> bool {
    return sprite.x < -offset as f64
        || (sprite.x + sprite.width as f64).round()
            >= (graphics::screen_width() as i32 + offset) as f64;
}

impl<'l> Game<'l> {
    fn make_column(
        &mut self,
        column: usize,
        rows: usize,
        n_safe: usize,
        n_danger: usize,
    ) -> (usize, usize) {
        // Function to create an individual column, return the number of danger
        // blocks created in the column
        let mut block_type: BlockKind;
        let mut row_safe_blocks;
        let mut row_danger_blocks;
        let mut x: usize;
        let mut width: i32;

        let mut blocks: Vec<Block>;

        'outer: loop {
            row_safe_blocks = 0;
            row_danger_blocks = 0;
            blocks = vec![];
            for row in 0..rows {
                if n_danger + row_danger_blocks > 40 {
                    if n_safe + row_safe_blocks > 160 {
                        break 'outer;
                    }
                    block_type = BlockKind::Safe;
                } else if n_safe + row_safe_blocks > 160 {
                    block_type = BlockKind::Danger;
                } else {
                    block_type = rand_type();
                }

                width = rand_int(5, 11);

                if matches!(block_type, BlockKind::Safe) {
                    row_safe_blocks += 1;
                } else if matches!(block_type, BlockKind::Danger) {
                    row_danger_blocks += 1;
                }

                if column == 0 {
                    x = 11 * column;
                } else {
                    x = (1.0
                        + self.blocks[column - 1][row].sprite.width as f64
                        + self.blocks[column - 1][row].sprite.x) as usize;
                }
                blocks.push(Block {
                    sprite: sprites::create_sprite(
                        x as f64,
                        row_height(row) as f64,
                        width as usize,
                        BLOCK_HEIGHT,
                        sprite_for_block_kind(&block_type, width as usize),
                    ),
                    kind: block_type,
                })
            }

            if row_safe_blocks > 0 {
                break 'outer;
            }
        }
        self.blocks.push(blocks);
        return (row_safe_blocks, row_danger_blocks);
    }
    pub fn make_blocks(&mut self, columns: usize, rows: usize) {
        // Populates the blocks array with game_sprite structs to be used as the
        // platforms that the jump_dude jumps on.
        let mut danger_blocks = 0;
        let mut safe_blocks = 0;
        let block_limit = false;

        while danger_blocks < 2 {
            for column in 0..columns {
                if block_limit {
                    break;
                }
                let (row_safe_block, row_danger_block) =
                    self.make_column(column, rows, safe_blocks, danger_blocks);

                danger_blocks += row_danger_block;
                safe_blocks += row_safe_block;
            }
        }

        let block_0_width = self.blocks[0][0].sprite.width;
        self.blocks[0][0] = Block {
            sprite: sprites::create_sprite(
                0.0,
                row_height(0) as f64,
                block_0_width,
                2,
                sprite_for_block_kind(&BlockKind::Start, block_0_width),
            ),
            kind: BlockKind::Start,
        }
    }
    fn controls(&mut self) {
        let key = graphics::get_char();
        if key == ('w' as i32) {
            self.character.sprite.dy = MOVE_SPEED;
            self.character.sprite.step();
        }
        if key == 'a' as i32 {
            self.character.sprite.dx -= MOVE_SPEED;
        }
        if key == 'd' as i32 {
            self.character.sprite.dx += JUMP_SPEED;
        }
        // if (key == 't') *treausre_stop = !*treausre_stop;
        // if (key) face_direction(jump_dude, colided_block);
    }

    fn gravity(&mut self) {
        if self.character.airborne {
            self.character.sprite.dy += 0.01;
        } else {
            self.character.sprite.dy = 0.0;
        }
    }
    pub fn char_move(&mut self) {
        self.character.sprite.step();
        self.treasure.step();

        for row in &mut self.blocks {
            for block in row {
                block.sprite.step();
            }
        }
    }

    pub fn get_block_collisions(&self) -> Option<(&Block, Direction)> {
        for col in &self.blocks {
            for block in col {
                if matches!(block.kind, BlockKind::Empty) {
                    continue;
                }
                if on_top_of(&self.character.sprite, &block.sprite, 0.0) {
                    return Some((&block, Direction::Down));
                }
                if (on_top_of(&block.sprite, &self.character.sprite, -1.0))
                    && self.character.sprite.dy < 0.0
                {
                    return Some((&block, Direction::Down));
                }
                if to_the_side(&self.character.sprite, &block.sprite) {
                    return Some((&block, Direction::Side));
                }
            }
        }
        return None;
    }

    pub fn respawn(&mut self) {
        let mut spawn_column: usize;
        let mut spawn_block: &Block;

        loop {
            spawn_column = rand_int(0, self.blocks.len() as i32) as usize;
            spawn_block = &self.blocks[spawn_column][0];

            if matches!(spawn_block.kind, BlockKind::Safe) {
                break;
            }
        }

        for i in 0..4 {
            self.screen.draw_string(
                self.character.sprite.x as i32,
                self.character.sprite.y as i32 + i,
                String::from("    "),
            );
        }

        std::thread::sleep(std::time::Duration::new(0, 5 * 10u32.pow(8)));
        self.character.sprite.x = spawn_block.sprite.x + 1.0;
        self.character.sprite.y = row_height(0) as f64 - 3.5;
        self.character.sprite.dx = spawn_block.sprite.dx;
        self.character.sprite.dy = 0.0;
        self.character.airborne = false;

        // respawn_animation(jump_dude);
        // sprite_turn_to(jump_dude, 0, 0);
        // clear_input_buffer();
    }

    pub fn accelerate_blocks(&mut self) {
        // Sets the speed of each row
        let mut direction = -2.0;

        // double speed;
        for row in 0..self.blocks[0].len() {
            // let speed = (rand_int(1, 2) % 2 + 1) / 15.0;
            direction *= -1.0;
            for col in 0..self.blocks.len() {
                self.blocks[col][row].sprite.dx = BLOCK_SPEED * direction;
            }
        }
    }

    fn get_block_cords(&self, check: &Block) -> (usize, usize) {
        for (c, col) in self.blocks.iter().enumerate() {
            for (r, block) in col.iter().enumerate() {
                if ptr::eq(block, check) {
                    return (c, r);
                }
            }
        }
        std::panic!("No block found");
    }

    pub fn colision(&mut self) {
        match self.get_block_collisions() {
            Some((block, direction)) => {
                let block_cords = self.get_block_cords(block);
                if matches!(direction, Direction::Down) {
                    if (matches!(block.kind, BlockKind::Danger)
                        || horizontal_edge_detect(&block.sprite, 0))
                    {
                        self.lives -= 1;
                        self.respawn();
                    } else if self.character.airborne {
                        let prev_colided_block =
                            &self.blocks[self.prev_colided_block.0][self.prev_colided_block.1];
                        let mut score = false;
                        if !std::ptr::eq(block, prev_colided_block) {
                            score = true;
                        }
                        self.character.sprite.dx = block.sprite.dx;
                        if score {
                            self.score += 1;
                        }
                        self.character.airborne = false;
                    }
                    self.character.sprite.dy = 0.0;
                }
                if matches!(direction, Direction::Up) {
                    self.character.sprite.dy = 0.0;
                }
                if matches!(direction, Direction::Side) {
                    self.character.sprite.dx = 0.0;
                }
                self.prev_colided_block = block_cords;
            }
            None => {
                self.character.airborne = true;
            }
        }
        if top_detect(&self.character.sprite, 0) {
            self.character.sprite.dy = 0.0;
        }
        if bottom_detect(&self.character.sprite, -1) {
            self.lives -= 1;
            self.respawn();
        }
        // if (to_the_side(treasure, jump_dude)) { open_treasure(lives, jump_dude, treasure, blocks);}
    }

    pub fn treasure_colide(&mut self) {
        if self.treasure.is_visable && to_the_side(&self.character.sprite, &self.treasure) {
            self.open_treasure();
        }
    }

    pub fn open_treasure(&mut self) {
        self.treasure.bitmap = TREASURE_OPEN;
        let treasure = &mut self.treasure;
        for i in 0..treasure.height {
            self.screen.draw_line(
                treasure.x.round() as i32,
                treasure.y.round() as i32 + i as i32,
                treasure.x.round() as i32 + treasure.width as i32 - 1,
                treasure.y.round() as i32 + i as i32,
                ' ',
            );
        }
        treasure.draw(&mut self.screen);
        self.screen.render();
        std::thread::sleep(std::time::Duration::new(0, 5 * 10u32.pow(8)));
        treasure.hide();
        self.lives += 2;
        self.respawn();
    }

    pub fn draw(&mut self) {
        self.screen
            .draw_string(2, 2, format!("Lives={},Score={}", self.lives, self.score,));
        self.treasure.draw(&mut self.screen);
        self.character.sprite.draw(&mut self.screen);
        for row in &self.blocks {
            for block in row {
                block.sprite.draw(&mut self.screen);
            }
        }
    }
    pub fn edge_detect(&mut self) {
        if bottom_detect(&self.character.sprite, 0) {
            self.lives -= 1;
            self.respawn();
        }
        if horizontal_edge_detect(&self.character.sprite, 0) {
            self.lives -= 1;
            self.respawn();
        }

        if horizontal_edge_detect(&self.treasure, 0) {
            self.treasure.dx = -self.treasure.dx;
        }
    }
    pub fn block_overlap(&mut self) {
        for row in &mut self.blocks {
            for block in row {
                if block.sprite.x < 0.0 && block.sprite.dx < 0.0 {
                    block.sprite.x = self.screen.width as f64;
                } else if block.sprite.x + block.sprite.width as f64
                    > self.screen.width as f64 + 11.0
                    && block.sprite.dx > 0.0
                {
                    block.sprite.x = 8.0 - (block.sprite.width as f64);
                }
            }
        }
    }
}

fn create_treasure<'a>(row: usize) -> sprites::Sprite<'a> {
    let mut treasure =
        sprites::create_sprite(2.0, (row_height(row) + 2) as f64, 3, 3, TREASURE_CLOSED);
    treasure.dx = 0.2;
    return treasure;
}

fn setup() -> Game<'static> {
    let screen = graphics::setup_screen();
    let rows = get_rows();
    let cols = get_columns();
    let mut game = Game {
        screen: screen,
        character: create_character(),
        treasure: create_treasure(rows - 1),
        time_instant: time::Instant::now(),
        score: 0,
        lives: 10,
        blocks: vec![],
        prev_colided_block: (0, 0),
    };

    game.make_blocks(cols, rows);
    game.accelerate_blocks();
    game.character.sprite.dx = game.blocks[0][0].sprite.dx;
    return game;
}

fn process(game: &mut Game) {
    loop {
        game.screen.clear();

        game.controls();
        game.colision();
        game.gravity();
        game.block_overlap();
        game.char_move();
        game.edge_detect();
        game.draw();
        game.treasure_colide();
        game.screen.render();
        std::thread::sleep(std::time::Duration::new(0, 10000000));
    }
}

fn main() {
    let default_panic = std::panic::take_hook();
    std::panic::set_hook(Box::new(move |info| {
        graphics::teardown();
        default_panic(info);
    }));
    let mut game = setup();
    process(&mut game);
}
