#ifndef COLORTABLEWIDGET_H
#define COLORTABLEWIDGET_H

// 此类用于色卡 通过滚轮来实现色卡的切换

#include <QItemDelegate>
#include <QPainter>
#include <QStyledItemDelegate>
#include <QTableWidget>

//色标表 后续做成json格式可以随时改变标签的
const QStringList labelList{"UR8/LL8", "UR7/LL7", "UR6/LL6", "UR5/LL5", "UR4/LL4", "UR3/LL3", "UR2/LL2", "UR1/LL1",
                  "UL1/LR1", "UL2/LR2", "UL3/LR3", "UL4/LR4", "UL5/LR5", "UL6/LR6", "UL7/LR7", "UL8/LR8",
                  "URE/LLE", "URD/LLD", "URC/LLC", "URB/LLB", "URA/LLA",
                  "ULA/LRA", "ULB/LRB", "ULC/LRC", "ULD/LRD", "ULE/LRE"};

//颜色表
const QList<QColor> labelColorList{
    QColor(150, 117, 148),QColor(170, 255, 255),QColor(255, 0, 127),
    QColor(170, 255, 127),QColor(0, 0, 127),    QColor(255, 255, 127),
    QColor(255, 170, 255),QColor(255, 255, 255),QColor(77, 99, 82),
    QColor(255, 0, 255),  QColor(255, 255, 0),  QColor(0, 0, 255),
    QColor(0, 255, 0),    QColor(255, 0, 0),    QColor(0, 255, 255),
    QColor(202, 200, 232),QColor(68, 114, 196), QColor(255, 80, 80),
    QColor(0, 51, 0),     QColor(102, 102, 255),QColor(204, 204, 0),
    QColor(102, 0, 51),   QColor(231, 232, 228),QColor(95, 95, 95),
    QColor(102, 0, 204),  QColor(255, 153, 0)};

class BackgroundDelegate : public QStyledItemDelegate
{
public:
  explicit BackgroundDelegate(QObject *parent = 0)
      : QStyledItemDelegate(parent){}
  void paint(QPainter *painter, const QStyleOptionViewItem &option,
             const QModelIndex &index) const
  {
    // Fill the background before calling the base class paint
    // otherwise selected cells would have a white background
    QVariant background = index.data(Qt::BackgroundRole);
    if (background.canConvert<QBrush>())
        painter->fillRect(option.rect, background.value<QBrush>());

    //将原始文本颜色拿到手
    auto textColor = index.data(Qt::TextColorRole);
    painter->setPen(textColor.value<QColor>());
    //将原本文本字体拿到手
    auto text = index.data(Qt::TextDate);
    //在原有的位置重新绘制文本
    painter->drawText(option.rect,Qt::AlignCenter,text.value<QString>());

    if(option.state & QStyle::State_Selected)
    {
        painter->save();
        QPen pen(Qt::red, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin);
        int w = pen.width()/2;
        painter->setPen(pen);
        painter->drawRect(option.rect.adjusted(w,w,-w,-w));
        painter->restore();
    }
  }

};


class ColorTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit ColorTableWidget(QWidget *parent = nullptr);

private:
    void initColorTable();
private:
    int m_colorLabelIndex;
signals:
    void colorLabelChangeIndex(int index);

};

#endif // COLORTABLEWIDGET_H
