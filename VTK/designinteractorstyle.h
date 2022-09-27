#ifndef DESIGNINTERACTORSTYLE_H
#define DESIGNINTERACTORSTYLE_H

#include <QObject>
#include <QDebug>
#include <vtkPolyData.h>

//vtk
#include <vtkActor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkRenderWindow.h>

class DesignInteractorStyle : public QObject , public vtkInteractorStyleTrackballCamera
{
    Q_OBJECT
public:
    explicit DesignInteractorStyle(QObject *parent = nullptr);

    static DesignInteractorStyle* New()
    {
        return new DesignInteractorStyle;
    }
    vtkTypeMacro(DesignInteractorStyle, vtkInteractorStyleTrackballCamera);

    virtual ~DesignInteractorStyle() {}

    // vtkInteractorStyle interface
public:
    virtual void OnLeftButtonDown() override;
    virtual void OnLeftButtonUp() override;
    virtual void OnRightButtonDown() override; // 避免vtk的GrabFocus接口占用交互命令
    virtual void OnRightButtonUp() override;
    virtual void OnMiddleButtonDown() override;
    virtual void OnMiddleButtonUp() override;
    virtual void OnMouseMove() override;
    virtual void OnMouseWheelForward() override;
    virtual void OnMouseWheelBackward() override;
    virtual void OnKeyPress() override;
    virtual void OnKeyRelease() override;
    virtual void OnChar() override;


    void setPolyData(vtkPolyData *newPolyData);
    void setRenderer(vtkRenderer *newRenderer);

    void setPolyDataActor(vtkActor *newPolyDataActor);

public slots:
    void slot_changeKeyPressNumber(int number);

private:
    bool getOBBTreeIntersectWithLine(vtkPolyData *polyData, vtkPoints *intersecPoints, vtkIdList *intersecCells);
    void BFS(double *Position, int TriID);
    bool CellInSphere(double *Position, int TriID);

private:
    vtkNew<vtkActor> m_sphereActor;   //球actor
    vtkNew<vtkLookupTable> m_lut;     //色卡

    vtkPolyData *m_polyData;
    vtkActor *m_polyDataActor;
    vtkRenderer *m_renderer;

    bool m_leftButtonIsPress  = false;
    bool m_rightButtonIsPress = false;
    bool m_midButtonIsPress   = false;
    bool m_bShiftKeyIsPress = false;
    int m_keyPressNumber = 0;     //按下的数字键
    int m_lastKeyPressNumber =0;  //上一个按下的数字键(在按下shift键后保存，用于恢复)
    double MouseSphereRadius = 2.5;

    enum SelectMode
    {
        MultipleSelect = 0,  //多重选择,选择一大片
        SingleSelect = 1     //单个选择 选择一个三角面片
    };
    SelectMode triangleSelectMode = MultipleSelect;

signals:
    void sig_keyPressNumber(int number);





};

#endif // DESIGNINTERACTORSTYLE_H
