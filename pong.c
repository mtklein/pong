#include "neslib/neslib.h"
typedef unsigned char byte;

#pragma  bss-name(push, "ZEROPAGE")
#pragma data-name(push, "ZEROPAGE")
byte oam_off;
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

static byte state     = 0;
static byte frame_mod = 0;

void main(void) {
    ppu_off();
    {
        pal_bg(palette);
    }
    ppu_on_all();

    while (1) {
        if (++frame_mod == 60) {
            frame_mod = 0;
        }

        switch (state) {
            case 0:
                set_vram_update(vram_update);
                if (frame_mod == 0) {
                    vram_update[10] = 'o';
                    state = 1;
                }
                break;

            case 1:
                set_vram_update(NULL);
                if (frame_mod ==  0) { pal_bg_bright(3); }
                if (frame_mod == 30) { pal_bg_bright(4); }
                break;
        }

        ppu_wait_nmi();
    }
}
