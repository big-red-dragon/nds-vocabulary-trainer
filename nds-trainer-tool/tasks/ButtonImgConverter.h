#ifndef BUTTONIMGCONVERTER_H
#define BUTTONIMGCONVERTER_H

#include <Task.h>

class ButtonImgConverter : public Task
{
public:
	ButtonImgConverter(QStringList cliparams);

	// Task interface
public:
	void run(QString outdir, QString param) const;
};

#endif // BUTTONIMGCONVERTER_H
