// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "appliance.h"
#include "echoserver.h"

#include "config.h"
#include <QCoreApplication>
#include <QDebug>
#include <QtCore/QCommandLineOption>
#include <QtCore/QCommandLineParser>
#include <QtCore/QCoreApplication>
#include <QtCore/QLoggingCategory>
#include <pqxx/pqxx>

int main(int argc, char* argv[]) {
	QCoreApplication app(argc, argv);

	QCommandLineParser parser;
	parser.setApplicationDescription("QtWebSockets example: echoserver");
	parser.addHelpOption();

	QCommandLineOption dbgOption({"d", "debug"}, "Debug output [default: off].");
	parser.addOption(dbgOption);
	QCommandLineOption portOption({"p", "port"},
	                              "Port for echoserver [default: 1234].",
	                              "port", "1234");
	parser.addOption(portOption);
	parser.process(app);
	bool debug = parser.isSet(dbgOption);
	int  port  = parser.value(portOption).toInt();

	initConfig("smoe");

	qDebug() << "start";

	Appliance::loadAll();

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
