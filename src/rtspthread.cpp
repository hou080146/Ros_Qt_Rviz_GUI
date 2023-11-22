#include "../include/cyrobot_monitor/rtspthread.h"

RtspThread::RtspThread(QObject *parent) :
    QThread(parent)
{
}

void  RtspThread::run()
{    
    ffmpeg->Play();
}
