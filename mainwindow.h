#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QFont>
#include <QJsonDocument>
#include <QJsonObject>
#include <QMainWindow>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QVBoxLayout>

    class MainWindow : public QMainWindow {
	Q_OBJECT

      public:
	explicit MainWindow(QWidget* parent = nullptr);

      private Q_SLOTS:
	void notifyWSClient();

      private:
	QPlainTextEdit* m_textArea;
	QPushButton*    m_sendButton;
};

#endif // MAINWINDOW_H
