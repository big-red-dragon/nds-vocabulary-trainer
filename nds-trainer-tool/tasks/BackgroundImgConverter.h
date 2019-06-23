#ifndef BACKGROUNDIMGCONVERTER_H
#define BACKGROUNDIMGCONVERTER_H

#include "Task.h"

class BackgroundImgConverter : public Task
{
public:
	BackgroundImgConverter(QStringList cliparams);

	// Task interface
public:
	void run(QString outdir, QString filename) const;
};

#endif // BACKGROUNDIMGCONVERTER_H
