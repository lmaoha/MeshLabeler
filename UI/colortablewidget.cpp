#include "colortablewidget.h"
#include <QHeaderView>
#include <QDebug>
#include <QScrollBar>
#include <QPainter>

ColorTableWidget::ColorTableWidget(QWidget *parent)
    : QTableWidget{parent}
{
    initColorTable();
}

void ColorTableWidget::initColorTable()
{
    this->setRowCount(2);

    this->setEditTriggers(QAbstractItemView::NoEditTriggers);     //不允许编辑
    this->setSelectionMode(QAbstractItemView::SingleSelection);   //仅能单个选择
    this->setSelectionBehavior(QAbstractItemView::SelectColumns); //选整列

    //根据格子内容自动调整大小
    this->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    this->setVerticalHeaderLabels({"标签","颜色"});

    //添加item
    this->setColumnCount(labelList.count());
    int i =0;
    for(const auto &labelName : labelList)
    {
        this->setItem(0,i,new QTableWidgetItem(labelName));
        auto item = new QTableWidgetItem();
        item->setBackground(labelColorList.at(i));
        this->setItem(1,i,item);

        //使用代理模式 将默认的选择变蓝色的改为选择框变红色
        this->setItemDelegateForColumn(i,new BackgroundDelegate(this));
        i++;
    }

    connect(this,&ColorTableWidget::cellClicked,this,
            [&](int row,int column)
    {
        Q_UNUSED(row);
        m_colorLabelIndex = column;
        emit colorLabelChangeIndex(m_colorLabelIndex);
    });

}




