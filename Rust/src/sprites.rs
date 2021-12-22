use crate::graphics;

pub struct Sprite<'l> {
    pub width: usize,
    pub height: usize,
    pub x: f64,
    pub y: f64,
    pub dx: f64,
    pub dy: f64,
    pub is_visable: bool,
    pub bitmap: &'l str,
}
pub fn create_sprite(x: f64, y: f64, width: usize, height: usize, bitmap: &str) -> Sprite {
    let sprite = Sprite {
        width,
        height,
        x,
        y,
        dx: 0.0,
        dy: 0.0,
        is_visable: true,
        bitmap,
    };
    return sprite;
}

impl<'l> Sprite<'l> {
    pub fn draw(&self, screen: &mut graphics::Screen) {
        if !self.is_visable {
            return;
        }
        for (i, c) in self.bitmap.chars().enumerate() {
            let x = self.x.round() as i32 + i as i32 % self.width as i32;
            let y = self.y.round() as i32 + i as i32 / self.width as i32;

            if c != ' ' {
                screen.draw_char(x, y, c)
            }
        }
    }

    pub fn move_to(&mut self, x: f64, y: f64) -> bool {
        let x0 = self.x.round() as i32;
        let y0 = self.y.round() as i32;

        self.x = x;
        self.y = y;
        let x1 = x.round() as i32;
        let y1 = y.round() as i32;
        return (x1 != x0) || (y1 != y0);
    }

    pub fn move_by(&mut self, dx: f64, dy: f64) -> bool {
        return self.move_to(self.x + dx, self.y + dy);
    }
    pub fn step(&mut self) -> bool {
        return self.move_by(self.dx, self.dy);
    }


    pub fn hide(&mut self) {
        self.is_visable = false;
    }
}
