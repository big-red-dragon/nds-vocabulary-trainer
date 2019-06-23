#ifndef TASK_H
#define TASK_H

#include <QCommandLineOption>
#include <QDebug>

class QString;

class Task
{
public:
	Task(QCommandLineOption option) : opt(option) {}
	QCommandLineOption opt;

	virtual void run(QString outdir, QString param) const = 0;
	virtual ~Task() {}
};

#endif // TASK_H
