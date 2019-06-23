#include "TaskRunner.h"
#include "tasks/BackgroundImgConverter.h"
#include "tasks/ButtonImgConverter.h"
#include "tasks/VocabularyConverter.h"
#include <QSharedPointer>
#include <QGuiApplication>
#include <QFont>

int main(int argc, char *argv[])
{
	QGuiApplication a(argc, argv);
	QGuiApplication::processEvents();

	TaskRunner taskRunner;
	QList<QSharedPointer<Task>> list;

	list.push_back(QSharedPointer<Task>(new BackgroundImgConverter({"b", "background"})));
	list.push_back(QSharedPointer<Task>(new ButtonImgConverter({"x", "box"})));
	list.push_back(QSharedPointer<Task>(new VocabularyConverter({"v", "vocabulary"})));

	taskRunner.addTasks(list);
	return taskRunner.run(argc, argv);
}
