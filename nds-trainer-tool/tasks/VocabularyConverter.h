#ifndef VOCABULARYCONVERTER_H
#define VOCABULARYCONVERTER_H

#include "Task.h"

class VocabularyConverter : public Task
{
public:
	VocabularyConverter(QStringList cmdparams);

	// Task interface
public:
	void run(QString outdir, QString param) const;
};

#endif // VOCABULARYCONVERTER_H
