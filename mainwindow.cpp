#include "mainwindow.h"
#include <QFont>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent) {
	setWindowTitle("Echo Server GUI");
	resize(800, 600);
	// Create central widget and layout
	QWidget*     centralWidget = new QWidget(this);
	QVBoxLayout* layout        = new QVBoxLayout(centralWidget);
	setCentralWidget(centralWidget);

	// Create text area with JSON highlighting
	m_textArea = new QPlainTextEdit(this);
	QFont monoFont("Monospace");
	monoFont.setStyleHint(QFont::Monospace);
	m_textArea->setFont(monoFont);
	m_textArea->setPlaceholderText("Enter JSON message here...");

	// Create button
	QPushButton* sendButton = new QPushButton("Send to WebSocket Clients", this);
	sendButton->setShortcut(Qt::CTRL | Qt::Key_Return);
	connect(sendButton, &QPushButton::clicked, this, &MainWindow::notifyWSClient);

	// Add widgets to layout
	layout->addWidget(m_textArea);
	layout->addWidget(sendButton);
}

void sentoWSClient(const QString& text);

void MainWindow::notifyWSClient() {
	auto text = m_textArea->toPlainText();
	sentoWSClient(text);
}
