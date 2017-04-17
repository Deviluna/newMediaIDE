#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDateTime>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QFileSystemModel>
#include <QTextCodec>
#include <QTreeView>
#include <QProcess>
#include <addfiledialog.h>
#include <createprojectdialog.h>
#include <QFileDialog>
#include <QMenu>
#include <QAction>
#include <QDebug>
#include <adddirdialog.h>
#include <QFontDialog>
#include <QColorDialog>
#include <QImage>
#include <testwidget.h>
#include <closedialog.h>
#include <QIcon>
#include <welcomedialog.h>
#include <rmdirdialog.h>
#include <generatedialog.h>
#include <QStandardPaths>
#include <argall.h>
#include <qstringlist.h>
#include <renamedialog.h>
#include <QCloseEvent>
#include <dirprodialog.h>
#include <qdesktopservices.h>
#include <QUrl>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QTime>

MainWindow::
MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    this->setWindowState(Qt::WindowMaximized);
    ui->setupUi(this);
    ui->splitter->setStretchFactor(1,3);
    setupMenu();
    initTabWidget();
    initListWidget();
}
MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::initListWidget(){
    ui->listWidget->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listWidget, SIGNAL(customContextMenuRequested(const QPoint&)), this, SLOT(ShowContextMenu(const QPoint&)));

}

void MainWindow::ShowContextMenu(const QPoint&){
    qDebug()<<ui->listWidget->currentIndex().data(0).toString();
    nowIndex=ui->listWidget->currentIndex();
    m_folderMenu=new QMenu(this);

    m_folderMenu->addAction("上移",this,SLOT(upClassify()));
    m_folderMenu->addAction("下移",this,SLOT(downClassify()));
    m_folderMenu->addSeparator();
    m_folderMenu->addAction("在本地打开",this,SLOT(openInExplorer()));
    m_folderMenu->addSeparator();
    m_folderMenu->addAction("重命名",this,SLOT(renameDir()));
    m_folderMenu->addAction("删除",this,SLOT(deleteDir()));
    m_folderMenu->exec(QCursor::pos());


}

void MainWindow::loadJson(){
    mstJsonObject=ArgAll::getJsonObject(mstPath);

}

void MainWindow::setupMenu(){
    //菜单连接槽函数
    connect(ui->actionS,SIGNAL(triggered()),this,SLOT(close()));
    connect(ui->actionAbout,SIGNAL(triggered()),this,SLOT(about()));
    connect(ui->actionCreate,SIGNAL(triggered()),this,SLOT(createProject()));
    connect(ui->actionOpen,SIGNAL(triggered()),this,SLOT(openProject()));
    connect(ui->actionX,SIGNAL(triggered()),this,SLOT(generate()));
}


void MainWindow::generate(){
    //打开构建界面
    GenerateDialog *gDialog=new GenerateDialog(this);
    gDialog->setProjectPath(rootPath);
    gDialog->setDirList();
    gDialog->exec();
}


void MainWindow::initProgramme(){
    //每次启动时候使用（被main.cpp调用）
    QFile file(ArgAll::getSettingPath());
    QStringList settings=ArgAll::parsePSTJson(ArgAll::getSettingPath());
    if(settings[0]!=""){
        loadProject(settings[0]);
    }
    else {
        firstUse();
    }
}

void MainWindow::closeAllTab(){
    int max=ui->tabWidget->count();
    for(int i=0;i<max;i++){
        ui->tabWidget->removeTab(0);
    }
}

void MainWindow::CloseNowTab(){
    ui->tabWidget->removeTab(ui->tabWidget->currentIndex());
}

void MainWindow::initTabWidget(){
    connect(ui->tabWidget,SIGNAL(tabCloseRequested(int)),this,SLOT(closeTab(int)));
    closeAllTab();
}

bool MainWindow::renameNowTab(QString newName){
    int nowIndex=ui->tabWidget->currentIndex();
    ui->tabWidget->setTabText(nowIndex,newName);
}

void MainWindow::closeTab(int a){
    TestWidget *nowWidget=(TestWidget*)ui->tabWidget->widget(a);
    if(nowWidget->changed()){
        CloseDialog *closeDialog=new CloseDialog(this);
        closeDialog->setWindowTitle(tr("提示窗口"));
        if(closeDialog->exec()){
            //之后窗口的逻辑归窗口管了
        }
    }
    else
    {
        ui->tabWidget->removeTab(a);
    }
}

QWidget* MainWindow::Page(){
    return ui->tabWidget->currentWidget();
}


void MainWindow::setDirPro(QString path){
    //目前废弃
    DirProDialog *dpd=new DirProDialog(this);
    dpd->setPath(path);
    dpd->exec();
}

void MainWindow::openProject(){
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setDirectory(QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation));
    fileDialog->setWindowTitle(tr("打开项目"));
    fileDialog->setFileMode(QFileDialog::Directory);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        loadProject(path);
    }

}



void MainWindow::saveClassify(){

    QJsonArray classifyOrder;
    for(int i=0;i<ui->listWidget->count();i++){
        classifyOrder.push_back(ui->listWidget->item(i)->data(0).toString());
    }

    mstJsonObject.remove("classify");
    mstJsonObject.insert("classify",classifyOrder);


    ArgAll::outputJson(mstPath,mstJsonObject);
}


void MainWindow::closeEvent(QCloseEvent *event)
{
    //关闭时候，存储已经打开的标签页。下次打开软件时候，这些标签页会自动打开
    QJsonArray openedTabs;
    for(int i=0;i<ui->tabWidget->count();i++){
        TestWidget *tw=(TestWidget*)ui->tabWidget->widget(i);
        openedTabs.push_back(tw->getPath());
    }


    mstJsonObject.remove("openedTabs");
    mstJsonObject.insert("openedTabs",openedTabs);
    mstJsonObject.remove("lastIndex");
    mstJsonObject.insert("lastIndex",ui->tabWidget->currentIndex());

    saveClassify();

    // ArgAll::modifyJson(mstPath,"openedTab",openedTabs);


    // 这个时候再存mst啦。





}



void MainWindow::OpenProject(){
    QFileDialog* fileDialog = new QFileDialog(this);
    fileDialog->setWindowTitle(tr("打开项目"));
    fileDialog->setFileMode(QFileDialog::Directory);
    if(fileDialog->exec() == QDialog::Accepted) {
        QString path = fileDialog->selectedFiles()[0];
        loadProject(path);
    }

}

void MainWindow::firstUse(){
    WelcomeDialog *wDialog=new WelcomeDialog(this);
    wDialog->exec();
}

void MainWindow::createProject(){
    CreateProject();
}

void MainWindow::CreateProject(){
    CreateProjectDialog *createDialog=new CreateProjectDialog(this);
    if(createDialog->exec()){
        QString projectPath=createDialog->getProjectPath();
        QString projectName=createDialog->getProjectName();
        QDir *project=new QDir;
        bool exist = project->exists(projectPath+"/"+projectName);
        if(exist)
            QMessageBox::warning(this,tr("创建项目"),tr("分类已经存在！"));
        else
        {
            bool ok = project->mkdir(projectPath+"/"+projectName);
            if( ok ){
                loadProject(projectPath+"/"+projectName);

                //新建需要创建用户适配的json，之后projectName存在json里面，可以修改，每次load时候又显示出来。
                //这里以后要修改projectpath的地方
                mstPath=projectPath+"/"+projectName+"/"+ArgAll::settingName();
                ArgAll::createFile(mstPath);
                ArgAll::modifyJson(mstPath,"name",projectName);
            }
        }
    }
}

void MainWindow::loadProject(QString path){
    ui->listWidget->clear();
    ui->listWidget_2->clear();
    classifyList.clear();
    rootPath=path;
    nowPath=path;
    mstPath=path+"/"+ArgAll::settingName();
    closeAllTab();
    //为了保证鲁棒性，这里必须做修改。
    mstList=ArgAll::parseMSTJson(mstPath);
    //加一段，写入path到json中。
    ArgAll::modifyPSTJson("lastProject",path);
    loadJson();
    QJsonArray openedTabs=mstJsonObject.value("openedTabs").toArray();
    for (int i=0;i<openedTabs.size(); i++){
        QString tabPath=openedTabs.at(i).toString();
        if(tabPath.length()>0)
            loadFile(tabPath);
    }

    QJsonArray classifyNames=mstJsonObject.value("classify").toArray();
    for(int i=0;i<classifyNames.size();i++){
        classifyList<<classifyNames.at(i).toString();
        qDebug()<<classifyNames.at(i).toString();
    }
    qDebug()<<classifyList;
    ui->listWidget->addItems(classifyList);

    ui->tabWidget->setCurrentIndex(mstJsonObject.value("lastIndex").toInt());
    //loadClassify(path);
}

void MainWindow::loadClassify(QString path){
    QStringList dirNames=ArgAll::getDirName(path);
    ui->listWidget->addItems(dirNames);
}




void MainWindow::selectFile(const QModelIndex &index){

    if(!model->isDir(index)){
        loadFile(model->filePath(index));
    }
    else{
    }
}


void MainWindow::about(){
    QMessageBox::about(this,tr("About Media IDE"),tr("<h2>Media IDE v0.05</h2><p>only for evaluating</P><P>Copyright &copy; 2017 Deviluna Inc.</p>"));
}

void MainWindow::loadFile(QString path){


    for(int i=0;i<ui->tabWidget->count();i++){
        TestWidget *tw=(TestWidget*)ui->tabWidget->widget(i);
        if(tw->getPath().compare(path)==0){
            ui->tabWidget->setCurrentIndex(i);
            return ;
        }
    }
    QFileInfo fi=QFileInfo(path);


    //这个m要从一个统一的位置获取
    if(fi.suffix()=="m"){

        TestWidget *page=new TestWidget;
        page->loadFile(path);
        page->setRootpath(rootPath);
        ui->tabWidget->insertTab(ui->tabWidget->count()+1,page,fi.baseName());
        ui->tabWidget->setCurrentIndex(ui->tabWidget->count()-1);
    }

    if(fi.fileName()==ArgAll::settingName()){
        //点击打开项目设置窗口
        //
        qDebug()<<"点击打开项目设置窗口";


    }
}

void MainWindow::addArticle(){
    AddFileDialog *addDialog=new AddFileDialog(this);
    if(addDialog->exec()){
        loadFile(rootPath+"/"+nowIndex.data(0).toString()+"/"+addDialog->getinput()+".m");
        refreshArticleList();

    }

}

void MainWindow::refreshArticleList(){

    QJsonObject json=ArgAll::getJsonObject(nowArticleOrderPath);
    QStringList articleOrder;
    QJsonArray jarray=json.value("article").toArray();
    for(int i=0;i<jarray.size();i++)
        articleOrder<<jarray.at(i).toString();
    ui->listWidget_2->clear();
    ui->listWidget_2->addItems(articleOrder);
}

void MainWindow::openInExplorer(){
    qDebug()<<rootPath+"/"+nowIndex.data(0).toString();
    QDesktopServices::openUrl(QUrl::fromLocalFile(rootPath+"/"+nowIndex.data(0).toString()));
}

void MainWindow::renameDir(){
    RenameDialog *rd=new RenameDialog(this);
    rd->setNowName(model->fileName(nowIndex));
    rd->exec();
}


void MainWindow::renameNowDir(QString newName){
    //重命名
    QString path=rootPath+"/"+nowIndex.data(0).toString();
    QFileInfo qfi(path);
    for(int i=0;i<ui->tabWidget->count();i++){
        TestWidget *tw=(TestWidget*)ui->tabWidget->widget(i);
        QFileInfo lsqfi(tw->getPath());
        if(lsqfi.canonicalPath()==qfi.filePath()){
            ui->tabWidget->removeTab(i);
        }
    }
    if(QFile::rename(qfi.filePath(),qfi.canonicalPath()+"/"+newName)){
        ui->listWidget->takeItem(nowIndex.row());
        ui->listWidget->insertItem(nowIndex.row(),newName);

    }


}



void MainWindow::deleteDir(){
    RmdirDialog *rmdir=new RmdirDialog(this);
    if(rmdir->exec()){
        QDir *nowDir=new QDir(rootPath+"/"+nowIndex.data(0).toString());
        if(!nowDir->removeRecursively())
            qDebug()<<"并没有成功删除";
        else{
            ui->listWidget->takeItem(nowIndex.row());

        }
    }


}

void MainWindow::deleteFile(){
    QString path=model->filePath(nowIndex);
    for(int i=0;i<ui->tabWidget->count();i++){
        TestWidget *tw=(TestWidget*)ui->tabWidget->widget(i);
        if(tw->getPath().compare(path)==0){
            ui->tabWidget->removeTab(i);
        }
    }
    model->remove(nowIndex);
}

void MainWindow::addDir(){

    addDirDialog *addDialog=new addDirDialog(this);
    if(addDialog->exec()){
        QString dirName=addDialog->getinput();
        model->mkdir(nowIndex,dirName);
    }
}


void MainWindow::on_pushButton_8_clicked()
{
    addDirDialog *addDialog=new addDirDialog(this);
    if(addDialog->exec()){
        QString dirName=addDialog->getinput();
        QDir *project=new QDir;

        QString dirPath=rootPath+"/"+dirName;
        bool exist = project->exists(dirPath);
        if(exist)
            QMessageBox::warning(this,tr("添加分类"),tr("分类已经存在！"));
        else
        {
            bool ok = project->mkdir(dirPath);
            if( ok ){

            }
            else{
                QMessageBox::warning(this,tr("创建分类"),tr("失败！"));
            }
        }

    }
}


void MainWindow::on_pushButton_2_clicked()
{
    closeAllTab();
}

void MainWindow::addClassify(QString name){
    QDir *nowDir=new QDir(rootPath);
    nowDir->mkdir(name);
    ui->listWidget->addItem(name);



}

void MainWindow::on_pushButton_clicked()
{
    //添加分类

    addDirDialog *aDialog=new addDirDialog(this);
    if(aDialog->exec()){
        addClassify(aDialog->getinput());

    }
}

void MainWindow::on_listWidget_customContextMenuRequested(const QPoint &pos)
{

}

void MainWindow::on_listWidget_clicked(const QModelIndex &index)
{
    nowIndex=index;
    QString articleOrderPath=rootPath+"/"+index.data(0).toString()+"/"+ArgAll::articleOrderName();
    QFileInfo qfi(articleOrderPath);
    if(!qfi.exists()){
        ArgAll::outputFile(articleOrderPath,"");
    }
    else{
        nowArticleOrderPath=articleOrderPath;
        refreshArticleList();
    }


}

void MainWindow::on_listWidget_entered(const QModelIndex &index)
{
    qDebug()<<"enter?";
}

void MainWindow::upClassify(){


    int row=ui->listWidget->currentRow();
    if(row>0){
        QString up=ui->listWidget->item(row-1)->data(0).toString();
        QString down=ui->listWidget->item(row)->data(0).toString();
        ui->listWidget->item(row-1)->setData(0,down);
        ui->listWidget->item(row)->setData(0,up);
        ui->listWidget->setCurrentRow(row-1);

    }

}
void MainWindow::downClassify(){
    int row=ui->listWidget->currentRow();
    if(row+1<ui->listWidget->count()){
        QString down=ui->listWidget->item(row+1)->data(0).toString();
        QString up=ui->listWidget->item(row)->data(0).toString();
        ui->listWidget->item(row+1)->setData(0,up);
        ui->listWidget->item(row)->setData(0,down);
        ui->listWidget->setCurrentRow(row+1);

    }

}
void MainWindow::upArticle(){
    int row=ui->listWidget_2->currentRow();
    if(row>0){
        QString up=ui->listWidget_2->item(row-1)->data(0).toString();
        QString down=ui->listWidget_2->item(row)->data(0).toString();
        ui->listWidget_2->item(row-1)->setData(0,down);
        ui->listWidget_2->item(row)->setData(0,up);
        ui->listWidget_2->setCurrentRow(row-1);

    }


}
void MainWindow::downArticle(){
    int row=ui->listWidget_2->currentRow();
    if(row+1<ui->listWidget_2->count()){
        QString down=ui->listWidget_2->item(row+1)->data(0).toString();
        QString up=ui->listWidget_2->item(row)->data(0).toString();
        ui->listWidget_2->item(row+1)->setData(0,up);
        ui->listWidget_2->item(row)->setData(0,down);
        ui->listWidget_2->setCurrentRow(row+1);
    }
}
void MainWindow::on_toolButton_clicked()
{
    upClassify();
}

void MainWindow::on_toolButton_2_clicked()
{
    downClassify();
}

void MainWindow::on_pushButton_3_clicked()
{
    addArticle();
}

void MainWindow::on_listWidget_2_clicked(const QModelIndex &index)
{
    loadFile(rootPath+"/"+nowIndex.data(0).toString()+"/"+index.data(0).toString());
}

void MainWindow::on_toolButton_3_clicked()
{
    upArticle();
}

void MainWindow::on_toolButton_4_clicked()
{
    downArticle();
}
