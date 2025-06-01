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

static const byte text[]="Hello Pong!";

void main(void) {
    ppu_off();
    {
        pal_bg(palette);

        vram_adr  (NTADR_A(10,14));
        vram_write(text, sizeof text);
    }
    ppu_on_all();

    while (1) {

    }
}
