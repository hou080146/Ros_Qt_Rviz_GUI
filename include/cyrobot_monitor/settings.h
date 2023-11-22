#ifndef SETTINGS_H
#define SETTINGS_H

#include <QWidget>
#include <QSettings>
#include <QMessageBox>
#include <QInputDialog>
#include <QFileDialog>
#include <map>
#include <QStandardItemModel>
#include <QStandardPaths>
#include <QFile>
#include <QStringListModel>

#include "addtopics.h"
#include "qrviz.hpp"
namespace Ui {
class Settings;
}

class Settings : public QWidget
{
    Q_OBJECT

public:
    explicit Settings(QWidget *parent = 0);
    ~Settings();
private slots:
    //保存
    void slot_ok_btn_click();
    //取消
    void slot_cancel_btn_click();
    //连接
    void on_btn_connect_clicked();
    //断开
    void on_btn_disconnect_clicked();

public slots:
    void rviz_tree_sendsignal(QTreeWidgetItem *choose, QString name);

    void RvizGetModel(QAbstractItemModel *model);

    //接受mainwindow传过来的rvic45
    void receiveRviz(QRviz *rviz);


private slots:
    void on_treeView_rvizDisplayTree_clicked(const QModelIndex &index);
    void on_btn_rvizSaveDisplaySet_clicked();
    void on_btn_rvizReadDisplaySet_clicked();
    void on_btn_rename_topic_clicked();
    void on_btn_remove_topic_clicked();
    void on_btn_add_topic_clicked();

private:
    void initForm();
    void inform(QString strdata);
    void loadStyle(const QString &qssFile);
    QString getUserName();
    QString JudgeDisplayNewName(QString name);
    void connection();
    QString appPath();
    QString appName();


public:

    //自动保存读取配置文件
    void rviz_save();
    void rviz_read();

    void set_log(QAbstractItemModel *listModel);
    void initTopicList(QMap<QString,QString> topicList);



private slots:
    void on_btn_refreash_topic_clicked();

private slots:
    void on_cbx_style_currentIndexChanged(int index);

private:
    Ui::Settings *ui;


    QRviz *map_rviz_ = nullptr;
    //rviz节点树
    AddTopics *addtopic_form = nullptr;

    QAbstractItemModel* m_modelRvizDisplay;
    QString m_sRvizDisplayChooseName_;


signals:
    //mainwindow接受信号
    void btnconnect();
    void btndisconnect();

    void rviz_tree_signal(QTreeWidgetItem *choose, QString name);

    void btnRflushTopicList();
};

#endif // SETTINGS_H
