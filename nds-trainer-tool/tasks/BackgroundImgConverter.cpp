#include "BackgroundImgConverter.h"
#include <QCommandLineOption>
#include <QImage>
#include <QTextStream>
#include "TyFile.h"

BackgroundImgConverter::BackgroundImgConverter(QStringList cliparams) :
	Task(QCommandLineOption(cliparams, "Image for the background", "path"))
{}


void BackgroundImgConverter::run(QString outdir, QString filename) const
{
	QImage bgPic(filename);
	if(bgPic.height() != 192 || bgPic.width() != 256) {
		QTextStream out(stdout);
		throw "Error: Background Image dimensions must be 256x192!";
	}
	TyFile bgFile(outdir + "/background.bin");
	bgFile.open(QIODevice::WriteOnly);
	for(int y = 0; y < bgPic.height(); y++) {
		for(int x = 0; x < bgPic.width(); x++) {
			QRgb c = bgPic.pixel(x, y);
			uint r = uint(qRed(c) >> 3);
			uint g = uint(qGreen(c) >> 3);
			uint b = uint(qBlue(c) >> 3);
			ushort s = ushort(b << 10 | g << 5 | r);
			bgFile.writeShort(short(s));
		}
	}
	bgFile.close();
}
