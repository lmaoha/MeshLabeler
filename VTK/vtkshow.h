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
#include <vtkFeatureEdges.h>


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

    //设置简化三角形个数
    void setReductionCount(int reductionCount);

    //显示VTK主窗口
    int showVtk(const QString stlFileName);

    bool saveVtP(QString vtpFileName);

    vtkNew<DesignInteractorStyle> m_vtkStyle;

private:

    void openSTLFile(QString fileName);
    void openPLYFile(QString fileName);
    void openVTPfile(QString fileName);

    void iniColorTable();

    //简化三角形
    void decimatePro(const int &triangleCount);

    //初始化标量
    void iniScalars();
signals:

private:
    QVTKWidget *m_vtkWidget;
    vtkNew<vtkNamedColors> m_colors;
    vtkSmartPointer<vtkRenderWindow> m_renderWindow;
    vtkSmartPointer<vtkRenderer> m_renderer;
    vtkNew<vtkActor> polydataActor;          //载入模型actor
    vtkSmartPointer<vtkPolyData> m_polyData;   //载入模型的polyData
    vtkSmartPointer<vtkFeatureEdges> m_featureEdges;    //特征边缘
    vtkNew<vtkLookupTable> lut;

    int m_reductionCount;   //简化三角形个数
public:


};

#endif // VTKSHOW_H
