#include "NDSFont.h"
#include <QVector>
#include <QPainter>
#include "TyDataStream.h"
#include <QDebug>

NDSFont::NDSFont()
{
    init();
}

NDSFont::NDSFont(QString name)
{
    init();
    setFont(name);
}

void NDSFont::setFont(QString name)
{
    font.fromString(name);

    nextId = 0;
    fontset.clear();
    offset = 0;
}

void NDSFont::init()
{

}

QVector<short> NDSFont::translate(QString t)
{
    QVector<short> ret(t.size());
    for(int i = 0; i < t.size(); i++) {
        QChar c = t.at(i);
        if(c == '\n') {
            ret[i] = -1;
            continue;
        }

        if(!fontset.contains(c)) {
            // Add Character to font
            FontChar fc;
            fc.id = nextId;
            nextId++;
            QFontMetrics metrics(font);
            QRect r = metrics.boundingRect(c);
            QImage image(r.width()+2, r.height()+2, QImage::Format_ARGB32_Premultiplied);
            QPainter painter(&image);
            painter.setBrush(QBrush(Qt::transparent));
            painter.setCompositionMode(QPainter::CompositionMode_Source);
            painter.fillRect(0, 0, image.width(), image.height(), QBrush(Qt::transparent));
            painter.setFont(font);
            painter.drawText(-r.x()+1, -r.y()+1, c);
            painter.drawText(-r.x()+1, -r.y()+1, c);
            painter.drawText(-r.x()+1, -r.y()+1, c);
            painter.drawText(-r.x()+1, -r.y()+1, c);
            fc.img = image;
            fc.ypos = r.y();
            fc.width = metrics.width(c);
            fc.leftBearing = metrics.leftBearing(c);
            fc.rightBearing = metrics.rightBearing(c);
            fontset[c] = fc;
        }
        ret[i] = fontset[c].id;
    }
    return ret;
}

void NDSFont::save(QIODevice *d)
{

    offset = d->pos();
    TyDataStream stream(d);
    int posChars = d->pos();
    QVector<cEntry> entries(nextId);
    d->seek(posChars + nextId * 8 + 4);
    QMap<QChar, FontChar>::const_iterator it;
    for(it = fontset.constBegin(); it != fontset.constEnd(); ++it) {
        const FontChar &f = it.value();
        int align = d->pos() % 16;
        if(align) {
            for(int i = 0; i < 16-align; i++) {
                stream.writeChar(0);
            }
        }

        entries[f.id].offset = d->pos();
        entries[f.id].width = f.width;
        entries[f.id].ypos = -f.ypos;
        entries[f.id].leftBearing = f.leftBearing;

        stream.writeChar(f.img.width());
        stream.writeChar(f.img.height());
        for(int y = 0; y < f.img.height(); y++) {
            const QRgb *color = (const QRgb *) f.img.constScanLine(y);
            for(int x = 0; x < f.img.width(); x++) {
                int a = qAlpha(color[x]);
                stream.writeChar(a);
            }
        }
    }

    int p2 = d->pos();

    d->seek(posChars);
    stream.writeInt(entries.size());
    for(int i = 0; i < entries.size(); i++) {
        stream.writeInt(entries.at(i).offset);
        stream.writeChar(entries.at(i).width);
        stream.writeChar(entries.at(i).ypos);
        stream.writeChar(entries.at(i).leftBearing);
        stream.writeChar(0);
    }
    d->seek(p2);
}
