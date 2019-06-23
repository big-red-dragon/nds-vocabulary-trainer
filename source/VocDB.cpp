#include "VocDB.h"
#include <nds.h>
#include "TyColor.h"
#include "TyFile.h"
#include "TyString.h"

VocDB::VocDB()
{

}

VocDB::VocDB(TyFile *src)
{
    setFile(src);
}

void VocDB::setFile(TyFile *src)
{
    f = src;

    numEntries = f->readInt();
    numColumns = f->readInt();
    ColumnOffsets = new int[numColumns];
    int * FontOffsets = new int[numColumns];
    fonts = new fontset[numColumns];
    for(int i = 0; i < numColumns; i++) {
        ColumnOffsets[i] = f->readInt();
        FontOffsets[i] = f->readInt();
    }
    for(int i = 0; i < numColumns; i++) {
        f->seek(FontOffsets[i]);
        int numChars = f->readInt();
        fonts[i].charData = new fontChar[numChars];
        fontChar *charData = fonts[i].charData;
        for(int j = 0; j < numChars; j++) {
            charData[j].offset = f->readInt();
            charData[j].width = f->readChar();
            charData[j].ypos = f->readChar();
            charData[j].leftBearing = f->readChar();
            charData[j].reserved = f->readChar();
        }
    }
}

TyString VocDB::getEntry(int id, int col)
{
    ushort * buffer = new ushort[128];
    int columnOffset = ColumnOffsets[col];
    f->seek(columnOffset + id * 4);
    int entryOffset = f->readInt();
    f->seek(entryOffset);
    ushort length = f->readShort();
    if(length > 127)
        length = 127;
    buffer[0] = length;
    for(short i = 1; i < length+1; i++) {
        buffer[i] = f->readShort();
    }
    return TyString(buffer);
}

void VocDB::renderString(int x, int y, int w, TyString str, int column, ushort * buffer)
{
    bool newLine = true;
    int cx = 0;

    TyColor newColor(0, 0, 0);
    if(column == 2)
        newColor = TyColor(21, 21, 21);
    else
        newColor = TyColor(31, 31, 31);

    for(short i = 0; i < str.length(); i++) {
        if(newLine) {
            newLine = false;
            int totalWidth = 0;
            for(short k = i; k < str.length(); k++) {
                if(str[k] == 0xffff)
                    break;
                fontChar &fc = fonts[column].charData[str[k]];
                totalWidth += fc.width+1;
            }
            cx = x + (w-totalWidth)/2;
        }
        if(str[i] == 0xffff) {
            newLine = true;
            y += 11; // New Line Distance
            continue;
        }
        fontChar &fc = fonts[column].charData[str[i]];
        f->seek(fc.offset);
        uchar imgWidth = f->readChar();
        uchar imgHeight = f->readChar();
        uint draw_x = cx + fc.leftBearing;
        uint draw_y = y - fc.ypos;
        for(int py = 0; py < imgHeight; py++) {
            for(int px = 0; px < imgWidth; px++) {
                int p = w*(py+draw_y) + px + draw_x;
                TyColor Color = TyColor(buffer[p]);
                uchar alpha = f->readChar();

                Color.blend(newColor, alpha);

                buffer[p] = Color.toRGB15();
            }
        }
        cx += fc.width+1;
    }
}

void VocDB::renderEntry(int x, int y, int w, int id, int column, ushort *buffer)
{
    TyString str = getEntry(id, column);
    renderString(x, y, w, str, column, buffer);
}
