#include <QtCore/QCoreApplication>
//#include <cstdio>
//#include <cstdlib>
#include "wrpstats.h"

int main(int argc, char *argv[])
{
	QCoreApplication a(argc, argv);

	wrpStats(argc, argv);

	return 0;
}
