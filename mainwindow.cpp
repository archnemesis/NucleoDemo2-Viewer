#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "connectdialog.h"
#include <QSettings>
#include <QString>
#include <QRegExp>
#include <QList>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    parser = new StreamParser();
    connect(parser, &StreamParser::frameReceived, this, &MainWindow::on_parser_frameReceived);

    connectingMessage = new QMessageBox(this);
    connect(connectingMessage, &QMessageBox::rejected, this, &MainWindow::on_connectingMessage_cancelled);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionConnect_triggered()
{
    if (tcpSocket != nullptr) {
        return;
    }

    ConnectDialog dlg(this);
    int result = dlg.exec();

    if (result == ConnectDialog::Accepted) {
        tcpSocket = new QTcpSocket(this);
        tcpSocket->connectToHost(dlg.getHostname(), dlg.getPort());
        connectedHost = dlg.getHostname();
        connectedPort = dlg.getPort();
        ui->actionConnect->setEnabled(false);

        connectingMessage->setText(QString("Connecting to %1:%2...").arg(connectedHost).arg(connectedPort));
        connectingMessage->setInformativeText(QString("Opening connection..."));
        connectingMessage->setStandardButtons(QMessageBox::Cancel);
        connectingMessage->setDefaultButton(QMessageBox::Cancel);
        connectingMessage->show();

        connect(tcpSocket, &QTcpSocket::connected, this, &MainWindow::on_tcpSocket_connected);
        connect(tcpSocket, &QTcpSocket::disconnected, this, &MainWindow::on_tcpSocket_disconnected);
        connect(tcpSocket, &QTcpSocket::readyRead, this, &MainWindow::on_tcpSocket_readyRead);
        connect(tcpSocket, &QTcpSocket::bytesWritten, this, &MainWindow::on_tcpSocket_bytesWritten);
    }
}

void MainWindow::on_tcpSocket_connected()
{
    connectingMessage->hide();
    ui->actionConnect->setEnabled(false);
    ui->actionDisconnect->setEnabled(true);
    setWindowTitle(QString("NucleoDemo2-Viewer [%1:%2]").arg(connectedHost).arg(connectedPort));

    struct HostInfo {
        QString hostname;
        int port;
    };

    QList<HostInfo> hosts;
    QSettings settings;
    int index = -1;

    int size = settings.beginReadArray("hostHistory");
    for (int i = 0; i < size; i++) {
        settings.setArrayIndex(i);
        HostInfo host;
        host.hostname = settings.value("host").toString();
        host.port = settings.value("port").toInt();
    }
    settings.endArray();

    for (int i = 0; i < hosts.count(); i++) {
        if (hosts.at(i).hostname == connectedHost && hosts.at(i).port == connectedPort) {
            index = i;
        }

    }

    if (index >= 0) {
        // remove and insert at top of list
        if (index > 0) {
            HostInfo host;
            host.hostname = connectedHost;
            host.port = connectedPort;
            hosts.removeAt(index);
            hosts.insert(0, host);
        }
    }
    else {
        HostInfo host;
        host.hostname = connectedHost;
        host.port = connectedPort;
        hosts.insert(0, host);

        if (hosts.count() > 10) {
            hosts.removeLast();
        }
    }

    settings.remove("hostHistory");
    settings.beginWriteArray("hostHistory", hosts.count());
    for (int i = 0; i < hosts.count(); i++) {
        settings.setArrayIndex(i);
        settings.setValue("host", hosts.at(i).hostname);
        settings.setValue("port", hosts.at(i).port);
    }
    settings.endArray();
}

void MainWindow::on_tcpSocket_disconnected()
{
    ui->actionConnect->setEnabled(true);
    ui->actionDisconnect->setEnabled(false);
    setWindowTitle("NucleoDemo2-Viewer");
    tcpSocket->close();
    tcpSocket->deleteLater();
    tcpSocket = nullptr;
}

void MainWindow::on_tcpSocket_bytesWritten(qint64 bytes)
{

}

void MainWindow::on_tcpSocket_readyRead()
{
    QByteArray bytes = tcpSocket->readAll();
    parser->input(bytes);
}

void MainWindow::on_parser_frameReceived(quint32 sequence, QByteArray *data)
{
    ui->plainTextEdit->appendPlainText(QString("Received Sequence: %1\r\n").arg(sequence));
    delete data;
}


void MainWindow::on_actionDisconnect_triggered()
{
    tcpSocket->disconnectFromHost();
}

void MainWindow::on_connectingMessage_cancelled()
{
    if (tcpSocket) {
        ui->actionConnect->setEnabled(true);
        ui->actionDisconnect->setEnabled(false);
        tcpSocket->abort();
        tcpSocket->deleteLater();
        tcpSocket = nullptr;
    }
}
