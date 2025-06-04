// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "echoserver.h"
#include "QtWebSockets/qwebsocket.h"
#include "QtWebSockets/qwebsocketserver.h"
#include "appliance.h"
#include <QtCore/QDebug>
#include <fmt/format.h>

#include "appliancemap.h"

extern ClientMap clientMap;

QT_USE_NAMESPACE

//! [constructor]
EchoServer::EchoServer(quint16 port, bool debug, QObject* parent)
    : QObject(parent),
      wsServer(new QWebSocketServer(QStringLiteral("Echo Server"),
                                    QWebSocketServer::NonSecureMode, this)),
      m_debug(debug) {
	if (wsServer->listen(QHostAddress::Any, port)) {
		if (m_debug)
			qDebug() << "Echoserver listening on port" << port;
		connect(wsServer, &QWebSocketServer::newConnection,
		        this, &EchoServer::onNewConnection);
		connect(wsServer, &QWebSocketServer::closed, this, &EchoServer::closed);
	}
}

//! [constructor]

EchoServer::~EchoServer() {
	wsServer->close();
	clientMap.clear();
}

//! [onNewConnection]
void EchoServer::onNewConnection() {
	QWebSocket* pSocket = wsServer->nextPendingConnection();

	fmt::print("New connection: {} ({})\n", pSocket->peerAddress().toString().toStdString(), (void*)pSocket);
	auto c      = std::make_shared<Client>();
	c->qWSocket = pSocket;
	clientMap.insert(c);

	fmt::print("Clients: {}\n", clientMap.size());

	connect(pSocket, &QWebSocket::textMessageReceived, this, &EchoServer::processTextMessage);
	connect(pSocket, &QWebSocket::binaryMessageReceived, this, &EchoServer::processBinaryMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &EchoServer::socketDisconnected);
	connect(c.get(), &Client::sendMessage, this, &EchoServer::sendMessage);

	//send a message using all the registered possible type to get the mac asap
	//normally on connect most devices will send by themself but there is not fault in asking once
	c->sendInitialPacket();
}
//! [onNewConnection]

//! [processTextMessage]
void EchoServer::processTextMessage(const QString& message) {
	auto& cache = clientMap.get<ByQWebsocket>();

	{
		QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());

		if (auto iter = cache.find(pClient); iter != cache.end()) {
			auto el = iter->get();
			el->msgRec++;
			if (m_debug) {
				fmt::print("Message nr {} received from client {} ({}) (mac: {}):\n{}\n",
				           el->msgRec, pClient->peerAddress().toString().toStdString(),
				           (void*)pClient,
				           el->mac,
				           message.toStdString());
			}
			el->appliance->decodePacket(message);
		}
	}

	for (const auto& client : cache) {
		client->qWSocket->sendTextMessage(message);
	}
}
//! [processTextMessage]

//! [processBinaryMessage]
void EchoServer::processBinaryMessage(QByteArray message) {
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	if (m_debug)
		qDebug() << "Binary Message received:" << message;
	if (pClient) {
		pClient->sendBinaryMessage(message);
	}
}
//! [processBinaryMessage]

//! [socketDisconnected]
void EchoServer::socketDisconnected() {
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	if (m_debug)
		qDebug() << "socketDisconnected:" << pClient;
	// if (pClient) {
	// 	m_clients.erase(pClient);
	// 	pClient->deleteLater();
	// }
}

void EchoServer::sendMessage(const QString& message, Client* client) {
	client->qWSocket->sendTextMessage(message);
}
//! [socketDisconnected]
