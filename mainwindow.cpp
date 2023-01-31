#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QDebug>
#include <QTextCodec>
#include <QKeyEvent>

#pragma execution_character_set("utf-8")

const QString VER = "0.03";

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle("标注软件 " + VER);
    m_appPath = QCoreApplication::applicationDirPath();

    m_config = new QSettings(m_appPath + "/config.ini",QSettings::IniFormat,this);
    m_config->setIniCodec(QTextCodec::codecForName("UTF-8"));

    m_vtk.setWidget(ui->qvtkWidget);

    m_vtk.m_vtkStyle->connect(m_vtk.m_vtkStyle,&DesignInteractorStyle::sig_keyPressNumber,this
                              ,[&](int keyNumber)
    {
        ui->colorTabel->selectColumn(keyNumber-1);
    });

    m_vtk.m_vtkStyle->connect(this,&MainWindow::KeyPressNumber,m_vtk.m_vtkStyle,&DesignInteractorStyle::slot_changeKeyPressNumber);

    connect(ui->tableWidget,&MyTableWidget::clickFilePath,this,&MainWindow::openFile);

    //表格选择了变色框 并发送信号
    connect(ui->colorTabel,&QTableWidget::cellClicked,this,[&](int row,int column)
    {
        Q_UNUSED(row);
        emit KeyPressNumber(column + 1);
    });

    ui->colorTabel->selectColumn(0);
    loadConfig();
    //改变简化三角形数值后立即保存至VTK内，下次打开文件后作用
    connect(ui->reductionCount_lineEdit,&QLineEdit::editingFinished, this, [&]()
    {
        m_reductionCount = ui->reductionCount_lineEdit->text().toInt();
        m_vtk.setReductionCount(m_reductionCount);
        saveConfig();
    });

    ui->reductionCount_lineEdit->setValidator(new QRegExpValidator(QRegExp("^[0-9]*[1-9][0-9]*$"), this));
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

    bool ok;
    m_reductionCount = m_config->value("REDUCTION_COUNT").toInt(&ok);
    if (!ok)
    {
        m_reductionCount = 21000;
        qDebug()<<"读取 REDUCTION_COUNT 数据失败，修改为默认参数21000";
    }
    ui->reductionCount_lineEdit->setText(QString::number(m_reductionCount));
    m_vtk.setReductionCount(m_reductionCount);

    m_config->endGroup();
}

void MainWindow::saveConfig()
{
    m_config->clear();
    m_config->beginGroup("path");
    m_config->setValue("INPUT_FILE_NAME", m_inputFileName);
    m_config->setValue("OUTPUT_FILE_NAME", m_outputFileName);
    m_config->setValue("LAST_OPEN_PATH", m_lastOpenPath);
    m_config->setValue("REDUCTION_COUNT", m_reductionCount);

    m_config->endGroup();

}

void MainWindow::openFile(QString filePath)
{
    m_inputFileName = filePath;
    m_lastOpenPath = filePath;
    ui->fileName_label->setText(m_inputFileName);
    m_vtk.showVtk(m_inputFileName);
    saveConfig();
}

void MainWindow::on_inputFile_btn_clicked()
{
    loadConfig();

    QStringList fileList = QFileDialog::getOpenFileNames(this,
                                                tr("文件对话框！"),
                                                m_lastOpenPath,
                                                "File(*.vtp *.stl *.ply *.obj);;"
                                                "VTP File(*.vtp);;"
                                                "STL File(*.stl);;"
                                                "PLY File(*.ply);;"
                                                "OBJ File(*.obj)");
    if (fileList.isEmpty())
    {
        m_inputFileName.clear();
        return;
    }

    ui->tableWidget->addStlFileToTable(fileList);

    m_inputFileName =  ui->tableWidget->item(0,0)->text();
    m_lastOpenPath = m_inputFileName;
//    ui->tableWidget->currentTableIndex = 0;
    ui->fileName_label->setText(m_inputFileName);
    ui->tableWidget->cellDoubleClicked(0,0);

    saveConfig();

}

void MainWindow::on_outPut_btn_clicked()
{
    if (m_inputFileName.isEmpty())
    {
        return;
    }
    QString fileName;
    if (0 == m_lastOpenPath.right(4).compare(".ply",Qt::CaseInsensitive)
     || 0 == m_lastOpenPath.right(4).compare(".vtp",Qt::CaseInsensitive)
     || 0 == m_lastOpenPath.right(4).compare(".stl",Qt::CaseInsensitive)
     || 0 == m_lastOpenPath.right(4).compare(".obj",Qt::CaseInsensitive))
    {
        m_lastOpenPath = m_lastOpenPath.left(m_lastOpenPath.count() - 4);
    }

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
    m_vtk.saveVtP(m_outputFileName);
    saveConfig();

    ui->tableWidget->item(ui->tableWidget->currentTableIndex,1)->setBackground(QBrush(Qt::green));
    ui->tableWidget->item(ui->tableWidget->currentTableIndex,1)->setText("已保存");
}


