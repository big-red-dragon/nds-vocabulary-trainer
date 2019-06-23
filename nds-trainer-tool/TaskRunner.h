#ifndef TASKRUNNER_H
#define TASKRUNNER_H

#include <QList>
class Task;

class TaskRunner
{
public:
	void addTasks(const QList<QSharedPointer<Task>> tasks) {
		this->tasks = tasks;
	}
	int run(int argc, char *argv[]) const;

private:
	QList<QSharedPointer<Task>> tasks;
};

#endif // TASKRUNNER_H
