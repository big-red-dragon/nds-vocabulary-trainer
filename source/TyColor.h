#ifndef TYCOLOR_H
#define TYCOLOR_H

#include "defines.h"
#include <nds.h>

struct TyColor {
    inline TyColor(ushort rgb15) {
        b = (rgb15 & 0x7C00) >> 10;
        g = (rgb15 & 0x03E0) >> 5;
        r = (rgb15 & 0x001F) >> 0;
    }
    inline TyColor(uchar r, uchar g, uchar b) {
        this->r = r;
        this->g = g;
        this->b = b;
    }

    inline ushort toRGB15() {
        return b << 10 | g << 5 | r << 0 | 1 << 15;
    }

    inline void blend(TyColor other, uchar alpha) {
        float a = float(alpha) / 255;
        r = a * other.r + (1-a) * r;
        g = a * other.g + (1-a) * g;
        b = a * other.b + (1-a) * b;
    }

    uchar r;
    uchar g;
    uchar b;
    uchar a;
};

#endif // TYCOLOR_H
