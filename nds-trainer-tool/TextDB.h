#ifndef TEXTDB_H
#define TEXTDB_H

#include <QString>
#include <QFont>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QVector>
#include "NDSFont.h"
#include <QHash>

class TextDB
{
public:
    TextDB();
    void fromFile(QString path);
    void save(QString path);

private:
    void addFont(QString name);
    struct column {
        QList<QVector<short> > data;
        NDSFont *font;
    };

    QHash<QString, NDSFont> fonts;
    QList<column> columns;
};

#endif // TEXTDB_H
