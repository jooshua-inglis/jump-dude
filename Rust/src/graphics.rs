use ncurses::*;

pub struct Screen {
    pub width: usize,
    pub height: usize,
    pixels: Vec<Vec<char>>,
    prev_pixels: Vec<Vec<char>>,
}

impl Screen {
    pub fn render(&mut self) {
        let mut changed = false;

        for x in 0..self.width {
            for y in 0..self.height {
                let val = self.pixels[y][x];
                mvaddch(y as i32, x as i32, val as u32);
                self.prev_pixels[y][x] = val;
                changed = true
            }
        }

        if changed {
            refresh();
        }
    }

    pub fn clear(&mut self) {
        self.pixels = vec![vec![' '; self.width]; self.height]
    }

    pub fn draw_char(&mut self, x: i32, y: i32, value: char) {
        let w = self.width;
        let h = self.height;

        if x >= w as i32 || y >= h as i32 || x < 0 || y < 0 {
            return;
        }

        self.pixels[y as usize][x as usize] = value;
    }

    pub fn draw_line(&mut self, mut x1: i32, mut y1: i32, mut x2: i32, mut y2: i32, val: char) {
        if x1 == x2 {
            let min = std::cmp::min(y1, y2);
            let max = std::cmp::max(y1, y2);
            for i in min..max {
                self.draw_char(x1, i, val);
            }
        } else if y1 == y2 {
            let min = std::cmp::min(x1, x2);
            let max = std::cmp::max(x1, x2);
            for i in min..max {
                self.draw_char(i, y1, val);
            }
        } else {
            if x1 > x2 {
                let mut tmp = x1;
                x1 = x2;
                x2 = tmp;
                tmp = y1;
                y1 = y2;
                y2 = tmp;
            }
            let dx = (x2 - x1) as f64;
            let dy = (y2 - y1) as f64;

            let mut err = 0.0;
            let derr = (dy / dx).abs();

            let y = y1;
            let mut x = x1;
            loop {
                if dx > 0.0 && x <= x2 {
                    break;
                } else if x >= x2 {
                    break;
                }

                self.draw_char(x, y, val);
                err += derr;
                while err >= 0.5 && if dy > 0.0 { y <= y2 } else { y >= y2 } {
                    self.draw_char(x, y, val);
                    // Casting like this might cause an issue, like if it's not cast to 1
                    // y += (dy > 0.0) as usize - (dy < 0.0) as usize;

                    err -= 1.0;
                }
                if dx > 0.0 {
                    x += 1
                } else {
                    x -= 1
                }
            }
        }
    }

    pub fn draw_string(&mut self, x: i32, y: i32, text: String) {
        for (i, c) in text.chars().enumerate() {
            self.draw_char(x + i as i32, y, c)
        }
    }


    // There's a better way to do this, but doing this for now
}
pub fn setup_screen() -> Screen {
    initscr();
    noecho();
    curs_set(CURSOR_VISIBILITY::CURSOR_INVISIBLE);
    timeout(0);
    clear();
    let width = screen_width();
    let height = screen_height();

    let pixels = vec![vec![' '; width]; height];
    let prev_pixles = vec![vec![' '; width]; height];
    let screen = Screen {
        width: width,
        height: height,
        pixels: pixels,
        prev_pixels: prev_pixles,
    };

    return screen;
}
pub fn wait_char() -> i32 {
    timeout(-1);
    let c = getch();
    timeout(0);

    return c;
}
pub fn get_char() -> i32 {
    let val = getch();

    return val;
}

fn get_dims() -> (usize, usize) {
    let mut width = 0;
    let mut height = 0;

    let scr = constants::stdscr();
    getmaxyx(scr, &mut height, &mut width);
    return (width as usize, height as usize);
}

pub fn screen_width() -> usize {
    return get_dims().0;
}

pub fn screen_height() -> usize {
    return get_dims().1;
}

pub fn teardown() {
    endwin();
    println!("goodbye");
}
