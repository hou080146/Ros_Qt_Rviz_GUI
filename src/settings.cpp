#include "../include/cyrobot_monitor/settings.h"
#include "ui_settings.h"
#include<QDebug>
//窗口居中
#include <QDesktopWidget>
#include <QApplication>


//全局变量，连接标志位
bool isconnect = false;

Settings::Settings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Settings)
{
    ui->setupUi(this);

    initForm();
    //rviz节点树
    addtopic_form = new AddTopics();
    connection();


}

Settings::~Settings()
{
    delete ui;
    delete addtopic_form;
}

void Settings::initForm()
{
    //窗口居中
    QDesktopWidget *deskdop=QApplication::desktop();
    move((deskdop->width()-this->width())/2, (deskdop->height()-this->height())/2);
    //模态
    setWindowModality(Qt::ApplicationModal);
    ui->tabWidget->setCurrentIndex(0);

    ui->checkBox_autoConnect->setEnabled(true);
    ui->checkbox_use_environment->setEnabled(true);

    QSettings main_setting("topic_setting","cyrobot_monitor");
    ui->lineEdit_odm->setText(main_setting.value("topic_odom","raw_odom").toString());
    ui->lineEdit_power->setText(main_setting.value("topic_power","power").toString());
    ui->lineEdit_power_min->setText(main_setting.value("power_min","10").toString());
    ui->lineEdit_power_max->setText(main_setting.value("power_max","12").toString());
    ui->lineEdit_RemoteIP->setText(main_setting.value("RemoteIP","127.0.0.1").toString());
//    ui->lineEdit_odom->setText(main_setting.value("odom_topic","amcl_pose").toString());
    ui->lineEdit_masterIP->setText(main_setting.value("MasterIP", "http://127.0.0.1:11311/").toString());
    ui->lineEdit_localIP->setText(main_setting.value("LocalIP", "127.0.0.1").toString());
    ui->checkbox_use_environment->setEnabled(main_setting.value("UseEnvironment", false).toBool());

    //floor
//    ui->spinBox_fllor->setValue(main_setting.value("Floor","2").toInt());

    QSettings vedio_setting("vedio_setting", "vedio_setting");
    ui->lineEdit_vedio0->setText(vedio_setting.value("vedio0", "rtsp://127.0.0.1/live/test").toString());
    ui->lineEdit_vedio1->setText(vedio_setting.value("vedio1", "rtsp://127.0.0.1/live/test").toString());
    ui->lineEdit_vedio2->setText(vedio_setting.value("vedio2", "rtsp://127.0.0.1/live/test").toString());
    ui->lineEdit_vedio3->setText(vedio_setting.value("vedio3", "rtsp://127.0.0.1/live/test").toString());

    QSettings view_setting("view_setting", "view_setting");
    ui->lineEdit_ncdistance0->setText(view_setting.value("ncdistance0","0.001").toString());
    ui->cmb_targetFrame0->setCurrentIndex(view_setting.value("targetFrame0","0").toInt());
    ui->lineEdit_distance->setText(view_setting.value("distance","8").toString());
    ui->lineEdit_yaw->setText(view_setting.value("yaw","0.785398").toString());
    ui->lineEdit_pitch->setText(view_setting.value("pitch","0.785398").toString());

    ui->lineEdit_ncdistance1->setText(view_setting.value("ncdistance1","0.001").toString());
    ui->cmb_targetFrame1->setCurrentIndex(view_setting.value("targetFrame1","0").toInt());
    ui->lineEdit_scale->setText(view_setting.value("scale","70").toString());
    ui->lineEdit_angle->setText(view_setting.value("angle","0.000").toString());

    int m_style = main_setting.value("Style", "0").toInt();
    ui->cbx_style->setCurrentIndex(m_style);
    on_cbx_style_currentIndexChanged(m_style);




    if(main_setting.value("Rviz_AutoWR", false).toBool() == true)
    {
        ui->cbx_rviz_autoRW->setCheckState(Qt::Checked);
    }
    else
    {
        ui->cbx_rviz_autoRW->setCheckState(Qt::Unchecked);
    }


    //自动连接,初始化时信号还没连接所以这里发信号没用，但是要改ui
    if(main_setting.value("AutoConnect", false).toBool() == true)
    {
        ui->checkBox_autoConnect->setCheckState(Qt::Checked);
//        emit btnconnect();
//        ui->btn_connect->setEnabled(false);
//        ui->btn_disconnect->setEnabled(true);
    }
    else
    {
        ui->checkBox_autoConnect->setCheckState(Qt::Unchecked);
//        ui->btn_connect->setEnabled(true);
//        ui->btn_disconnect->setEnabled(false);

    }
}

void Settings::connection()
{
    connect(addtopic_form, SIGNAL(Topic_choose(QTreeWidgetItem *, QString)), this, SLOT(rviz_tree_sendsignal(QTreeWidgetItem *, QString)));
    connect(ui->btn_ok,SIGNAL(clicked()),this,SLOT(slot_ok_btn_click()));
    connect(ui->btn_cancel,SIGNAL(clicked()),this,SLOT(slot_cancel_btn_click()));
}

void Settings::rviz_tree_sendsignal(QTreeWidgetItem *choose, QString name)
{
    emit rviz_tree_signal(choose, name);
}

void Settings::inform(QString strdata)
{
    QMessageBox m_r;
    m_r.setWindowTitle("提示");
    m_r.setText(strdata);
    m_r.exec();
}

void Settings::RvizGetModel(QAbstractItemModel *model)
{
    m_modelRvizDisplay = model;
    ui->treeView_rvizDisplayTree->setModel(model);
}
void Settings::slot_ok_btn_click()
{
    QSettings main_setting("topic_setting","cyrobot_monitor");
    main_setting.setValue("topic_odom",ui->lineEdit_odm->text());
    main_setting.setValue("topic_power",ui->lineEdit_power->text());
    main_setting.setValue("power_min",ui->lineEdit_power_min->text());
    main_setting.setValue("power_max",ui->lineEdit_power_max->text());
    main_setting.setValue("RemoteIP",ui->lineEdit_RemoteIP->text());
    main_setting.setValue("MasterIP",ui->lineEdit_masterIP->text());
    main_setting.setValue("LocalIP",ui->lineEdit_localIP->text());
    main_setting.setValue("AutoConnect",ui->checkBox_autoConnect->checkState());
    main_setting.setValue("UseEnvironment",ui->checkbox_use_environment->checkState());
    main_setting.setValue("Rviz_AutoWR",ui->cbx_rviz_autoRW->checkState());
    main_setting.setValue("Style", ui->cbx_style->currentIndex());

    //floor
//    main_setting.setValue("Floor", ui->spinBox_fllor->value());

    QSettings vedio_setting("vedio_setting", "vedio_setting");
    vedio_setting.setValue("vedio0", ui->lineEdit_vedio0->text());
    vedio_setting.setValue("vedio1", ui->lineEdit_vedio1->text());
    vedio_setting.setValue("vedio2", ui->lineEdit_vedio2->text());
    vedio_setting.setValue("vedio3", ui->lineEdit_vedio3->text());

    QSettings view_setting("view_setting", "view_setting");
    view_setting.setValue("ncdistance0", ui->lineEdit_ncdistance0->text());
    view_setting.setValue("targetFrame0", ui->cmb_targetFrame0->currentIndex());
    view_setting.setValue("distance", ui->lineEdit_distance->text());
    view_setting.setValue("yaw", ui->lineEdit_yaw->text());
    view_setting.setValue("pitch", ui->lineEdit_pitch->text());
//    ui->lineEdit_ncdistance->setValue(view_setting.value("ncdistance0","").toDouble());
//    ui->lineEdit_targetFrame->setCurrentIndex(view_setting.value("targetFrame0","").toInt());
//    ui->lineEdit_distance->setValue(view_setting.value("distance","").toDouble());
//    ui->lineEdit_yaw->setValue(view_setting.value("yaw","").toDouble());
//    ui->lineEdit_pitch->setValue(view_setting.value("pitch","").toDouble());
    view_setting.setValue("ncdistance1", ui->lineEdit_ncdistance1->text());
    view_setting.setValue("targetFrame1", ui->cmb_targetFrame1->currentIndex());
    view_setting.setValue("scale", ui->lineEdit_scale->text());
    view_setting.setValue("angle", ui->lineEdit_angle->text());

//    ui->lineEdit_ncdistance_2->setValue(view_setting.value("ncdistance1","").toDouble());
//    ui->cmb_targetFrame_2->setCurrentIndex(view_setting.value("targetFrame1","").toInt());
//    ui->lineEdit_scale->setValue(view_setting.value("scale","").toDouble());
//    ui->lineEdit_Angle->setValue(view_setting.value("angle","").toDouble());

    //保存地图配置
    if(ui->cbx_rviz_autoRW->checkState() == true)
    {
        rviz_save();
    }

//    QMessageBox::critical(NULL, "保存成功！", "保存成功，部分功能需重启后生效！", QMessageBox::No | QMessageBox::Yes, QMessageBox::Yes);
    this->close();
}
void Settings::slot_cancel_btn_click()
{
    this->close();
}


void Settings::on_btn_connect_clicked()
{
    if(isconnect == false)
    {
        emit btnconnect();
    }

//    ui->btn_connect->setEnabled(false);
//    ui->btn_disconnect->setEnabled(true);

}

void Settings::on_btn_disconnect_clicked()
{
    if(isconnect == true)
    {
        emit btndisconnect();
    }

//    ui->btn_connect->setEnabled(true);
//    ui->btn_disconnect->setEnabled(false);
}

void Settings::on_btn_add_topic_clicked()
{
    addtopic_form->show();
}

void Settings::on_btn_remove_topic_clicked()
{
    if (ui->treeView_rvizDisplayTree->currentIndex().row() >= 0)
    {
        m_sRvizDisplayChooseName_ = ui->treeView_rvizDisplayTree->currentIndex().data().value<QString>();
        map_rviz_->RemoveDisplay(m_sRvizDisplayChooseName_);
        if (ui->treeView_rvizDisplayTree->currentIndex().row() >= 0)
        {
            on_treeView_rvizDisplayTree_clicked(ui->treeView_rvizDisplayTree->currentIndex());
        }
        else
        {
            m_sRvizDisplayChooseName_.clear();
        }
    }
    else
    {
        inform("请选择Display后再执行此操作");
    }
}

QString Settings::JudgeDisplayNewName(QString name)
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

void Settings::receiveRviz(QRviz *rviz)
{
    if(rviz == nullptr)
    {
        return;
    }
    map_rviz_ = rviz;
}

void Settings::on_btn_rename_topic_clicked()
{
    if (ui->treeView_rvizDisplayTree->currentIndex().row() < 0)
    {
        inform("请选择Display后再执行此操作");
        return ;
    }
    QString dlgTitle = "重命名";
    QString txtlabel = "请输入名字：";
    QString defaultInupt = m_sRvizDisplayChooseName_;
    QLineEdit::EchoMode echoMode = QLineEdit::Normal;
    bool ok = false;
    QString newname = QInputDialog::getText(this, dlgTitle, txtlabel, echoMode, defaultInupt, &ok);
    if (ok && !newname.isEmpty())
    {
        if (newname != defaultInupt)
        {
            QString nosamename = JudgeDisplayNewName(newname);
            map_rviz_->RenameDisplay(defaultInupt, nosamename);
            m_sRvizDisplayChooseName_ = nosamename;
            if (nosamename != newname)
            {
                inform("命名重复！命名已自动更改为" + nosamename);
            }
        }
    }
    else if (ok)
    {
        inform("输入内容为空，重命名失败");
    }
}

QString Settings::getUserName()
{
    QString userPath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString userName = userPath.section("/", -1, -1);
    return userName;
}

QString Settings::appPath()
{
    static QString path;
        if (path.isEmpty()) {
    #ifdef Q_OS_ANDROID
            //默认安卓根目录
            path = "/storage/emulated/0";
            //带上程序名称作为目录 前面加个0方便排序
            path = path + "/0" + appName();
    #else
            path = qApp->applicationDirPath();
    #endif
        }

        return path;
}

QString Settings::appName()
{
    //没有必要每次都获取,只有当变量为空时才去获取一次
    static QString name;
    if (name.isEmpty()) {
        name = qApp->applicationFilePath();
        //下面的方法主要为了过滤安卓的路径 lib程序名_armeabi-v7a
        QStringList list = name.split("/");
        name = list.at(list.count() - 1).split(".").at(0);
        name.replace("_armeabi-v7a", "");
    }

    return name;
}
void Settings::rviz_save()
{
    if (map_rviz_ == nullptr || isconnect != true)
    {
        return;
    }
    QString path = appPath() + "/Config" + "/auto_save.yaml";
    qDebug()<< "rviz_save: " + path;
    map_rviz_->OutDisplaySet(path);
}
void Settings::rviz_read()
{
    if (map_rviz_ == nullptr || isconnect != true)
    {
        return;
    }
    QString path = appPath() + "/Config" + "/auto_save.yaml";
    qDebug()<< "rviz_read: " + path;
    if (!path.isEmpty())
    {
        map_rviz_->ReadDisplaySet(path);
    }
}

void Settings::on_btn_rvizReadDisplaySet_clicked()
{
    if (map_rviz_ == nullptr)
    {
        return;
    }
    QString apppath = appPath();
    QString path = QFileDialog::getOpenFileName(this, "导入 RVIZ Display 配置", apppath + "/Config", "YAML(*.yaml);;ALL(*.*)");
    if (!path.isEmpty())
    {
        map_rviz_->ReadDisplaySet(path);
    }
}

void Settings::on_btn_rvizSaveDisplaySet_clicked()
{
    if (map_rviz_ == nullptr)
    {
        return;
    }
    QString apppath = appPath();
    QString path = QFileDialog::getSaveFileName(this, "导出 RVIZ Display 配置", apppath + "/Config", "YAML(*.yaml)");

    if (!path.isEmpty())
    {
        if (path.section('/', -1, -1).indexOf('.') < 0)
        {
            path = path + ".yaml";
        }
        map_rviz_->OutDisplaySet(path);
    }
}

void Settings::on_treeView_rvizDisplayTree_clicked(const QModelIndex &index)
{
    m_sRvizDisplayChooseName_ = index.data().value<QString>();
    if (index.parent().row() == -1)   // Display 的根节点
    {
        if (index.row() > 1)
        {
            ui->btn_remove_topic->setEnabled(true);
            ui->btn_rename_topic->setEnabled(true);
        }
        else
        {
            ui->btn_remove_topic->setEnabled(false);
            ui->btn_rename_topic->setEnabled(false);
        }
    }
    else
    {
        ui->btn_remove_topic->setEnabled(false);
        ui->btn_rename_topic->setEnabled(false);
    }
}


void Settings::loadStyle(const QString &qssFile)
{
    //加载样式表
    QString qss;
    QFile file(qssFile);
    if (file.open(QFile::ReadOnly)) {
        //用QTextStream读取样式文件不用区分文件编码 带bom也行
        QStringList list;
        QTextStream in(&file);
        //in.setCodec("utf-8");
        while (!in.atEnd()) {
            QString line;
            in >> line;
            list << line;
        }
        file.close();
        qss = list.join("\n");
        QString paletteColor = qss.mid(20, 7);
        qApp->setPalette(QPalette(paletteColor));
        //用时主要在下面这句
        qApp->setStyleSheet(qss);
    }
}
void Settings::on_cbx_style_currentIndexChanged(int index)
{
        switch (index) {
            case 0: loadStyle(":/qss/qss/blacksoft.css");
            break;
            case 1: loadStyle(":/qss/qss/lightblue.css");
            break;
            case 2: loadStyle(":/qss/qss/flatgray.css");
            break;
        }
}

void Settings::set_log(QAbstractItemModel *listModel)
{
    ui->view_logging->setModel(listModel);
}

void Settings::initTopicList(QMap<QString,QString> topic_list)
{
    ui->topic_listWidget->clear();
    ui->topic_listWidget->addItem(QString("%1   (%2)").arg("Name","Type"));
    for(QMap<QString,QString>::iterator iter=topic_list.begin();iter!=topic_list.end();iter++)
    {
       ui->topic_listWidget->addItem(QString("%1   (%2)").arg(iter.key(),iter.value()));
    }
}

void Settings::on_btn_refreash_topic_clicked()
{
    emit btnRflushTopicList();
}
