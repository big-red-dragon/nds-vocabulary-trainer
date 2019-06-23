#include "TyString.h"
#include <nds.h>

TyString::TyString()
{
    init();
}

TyString::TyString(ushort *raw)
{
    data = raw;
}

void TyString::init()
{
    data = 0;
}

ushort TyString::length() const
{
    return data[0];
}

void TyString::copyFrom(TyString str)
{
    if(data != 0) {
        destroy();
    }
    data = new ushort[str.length()+1];
    memcpy(data, str.data, str.length()+1);
}

void TyString::moveFrom(TyString str)
{
    if(data != 0) {
        destroy();
    }
    data = str.data;
}

void TyString::destroy()
{
    delete[] data;
}

