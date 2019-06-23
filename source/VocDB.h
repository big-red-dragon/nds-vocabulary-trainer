#ifndef VOCDB_H
#define VOCDB_H

#include "defines.h"

class TyString;
class TyFile;

class VocDB
{
public:
    VocDB();
    VocDB(TyFile *src);
    void setFile(TyFile *src);
    void renderEntry(int x, int y, int w, int id, int column, ushort *buffer = back_buffer);
    void renderString(int x, int y, int w, TyString str, int column, ushort *buffer = back_buffer);
    TyString getEntry(int id, int col);

    int numEntries;

private:
    struct fontChar {
        uint offset;
        uchar width;
        uchar ypos;
        uchar leftBearing;
        uchar reserved;
    };

    struct fontset {
        fontChar * charData;
    };

    fontset * fonts;

    TyFile *f;
    int numColumns;
    int *ColumnOffsets;
    int *FontOffsets;
};

#endif // VOCDB_H
