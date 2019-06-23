#include <nds.h>

#include "main.h"

#include "Trainer.h"

ushort back_buffer[256*192*2];

void sleep(int milliseconds)
{
    int frames = milliseconds * 6 / 100;
    while(frames) {
        swiWaitForVBlank();
        frames--;
    }
}

int main(int argc, char **argv) {
    Trainer v;

    while(1) {
        scanKeys();
        v.onKeyEvent(keysDown());
        sleep(50);
    }
    return 0;
}
