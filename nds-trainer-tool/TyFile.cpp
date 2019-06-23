#include "TyFile.h"
#include <QtEndian>
#include <QDebug>

TyFile::TyFile(QObject *parent) :
    QFile(parent)
{
    init();
}

TyFile::TyFile(const QString &name) : QFile(name)
{
    init();
}

void TyFile::setByteOrder(ByteOrder bo)
{
    endianness = bo;
}

TyFile::ByteOrder TyFile::byteOrder() {
    return endianness;
}

void TyFile::init()
{
    endianness = LittleEndian;
}

qint64 TyFile::writeInt(int v)
{
    if(endianness == BigEndian) {
        v = qToBigEndian(v);
    }
    return this->write(reinterpret_cast<char*>(&v), sizeof(int));
}

qint64 TyFile::writeShort(short v)
{
    if(endianness == BigEndian) {
        v = qToBigEndian(v);
    }
    return this->write(reinterpret_cast<char*>(&v), sizeof(short));
}

qint64 TyFile::writeChar(char v)
{
    return this->write(reinterpret_cast<char*>(&v), sizeof(char));
}

qint64 TyFile::writeSZString(QString v)
{
	return this->write(v.toLatin1().append('\0'));
}

qint64 TyFile::writeFloat(float v)
{
    int x = reinterpret_cast<int&>(v);
    if(endianness == BigEndian) {
        x = qToBigEndian(x);
    }
    return this->write(reinterpret_cast<char*>(&x), sizeof(float));
}

int TyFile::readInt()
{
    int ret;
    this->read(reinterpret_cast<char*>(&ret), sizeof(int));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

qint64 TyFile::readInt64()
{
    qint64 ret;
    this->read(reinterpret_cast<char*>(&ret), sizeof(qint64));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

short TyFile::readShort()
{
    short ret;
    this->read(reinterpret_cast<char*>(&ret), sizeof(short));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

float TyFile::readFloat()
{
    int ret;
    this->read(reinterpret_cast<char*>(&ret), sizeof(int));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return reinterpret_cast<float&>(ret);
}

char TyFile::readChar()
{
    char ret;
    this->read(reinterpret_cast<char*>(&ret), sizeof(char));
    return ret;
}

QString TyFile::readSZString()
{
    QString result;
    char c;
    while(1) {
        if(read(&c, 1) == 0 || c == '\0')
            break;
        result.append(QChar(c));
    }
    return result;
}

qint32 TyFile::readVarint()
{
    char c;
    int result = 0;
    for(int i = 0; i < 35; i += 7) {
        read(&c, 1);
        result |= (c&0x7f) << i;
        if((c&0x80) == 0)
            break;
    }
    return result;
}

qint64 TyFile::readVarint64()
{
    char c;
    qint64 result = 0;
    for(int i = 0; i < 10; i++) {
        read(&c, 1);
        result |= (c&0x7f) << i*7;
        if((c&0x80) == 0)
            break;
    }
    return result;
}
