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
static byte score_l, score_r;
#pragma data-name(pop)
#pragma  bss-name(pop)

static const byte palette[32] = {
    /* background palettes */
    0x0f,0x00,0x10,0x30,
    0x0f,0x00,0x10,0x30,
    0x0f,0x00,0x10,0x30,
    0x0f,0x00,0x10,0x30,
    /* sprite palettes */
    0x0f,0x11,0x21,0x31,
    0x0f,0x11,0x21,0x31,
    0x0f,0x11,0x21,0x31,
    0x0f,0x11,0x21,0x31,
};

static const char p1_win[] = "P1 WINS!";
static const char p2_win[] = "P2 WINS!";

static void draw_score(void) {
    byte t;
    t = score_l / 10;
    oam_off = oam_spr(112, 16, '0' + t, 0, oam_off);
    t = score_l % 10;
    oam_off = oam_spr(120, 16, '0' + t, 0, oam_off);
    t = score_r / 10;
    oam_off = oam_spr(144, 16, '0' + t, 0, oam_off);
    t = score_r % 10;
    oam_off = oam_spr(152, 16, '0' + t, 0, oam_off);
}

static void reset_ball(signed char dir) {
    ball.x = 120; ball.y = 120;
    ball_vx = dir; ball_vy = 1;
}

static void show_winner(byte player) {
    const char *msg = player == 1 ? p1_win : p2_win;
    byte i = 0;

    /* draw the final score so the screen shows the winning value */
    oam_off = 0;
    draw_score();
    oam_hide_rest(oam_off);

    ppu_off();
    vram_adr(NTADR_A(12, 14));
    while (msg[i]) {
        vram_put(msg[i]);
        ++i;
    }
    ppu_on_all();
    while (1)
        ppu_wait_nmi();
}

static void check_winner(void) {
    if (score_l >= 11 && score_l >= score_r + 2) show_winner(1);
    if (score_r >= 11 && score_r >= score_l + 2) show_winner(2);
}


void main(void) {
    ppu_off();
    pal_all(palette);

    paddle_l.x = 16;  paddle_l.y = 112;
    paddle_r.x = 232; paddle_r.y = 112;
    ball.x = 120;     ball.y = 120;
    ball_vx = 1;      ball_vy = 1;
    score_l = 0;      score_r = 0;

    oam_clear();
    ppu_on_all();
    ppu_wait_nmi();

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
                signed char diff = (ball.y + 4) - (paddle_l.y + 8);
                ball_vx = -ball_vx;
                ball.x = paddle_l.x + 8;
                ball_vy = diff >> 2;
                if (!ball_vy) {
                    if (diff > 0) ball_vy = 1;
                    else if (diff < 0) ball_vy = -1;
                }
            }
        } else {
            if (ball.x + 8 >= paddle_r.x &&
                ball.y + 4 >= paddle_r.y && ball.y <= paddle_r.y + 16) {
                signed char diff = (ball.y + 4) - (paddle_r.y + 8);
                ball_vx = -ball_vx;
                ball.x = paddle_r.x - 8;
                ball_vy = diff >> 2;
                if (!ball_vy) {
                    if (diff > 0) ball_vy = 1;
                    else if (diff < 0) ball_vy = -1;
                }
            }
        }

        if (ball.x <= 0) {
            ++score_r;
            check_winner();
            reset_ball(-1);
        } else if (ball.x >= 248) {
            ++score_l;
            check_winner();
            reset_ball(1);
        }

        oam_off = 0;
        oam_off = oam_spr(paddle_l.x, paddle_l.y, '|', 0, oam_off);
        oam_off = oam_spr(paddle_l.x, paddle_l.y + 8, '|', 0, oam_off);
        oam_off = oam_spr(paddle_r.x, paddle_r.y, '|', 0, oam_off);
        oam_off = oam_spr(paddle_r.x, paddle_r.y + 8, '|', 0, oam_off);
        oam_off = oam_spr(ball.x, ball.y, 'o', 0, oam_off);
        draw_score();
        oam_hide_rest(oam_off);

        ppu_wait_nmi();
    }
}
