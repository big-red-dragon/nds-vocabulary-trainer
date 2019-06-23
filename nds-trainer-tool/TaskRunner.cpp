#include "TaskRunner.h"
#include <QCommandLineParser>
#include "Task.h"
#include <QTextStream>

namespace  {
// cmdargs parses all command line parameters into a QStringList
QStringList cmdargs(int argc, char *argv[]) {
	QStringList list;
	for (int a = 0; a < argc; ++a) {
		list << QString::fromLocal8Bit(argv[a]);
	}
	return list;
}
}

int TaskRunner::run(int argc, char *argv[]) const
{
	bool hasError = false;

	QStringList arglist = cmdargs(argc, argv);
	QCommandLineParser parser;
	parser.setApplicationDescription("Image and Vocabulary converter for the NDS Trainer");
	parser.addHelpOption();

	for(const QSharedPointer<Task>& t : tasks) {
		parser.addOption(t->opt);
	}

	QCommandLineOption optOutput({"o", "output"}, "Output directory", "dir", ".");
	parser.addOption(optOutput);

	parser.process(arglist);

	QString outDir = parser.value(optOutput);

	for(const QSharedPointer<Task>& t : tasks) {
		if(parser.isSet(t->opt)) {
			try {
				t->run(outDir, parser.value(t->opt));
			} catch(const char *msg) {
				QTextStream stream(stdout);
				stream << QString(msg);
				hasError = true;
			}
		}
	}

	return hasError ? 1 : 0;
}
