#ifndef GENERATEDIALOG_H
#define GENERATEDIALOG_H

#include <QDialog>
#include <testwidget.h>
#include <qjsonarray.h>

namespace Ui {
class GenerateDialog;
}

class GenerateDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateDialog(QWidget *parent = 0);
    ~GenerateDialog();
    void setProjectPath(QString path);
    void setDirList();

private slots:
    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_toolButton_clicked();

    void on_toolButton_2_clicked();

    void on_listWidget_doubleClicked(const QModelIndex &index);

    void on_pushButton_4_clicked();

private:
    Ui::GenerateDialog *ui;
    QString projectPath;
    bool genHtml(QString path,QString prefix,QString index,QString dirPath);//要不统一标识从path里面拿？父文件夹名字？？？不行。。。这里要断绝输出文件和编辑文件的名字，防止误操作。
    TestWidget *tw;
    void genDir(QString path,QString dirPath);
    void genIndex();
    QStringList dirList;
    QString indexlist;
    QString *nowIndexList;
    QString classifylist1,classifylist2,classifylist3,classifylist4;
    QJsonArray json_array;
    void gitUpdate();
    int jsonCount;
    bool outputJson();
    void generate();
    QStringList randomList;
    bool randomExist(QString random);
    QJsonObject appJson;
    QJsonArray appArray;
};

#endif // GENERATEDIALOG_H
