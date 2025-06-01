// Copyright (C) 2016 Kurt Pattyn <pattyn.kurt@gmail.com>.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause
#include "echoserver.h"
#include "QtWebSockets/qwebsocket.h"
#include "QtWebSockets/qwebsocketserver.h"
#include <QtCore/QDebug>
#include <fmt/format.h>



//boost multi index
std::map<QWebSocket*, State> m_clients;

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
	m_clients.clear();
}

//! [onNewConnection]
void EchoServer::onNewConnection() {
	QWebSocket* pSocket = wsServer->nextPendingConnection();
	fmt::print("New connection: {}\n", pSocket->peerAddress().toString().toStdString());
	fmt::print("Clients: {}\n", m_clients.size());
	m_clients.insert(std::make_pair(pSocket, State()));

	connect(pSocket, &QWebSocket::textMessageReceived, this, &EchoServer::processTextMessage);
	connect(pSocket, &QWebSocket::binaryMessageReceived, this, &EchoServer::processBinaryMessage);
	connect(pSocket, &QWebSocket::disconnected, this, &EchoServer::socketDisconnected);
}
//! [onNewConnection]

//! [processTextMessage]
void EchoServer::processTextMessage(QString message) {
	QWebSocket* pClient = qobject_cast<QWebSocket*>(sender());
	auto&       state   = m_clients.at(pClient);
	state.msgRec++;
	if (m_debug) {
		fmt::print("Message {} received from client {}:\n{}\n", state.msgRec, pClient->peerAddress().toString().toStdString(), message.toStdString());
	}
	for (auto& [client, state] : m_clients) {
		state.msgSent++;
		client->sendTextMessage(message);
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
	if (pClient) {
		m_clients.erase(pClient);
		pClient->deleteLater();
	}
}
//! [socketDisconnected]

void sentoWSClient(const QString& text) {
	qDebug() << "sendin msg to " << m_clients.size() << "client";
	for (auto& client : m_clients) {
		client.first->sendTextMessage(text);
	}
}
