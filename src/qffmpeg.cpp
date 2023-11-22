#include "../include/cyrobot_monitor/qffmpeg.h"
#include <QDateTime>
#include <QDebug>

QFFmpeg::QFFmpeg(QObject *parent) :
    QObject(parent)
{    

    //初始化FFMPEG  调用了这个才能正常适用编码器和解码器
    pFormatCtx = avformat_alloc_context();  //init FormatContext
    //初始化FFmpeg网络模块
    avformat_network_init();    //init FFmpeg network
}

QFFmpeg::~QFFmpeg()
{

}

bool QFFmpeg::Init()
{

    //open Media File
//    qDebug() << url.toStdString().c_str();
    ret = avformat_open_input(&pFormatCtx,url.toStdString().c_str(),NULL,NULL);
    if(ret != 0){
        av_strerror(ret,errors,sizeof(errors));
        qDebug() <<"Failed to open video: ["<< ret << "]"<< errors << endl;
        return false;
    }

    //Get audio information
    ret = avformat_find_stream_info(pFormatCtx,NULL);
    if(ret != 0){
        av_strerror(ret,errors,sizeof(errors));
        qDebug() <<"Failed to get audio info: ["<< ret << "]"<< errors << endl;
        return false;
    }

    videoIndex = -1;

    ///循环查找视频中包含的流信息，直到找到视频类型的流
    ///便将其记录下来 videoIndex
    ///这里我们现在只处理视频流  音频流先不管他
    for (uint i = 0; i < pFormatCtx->nb_streams; i++) {
        if (pFormatCtx->streams[i]->codecpar->codec_type == AVMEDIA_TYPE_VIDEO) {
            videoIndex = i;
        }
    }
//    qDebug() << "1:";
    ///videoIndex-1 说明没有找到视频流
    if (videoIndex == -1) {
        printf("Didn't find a video stream.\n");
        return false;
    }

    //配置编码上下文，AVCodecContext内容
    //1.查找解码器
    pCodec = avcodec_find_decoder(pFormatCtx->streams[videoIndex]->codecpar->codec_id);
    //2.初始化上下文
    pCodecCtx = avcodec_alloc_context3(pCodec);
    //3.配置上下文相关参数
    avcodec_parameters_to_context(pCodecCtx,pFormatCtx->streams[videoIndex]->codecpar);
    //4.打开解码器
    ret = avcodec_open2(pCodecCtx, pCodec, NULL);
    if(ret != 0){
        av_strerror(ret,errors,sizeof(errors));
        qDebug() <<"Failed to open Codec Context: ["<< ret << "]"<< errors << endl;
        return false;
    }

    //初始化视频帧
    pFrame = av_frame_alloc();
    pFrameRGB = av_frame_alloc();
    //为out_buffer申请一段存储图像的内存空间
    out_buffer = (unsigned char*)av_malloc(av_image_get_buffer_size(AV_PIX_FMT_RGB32,pCodecCtx->width,pCodecCtx->height,1));
    //实现AVFrame中像素数据和Bitmap像素数据的关联
    av_image_fill_arrays(pFrameRGB->data,pFrameRGB->linesize, out_buffer,
                   AV_PIX_FMT_RGB32,pCodecCtx->width, pCodecCtx->height,1);
    //为AVPacket申请内存
    packet = (AVPacket *)av_malloc(sizeof(AVPacket));
    //打印媒体信息
    av_dump_format(pFormatCtx,0,"rtsp://127.0.0.1/live/test",0);
    //初始化一个SwsContext
    img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height,
                pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height,
                AV_PIX_FMT_RGB32, SWS_BICUBIC, NULL, NULL, NULL);

    //设置视频label的宽高为视频宽高
//    ui->label->setGeometry(0, 0, pCodecCtx->width, pCodecCtx->height);
    this->mVideoWidth = pCodecCtx->width;
    this->mVideoHeight = pCodecCtx->height;

    m_isConnect = true;
    return true;
}


//线程
void QFFmpeg::Play()
{

    //读取帧数据，并通过av_read_frame的返回值确认是不是还有视频帧
    while(1){

//        qDebug() << "while in :" << n++;

        //先判断标志位，否则程序会崩溃
        if(m_isConnect == true && av_read_frame(pFormatCtx,packet) >=0)//取帧正常进行显示
        {
            //判断视频帧
            if(packet->stream_index == videoIndex){
                //解码视频帧
                ret = avcodec_send_packet(pCodecCtx, packet);
                ret = avcodec_receive_frame(pCodecCtx, pFrame);
                if(ret != 0){
                    av_strerror(ret,errors,sizeof(errors));
                    qDebug() <<"Failed to decode video frame: ["<< ret << "]"<< errors << endl;
                }
                if (ret == 0) {
                    //处理图像数据
                    sws_scale(img_convert_ctx,
                                            (const unsigned char* const*) pFrame->data,
                                            pFrame->linesize, 0, pCodecCtx->height, pFrameRGB->data,
                                            pFrameRGB->linesize);
                    QImage img((uchar*)pFrameRGB->data[0],pCodecCtx->width,pCodecCtx->height,QImage::Format_RGB32);
                    emit GetImage(img);
                    m_isConnect = true;//确定资源存在
                    //释放前需要一个延时
    //                Delay(1);
                }
            }
            //释放packet空间
            av_packet_unref(packet);

            QMutexLocker locker(&m_lock);// 此处加锁,防止访问冲突
            if(!m_isCanRun)//判断是否结束线程
            {
                break;
            }
        }
        else  //没取到帧尝试重新连接
        {
//             emit NoVedio();
            QThread::msleep(100);//加延迟否则这里直接崩溃
//            qDebug()<< "1: ";
            if(m_isConnect == true)
            {
                //释放资源再尝试连接
                av_free(out_buffer);
                av_free(pFrameRGB);

                sws_freeContext(img_convert_ctx);
                avcodec_close(pCodecCtx);
                avcodec_free_context(&pCodecCtx);
                avformat_close_input(&pFormatCtx);

                m_isConnect = false;//资源已经释放

            }

//            qDebug()<< "2: ";
            Init();//尝试连接
//            qDebug()<< "3: ";

            QMutexLocker locker(&m_lock);// 此处加锁,防止访问冲突
            if(!m_isCanRun)//判断是否结束线程
            {
                break;
            }
            continue;
        }

    }

    if(m_isConnect == true)
    {
        //释放资源结束线程
        av_free(out_buffer);
        av_free(pFrameRGB);

        sws_freeContext(img_convert_ctx);
        avcodec_close(pCodecCtx);
        avcodec_free_context(&pCodecCtx);
        avformat_close_input(&pFormatCtx);
//        emit NoVedio();
    }


}


//以毫秒为单位设置延时
void QFFmpeg::Delay(int msec)
{
    QTime dieTime = QTime::currentTime().addMSecs(msec);
    while(QTime::currentTime() < dieTime){
        QCoreApplication::processEvents(QEventLoop::AllEvents,100);
    }
}

void QFFmpeg::SetRunFlag(bool m_isCanRun)
{
    QMutexLocker locker(&m_lock);// 此处加锁,防止访问冲突
    this->m_isCanRun = m_isCanRun;
}

bool QFFmpeg::GetRunFlag()
{
    return this->m_isCanRun;
}
