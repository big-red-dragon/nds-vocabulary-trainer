#ifndef TYSTRING_H
#define TYSTRING_H

#include "defines.h"

class TyString
{
public:
    TyString();
    TyString(ushort *raw);
    void moveFrom(TyString str);
    void copyFrom(TyString str);
    ushort length() const;
    void destroy();
    void init();
    inline ushort at(int v) const { return data[v+1]; }
    inline ushort operator[](int v) const { return at(v); }
    inline bool operator==(TyString other) const;
    inline int numLines() const;

private:
    ushort * data;
};

bool TyString::operator==(TyString other) const
{
    if(other.length() != length())
        return false;
    for(ushort i = 0; i != length(); i++) {
        if(other.at(i) != at(i))
            return false;
    }
    return true;
}

int TyString::numLines() const
{
    int ret = 1;
    for(int x = 0; x < length(); x++) {
        if(at(x) == 0xffff)
            ret++;
    }
    return ret;
}

#endif // TYSTRING_H
