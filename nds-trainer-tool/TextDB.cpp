#include "TextDB.h"
#include "TyFile.h"
#include <QFile>
#include <QStringList>
#include <QRegExp>
#include <QDebug>
#include <QTextStream>

TextDB::TextDB()
{
}

void TextDB::fromFile(QString path)
{
    columns.clear();
    QFile file(path);
    QTextStream stream(&file);
    file.open(QIODevice::ReadOnly);
    while(!stream.atEnd()) {
        QString line = stream.readLine();
        if(line.startsWith('#')) {
            QString fontname = line.mid(1).trimmed();
            addFont(fontname);
        } else {
            if(line.trimmed().isEmpty())
                continue;
            QStringList sl = line.split(QRegExp("\\t+"));
            for(int i = 0; i < columns.size(); i++) {
                QString str = sl.value(i).trimmed();
                str.replace("\\n", "\n");
                NDSFont *f = columns[i].font;
                QVector<short> v = f->translate(str);
                columns[i].data.append(v);
            }
        }
    }
    file.close();
}

void TextDB::save(QString path)
{
    if(columns.size() == 0) {
        qDebug() << "Error: No Data";
        return;
    }

    QList<int> offsetFonts;

    TyFile file(path);
    file.open(QIODevice::WriteOnly);
    int numEntries = columns[0].data.size();
    int numColumns = columns.size();
    file.writeInt(numEntries);
    file.writeInt(numColumns);
    file.seek(8 + numColumns * 8);
    int sp = file.pos() + numColumns * numEntries * 4;
    for(int i = 0; i < numColumns; i++) {
        for(int j = 0; j < numEntries; j++) {
            file.writeInt(sp);
            sp += columns[i].data[j].size()*2+2; // 2 = short length
        }
    }
    for(int i = 0; i < numColumns; i++) {
        for(int j = 0; j < numEntries; j++) {
            QVector<short> &data = columns[i].data[j];
            file.writeShort(data.size());
            for(int k = 0; k < data.size(); k++) {
                file.writeShort(data[k]);
            }
        }
    }
    if(file.pos() % 16) {
        file.seek(file.pos() + (16 - file.pos() % 16));
    }
    for(int i = 0; i < numColumns; i++) {
        NDSFont *font = columns[i].font;
        if(font->offset == 0)
            font->save(&file);
        offsetFonts.append(font->offset);
    }
    file.seek(8);
    for(int i = 0; i < numColumns; i++) {
        file.writeInt((8 + numColumns * 8) + (i * numEntries * 4)); // header + pos
        file.writeInt(offsetFonts.at(i));
    }
    file.close();
}

void TextDB::addFont(QString name)
{
    column col;
    if(!fonts.contains(name)) {
        fonts.insert(name, NDSFont(name));
    }
    col.font = &fonts[name];
    columns.append(col);
}
