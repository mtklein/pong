#include "neslib/neslib.h"
typedef unsigned char byte;

#pragma  bss-name(push, "ZEROPAGE")
#pragma data-name(push, "ZEROPAGE")
byte oam_off;
#pragma data-name(pop)
#pragma  bss-name(pop)

void main(void) {
}
