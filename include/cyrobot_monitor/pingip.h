#ifndef PINGIP_H
#define PINGIP_H

#include <QThread>

#include <QObject>
#include <QProcess>
#include<qdebug.h>
#include <QTextCodec>

#include <QRegularExpression>

class PingIP : public QThread
{
    Q_OBJECT


public:
    explicit PingIP(QObject *parent = 0);

    void setIp(QString ip);
    void setFlag(bool b);
protected:
    void run();
signals:
    void SignalPing(QString time);
private:
    QString ip;
    bool flag  = true;
};

#endif // PINGIP_H
