#include "MainWindow.h"

#include <QRegExp>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi(this);
    stackedWidget->setCurrentWidget(loginPage);
    socket = new QTcpSocket(this);
    connect(socket, SIGNAL(readyRead()), this, SLOT(readyRead()));
    connect(socket, SIGNAL(connected()), this, SLOT(connected()));
}

void MainWindow::on_loginButton_clicked()
{
    QRegExp nameRegex("^[a-zA-Z0-9_.-]*$");
    if(nameRegex.indexIn(userLineEdit->text()) != -1)
    {
        socket->connectToHost(serverLineEdit->text(), 4200);

        if(!socket->waitForConnected(2000))
        {
            QMessageBox::critical(this, tr("Error"), tr("Cannot connect to server"));
        }
    }
    else
        QMessageBox::critical(this, tr("Incorrect nickname"), tr("Nickname should contain only letters or digits."));
}
//sendmsg
void MainWindow::on_sayButton_clicked()
{
    QString message = sayLineEdit->text().trimmed();
    if(!message.isEmpty())
    {
        socket->write(QString(message + "\n").toUtf8());
    }
    sayLineEdit->clear();
    sayLineEdit->setFocus();
}
//readmsg
void MainWindow::readyRead()
{
    while(socket->canReadLine())
    {
        QString line = QString::fromUtf8(socket->readLine()).trimmed();
        // msg should be like "username:The message"
        QRegExp messageRegex("^([^:]+):(.*)$");
        QRegExp nameRegex("^[a-zA-Z0-9_.-]*$");
        // if it is ok write it
        if(messageRegex.indexIn(line) != -1)
        {
            QString user = messageRegex.cap(1);
            QString message = messageRegex.cap(2);
                if(nameRegex.indexIn(user) != -1)
                    roomTextEdit->append("<b>" + user + "</b>: " + message);
        }
    }
}
// Switch to Chat if connected and send our name to server
void MainWindow::connected()
{
    stackedWidget->setCurrentWidget(chatPage);
    socket->write(QString("/me:" + userLineEdit->text() + "\n").toUtf8());
}
