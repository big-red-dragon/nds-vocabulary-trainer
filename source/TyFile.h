#ifndef TYFILE_H
#define TYFILE_H

#include <stdio.h>

class TyFile
{
public:
    TyFile();
    TyFile(const char *path);

    void setFileName(const char *path);
    bool open(const char *mode);
    void close();
    void flush();

    void seek(int p);

    int readInt();
    short readShort();
    char readChar();
    void readData(char *dst, int size);

    void writeInt(int i);
    void writeShort(short s);
    void writeChar(char c);
    void writeData(char *src, int size);

private:
    const char *filename;
    FILE * fp;
};

#endif // TYFILE_H
