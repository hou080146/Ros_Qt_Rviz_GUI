#include "../include/cyrobot_monitor/pingip.h"

PingIP::PingIP(QObject *parent):
    QThread(parent)
{

}


void PingIP::run()
{

    QRegularExpression regexp;
    regexp.setPattern("time=\\d+.\\w+");
    QProcess cmd;
    QString result;
    QString remotIP  = PingIP::ip;
    while(true)
    {

        cmd.start("ping "+ remotIP + " -c 1");//ping ip地址 -c 1表示只ping一次就结束
        cmd.waitForFinished(-1);			//等待ping完成
        result = cmd.readAll();	//将ping之后的结果全部读到outstr中
//        qDebug()<< result ;

        QRegularExpressionMatch match = regexp.match(result, 0);
//        qDebug()<<match.hasMatch() ;
        result = match.captured();
        if(match.hasMatch())	//通过判断有无“往返行程的估计时间”这几个字来检测网路是否通
        {
//            qDebug()<< match.captured() ;
            result = result.mid(5);
            emit SignalPing(result);	//发送网络连接的信号
        }
        else
        {
//                qDebug() << match.captured();
             emit SignalPing("~");	//发送网络断开的信号

        }
        QThread::msleep(1000);
        if(flag == false)
        {
            return;
        }
    }
//    cmd.close();

}
void PingIP::setIp(QString ip)
{
    PingIP::ip = ip;
}
void PingIP::setFlag(bool flag)
{
    PingIP::flag = flag;
}
