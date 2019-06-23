#include "ButtonImgConverter.h"
#include "Task.h"
#include <QImage>
#include "TyFile.h"

ButtonImgConverter::ButtonImgConverter(QStringList cliparams) :
	Task(QCommandLineOption(cliparams, "Image for the button box", "path")) {
}


void ButtonImgConverter::run(QString outdir, QString filename) const
{
	QImage boxPic(filename);
	TyFile boxFile(outdir + "/box.bin");
	boxFile.open(QIODevice::WriteOnly);
	boxFile.writeChar(static_cast<char>(boxPic.width()));
	boxFile.writeChar(static_cast<char>(boxPic.height()));
	for(int y = 0; y < boxPic.height(); y++) {
		for(int x = 0; x < boxPic.width(); x++) {
			QRgb c = boxPic.pixel(x, y);
			boxFile.writeChar(static_cast<char>(255-qGray(c)));
		}
	}

	boxFile.close();
}
