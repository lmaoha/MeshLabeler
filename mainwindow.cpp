#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "label.cpp"
#include <QFileDialog>
#include <QDebug>
#include <QTextCodec>
#include <QKeyEvent>

#pragma execution_character_set("utf-8")

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_appPath = QCoreApplication::applicationDirPath();

    m_config = new QSettings(m_appPath + "/config.ini",QSettings::IniFormat,this);
    m_config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    loadConfig();

    ui->qvtkWidget->SetRenderWindow(m_vtkRenderWin);

    showVTK();
    ui->fileName_label->setText(m_inputFileName);
    m_vtkEvtConn = vtkSmartPointer<vtkEventQtSlotConnect>::New();
    m_vtkEvtConn->Connect(m_vtkRenderWin->GetInteractor(),vtkCommand::KeyPressEvent,this,SLOT(valueChange()));

    connect(ui->tableWidget,&MyTableWidget::clickFilePath,this,&MainWindow::openStlFile);

}

MainWindow::~MainWindow()
{
    delete m_config;
    delete ui;
}

void MainWindow::loadConfig()
{
    m_config->beginGroup("path");
    m_inputFileName  = m_config->value("INPUT_FILE_NAME").toString();
    m_outputFileName = m_config->value("OUTPUT_FILE_NAME").toString();
    m_lastOpenPath   = m_config->value("LAST_OPEN_PATH").toString();
    m_config->endGroup();
}

void MainWindow::saveConfig()
{
    m_config->clear();
    m_config->beginGroup("path");
    m_config->setValue("INPUT_FILE_NAME", m_inputFileName);
    m_config->setValue("OUTPUT_FILE_NAME", m_outputFileName);
    m_config->setValue("LAST_OPEN_PATH", m_lastOpenPath);

    m_config->endGroup();

}

void MainWindow::openStlFile(QString stlFilePath)
{
    m_inputFileName = stlFilePath;
    m_lastOpenPath = stlFilePath;
    ui->fileName_label->setText(m_inputFileName);
    showVTK();
    saveConfig();
}


void MainWindow::on_inputFile_btn_clicked()
{
    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                tr("文件对话框！"),
                                                m_lastOpenPath,
                                                "STL Files(*.stl)");
    if (fileList.isEmpty())
    {
        return;
    }
    m_inputFileName = fileList.at(0);
    m_lastOpenPath = fileList.at(0);
    ui->fileName_label->setText(m_inputFileName);
//    showVTK();
    saveConfig();

    ui->tableWidget->addStlFileToTable(fileList);
}

void MainWindow::on_outPut_btn_clicked()
{
    QString fileName;
    fileName = QFileDialog::getSaveFileName(this,
                                                tr("文件对话框！"),
                                                m_lastOpenPath,
                                                "vtp Files(*.vtp)");
    if (fileName.isEmpty())
    {
        return;
    }
    m_outputFileName = fileName;
    m_lastOpenPath = fileName;
    saveVTP();
    saveConfig();

    ui->tableWidget->item(ui->tableWidget->currentTableIndex,1)->setBackground(QBrush(Qt::green));
    ui->tableWidget->item(ui->tableWidget->currentTableIndex,1)->setText("已保存");
}

void MainWindow::valueChange()
{
//    qDebug()<<"ValueChange" << PressFlag;
    ui->spinBox->setValue(PressFlag);
}


void MainWindow::on_spinBox_valueChanged(int arg1)
{
    if(PressFlag == arg1)
    {
        return;
    }

    PressFlag = arg1;

}
