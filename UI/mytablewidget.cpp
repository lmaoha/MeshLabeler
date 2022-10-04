#include "mytablewidget.h"

#include <QDragEnterEvent>
#include <QUrl>
#include <QMimeData>
#include <QDropEvent>
#include <QHeaderView>
#include <QDebug>


#pragma execution_character_set("utf-8")

MyTableWidget::MyTableWidget(QWidget *parent)
    : QTableWidget{parent}
{
    this->setAcceptDrops(true); //开启拖拽事件开关
    this->setEditTriggers(QAbstractItemView::NoEditTriggers); //不允许编辑
    this->setSelectionBehavior(QAbstractItemView::SelectRows); //选整行
    this->horizontalHeader()->setStretchLastSection(true);
    this->setHorizontalHeaderLabels({"文件名称","保存状态"});
//    this->setAlternatingRowColors(true);  //隔行变色

    connect(this,&MyTableWidget::cellDoubleClicked,this,
            [&](int row,int column)
    {
        Q_UNUSED(column);
        currentTableIndex = row;
        //倒序索引
        int index = m_fileList.count() - 1 - row;
        emit clickFilePath(m_fileList.at(index));
    });
}

/**************************************************************************************************
 *函数名： addStlFileToTable
 *时间：   2022-09-12 12:24:08
 *用户：
 *参数：   const QStringList &fileList  必须是stl文件
 *返回值： 无
 *描述：   添加stl文件到 表格中  过滤掉重复文件
*************************************************************************************************/
void MyTableWidget::addStlFileToTable(const QStringList &fileList)
{
    for(const auto &filePath : fileList)
    {
        if(m_fileList.contains(filePath,Qt::CaseInsensitive))
        {
            QMessageBox::warning(this,"警告",filePath+"重复",QMessageBox::Ok);
            continue;
        }
        this->insertRow(0);
        QString fileName = filePath.mid(filePath.lastIndexOf("/")+1);
        this->setItem(0, 0, new QTableWidgetItem(fileName));
        this->setItem(0, 1, new QTableWidgetItem(""));
        m_fileList.append(filePath);

    }
}

/**************************************************************************************************
 *函数名： dragEnterEvent
 *时间：   2022-09-12 14:59:20
 *用户：
 *参数：   QDragEnterEvent *event
 *返回值： 无
 *描述：   进入事件
*************************************************************************************************/
void MyTableWidget::dragEnterEvent(QDragEnterEvent *event)
{
    event->accept();
    if (event->mimeData()->hasUrls())
    {
        event->acceptProposedAction();
    }
    else
    {
        event->ignore();
    }
}



/**************************************************************************************************
 *函数名： dropEvent
 *时间：   2022-09-12 14:53:16
 *用户：
 *参数：   QDropEvent *event
 *返回值： 无
 *描述：   拖拽松开事件，过滤非stl文件，将stl文件表格中打开并显示
*************************************************************************************************/
void MyTableWidget::dropEvent(QDropEvent *event)
{
    QStringList fileList{};
    for(const auto &url : event->mimeData()->urls())
    {
        const auto &fileName = url.toLocalFile();
        if (0 == fileName.right(4).compare(".stl",Qt::CaseInsensitive))
        {
            fileList.append(fileName);
        }
    }
    if (!fileList.isEmpty())
    {
        //将文件序列添加到表格中
        addStlFileToTable(fileList);
    }
}
