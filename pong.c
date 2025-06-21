#include "neslib/neslib.h"
typedef unsigned char byte;

typedef struct {
    byte x;
    byte y;
} Sprite;

#pragma  bss-name(push, "ZEROPAGE")
#pragma data-name(push, "ZEROPAGE")
byte oam_off;
static Sprite paddle_l;
static Sprite paddle_r;
static Sprite ball;
static signed char ball_vx;
static signed char ball_vy;
static byte pad1, pad2;
#pragma data-name(pop)
#pragma  bss-name(pop)

static const byte palette[16] = {
    0x0f,0,0,0x31,
    0,0,0,0,
    0,0,0,0,
    0,0,0,0,
};

static byte vram_update[] = {
    MSB(NTADR_A(10,14)) | NT_UPD_HORZ,
    LSB(NTADR_A(10,14)),
    11,
    'H', 'e', 'l', 'l', 'o',  ' ',  'P', 'i', 'n', 'g',  '!',
    NT_UPD_EOF,
};

void main(void) {
    ppu_off();
    pal_bg(palette);

    paddle_l.x = 16;  paddle_l.y = 112;
    paddle_r.x = 232; paddle_r.y = 112;
    ball.x = 120;     ball.y = 120;
    ball_vx = 1;      ball_vy = 1;

    set_vram_update(vram_update);
    oam_clear();
    ppu_on_all();
    ppu_wait_nmi();
    set_vram_update(NULL);

    while (1) {
        pad1 = pad_poll(0);
        pad2 = pad_poll(1);

        if (pad1 & PAD_UP   && paddle_l.y > 16)                paddle_l.y -= 2;
        if (pad1 & PAD_DOWN && paddle_l.y < 224 - 16)          paddle_l.y += 2;
        if (pad2 & PAD_UP   && paddle_r.y > 16)                paddle_r.y -= 2;
        if (pad2 & PAD_DOWN && paddle_r.y < 224 - 16)          paddle_r.y += 2;

        ball.x += ball_vx;
        ball.y += ball_vy;

        if (ball.y <= 16 || ball.y >= 224 - 8) ball_vy = -ball_vy;

        if (ball_vx < 0) {
            if (ball.x <= paddle_l.x + 8 &&
                ball.y + 4 >= paddle_l.y && ball.y <= paddle_l.y + 16) {
                ball_vx = -ball_vx;
                ball.x = paddle_l.x + 8;
            }
        } else {
            if (ball.x + 8 >= paddle_r.x &&
                ball.y + 4 >= paddle_r.y && ball.y <= paddle_r.y + 16) {
                ball_vx = -ball_vx;
                ball.x = paddle_r.x - 8;
            }
        }

        if (ball.x <= 0 || ball.x >= 248) ball_vx = -ball_vx;

        oam_off = 0;
        oam_off = oam_spr(paddle_l.x, paddle_l.y, '|', 0, oam_off);
        oam_off = oam_spr(paddle_l.x, paddle_l.y + 8, '|', 0, oam_off);
        oam_off = oam_spr(paddle_r.x, paddle_r.y, '|', 0, oam_off);
        oam_off = oam_spr(paddle_r.x, paddle_r.y + 8, '|', 0, oam_off);
        oam_off = oam_spr(ball.x, ball.y, 'o', 0, oam_off);
        oam_hide_rest(oam_off);

        ppu_wait_nmi();
    }
}
