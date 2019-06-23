#ifndef TYDATASTREAM_H
#define TYDATASTREAM_H

class QIODevice;
class QByteArray;
#include <QString>

class TyDataStream
{
public:
    enum ByteOrder { LittleEndian = 1, BigEndian = 2, ToLE = 1, ToBE = 2};
    TyDataStream();
    TyDataStream(QIODevice* device);

    void setDevice(QIODevice* device);

    void setByteOrder(ByteOrder bo);
    ByteOrder byteOrder();

    qint64 writeInt(int v);
    qint64 writeShort(short v);
    qint64 writeChar(char v);
    qint64 writeSZString(QString v);
    qint64 writeFloat(float v);

    QString readSZString();
    int readInt();
    qint64 readInt64();
    short readShort();
    char readChar();
    float readFloat();
    qint32 readVarint();
    qint64 readVarint64();

    TyDataStream &operator +=(int v);
    TyDataStream &operator -=(int v);
    char operator [] (int v);

    TyDataStream &operator >>(int& v) { v = readInt(); return *this; }
    TyDataStream &operator >>(short& v) { v = readShort(); return *this; }
    TyDataStream &operator >>(char& v) { v = readChar(); return *this; }
    TyDataStream &operator >>(float& v) { v = readFloat(); return *this; }
    TyDataStream &operator >>(QString& v) { v = readSZString(); return *this; }
    TyDataStream &operator >>(ByteOrder v) { setByteOrder(v); return *this; }

    TyDataStream &operator <<(int v) { writeInt(v); return *this; }
    TyDataStream &operator <<(short v) { writeShort(v); return *this; }
    TyDataStream &operator <<(char v) { writeChar(v); return *this; }
    TyDataStream &operator <<(float v) { writeFloat(v); return *this; }
    TyDataStream &operator <<(const char* v);
    TyDataStream &operator <<(QByteArray v);
    TyDataStream &operator <<(QString v) { writeSZString(v); return *this; }
    TyDataStream &operator <<(ByteOrder v) { setByteOrder(v); return *this; }

private:
    QIODevice* m_device;
    ByteOrder endianness;
    void init();
};

#endif // TYDATASTREAM_H
