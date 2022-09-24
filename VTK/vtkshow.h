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
#include <vtkInteractorStyleTrackballCamera.h>
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
using namespace std;



class DesignInteractorStyle : public vtkInteractorStyleTrackballCamera
{
public:
    static DesignInteractorStyle* New()
    {
        return new DesignInteractorStyle;
    }
    vtkTypeMacro(DesignInteractorStyle, vtkInteractorStyleTrackballCamera);

    DesignInteractorStyle() {}
    virtual ~DesignInteractorStyle() {}

    virtual void OnLeftButtonDown() override {}
    virtual void OnLeftButtonUp() override {}
    virtual void OnRightButtonDown() override { this->StartRotate(); } // 避免vtk的GrabFocus接口占用交互命令
    virtual void OnRightButtonUp() override { this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp(); }
    virtual void OnMouseMove() override { this->vtkInteractorStyleTrackballCamera::OnMouseMove(); }
    virtual void OnMouseWheelForward() override;
    virtual void OnMouseWheelBackward() override;
    virtual void OnChar() override;
};

/**************************************************************************************************
 *函数名：OnChar
 *时间：   2022-09-25 00:42:59
 *用户：   李旺
 *参数：  无
 *返回值：无
 *描述：  重写事件覆盖原有按键操作 屏蔽按键
 *      按键事件传递过程(数字键或字母键）
 *      1.vtkInteractorStyleTrackballCamera::OnKeyPress()
 *      2.vtkCallbackCommand::KeyPressFunction()
 *      3.vtkInteractorStyleTrackballCamera::OnChar()
 *      4.vtkCallbackCommand::charEventFunction()
 *
 *      ctrl 或者shift键等 则
 *      1.vtkInteractorStyleTrackballCamera::OnKeyPress()
 *      2.vtkCallbackCommand::KeyPressFunction()
*************************************************************************************************/
inline void DesignInteractorStyle::OnChar()
{
    vtkRenderWindowInteractor *rwi = this->Interactor;
    const std::string key = rwi->GetKeySym();
    if ("s" == key || "w" == key || "3" == key)
    {
        return;
    }
    vtkInteractorStyleTrackballCamera::OnChar();
}

class VtkShow : public QWidget
{
    Q_OBJECT
public:
    explicit VtkShow(QWidget *parent = nullptr);
    void setWidget(QVTKWidget *vtkWidget);

    int showVtk(const QString stlFileName);

    bool saveVtP(QString vtpFileName);

    static bool CellInSphere(double *Position,int TriID);
    static void BFS(double *Position, int TriID);
    static void LeftButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void LeftButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void MiddleButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void MiddleButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void RightButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void RightButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void KeyPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void KeyReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void MouseMoveFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void MouseWheelForwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
    static void MouseWheelBackwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);

    //获取鼠标点击与模型的cellID号(可能是多个）
    static bool getOBBTreeIntersectWithLine(vtkRenderWindowInteractor *vtkInter, vtkPolyData *polyData, vtkPoints *intersecPoints, vtkIdList *intersecCells);

    static vtkNew<vtkCallbackCommand> MouseMoveCallback;
    static vtkNew<vtkCallbackCommand> LeftButtonPressCallback;
    static vtkNew<vtkCallbackCommand> LeftButtonReleaseCallback;
    static vtkNew<vtkCallbackCommand> MiddleButtonPressCallback;
    static vtkNew<vtkCallbackCommand> MiddleButtonReleaseCallback;
    static vtkNew<vtkCallbackCommand> RightButtonPressCallback;
    static vtkNew<vtkCallbackCommand> RightButtonReleaseCallback;
    static vtkNew<vtkCallbackCommand> KeyPressCallback;
    static vtkNew<vtkCallbackCommand> KeyReleaseCallback;
    static vtkNew<vtkCallbackCommand> MouseWheelForwardCallback;
    static vtkNew<vtkCallbackCommand> MouseWheelBackwardCallback;

    static vtkNew<vtkLookupTable> lut;              //不同色差
    static vtkNew<vtkActor> sphereActor;            //鼠标跟随球
    static vtkNew<vtkActor> polydataActor;          //载入模型actor
    static vtkSmartPointer<vtkPolyData> polydata;   //载入模型的polyData
    static bool LeftButtonIsPress;     //鼠标左键按下  按下：true   松开：false
    static int  KeyPressFlag;          //键盘按下的标签 0-9
    static double MouseSphereRadius;   //鼠标球体半径
    enum SelectMode
    {
        MultipleSelect = 0,  //多重选择,选择一大片
        SingleSelect = 1     //单个选择 选择一个三角面片
    };
    static SelectMode triangleSelectMode;

    vtkSmartPointer<vtkRenderWindow> m_renderWindow;

private:
    void initCallbackCommand();

signals:

private:
    QVTKWidget *m_vtkWidget;
    vtkNew<vtkNamedColors> m_colors;
    vtkSmartPointer<vtkRenderer> m_renderer;

public:


};

#endif // VTKSHOW_H
