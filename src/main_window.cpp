/**
 * @file /src/main_window.cpp
 *
 * @brief Implementation for the qt gui->
 *
 * @date February 2011
 **/
/*****************************************************************************
** Includes
*****************************************************************************/

#include <QtGui>
#include <QMessageBox>
#include <iostream>
#include "../include/cyrobot_monitor/main_window.hpp"

//定义在setting
extern bool isconnect;

//test
/*****************************************************************************
** Namespaces
*****************************************************************************/

namespace cyrobot_monitor {

using namespace Qt;

/*****************************************************************************
** Implementation [MainWindow]
*****************************************************************************/

MainWindow::MainWindow(int argc, char** argv, QWidget *parent)
	: QMainWindow(parent),
    ui(new Ui::MainWindowDesign), 
    qnode(argc,argv)
{
    ui->setupUi(this);
    ping = new PingIP(this);

    //初始化界面
    initUis();
    //初始化rviz订阅节点的名称
    initData();
    //读取配置文件，url和是否使用环境变量
//    ReadSettings();

    initVedio();


    connect(ping,SIGNAL(SignalPing(QString)),this,SLOT(updataPing(QString)));
    ui->tab_manager->setCurrentIndex(0);

    set =new Settings();
    set->set_log(qnode.loggingModel());
    //刷新话题列表
    connect(set,SIGNAL(btnRflushTopicList()),this,SLOT(refreashTopicList()));

    //rviz节点树
    //绑定添加rviz话题信号,用来添加话题
    connect(set, SIGNAL(rviz_tree_signal(QTreeWidgetItem *, QString)), this, SLOT(slot_choose_topic(QTreeWidgetItem *, QString)));

    //链接connect
    connect(set, SIGNAL(btnconnect()), this, SLOT(Rosconnect()));
    connect(set, SIGNAL(btndisconnect()), this, SLOT(Rosdisconnect()));
    connections();

    //由于初始化配置时还没连接，所以这里判断一下
    QSettings main_setting("topic_setting","cyrobot_monitor");
    if(main_setting.value("AutoConnect", false).toBool() == true)
    {
        Rosconnect();
    }

    openVedio();

}

//析构函数
MainWindow::~MainWindow()
{

//    if( base_cmd)
//    {
//        delete base_cmd;
//        base_cmd = nullptr;
//    }
    if(map_rviz_)
    {
        map_rviz_->quit();
        delete map_rviz_;
        map_rviz_ = nullptr;
    }

    if(model_rviz)
    {
        model_rviz->quit();
        delete model_rviz;
        model_rviz = nullptr;
    }


    delete set;

    //关闭窗口ping线程安全退出
    if(ping->isRunning() == true)
    {
        ping->setFlag(false);
        ping->quit();
        ping->wait();
        delete ping;
    }

    //视频线程安全退出
    if(ffmpeg1->GetRunFlag() == true)
    {
        ffmpeg1->SetRunFlag(false);
        rtsp1->wait();
        delete ffmpeg1;
        delete rtsp1;
    }
    if(ffmpeg2->GetRunFlag() == true)
    {
        ffmpeg2->SetRunFlag(false);
        rtsp2->wait();
        delete ffmpeg2;
        delete rtsp2;
    }

    if(ffmpeg3->GetRunFlag() == true)
    {
        ffmpeg3->SetRunFlag(false);
        rtsp3->wait();
        delete ffmpeg3;
        delete rtsp3;
    }

    if(ffmpeg4->GetRunFlag() == true)
    {
        ffmpeg4->SetRunFlag(false);
        rtsp4->wait();
        delete ffmpeg4;
        delete rtsp4;
    }

}

//处理用户双击对应通道最大化处理
bool MainWindow::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type()==QEvent::MouseButtonDblClick){
        if (obj==ui->label_video0){
            if (video1Max){

                tempWidth=372;
                tempHeight=349;
                ui->label_video1->setVisible(true);
                ui->label_video2->setVisible(true);
                ui->label_video3->setVisible(true);
            }else{
                tempWidth=748;
                tempHeight=702;

                ui->label_video1->setVisible(false);
                ui->label_video2->setVisible(false);
                ui->label_video3->setVisible(false);
            }
            video1Max=!video1Max;
        }else if (obj==ui->label_video1){
            if (video2Max){
                tempWidth=372;
                tempHeight=349;

                ui->label_video0->setVisible(true);
                ui->label_video2->setVisible(true);
                ui->label_video3->setVisible(true);
            }else{
                tempWidth=748;
                tempHeight=702;

                ui->label_video0->setVisible(false);
                ui->label_video2->setVisible(false);
                ui->label_video3->setVisible(false);
            }
            video2Max=!video2Max;
        }else if (obj==ui->label_video2){
            if (video3Max){
                tempWidth=372;
                tempHeight=349;

                ui->label_video0->setVisible(true);
                ui->label_video1->setVisible(true);
                ui->label_video3->setVisible(true);
            }else{
                tempWidth=748;
                tempHeight=702;

                ui->label_video0->setVisible(false);
                ui->label_video1->setVisible(false);
                ui->label_video3->setVisible(false);
            }
            video3Max=!video3Max;
        }else if (obj==ui->label_video3){
            if (video4Max){
                tempWidth=372;
                tempHeight=349;

                ui->label_video0->setVisible(true);
                ui->label_video1->setVisible(true);
                ui->label_video2->setVisible(true);
            }else{
                tempWidth=748;
                tempHeight=702;

                ui->label_video0->setVisible(false);
                ui->label_video1->setVisible(false);
                ui->label_video2->setVisible(false);
            }
            video4Max=!video4Max;
        }
    }
    return QObject::eventFilter(obj,event);
}

void MainWindow::SetImage1(const QImage &image)
{
    if (image.height()>0){

        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth,tempHeight,Qt::KeepAspectRatio));
        ui->label_video0->setPixmap(pix);
    }
}
void MainWindow::SetImage2(const QImage &image)
{
    if (image.height()>0){
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth,tempHeight,Qt::KeepAspectRatio));
        ui->label_video1->setPixmap(pix);
    }
}
void MainWindow::SetImage3(const QImage &image)
{
    if (image.height()>0){
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth,tempHeight,Qt::KeepAspectRatio));
        ui->label_video2->setPixmap(pix);
    }
}
void MainWindow::SetImage4(const QImage &image)
{
    if (image.height()>0){
        QPixmap pix = QPixmap::fromImage(image.scaled(tempWidth,tempHeight,Qt::KeepAspectRatio));
        ui->label_video3->setPixmap(pix);
    }
}


void MainWindow::openVedio()
{
    QSettings vedio_setting("vedio_setting", "vedio_setting");
    ffmpeg1->SetUrl(vedio_setting.value("vedio0", "rtsp://127.0.0.1/live/test").toString());
    ffmpeg2->SetUrl(vedio_setting.value("vedio1", "rtsp://127.0.0.1/live/test").toString());
    ffmpeg3->SetUrl(vedio_setting.value("vedio2", "rtsp://127.0.0.1/live/test").toString());
    ffmpeg4->SetUrl(vedio_setting.value("vedio3", "rtsp://127.0.0.1/live/test").toString());

    if (ffmpeg1->Init()){
        ffmpeg1->SetRunFlag(true);
        rtsp1->start();
    }
    if (ffmpeg2->Init()){
        ffmpeg2->SetRunFlag(true);
        rtsp2->start();
    }
    if (ffmpeg3->Init()){
        ffmpeg3->SetRunFlag(true);
        rtsp3->start();
    }
    if (ffmpeg4->Init()){
        ffmpeg4->SetRunFlag(true);
        rtsp4->start();
    }
}


void MainWindow::initVedio()
{
//    tempWidth=320;
//    tempHeight=180;
//    tempWidth=ui->widget_2->width()/2;
//    tempHeight=ui->widget_2->height()/2;
    video1Max=false;
    video2Max=false;
    video3Max=false;
    video4Max=false;
    ffmpeg1=new QFFmpeg(this);
    ffmpeg2=new QFFmpeg(this);
    ffmpeg3=new QFFmpeg(this);
    ffmpeg4=new QFFmpeg(this);
    ui->label_video0->installEventFilter(this);
    ui->label_video1->installEventFilter(this);
    ui->label_video2->installEventFilter(this);
    ui->label_video3->installEventFilter(this);

    connect(ffmpeg1,SIGNAL(GetImage(QImage)),this,SLOT(SetImage1(QImage)));
    connect(ffmpeg2,SIGNAL(GetImage(QImage)),this,SLOT(SetImage2(QImage)));
    connect(ffmpeg3,SIGNAL(GetImage(QImage)),this,SLOT(SetImage3(QImage)));
    connect(ffmpeg4,SIGNAL(GetImage(QImage)),this,SLOT(SetImage4(QImage)));
//    connect(ffmpeg1,SIGNAL(NoVedio()),this,SLOT(UpdataNoVedio1()));
//    connect(ffmpeg2,SIGNAL(NoVedio()),this,SLOT(UpdataNoVedio2()));
//    connect(ffmpeg3,SIGNAL(NoVedio()),this,SLOT(UpdataNoVedio3()));
//    connect(ffmpeg4,SIGNAL(NoVedio()),this,SLOT(UpdataNoVedio4()));


    rtsp1=new RtspThread(this);
    rtsp1->setffmpeg(ffmpeg1);
    rtsp2=new RtspThread(this);
    rtsp2->setffmpeg(ffmpeg2);
    rtsp3=new RtspThread(this);
    rtsp3->setffmpeg(ffmpeg3);
    rtsp4=new RtspThread(this);
    rtsp4->setffmpeg(ffmpeg4);





}






void MainWindow::slot_show_image(int frame_id, QImage image)
{
    switch (frame_id)
    {
    case 0:
        ui->label_image00->setPixmap(QPixmap::fromImage(image).scaled(ui->label_image00->width(),ui->label_image00->height()));
        break;
    case 1:
        ui->label_image01->setPixmap(QPixmap::fromImage(image).scaled(ui->label_image01->width(),ui->label_image01->height()));
        break;
    }
}



//初始化UI
void MainWindow::initUis()
{
    QSettings main_setting("topic_setting","cyrobot_monitor");

    setWindowIcon(QIcon(":/images/robot.png"));

    /*m_DashBoardy =new DashBoard(ui->widget_speed_y);
    m_DashBoardy->setGeometry(ui->widget_speed_y->rect());
    m_DashBoardy->set_speed(0)*/;

    ui->tab_manager->setCurrentIndex(0);
    ui->tabWidget->setCurrentIndex(0);
    
    //qucik treewidget
//    ui->treeWidget_quick_cmd->setHeaderLabels(QStringList()<<"key"<<"values");
//    ui->treeWidget_quick_cmd->setHeaderHidden(true);

    battery = new Battery(ui->widget_battery);

    ui->tab_manager->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(0,false);
    ui->btn_floor->setEnabled(false);
    ui->btn_floor_2->setEnabled(false);


}

void MainWindow::initData()
{
    m_mapRvizDisplays.insert("Axes", RVIZ_DISPLAY_AXES);
    m_mapRvizDisplays.insert("Camera", RVIZ_DISPLAY_CAMERA);
    m_mapRvizDisplays.insert("DepthCloud", RVIZ_DISPLAY_DEPTHCLOUD);
    m_mapRvizDisplays.insert("Effort", RVIZ_DISPLAY_EFFORT);
    m_mapRvizDisplays.insert("FluidPressure", RVIZ_DISPLAY_FLUIDPRESSURE);
    m_mapRvizDisplays.insert("Grid", RVIZ_DISPLAY_GRID);
    m_mapRvizDisplays.insert("GridCells", RVIZ_DISPLAY_GRIDCELLS);
    m_mapRvizDisplays.insert("Group", RVIZ_DISPLAY_GROUP);
    m_mapRvizDisplays.insert("Illuminance", RVIZ_DISPLAY_ILLUMINANCE);
    m_mapRvizDisplays.insert("Image", RVIZ_DISPLAY_IMAGE);
    m_mapRvizDisplays.insert("InterativerMarker", RVIZ_DISPLAY_INTERATIVEMARKER);
    m_mapRvizDisplays.insert("LaserScan", RVIZ_DISPLAY_LASERSCAN);
    m_mapRvizDisplays.insert("Map", RVIZ_DISPLAY_MAP);
    m_mapRvizDisplays.insert("Marker", RVIZ_DISPLAY_MARKER);
    m_mapRvizDisplays.insert("MarkerArray", RVIZ_DISPLAY_MARKERARRAY);
    m_mapRvizDisplays.insert("Odometry", RVIZ_DISPLAY_ODOMETRY);
    m_mapRvizDisplays.insert("Path", RVIZ_DISPLAY_PATH);
    m_mapRvizDisplays.insert("PointCloud", RVIZ_DISPLAY_POINTCLOUD);
    m_mapRvizDisplays.insert("PointCloud2", RVIZ_DISPLAY_POINTCLOUD2);
    m_mapRvizDisplays.insert("PointStamped", RVIZ_DISPLAY_POINTSTAMPED);
    m_mapRvizDisplays.insert("Polygon", RVIZ_DISPLAY_POLYGON);
    m_mapRvizDisplays.insert("Pose", RVIZ_DISPLAY_POSE);
    m_mapRvizDisplays.insert("PoseArray", RVIZ_DISPLAY_POSEARRAY);
    m_mapRvizDisplays.insert("PoseWithCovariance", RVIZ_DISPLAY_POSEWITHCOVARIANCE);
    m_mapRvizDisplays.insert("Range", RVIZ_DISPLAY_RANGE);
    m_mapRvizDisplays.insert("RelativeHumidity", RVIZ_DISPLAY_RELATIVEHUMIDITY);
    m_mapRvizDisplays.insert("RobotModel", RVIZ_DISPLAY_ROBOTMODEL);
    m_mapRvizDisplays.insert("TF", RVIZ_DISPLAY_TF);
    m_mapRvizDisplays.insert("Temperature", RVIZ_DISPLAY_TEMPERATURE);
    m_mapRvizDisplays.insert("WrenchStamped", RVIZ_DISPLAY_WRENCHSTAMPED);
}




void MainWindow::initRviz()
{
    //隐藏防止点击导航按键崩溃
    ui->label_rvizShow->hide();
    ui->label_modelShow->hide();

    map_rviz_=new QRviz(ui->verticalLayout_build_map,"qrviz");//在main_window.cpp中初始化Qrviz指针对象
    connect(map_rviz_, &QRviz::ReturnModelSignal, this, &MainWindow::RvizGetModel);//获取tree，在RvizGetModel函数中放进ui里显示，QRviz::GetDisplayTreeModel()发出
    connect(map_rviz_, &QRviz::ReturnModelSignal, set, &Settings::RvizGetModel);
    map_rviz_->GetDisplayTreeModel();//发出ReturnModelSignal信号，更新tree
    
    QMap<QString, QVariant> namevalue_Grid;
    namevalue_Grid.insert("Line Style", "Lines");
    namevalue_Grid.insert("Color", QColor(160, 160, 160));
    namevalue_Grid.insert("Plane Cell Count", 10);
    map_rviz_->DisplayInit(RVIZ_DISPLAY_GRID, "Grid", true, namevalue_Grid);


    //把地图指针传给设置
    connect(this,SIGNAL(rviz_to_setting(QRviz*)), set, SLOT(receiveRviz(QRviz*)));
    emit rviz_to_setting(map_rviz_);

    
    //用于显示机械臂
    model_rviz = new QRviz(ui->verticalLayout_robotmodel,"qrviz_model");

    QMap<QString, QVariant> namevalue_Model;
    namevalue_Model.insert("Update Interval", 0);
    namevalue_Model.insert("Alpha", 1);

    model_rviz->SetGlobalOptions("base_link",QColor(25, 25, 25, 255),30);
    model_rviz->DisplayInit(RVIZ_DISPLAY_GRID, "Grid", true, namevalue_Grid);
    model_rviz->DisplayInit(RVIZ_DISPLAY_ROBOTMODEL, "RobotModel", true, namevalue_Model);
    model_rviz->setView(0);

}

void MainWindow::RvizGetModel(QAbstractItemModel *model)
{
    m_modelRvizDisplay = model;
}

void MainWindow::connections()
{
//    QObject::connect(&qnode, SIGNAL(loggingUpdated()), this, SLOT(updateLoggingView()));
    QObject::connect(&qnode, SIGNAL(rosShutdown()), this, SLOT(slot_rosShutdown()));
    QObject::connect(&qnode, SIGNAL(Master_shutdown()), this, SLOT(slot_rosShutdown()));


    //电源的信号
    connect(&qnode,SIGNAL(power(float)),this,SLOT(slot_power(float)));
    //机器人位置信号,用于状态条显示和返航点
    connect(&qnode,SIGNAL(position(QString,double,double,double,double)),this,SLOT(slot_position_change(QString,double,double,double,double)));
    //绑定快捷按钮相关函数
//    connect(ui->quick_cmd_add_btn,SIGNAL(clicked()),this,SLOT(quick_cmd_add()));
//    connect(ui->quick_cmd_remove_btn,SIGNAL(clicked()),this,SLOT(quick_cmd_remove()));

    //qnode图像显示订阅video话题
    connect(&qnode,SIGNAL(Show_image(int,QImage)),this,SLOT(slot_show_image(int,QImage)));

    //设置2D Pose
    connect(ui->set_pos_btn,SIGNAL(clicked()),this,SLOT(slot_set_2D_Pos()));
    //设置2D goal
    connect(ui->set_goal_btn,SIGNAL(clicked()),this,SLOT(slot_set_2D_Goal()));
    //设置3D goal
//    connect(ui->set_3dgoal_btn,SIGNAL(clicked()),this,SLOT(slot_set_3D_Goal()));
    //设置MoveCamera
    connect(ui->move_camera_btn,SIGNAL(clicked()),this,SLOT(slot_move_camera_btn()));
    //设置Select
    connect(ui->set_select_btn,SIGNAL(clicked()),this,SLOT(slot_set_select()));
    //设置返航点
    connect(ui->set_return_btn,SIGNAL(clicked()),this,SLOT(slot_set_return_point()));
    //返航
    connect(ui->return_btn,SIGNAL(clicked()),this,SLOT(slot_return_point()));
    //左工具栏tab索引改变
    connect(ui->tab_manager,SIGNAL(currentChanged(int)),this,SLOT(slot_tab_manage_currentChanged(int)));
    //右工具栏索引改变
    connect(ui->tabWidget,SIGNAL(currentChanged(int)),this,SLOT(slot_tab_Widget_currentChanged(int)));

}

//刷新当前坐标
void MainWindow::slot_position_change(QString frame,double x,double y,double z,double w)
{
    //更新ui显示
    this->m_postion_frame = frame;
    this->m_postion_x = x;
    this->m_postion_y = y;
    this->m_postion_z = z;
    this->m_postion_w = w;
    statusBar()->showMessage("坐标 Faerm:"+frame+ \
                             " X:"+QString::number(x,10,6)+ \
                             " Y:"+QString::number(y,10,6) + \
                             " Z:"+QString::number(z,10,6) + \
                             " W:"+QString::number(w,10,6)+
                             "    返航点 X:" + \
                             QString::number(m_goal_x,10,6)+ \
                             " Y:"+QString::number(m_goal_y,10,6) + \
                             " Z:"+QString::number(m_goal_z,10,6) + \
                              " W:"+QString::number(m_goal_w,10,6));
}
//刷新返航地点
void MainWindow::slot_set_return_point()
{
    //写入
    m_goal_x = m_postion_x;
    m_goal_y = m_postion_y;
    m_goal_z = m_postion_z;
    m_goal_w = m_postion_w;
    //更新ui返航点显示
    statusBar()->showMessage("坐标 Faerm: "+m_postion_frame+ \
                             "  X:"+QString::number(m_postion_x,10,6)+ \
                             "  Y:"+QString::number(m_postion_y,10,6) + \
                             "  Z:"+QString::number(m_postion_z,10,6) + \
                             "  W:"+QString::number(m_postion_w,10,6)+
                             "      返航点 X:" + \
                             QString::number(m_goal_x,10,6)+ \
                             "  Y:"+QString::number(m_goal_y,10,6) + \
                             "  Z:"+QString::number(m_goal_z,10,6) + \
                             "  W:"+QString::number(m_goal_w,10,6));


}
//返航
void MainWindow::slot_return_point()
{
    qnode.set_goal(m_postion_frame,m_goal_x,m_goal_y,m_goal_z,m_goal_w);
}
//设置导航当前位置按钮的槽函数
void MainWindow::slot_set_2D_Pos()
{
 map_rviz_->Set_Pos();
// ui->label_map_msg->setText("请在地图中选择机器人的初始位置");
}
//设置导航目标位置按钮的槽函数
void MainWindow::slot_set_2D_Goal()
{
  map_rviz_->Set_Goal();
//  ui->label_map_msg->setText("请在地图中选择机器人导航的目标位置");
}
void MainWindow::slot_move_camera_btn()
{
    map_rviz_->Set_MoveCamera();
    qDebug()<<"move camera";
}
void MainWindow::slot_set_select()
{
    map_rviz_->Set_Select();
}

void MainWindow::slot_set_3D_Goal()
{
      map_rviz_->Set_3DGoal();
}


//选中要添加的话题的槽函数
void MainWindow::slot_choose_topic(QTreeWidgetItem *choose, QString name)
{
    QString ClassID = choose->text(0);
    // 检查重名
    name = JudgeDisplayNewName(name);
    
    qDebug() << "choose topic ClassID:" << ClassID << ", name:" << name;
    QMap<QString, QVariant> namevalue;
    namevalue.clear();
    map_rviz_->DisplayInit(m_mapRvizDisplays[ClassID], name, true, namevalue);
}

///
/// \brief 检查重名
/// \param name
/// \return
///
QString MainWindow::JudgeDisplayNewName(QString name)
{
    if (m_modelRvizDisplay != nullptr)
    {
        bool same = true;
        while (same)
        {
            same = false;
            for (int i = 0; i < m_modelRvizDisplay->rowCount(); i++)
            {
                //m_sRvizDisplayChooseName = index.data().value<QString>();
                if (m_modelRvizDisplay->index(i, 0).data().value<QString>() == name)
                {
                    if (name.indexOf("_") != -1)
                    {
                        int num = name.section("_", -1, -1).toInt();
                        name = name.left(name.length() - name.section("_", -1, -1).length() - 1);
                        if (num <= 1)
                        {
                            num = 2;
                        }
                        else
                        {
                            num++;
                        }
                        name = name + "_" + QString::number(num);
                    }
                    else {
                      name = name + "_2";
                    }
                    same = true;
                    break;
                }
            }
        }
    }
    return name;
}

//左工具栏索引改变
void MainWindow::slot_tab_manage_currentChanged(int index)
{
    switch (index) {
    case 0:

        break;
    case 1:

        ui->tabWidget->setCurrentIndex(0);
        break;
    case 2:
        break;

    }
}
//右工具栏索引改变
void MainWindow::slot_tab_Widget_currentChanged(int index)
{
    switch (index) {
    case 0:

        break;
    case 1:
        ui->tab_manager->setCurrentIndex(0);
        break;
    case 2:
        break;

    }
}




/*****************************************************************************
** Implementation [Slots]
*****************************************************************************/

void MainWindow::showNoMasterMessage() {
	QMessageBox msgBox;
	msgBox.setText("Couldn't find the ros master.");
	msgBox.exec();
    close();
}

void MainWindow::inform(QString strdata)
{
    QMessageBox m_r;
    m_r.setWindowTitle("提示");
    m_r.setText(strdata);
    m_r.exec();
}
bool MainWindow::AskInform(QString strdata)
{
    QMessageBox m_r;

    m_r.setWindowTitle("提示");
    m_r.setText(strdata);
    m_r.addButton(tr("确认"), QMessageBox::ActionRole);
    m_r.addButton(tr("取消"), QMessageBox::ActionRole);

    int isok = m_r.exec();
    if (isok == 1)
    {
        return false;
    }
    else
    {
        return true;
    }
}

/*
 * These triggers whenever the button is clicked, regardless of whether it
 * is already checked or not.
 */


void MainWindow::refreashTopicList()
{
    QMap<QString,QString> topic_list= qnode.get_topic_list();
    set->initTopicList(topic_list);
}
//当ros与master的连接断开时
void MainWindow::slot_rosShutdown()
{
    ui->label_robot_staue_img->setPixmap(QPixmap::fromImage(QImage("://images/disconnect.png")));
    ui->label_statue_text->setStyleSheet("color:red;");
    ui->label_statue_text->setText("离线");
}
void MainWindow::slot_power(float p)
{

    ui->label_power->setText(QString::number(static_cast<double>(p)).mid(0,5)+"V");
    double n=(static_cast<double>(p)-10)/1.5;
    int value = static_cast<int>(n*100);
    battery->inputValue(value>100?100:value);
//    ui->progressBar->setValue(value>100?100:value);
    //当电量过低时发出提示
    if(n*100<=20)
    {
//         ui->progressBar->setStyleSheet("QProgressBar::chunk {background-color: red;width: 20px;} QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}");
          // QMessageBox::warning(NULL, "电量不足", "电量不足，请及时充电！", QMessageBox::Yes , QMessageBox::Yes);
    }
    else{
//        ui->progressBar->setStyleSheet("QProgressBar {border: 2px solid grey;border-radius: 5px;text-align: center;}");
    }
}



/*****************************************************************************
** Implemenation [Slots][manually connected]
*****************************************************************************/

/**
 * This function is signalled by the underlying model. When the model changes,
 * this will drop the cursor down to the last line in the QListview to ensure
 * the user can always see the latest log message.
 */
//void MainWindow::updateLoggingView() {
//        ui->view_logging->scrollToBottom();
//}

/*****************************************************************************
** Implementation [Menu]
*****************************************************************************/

void MainWindow::on_actionAbout_triggered() {
    //QMessageBox::about(this, tr("About ..."),tr("<h2>PACKAGE_NAME Test Program 0.10</h2><p>Copyright Yujin Robot</p><p>This package needs an about description.</p>"));
}

/*****************************************************************************
** Implementation [Configuration]
*****************************************************************************/

void MainWindow::ReadSettings() {
    QSettings settings("Qt-Ros Package", "cyrobot_rviz_tree");
    restoreGeometry(settings.value("geometry").toByteArray());
    restoreState(settings.value("windowState").toByteArray());


    //读取快捷指令的setting
//    QSettings quick_setting("quick_setting","cyrobot_rviz_tree");
//    QStringList ch_key=quick_setting.childKeys();
//    for(auto c:ch_key)
//    {
//        add_quick_cmd(c,quick_setting.value(c,QString("")).toString());
//    }

    QSettings remoteip_setting("topic_setting","cyrobot_monitor");
    ping->setIp(remoteip_setting.value("RemoteIP", QString("127.0.0.1")).toString());


}

void MainWindow::WriteSettings() {
    QSettings settings("Qt-Ros Package", "cyrobot_rviz_tree");
    settings.setValue("geometry", saveGeometry());
    //settings.setValue("windowState", saveState());

    //存下快捷指令的setting
//    QSettings quick_setting("quick_setting","cyrobot_rviz_tree");
//    quick_setting.clear();
//    for(int i=0;i<ui->treeWidget_quick_cmd->topLevelItemCount();i++)
//    {
//        QTreeWidgetItem *top=ui->treeWidget_quick_cmd->topLevelItem(i);
//        QTextEdit *cmd_val= static_cast<QTextEdit *>(ui->treeWidget_quick_cmd->itemWidget(top->child(0),1));
//        quick_setting.setValue(top->text(0),cmd_val->toPlainText());
//    }
}
void MainWindow::updataPing(QString str)
{

        ui->label_Pingms->setText(str+"ms");
}

void MainWindow::closeEvent(QCloseEvent *event)
{
//    WriteSettings();
    QMainWindow::closeEvent(event);
}

}  // namespace cyrobot_monitor





/// \brief 连接ROS
void cyrobot_monitor::MainWindow::Rosconnect()
{

    QSettings ROS_setting("topic_setting","cyrobot_monitor");
    QString master_IP = ROS_setting.value("MasterIP", QString("http://127.0.0.1:11311/")).toString();
    QString local_IP = ROS_setting.value("LocalIP", QString("127.0.0.1")).toString();
    bool useENV = ROS_setting.value("UseEnvironment", false).toBool();
    rvizCfgRW = ROS_setting.value("Rviz_AutoWR", false).toBool();
    //如果使用环境变量
    if ( true == useENV )
    {
        if ( !qnode.init() )
        {
            //showNoMasterMessage();
            isconnect = false;
            QMessageBox::warning(nullptr, "失败", "连接ROS Master失败！请检查你的网络或连接字符串！", QMessageBox::Yes, QMessageBox::Yes);
            ui->label_robot_staue_img->setPixmap(QPixmap::fromImage(QImage("://images/disconnect.png")));
            ui->label_statue_text->setStyleSheet("color:red;");
            ui->label_statue_text->setText("离线");
            ui->tab_manager->setTabEnabled(1,false);
            ui->btn_floor->setEnabled(false);
            ui->btn_floor_2->setEnabled(false);
//            ui->tabWidget->setTabEnabled(0,false);
//            ui->tabWidget->setTabEnabled(1,false);

            return ;
        }
    }
    //如果不使用环境变量
    else
    {
        if ( !qnode.init(master_IP.toStdString(), local_IP.toStdString()) )
        {
            isconnect = false;
            QMessageBox::warning(nullptr, "失败", "连接ROS Master失败！请检查你的网络或连接字符串！", QMessageBox::Yes, QMessageBox::Yes);
            ui->label_robot_staue_img->setPixmap(QPixmap::fromImage(QImage("://images/disconnect.png")));
            ui->label_statue_text->setStyleSheet("color:red;");
            ui->label_statue_text->setText("离线");
            ui->tab_manager->setTabEnabled(1,false);
            ui->btn_floor->setEnabled(false);
            ui->btn_floor_2->setEnabled(false);
            ui->tabWidget->setTabEnabled(0,false);

            //showNoMasterMessage();
            return ;
        }
    }
    isconnect = true;
    ui->tab_manager->setTabEnabled(1,true);
    ui->tabWidget->setTabEnabled(0,true);
    ui->btn_floor->setEnabled(true);
    ui->btn_floor_2->setEnabled(true);

    //初始化rviz
    initRviz();
    

    ui->label_robot_staue_img->setPixmap(QPixmap::fromImage(QImage("://images/connect.png")));
    ui->label_statue_text->setStyleSheet("color:green;");
    ui->label_statue_text->setText("在线");

    //显示话题列表
    refreashTopicList();

    if(rvizCfgRW == true)
    {
        set->rviz_read();
    }
    //打开ping线程
    ping->setFlag(true);
    ping->start();

    //设置地图视角和跟随
    map_rviz_->setView(1);
    ui->btn_setView->setText("第三人称");

}

/// \brief 断开ROS
void cyrobot_monitor::MainWindow::Rosdisconnect()
{

    qnode.disinit();
    isconnect = false;
    map_rviz_->quit();
    delete map_rviz_;
    map_rviz_ = nullptr;
    model_rviz->quit();
    delete model_rviz;
    model_rviz = nullptr;

    ui->label_rvizShow->show();
    ui->label_modelShow->show();

    //退出ping线程
    if(ping->isRunning() == true)
    {
        ping->setFlag(false);
        ping->quit();
    }

    ui->label_Pingms->setText("~");
    ui->tab_manager->setTabEnabled(1,false);
    ui->tabWidget->setTabEnabled(0,false);
    ui->btn_floor->setEnabled(false);
    ui->btn_floor_2->setEnabled(false);
}




void cyrobot_monitor::MainWindow::on_btn_config_clicked()
{
    set->show();
}






//void cyrobot_monitor::MainWindow::on_btn_floor_clicked()
//{
//    qnode.set_floor(ui->floor_Slider->value());
//}

void cyrobot_monitor::MainWindow::on_btn_floor_clicked()
{
    qnode.set_floor(0);
}

void cyrobot_monitor::MainWindow::on_btn_floor_2_clicked()
{
    qnode.set_floor(1);
}

void cyrobot_monitor::MainWindow::on_btn_setView_clicked()
{
    if(ui->btn_setView->text() == "第三人称")
    {
        map_rviz_->setView(2);
        ui->btn_setView->setText("俯视角");

    }
    else if(ui->btn_setView->text() == "俯视角")
    {
        map_rviz_->setView(1);
        ui->btn_setView->setText("第三人称");
    }
}

void cyrobot_monitor::MainWindow::on_public_btn_clicked()
{
    map_rviz_->Set_Public();
}

void cyrobot_monitor::MainWindow::on_btn_top_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 2,true);
}

void cyrobot_monitor::MainWindow::on_btn_bot_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 2,false);
}

void cyrobot_monitor::MainWindow::on_btn_left_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 1,false);
}

void cyrobot_monitor::MainWindow::on_btn_right_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 1,true);
}

void cyrobot_monitor::MainWindow::on_btn_max_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 0,true);
}

void cyrobot_monitor::MainWindow::on_btn_min_clicked()
{
    int view = 2;
    if(ui->btn_setView->text() ==  "第三人称")
        view = 1;

    map_rviz_->setViewConfig(view, 0,false);
}
