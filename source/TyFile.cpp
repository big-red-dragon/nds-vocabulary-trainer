#include "TyFile.h"

TyFile::TyFile()
{
}

TyFile::TyFile(const char *path)
{
    setFileName(path);
}

void TyFile::setFileName(const char *path)
{
    filename = path;
}

bool TyFile::open(const char *mode)
{
    fp = fopen(filename, mode);
    if(!fp)
        return false;
    return true;
}

void TyFile::close()
{
    fclose(fp);
}

void TyFile::flush()
{
    fflush(fp);
}

int TyFile::readInt()
{
    int r;
    fread(&r, 4, 1, fp);
    return r;
}

short TyFile::readShort()
{
    short r;
    fread(&r, 2, 1, fp);
    return r;
}

char TyFile::readChar()
{
    char r;
    r = fgetc(fp);
    return r;
}

void TyFile::readData(char *dst, int size)
{
    fread(dst, size, 0, fp);
}

void TyFile::writeInt(int i)
{
    fwrite(&i, 4, 1, fp);
}

void TyFile::writeShort(short s)
{
    fwrite(&s, 2, 1, fp);
}

void TyFile::writeChar(char c)
{
    fputc(c, fp);
}

void TyFile::writeData(char *src, int size)
{
    fwrite(src, size, 1, fp);
}

void TyFile::seek(int p)
{
    fseek(fp, p, SEEK_SET);
}
