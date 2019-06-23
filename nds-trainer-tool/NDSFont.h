#ifndef NDSFONT_H
#define NDSFONT_H

#include <QFont>
#include <QImage>
#include <QMap>
#include <QFontMetrics>

class QString;
class QByteArray;


class NDSFont
{
public:
    NDSFont();
    NDSFont(QString name);
    void setFont(QString name);
    QVector<short> translate(QString t);
    void save(QIODevice *d);

    int offset;

private:
    void init();

    struct cEntry {
        int offset;
        char leftBearing;
        char width; // NOT the width of the image!!
        char ypos; // NOT the height of the image!!
        char reserved;
    };

    struct FontChar {
        short id;
        QImage img;
        char leftBearing;
        char rightBearing;
        char width; // NOT the width of the image!!
        char ypos; // NOT the height of the image!!
    };

    int nextId;
    QFont font;
    QMap<QChar, FontChar> fontset;

};

#endif // NDSFONT_H
