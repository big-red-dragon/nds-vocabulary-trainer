#include "VocabularyConverter.h"
#include "TextDB.h"
#include <QDir>

VocabularyConverter::VocabularyConverter(QStringList cmdparams) :
	Task(QCommandLineOption(cmdparams, "Image for the background", "path"))
{}


void VocabularyConverter::run(QString outdir, QString filepath) const
{
	TextDB db;
	db.fromFile(filepath);
	QString filename = filepath.section(QDir::separator(), -1);
	db.save(outdir + QDir::separator() + filename.mid(0, filename.lastIndexOf(".")) + ".voc");
}
