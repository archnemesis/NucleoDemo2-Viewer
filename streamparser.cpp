#include "streamparser.h"
#include <QDebug>

const char header_0 = 'E';
const char header_1 = 'A';

StreamParser::StreamParser(QObject *parent) : QObject(parent)
{
    frameInProgress = false;
    frameHeader0 = false;
    frameHeader1 = false;
    frameHeaderInProgress = false;
    frameHeaderCount = 0;
    frameProgress = 0;
    frameSequenceNumber = 0;
}

void StreamParser::input(QByteArray data)
{
    char c;

    for (int i = 0; i < data.count(); i++) {
        c = data.at(i);

        if (!frameInProgress) {
            if (!frameHeaderInProgress) {
                if (data.at(i) == 'E') {
                    frameHeader0 = true;
                    frameHeader1 = false;
                }
                else if (frameHeader0 == true && frameHeader1 == false && data.at(i) == 'A') {
                    frameHeader1 = true;
                    frameHeaderInProgress = true;
                    frameHeaderCount = 0;
                }
                else {
                    frameHeader0 = false;
                    frameHeader1 = false;
                }
            }
            else {
                frameSequenceNumber |= (quint32)((quint8)data.at(i) << (8 * frameHeaderCount));
                frameHeaderCount++;

                if (frameHeaderCount == 4) {
                    frameHeaderInProgress = false;
                    frameHeader0 = false;
                    frameHeader1 = false;
                    frameHeaderCount = 0;
                    frameInProgress = true;
                    frameProgress = 0;
                }
            }
        }
        else {
            buffer.append(data.at(i));
            frameProgress++;

            if (frameProgress == 1024) {
                emit frameReceived(frameSequenceNumber, new QByteArray(buffer));
                buffer.clear();
                frameProgress = 0;
                frameInProgress = false;
                frameSequenceNumber = 0;
                frameHeaderCount = 0;
            }
        }
    }
}

void StreamParser::processFrame()
{
    frameProgress = 0;
    frameInProgress = false;
}
