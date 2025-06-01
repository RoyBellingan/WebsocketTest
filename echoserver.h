// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#ifndef ECHOSERVER_H
#define ECHOSERVER_H

#include <QtCore/QByteArray>
#include <QtCore/QList>
#include <QtCore/QObject>

QT_FORWARD_DECLARE_CLASS(QWebSocketServer)
QT_FORWARD_DECLARE_CLASS(QWebSocket)

class EchoServer : public QObject {
	Q_OBJECT
      public:
	explicit EchoServer(quint16 port, bool debug = false, QObject* parent = nullptr);

	~EchoServer();

      Q_SIGNALS:
	void closed();

      private Q_SLOTS:
	void onNewConnection();
	void processTextMessage(QString message);
	void processBinaryMessage(QByteArray message);
	void socketDisconnected();

      private:
	QWebSocketServer* wsServer;
	bool              m_debug;
};

void sentoWSClient(const QString& text);

#endif //ECHOSERVER_H
