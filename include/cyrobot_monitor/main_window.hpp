/**
 * @file /include/cyrobot_monitor/main_window.hpp
 *
 * @brief Qt based gui for cyrobot_monitor.
 *
 * @date November 2010
 **/
#ifndef cyrobot_monitor_MAIN_WINDOW_H
#define cyrobot_monitor_MAIN_WINDOW_H

/*****************************************************************************
** Includes
*****************************************************************************/

//#include <QtGui/QMainWindow>
#include "ui_main_window.h"
#include "qnode.hpp"
#include "addtopics.h"
#include "settings.h"
#include "qrviz.hpp"
#include"pingip.h"
#include "qffmpeg.h"
#include "rtspthread.h"
//仪表盘头文件
#include "dashboard.h"
#include "switchbutton.h"

//
#include "QProcess"
#include <QStandardItemModel>
#include <QTreeWidgetItem>
#include <QSoundEffect>
#include <QComboBox>
#include <QSpinBox>
#include <QVariant>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QInputDialog>
#include <QFileDialog>
#include <map>
//rviz
#include <rviz/visualization_manager.h>
#include <rviz/render_panel.h>
#include <rviz/display.h>
#include <rviz/tool_manager.h>
#include <rviz/visualization_manager.h>
#include <rviz/render_panel.h>
#include <rviz/display.h>
#include<rviz/tool.h>

#include<rviz/properties/property_tree_widget.h>
//
#include"battery.h"

namespace Ui {
class MainWindow;
}

/*****************************************************************************
** Namespace
*****************************************************************************/
namespace cyrobot_monitor {

/*****************************************************************************
** Interface [MainWindow]
*****************************************************************************/
/**
 * @brief Qt central, all operations relating to the view part here.
 */
class MainWindow : public QMainWindow
{
    Q_OBJECT
    
    public:
        MainWindow(int argc, char** argv, QWidget *parent = nullptr);
        ~MainWindow();
    
        void ReadSettings(); // Load up qt program settings at startup
        void WriteSettings(); // Save qt program settings when closing
    
        void closeEvent(QCloseEvent *event); // Overloaded function
        void showNoMasterMessage();
        void initRviz();
        void initUis();
private slots:
        //vedio
        void SetImage1(const QImage &image);
        void SetImage2(const QImage &image);
        void SetImage3(const QImage &image);
        void SetImage4(const QImage &image);
        void openVedio();
protected:
        //视频双击放大事件
    bool eventFilter(QObject *obj, QEvent *event);

    public slots:
      /******************************************
      ** Auto-connections (connectSlotsByName())
      *******************************************/
        void on_actionAbout_triggered();//没用

        void slot_power(float p);
        void slot_rosShutdown();
//        void quick_cmds_check_change(int);
//        void cmd_output();
//        void cmd_error_output();
        void refreashTopicList();
        void RvizGetModel(QAbstractItemModel *model);
        /******************************************
        ** Manual connections
        *******************************************/


        void slot_tab_manage_currentChanged(int);
        void slot_tab_Widget_currentChanged(int);
        void slot_choose_topic(QTreeWidgetItem *choose, QString name);
        void slot_set_2D_Goal();
        void slot_set_3D_Goal();
        void slot_set_2D_Pos();
        void slot_set_select();
        void slot_move_camera_btn();

        //设置返航点
        void slot_set_return_point();
        //返航
        void slot_return_point();
        //机器人位置
        void slot_position_change(QString,double,double,double,double);
//        void quick_cmd_add();
//        void quick_cmd_remove();


        void updataPing(QString str);






private slots:
        void on_btn_setView_clicked();

private slots:
        void on_btn_floor_2_clicked();

private slots:
        void on_btn_floor_clicked();

private slots:


        //设置界面
        void on_btn_config_clicked();

        void Rosdisconnect();
          
        void Rosconnect();
        void slot_show_image(int frame_id, QImage image);


signals:
        //发送地图rviz给setting
        void rviz_to_setting(QRviz *rviz);

        
private:
        Ui::MainWindowDesign *ui;
        void initData();
        QString JudgeDisplayNewName(QString name);
      
        void connections();
        void add_quick_cmd(QString name,QString shell);
        
        void inform(QString);
        bool AskInform(QString);

        void initVedio();


        
        QNode qnode;


        switchbutton *m_switchbutton;
//        QProcess *quick_cmd = nullptr;
//        QProcess *close_remote_cmd = nullptr;
//        QProcess *base_cmd = nullptr;

        QRviz *map_rviz_ = nullptr;
        QStandardItemModel* treeView_rviz_model = nullptr; //meiyong
        //存放rviz treewidget当前显示的控件及控件的父亲的地址
        QMap <QWidget*,QTreeWidgetItem *> widget_to_parentItem_map;//kuaijiezhilin
        //存放状态栏的对应关系 display名 状态item
        QMap <QString,QTreeWidgetItem *> tree_rviz_stues; //meiyong
        //存放display的当前值 item名，参数名称和值
        QMap <QTreeWidgetItem*,QMap<QString,QString>> tree_rviz_values;//没用

        Settings *set = nullptr;
        QSoundEffect *media_player = nullptr;
        
        QAbstractItemModel* m_modelRvizDisplay;
        
        QMap<QString, QString> m_mapRvizDisplays;
        
        QString m_sRvizDisplayChooseName_;

        PingIP *ping = nullptr;

        QRviz *model_rviz = nullptr;

        //电池图标
        Battery *battery = nullptr;

        //地图配置数据读取标志位
        bool rvizCfgRW = false;

        //当前位置
        QString m_postion_frame ;
        double m_postion_x;
        double m_postion_y;
        double m_postion_z;
        double m_postion_w;
        //记录返航点
        double m_goal_x;
        double m_goal_y;
        double m_goal_z;
        double m_goal_w;

        //视频数据
        //长宽从界面动态获取会有bug
        int tempWidth=372;
        int tempHeight=349;
        bool video1Max;
        bool video2Max;
        bool video3Max;
        bool video4Max;

        QFFmpeg *ffmpeg1 = nullptr;
        QFFmpeg *ffmpeg2 = nullptr;
        QFFmpeg *ffmpeg3 = nullptr;
        QFFmpeg *ffmpeg4 = nullptr;

        RtspThread *rtsp1 = nullptr;
        RtspThread *rtsp2 = nullptr;
        RtspThread *rtsp3 = nullptr;
        RtspThread *rtsp4 = nullptr;




};
}// namespace cyrobot_monitor

#endif // cyrobot_monitor_MAIN_WINDOW_H
