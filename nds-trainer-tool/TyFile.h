#ifndef TYFILE_H
#define TYFILE_H

#include <QFile>
#include <QDebug>

class TyFile : public QFile
{
    Q_OBJECT
public:
    enum ByteOrder { LittleEndian = 1, BigEndian = 2, ToLE = 1, ToBE = 2};
	explicit TyFile(QObject *parent = nullptr);
    TyFile(const QString &name);
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

    TyFile &operator +=(int v) { seek(pos()+v); return *this; }
    TyFile &operator -=(int v) { seek(pos()-v); return *this; }
	char operator [] (int v) { long long p = pos(); seek(p+v); char r = readChar(); seek(p); return r; }

    TyFile &operator >>(int& v) { v = readInt(); return *this; }
    TyFile &operator >>(short& v) { v = readShort(); return *this; }
    TyFile &operator >>(char& v) { v = readChar(); return *this; }
    TyFile &operator >>(float& v) { v = readFloat(); return *this; }
    TyFile &operator >>(QString& v) { v = readSZString(); return *this; }
    TyFile &operator >>(ByteOrder v) { setByteOrder(v); return *this; }

    TyFile &operator <<(int v) { writeInt(v); return *this; }
    TyFile &operator <<(short v) { writeShort(v); return *this; }
    TyFile &operator <<(char v) { writeChar(v); return *this; }
    TyFile &operator <<(float v) { writeFloat(v); return *this; }
    TyFile &operator <<(const char* v) { write(v); return *this; }
    TyFile &operator <<(QByteArray v) { write(v); return *this; }
    TyFile &operator <<(QString v) { writeSZString(v); return *this; }
    TyFile &operator <<(ByteOrder v) { setByteOrder(v); return *this; }
    
signals:
    
public slots:

protected:
    ByteOrder endianness;

private:
    void init();
};

#endif // TYFILE_H
