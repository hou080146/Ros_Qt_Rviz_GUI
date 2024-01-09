#include "../include/cyrobot_monitor/qrviz.hpp"

QRviz::QRviz(QVBoxLayout *layout,QString node_name)
{
    nullmap.clear();

    this->layout=layout;
    this->nodename=node_name;


    //创建rviz容器
    render_panel_ = new rviz::RenderPanel;
    //向layout添加widget
    layout->addWidget(render_panel_);
    //初始化rviz控制对象
    manager_ = new rviz::VisualizationManager(render_panel_);





//    view_controller_ =  view_manager_->getCurrent();
//        view_manager_->setCurrentFrom(view_manager_->create("rviz/TopDownOrtho"));
//        view_controller_ =  view_manager_->getCurrent();

//            qDebug()<< view_controller_;

//     selection_manager_ = manager_->getSelectionManager();
//     selection_manager_->get3DPoint();

//    view_panel_ = new rviz::ViewsPanel();
//    layout->addWidget(view_panel_);
//    view_panel_->onInitialize();
//    view_panel_->setViewManager(view_manager_);
//    view_panel_->show();

//    qDebug()<< manager_->getFixedFrame();
//    view_controller_->lookAt(manager_->getFixedFrame());
    manager_->getSceneManager();


//    scene_manager_ = manager_->getSceneManager();

    ROS_ASSERT(manager_ != nullptr);
    
    // 获取Display组
    display_group_ = manager_->getRootDisplayGroup();

    
    //获取当前rviz控制对象的 tool控制对象
    tool_manager_=manager_->getToolManager();
    ROS_ASSERT(tool_manager_ != nullptr);
    
   //初始化camera 这行代码实现放大 缩小 平移等操作
    render_panel_->initialize(manager_->getSceneManager(),manager_);


    manager_->initialize();
    tool_manager_->initialize();
//    plugin = new rviz::PluginlibFactory<rviz::Tool>("rviz_plugins", "rviz::Tool");//加载插件，只要new了就行了

    view_manager_ = manager_->getViewManager();
//    view_manager_->initialize();



    
    //删除所有图层
    manager_->removeAllDisplays();

}

void QRviz::setView(int flag)
{
    QSettings view_setting("view_setting", "view_setting");
//    ui->lineEdit_ncdistance0->setText(view_setting.value("ncdistance0","0.001").toString());
//    ui->cmb_targetFrame0->setCurrentIndex(view_setting.value("targetFrame0","0").toInt());
//    ui->lineEdit_distance->setText(view_setting.value("distance","0.8").toString());
//    ui->lineEdit_yaw->setText(view_setting.value("yaw","0.785398").toString());
//    ui->lineEdit_pitch->setText(view_setting.value("pitch","0.785398").toString());

//    ui->lineEdit_ncdistance1->setText(view_setting.value("ncdistance1","0.001").toString());
//    ui->cmb_targetFrame1->setCurrentIndex(view_setting.value("targetFrame1","0").toInt());
//    ui->lineEdit_scale->setText(view_setting.value("scale","70").toString());
//    ui->lineEdit_angle->setText(view_setting.value("angle","0.000").toString());
    if(1 == flag)
    {
        //"rviz/TopDownOrtho"是没有Focal Point及以下的选项
        view_manager_->setCurrentFrom(view_manager_->create("rviz/ThirdPersonFollower"));
        view_manager_->setRenderPanel(render_panel_);

        view_manager_->getCurrent()->subProp("Target Frame")->setValue("base_link");
            view_manager_->getCurrent()->subProp("Near Clip Distance")->setValue(view_setting.value("ncdistance0","0.001").toString());
            view_manager_->getCurrent()->subProp("Focal Point")->setValue("1.90735e-06;-7.62939e-06;0");
            view_manager_->getCurrent()->subProp("Focal Shape Size")->setValue("0.05");
            view_manager_->getCurrent()->subProp("Focal Shape Fixed Size")->setValue("true");
            view_manager_->getCurrent()->subProp("Distance")->setValue(view_setting.value("distance","8").toString());
            view_manager_->getCurrent()->subProp("Yaw")->setValue(view_setting.value("yaw","0.785398").toString());
            view_manager_->getCurrent()->subProp("Pitch")->setValue(view_setting.value("pitch","0.785398").toString());
    }else if(0 == flag)
    {
        //"rviz/TopDownOrtho"是没有Focal Point及以下的选项
        view_manager_->setCurrentFrom(view_manager_->create("rviz/ThirdPersonFollower"));
        view_manager_->setRenderPanel(render_panel_);

        view_manager_->getCurrent()->subProp("Target Frame")->setValue("base_link");
            view_manager_->getCurrent()->subProp("Near Clip Distance")->setValue("0.001");
            view_manager_->getCurrent()->subProp("Focal Point")->setValue("1.90735e-06;-7.62939e-06;0");
            view_manager_->getCurrent()->subProp("Focal Shape Size")->setValue("0.0001");
            view_manager_->getCurrent()->subProp("Focal Shape Fixed Size")->setValue("true");
            view_manager_->getCurrent()->subProp("Distance")->setValue("0.8");
            view_manager_->getCurrent()->subProp("Yaw")->setValue("1.325");
            view_manager_->getCurrent()->subProp("Pitch")->setValue("0.23");
    }
    else if(2 == flag)
    {
        view_manager_->setCurrentFrom(view_manager_->create("rviz/TopDownOrtho"));
        view_manager_->setRenderPanel(render_panel_);
        view_manager_->getCurrent()->subProp("Target Frame")->setValue("base_link");
        view_manager_->getCurrent()->subProp("Near Clip Distance")->setValue(view_setting.value("ncdistance1","0.001").toString());
        view_manager_->getCurrent()->subProp("Scale")->setValue(view_setting.value("scale","70").toString());
        view_manager_->getCurrent()->subProp("Angle")->setValue(view_setting.value("angle","0.000").toString());
    }

}

QRviz::~QRviz()

{

    if (layout != nullptr && render_panel_ != nullptr)
    {
        layout->removeWidget(render_panel_);
    }
  
    if (render_panel_ != nullptr) delete render_panel_;
    if (manager_ != nullptr) delete manager_;
    
    if (current_tool != nullptr) current_tool = nullptr;
    if (tool_manager_ != nullptr) tool_manager_ = nullptr;
//    delete plugin;

    
    ROS_INFO("RVIZ is shutdown");
}


//获取Rviz Display树状图
void QRviz::GetDisplayTreeModel()
{
    rviz::PropertyTreeModel *rvizmodel = manager_->getDisplayTreeModel();

    QAbstractItemModel *model = rvizmodel;
    emit ReturnModelSignal(model);
}

///
/// \brief Rviz Display的初始化与设置
/// \param ClassID
/// \param namevalue
///
void QRviz::DisplayInit(QString ClassID, bool enabled, QMap<QString, QVariant> namevalue)
{
    int num = GetDisplayNum(ClassID);
    if (num == -1)
    {
        //Grid_=manager_->createDisplay("rviz/Grid","myGrid",enable);通过createDisplay函数创建图层，这里的"rviz/Grid" 是代表类名，必须和rviz的名称对应，第二个参数图层名称，可以随便起
        rviz::Display *rvizDisplay = manager_->createDisplay(ClassID, ClassID, true);
        ROS_ASSERT(rvizDisplay != nullptr);
        num = GetDisplayNum(ClassID);
    }
    if (!namevalue.empty())
    {
        QMap<QString, QVariant>::iterator it;
        for (it = namevalue.begin(); it != namevalue.end(); it++)
        {
            display_group_->getDisplayAt(num)->subProp(it.key())->setValue(it.value());
        }
    }
    display_group_->getDisplayAt(num)->setEnabled(enabled);
    
    //更新所有图层
    manager_->startUpdate();
}


void QRviz::DisplayInit(QString ClassID, QString name, bool enabled, QMap<QString, QVariant> namevalue)
{
    int num = GetDisplayNum(ClassID, name);
    if (num == -1)
    {
        rviz::Display *rvizDisplay = manager_->createDisplay(ClassID, name, true);
        ROS_ASSERT(rvizDisplay != nullptr);
        num = GetDisplayNum(ClassID, name);
    }
    if (!namevalue.empty())
    {
        QMap<QString, QVariant>::iterator it;
        for (it = namevalue.begin(); it != namevalue.end(); it++)
        {
            display_group_->getDisplayAt(num)->subProp(it.key())->setValue(it.value());
        }
    }
    display_group_->getDisplayAt(num)->setEnabled(enabled);
    
    //更新所有图层
    manager_->startUpdate();
}

///
/// \brief 删除Display
/// \param name
///
//应该是根据名称删除图层
void QRviz::RemoveDisplay(QString name)
{
    int num = GetDisplayNumName(name);
    if (num == -1)
    {
        return ;
    }
    delete display_group_->getDisplayAt(num);
//    rvizDisplays_.removeAt(num);
}
void QRviz::RemoveDisplay(QString ClassID, QString name)
{
    int num = GetDisplayNum(ClassID, name);
    if (num == -1)
    {
        return ;
    }
    delete display_group_->getDisplayAt(num);
//    rvizDisplays_.removeAt(num);
}

///
/// \brief 重命名Display
/// \param oldname
/// \param newname
///
/// name可以随便起，id不可以
void QRviz::RenameDisplay(QString oldname, QString newname)
{
    int num = GetDisplayNumName(oldname);
    if (num == -1)
    {
        return ;
    }
    display_group_->getDisplayAt(num)->setName(newname);
}

///
/// \brief 导出 RVIZ Display 配置
/// \param path
///
void QRviz::OutDisplaySet(QString path)
{
    if (!path.isEmpty())
    {
        if (manager_ == nullptr)
        {
            return;
        }
        rviz::Config con;
        manager_->save(con);
        rviz::YamlConfigWriter yamlconfigwriter;
        yamlconfigwriter.writeFile(con, path);
    }
}

///
/// \brief 导入 RVIZ Display 配置
/// \param path
///
void QRviz::ReadDisplaySet(QString path)
{
    if (!path.isEmpty())
    {
        if (manager_ == nullptr)
        {
            return;
        }
        rviz::YamlConfigReader yamlconfigreader;
        rviz::Config con;
        yamlconfigreader.readFile(con, path);
        manager_->load(con);
    }
}

///
/// \brief 根据Display的ClassID(和name)获得Display的序号
/// \param ClassID
/// \return 
///
int QRviz::GetDisplayNum(QString ClassID)
{
    int num = -1;
    for (int i = 0; i < display_group_->numDisplays(); i++)
    {
        if (display_group_->getDisplayAt(i) != nullptr)
        {
            if (ClassID == display_group_->getDisplayAt(i)->getClassId())
            {
                num = i;
                break;
            }
        }
    }
    return num;
}
int QRviz::GetDisplayNum(QString ClassID, QString name)
{
    int num = -1;
    for (int i = 0; i < display_group_->numDisplays(); i++)
    {
        if (display_group_->getDisplayAt(i) != nullptr)
        {
            if (ClassID == display_group_->getDisplayAt(i)->getClassId() && name == display_group_->getDisplayAt(i)->getName())
            {
                num = i;
                break;
            }
        }
    }
    return num;
}
int QRviz::GetDisplayNumName(QString name)
{
    int num = -1;
    for (int i = 0; i < display_group_->numDisplays(); i++)
    {
        if (display_group_->getDisplayAt(i) != nullptr)
        {
            if (name == display_group_->getDisplayAt(i)->getName())
            {
                num = i;
                break;
            }
        }
    }
    return num;
}

//设置全局显示
 void QRviz::SetGlobalOptions(QString frame_name,QColor backColor,int frame_rate)
 {
     manager_->setFixedFrame(frame_name);
     manager_->setProperty("Background Color",backColor);
     manager_->setProperty("Frame Rate",frame_rate);
     manager_->startUpdate();//更新所有图层
 }

// "rviz/MoveCamera";
// "rviz/Interact";
// "rviz/Select";
// "rviz/SetInitialPose";
// "rviz/SetGoal";
 //设置机器人导航初始位置
 void QRviz::Set_Pos()
 {
     //获取设置Pos的工具
     //添加工具

     current_tool= tool_manager_->addTool("rviz/SetInitialPose");
     //设置当前使用的工具为SetInitialPose（实现在地图上标点）
     tool_manager_->setCurrentTool( current_tool );
     manager_->startUpdate();//更新所有图层

//     tool_manager_->setCurrentTool()

 }
 //设置机器人导航目标点
 void QRviz::Set_Goal()
 {
//      qDebug()<<view_manager_->getCurrent();
//      view_manager_->setCurrentFrom(view_manager_->create("rviz/TopDownOrtho"));
//      qDebug()<<view_manager_->getCurrent();
     //添加工具
     current_tool= tool_manager_->addTool("rviz/SetGoal");
     //设置goal的话题
     rviz::Property* pro= current_tool->getPropertyContainer();
     pro->subProp("Topic")->setValue("/move_base_simple/goal");
     //设置当前frame
//     manager_->setFixedFrame("map");
     //设置当前使用的工具为SetGoal（实现在地图上标点）
     tool_manager_->setCurrentTool( current_tool );

     manager_->startUpdate();//更新所有图层

 }
 void QRviz::Set_MoveCamera()
 {
     //获取设置Pos的工具
     //添加工具

     current_tool= tool_manager_->addTool("rviz/MoveCamera");
     //设置当前使用的工具为SetInitialPose（实现在地图上标点）
     tool_manager_->setCurrentTool( current_tool );
     manager_->startUpdate();
 }
 void QRviz::Set_Select()
 {
     //获取设置Pos的工具
     //添加工具

     current_tool= tool_manager_->addTool("rviz/Select");
     //设置当前使用的工具为SetInitialPose（实现在地图上标点）
     tool_manager_->setCurrentTool( current_tool );
     manager_->startUpdate();//更新所有图层
 }

 void QRviz::Set_3DGoal()
 {
//     current_tool= tool_manager_->addTool("rviz_plugins/Goal3DTool");
     //设置当前使用的工具为SetInitialPose（实现在地图上标点）
//     tool_manager_->setCurrentTool( current_tool );
//     manager_->startUpdate();//更新所有图层
 }

 void QRviz::addTool( rviz::Tool* )
 {

 }
void QRviz::createDisplay(QString display_name,QString topic_name)
{
    
}
void QRviz::run()
{

}
