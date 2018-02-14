#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QNetworkSession>
#include <QTcpSocket>
#include <QMessageBox>
#include "streamparser.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionConnect_triggered();
    void on_actionDisconnect_triggered();
    void on_connectingMessage_cancelled();

    void on_tcpSocket_connected();
    void on_tcpSocket_disconnected();
    void on_tcpSocket_bytesWritten(qint64 bytes);
    void on_tcpSocket_readyRead();

    void on_parser_frameReceived(quint32 sequence, QByteArray *data);

private:
    Ui::MainWindow *ui;
    QNetworkSession *networkSession = nullptr;
    QTcpSocket *tcpSocket = nullptr;
    QMessageBox *connectingMessage = nullptr;
    QString connectedHost;
    int connectedPort;
    StreamParser *parser;

};

#endif // MAINWINDOW_H
