#ifndef QFFMPEG_H
#define QFFMPEG_H

//必须加以下内容,否则编译不能通过,为了兼容C和C99标准
#ifndef INT64_C
#define INT64_C
#define UINT64_C
#endif

//引入ffmpeg头文件
extern "C"{
    #include <libavcodec/avcodec.h>
    #include <libavformat/avformat.h>
    #include <libswscale/swscale.h>
    #include <libavutil/imgutils.h>
}

#include <QObject>
#include <QMutex>
#include <QImage>
#include <QTime>
#include<qcoreapplication.h>
#include <QThread>

class QFFmpeg : public QObject
{
    Q_OBJECT
public:
    explicit QFFmpeg(QObject *parent = 0);
    ~QFFmpeg();

    bool Init();
    void Play();
    void Delay(int msec);

    void SetUrl(QString url){this->url=url;}
    QString Url()const{return url;}
    int VideoWidth()const{return mVideoWidth;}
    int VideoHeight()const{return mVideoWidth;}
    void SetRunFlag(bool m_isCanRun);
    bool GetRunFlag();


private:
    int mVideoWidth;//视频宽度
    int mVideoHeight;//视频高度
    QMutex m_lock;
    bool m_isCanRun = false;
    bool m_isConnect = true;//连接状态flag，防止重新连接时重复释放资源

    AVFormatContext *pFormatCtx = nullptr;
    AVCodecContext *pCodecCtx = nullptr;
    const AVCodec *pCodec = nullptr;
    AVFrame *pFrame = nullptr,*pFrameRGB = nullptr;
    AVPacket *packet = nullptr;
    struct SwsContext *img_convert_ctx = nullptr;

    unsigned char *out_buffer = nullptr;
    int i,videoIndex;
    int ret;
    char errors[1024] = "";

    //rtsp地址:
    QString url ;


signals:
    void GetImage(const QImage &image);
//    void NoVedio();



};

#endif // QFFMPEG_H
