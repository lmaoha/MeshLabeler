#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <qevent.h>
#include <vtkEventQtSlotConnect.h>
#include <vtkSmartPointer.h>
#include "VTK/vtkshow.h"

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

signals:
    void KeyPressNumber(int number);

private slots:
    void on_inputFile_btn_clicked();

    void on_outPut_btn_clicked();

private:
    Ui::MainWindow *ui;
    QString m_appPath;      //程序路径所在位置
    QString m_lastOpenPath; //最后一次打开文件的路径
    QSettings  *m_config;

    VtkShow m_vtk;
    QString m_inputFileName;
    QString m_outputFileName;

};
#endif // MAINWINDOW_H
