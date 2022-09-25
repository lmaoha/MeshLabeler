#ifndef VTKSHOW_H
#define VTKSHOW_H
/**************************************************************************************************
此类是VTK操作，用于显示VTK和模型的交互
方法改进: 是否可以通过style 来用面向对象的方式来实现鼠标的事件，这样就可以不使用静态函数来做了。
将style 需要的对象都传入进去，
*************************************************************************************************/

#include <QWidget>
#include <QString>
#include <QDebug>


//vtk
#include <vtkLookupTable.h>
#include <QVTKWidget.h>
#include <vtkSmartPointer.h>
#include <vtkColor.h>
#include <vtkNew.h>
#include <vtkNamedColors.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkSTLReader.h>
#include <vtkProperty.h>
#include <vtkPolyDataMapper.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkDataArray.h>
#include <vtkPolyData.h>
#include <vtkCellData.h>
#include <vtkCamera.h>
#include <vtkViewport.h>

#include <vtkAutoInit.h>
VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)

#include "designinteractorstyle.h"
using namespace std;


class VtkShow : public QWidget
{
    Q_OBJECT
public:
    explicit VtkShow(QWidget *parent = nullptr);
    void setWidget(QVTKWidget *vtkWidget);

    int showVtk(const QString stlFileName);

    bool saveVtP(QString vtpFileName);

    vtkSmartPointer<vtkRenderWindow> m_renderWindow;
    vtkNew<DesignInteractorStyle> m_vtkStyle;

private:
    void initCallbackCommand();

signals:

private:
    QVTKWidget *m_vtkWidget;
    vtkNew<vtkNamedColors> m_colors;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkNew<vtkActor> polydataActor;          //载入模型actor
    vtkSmartPointer<vtkPolyData> polydata;   //载入模型的polyData
    vtkNew<vtkLookupTable> lut;
//    vtkNew<DesignInteractorStyle> m_style;

public:


};

#endif // VTKSHOW_H
