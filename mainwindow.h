#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <qevent.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkSmartPointer.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    void loadConfig();
    void saveConfig();

    void openStlFile(QString stlFilePath);

private slots:
    void on_inputFile_btn_clicked();

    void on_outPut_btn_clicked();

    void valueChange();
    void on_spinBox_valueChanged(int arg1);

private:
    Ui::MainWindow *ui;
    QString m_appPath;      //程序路径所在位置
    QString m_lastOpenPath; //最后一次打开文件的路径
    QSettings  *m_config;
    vtkSmartPointer<vtkEventQtSlotConnect> m_vtkEvtConn;

};
#endif // MAINWINDOW_H
