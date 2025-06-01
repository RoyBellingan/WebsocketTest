// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "echoserver.h"
#include <QCoreApplication>
#include <QDebug>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QLoggingCategory>

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription("QtWebSockets example: echoserver");
	parser.addHelpOption();

	QCommandLineOption dbgOption(QStringList() << "d" << "debug",
	                             QCoreApplication::translate("main", "Debug output [default: off]."));
	parser.addOption(dbgOption);
	QCommandLineOption portOption(QStringList() << "p" << "port",
	                              QCoreApplication::translate("main", "Port for echoserver [default: 1234]."),
	                              QCoreApplication::translate("main", "port"), QLatin1String("1234"));
	parser.addOption(portOption);
	parser.process(app);
	bool debug = parser.isSet(dbgOption);
	int  port  = parser.value(portOption).toInt();

	qDebug() << "start";

	EchoServer* server = new EchoServer(port, debug);
	QObject::connect(server, &EchoServer::closed, &app, &QCoreApplication::quit);

	return app.exec();
}

/*
send to shelly this

{
  "method": "Shelly.GetStatus",
  "id": 1,
  "params": {}
}

*/
