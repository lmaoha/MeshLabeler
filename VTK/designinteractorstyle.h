#ifndef DESIGNINTERACTORSTYLE_H
#define DESIGNINTERACTORSTYLE_H

#include <QObject>
#include <QDebug>
#include <vtkPolyData.h>

//vtk
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
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

    void setPolyData(vtkPolyData *newPolyData);
    void setRenderer(vtkRenderer *newRenderer);

    void setPolyDataActor(vtkActor *newPolyDataActor);

    void setLut(vtkLookupTable *newLut);

signals:
    void sig_keyPressNumber(int number);

public slots:
    void slot_changeKeyPressNumber(int number);

    // vtkInteractorStyle interface
protected:
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



private:
    //OBBtree与 直线与模型的相交点
    bool getOBBTreeIntersectWithLine(vtkPolyData *polyData, vtkPoints *intersecPoints, vtkIdList *intersecCells);

    //循环寻找点
    void BFS(const double *Position, const int TriID);

    //cell包含在球内
    bool CellInSphere(const double *Position, int TriID);

    void showCellID();

private:
    vtkNew<vtkActor> m_sphereActor;   //球actor
    vtkLookupTable *m_lut;     //色卡

    vtkNew<vtkActor2D> cellLabels;
    vtkNew<vtkActor2D> rectActor;
    vtkNew<vtkActor2D> pointLabels;

    vtkPolyData *m_polyData;
    vtkActor *m_polyDataActor;
    vtkRenderer *m_renderer;

    bool m_leftButtonIsPress  = false;
    bool m_rightButtonIsPress = false;
    bool m_midButtonIsPress   = false;
    bool m_bShiftKeyIsPress = false;
    int m_keyPressNumber = 1;     //按下的数字键
    int m_lastKeyPressNumber = 0;  //上一个按下的数字键(在按下shift键后保存，用于恢复)
    double MouseSphereRadius = 2.5;  //原始2.5

    enum SelectMode
    {
        MultipleSelect = 0,  //多重选择,选择一大片
        SingleSelect = 1     //单个选择 选择一个三角面片
    };
    SelectMode triangleSelectMode = MultipleSelect;







};

#endif // DESIGNINTERACTORSTYLE_H
