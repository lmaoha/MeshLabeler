#ifndef MYTABLEWIDGET_H
#define MYTABLEWIDGET_H

#include <QTableWidget>
#include <QMessageBox>



class MyTableWidget : public QTableWidget
{
    Q_OBJECT
public:
    explicit MyTableWidget(QWidget *parent = nullptr);

    //添加stl文件到表格中
    void addStlFileToTable(const QStringList &fileList);

signals:

    //选中的文件路径 (VTK将要打开的文件)
    void clickFilePath(QString filePath);

    // QWidget interface
protected:
    //子类中使用拖拽必须加上 dragMoveEvent 防止父类中的事件默认处理。
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override {Q_UNUSED(event)};
    void dropEvent(QDropEvent *event) override;

private:
    QStringList m_fileList;

public:
    int currentTableIndex;
};

#endif // MYTABLEWIDGET_H
