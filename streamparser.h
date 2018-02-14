#ifndef STREAMPARSER_H
#define STREAMPARSER_H

#include <QObject>

class StreamParser : public QObject
{
    Q_OBJECT
public:
    explicit StreamParser(QObject *parent = nullptr);
    void input(QByteArray data);

private:
    QByteArray buffer;
    int frameProgress;
    quint32 frameNumber;
    quint32 frameSequenceNumber;
    bool frameHeaderFound;
    bool frameInProgress;
    bool frameHeader0 = false;
    bool frameHeader1 = false;
    bool frameHeaderInProgress = false;
    int frameHeaderCount = 0;

    void processFrame();

signals:
    void frameReceived(quint32 sequence, QByteArray *data);
};

#endif // STREAMPARSER_H
