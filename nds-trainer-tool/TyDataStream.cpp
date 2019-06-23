#include "TyDataStream.h"
#include <QIODevice>
#include <QtEndian>
#include <QDebug>

TyDataStream::TyDataStream()
{
    init();
}

TyDataStream::TyDataStream(QIODevice *device)
{
    setDevice(device);
    init();
}

void TyDataStream::setDevice(QIODevice *device)
{
    m_device = device;
}

TyDataStream &TyDataStream::operator +=(int v) {
    m_device->seek(m_device->pos()+v);
    return *this;
}

TyDataStream &TyDataStream::operator -=(int v) {
    m_device->seek(m_device->pos()-v);
    return *this;
}

char TyDataStream::operator [](int v) {
    int p = m_device->pos();
    m_device->seek(p+v);
    char r = readChar();
    m_device->seek(p);
    return r;
}

TyDataStream &TyDataStream::operator <<(const char* v) {
    m_device->write(v);
    return *this;
}

TyDataStream &TyDataStream::operator <<(QByteArray v) {
    m_device->write(v);
    return *this;
}

void TyDataStream::setByteOrder(ByteOrder bo)
{
    endianness = bo;
}

TyDataStream::ByteOrder TyDataStream::byteOrder() {
    return endianness;
}

void TyDataStream::init()
{
    endianness = LittleEndian;
}

qint64 TyDataStream::writeInt(int v)
{
    if(endianness == BigEndian) {
        v = qToBigEndian(v);
    }
    return m_device->write(reinterpret_cast<char*>(&v), sizeof(int));
}

qint64 TyDataStream::writeShort(short v)
{
    if(endianness == BigEndian) {
        v = qToBigEndian(v);
    }
    return m_device->write(reinterpret_cast<char*>(&v), sizeof(short));
}

qint64 TyDataStream::writeChar(char v)
{
    return m_device->write(reinterpret_cast<char*>(&v), sizeof(char));
}

qint64 TyDataStream::writeSZString(QString v)
{
	return m_device->write(v.toLatin1().append('\0'));
}

qint64 TyDataStream::writeFloat(float v)
{
    int x = reinterpret_cast<int&>(v);
    if(endianness == BigEndian) {
        x = qToBigEndian(x);
    }
    return m_device->write(reinterpret_cast<char*>(&x), sizeof(float));
}

int TyDataStream::readInt()
{
    int ret;
    m_device->read(reinterpret_cast<char*>(&ret), sizeof(int));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

qint64 TyDataStream::readInt64()
{
    qint64 ret;
    m_device->read(reinterpret_cast<char*>(&ret), sizeof(qint64));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

short TyDataStream::readShort()
{
    short ret;
    m_device->read(reinterpret_cast<char*>(&ret), sizeof(short));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return ret;
}

float TyDataStream::readFloat()
{
    int ret;
    m_device->read(reinterpret_cast<char*>(&ret), sizeof(int));
    if(endianness == BigEndian) {
        ret = qToBigEndian(ret);
    }
    return reinterpret_cast<float&>(ret);
}

char TyDataStream::readChar()
{
    char ret;
    m_device->read(reinterpret_cast<char*>(&ret), sizeof(char));
    return ret;
}

QString TyDataStream::readSZString()
{
    QString result;
    char c;
    while(1) {
        if(!m_device->getChar(&c) || c == '\0')
            break;
        result.append(QChar(c));
    }
    return result;
}

qint32 TyDataStream::readVarint()
{
    char c;
    int result = 0;
    for(int i = 0; i < 4; i++) {
        m_device->getChar(&c);
        result |= (c&0x7f) << i*7;
        if((c&0x80) == 0)
            break;
    }
    return result;
}

qint64 TyDataStream::readVarint64()
{
    char c;
    qint64 result = 0;
    for(int i = 0; i < 10; i++) {
        m_device->getChar(&c);
        result |= (c&0x7f) << i*7;
        if((c&0x80) == 0)
            break;
    }
    return result;
}
