#include "designinteractorstyle.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkProperty.h"
#include "vtkRendererCollection.h"

#include <vtkOBBTree.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>

DesignInteractorStyle::DesignInteractorStyle(QObject *parent)
    : QObject{parent}
{
    m_lut->SetNumberOfTableValues(20);
    m_lut->Build();
    double white[4] = { 1.0,1.0,1.0,1.0 };
    m_lut->SetTableValue(0, white);
}

void DesignInteractorStyle::OnLeftButtonDown()
{
    //将右键旋转绑定到左键上
    if (m_rightButtonIsPress)
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        return;
    }

    m_leftButtonIsPress = true;

    vtkNew<vtkPoints> intersecPoints;
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(m_polyData,intersecPoints,intersecCells);

    if (!bRet || intersecCells->GetNumberOfIds() < 0)
    {
        return;
    }
    vtkIdType TriID =intersecCells->GetId(0);
    m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, m_keyPressNumber);    //改变向量的值
    m_polyData->GetCellData()->Modified();
    m_polyData->GetCellData()->GetScalars()->Modified();
    this->Interactor->Render();

    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

void DesignInteractorStyle::OnLeftButtonUp()
{
    m_leftButtonIsPress = false;
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void DesignInteractorStyle::OnRightButtonDown()
{
    m_rightButtonIsPress = true;
    this->vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
}

//右键事件 绑定到默认左键旋转操作
void DesignInteractorStyle::OnRightButtonUp()
{
    m_rightButtonIsPress = false;
    this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

//中键事件，绑定到默认中键操作
void DesignInteractorStyle::OnMiddleButtonDown()
{
    m_midButtonIsPress = true;
    vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}

void DesignInteractorStyle::OnMiddleButtonUp()
{
    m_midButtonIsPress = false;
    vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
}

void DesignInteractorStyle::OnMouseMove()
{
    //当右键点击时，采用鼠标左键默认的旋转方式。
    //当中键点击时，采用鼠标中键默认的移动方式
    if (m_rightButtonIsPress || m_midButtonIsPress)
    {
        vtkInteractorStyleTrackballCamera::OnMouseMove();
        return;
    }

    vtkNew<vtkPoints> intersecPoints;  //交互点集合
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(m_polyData,intersecPoints,intersecCells);

    static int i = 0;
    i++;
    if (!bRet /*|| i % 2*/)
    {
        return;
    }
    double position[3];
    intersecPoints->GetPoint(0,position);
    vtkIdType TriID =intersecCells->GetId(0);

    vtkNew<vtkSphereSource>  sphere;
    sphere->SetCenter(position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    m_sphereActor->SetMapper(sphereMapper);
    m_sphereActor->GetProperty()->SetOpacity(0.2);
    m_sphereActor->GetProperty()->SetColor(m_lut->GetTableValue(m_keyPressNumber));
    m_sphereActor->PickableOff();
    m_renderer->AddActor(m_sphereActor);
    this->Interactor->Render();

    if(!m_leftButtonIsPress)
    {
        return;
    }

    if (SelectMode::MultipleSelect == triangleSelectMode)
    {
        BFS(position, TriID);
        m_polyData->GetCellData()->Modified();
        m_polyData->GetCellData()->GetScalars()->Modified();
    }
    else if(SelectMode::SingleSelect == triangleSelectMode)
    {
        m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, m_keyPressNumber);
        m_polyData->GetCellData()->GetScalars()->Modified();
    }

    this->Interactor->Render();

    //此处不可向下传递事件了，会跑到默认的鼠标单击后移动事件中的旋转零件操作
//    this->vtkInteractorStyleTrackballCamera::OnMouseMove();
}


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
    qDebug()<<"key: "<<QString::fromStdString(key);
    if ("s" == key || "w" == key || "3" == key)
    {
        return;
    }
    vtkInteractorStyleTrackballCamera::OnChar();
}


void DesignInteractorStyle::OnMouseWheelForward()
{
    if (!this->Interactor->GetControlKey())
    {
        this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
        return;
    }

    MouseSphereRadius += 0.15;

    vtkNew<vtkPoints> intersecPoints;  //交互点集合
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(m_polyData,intersecPoints,intersecCells);

    static int i = 0;
    i++;
    if (!bRet /*|| i % 2*/)
    {
        return;
    }
    double position[3];
    intersecPoints->GetPoint(0,position);

    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkNew<vtkPolyDataMapper> spheremapper;
    spheremapper->SetInputConnection(sphere->GetOutputPort());
    m_sphereActor->SetMapper(spheremapper);
    m_sphereActor->GetProperty()->SetOpacity(0.2);
    m_sphereActor->GetProperty()->SetColor(m_lut->GetTableValue(m_keyPressNumber));
    m_sphereActor->PickableOff();
    m_renderer->AddActor(m_sphereActor);
    this->Interactor->Render();
}

void DesignInteractorStyle::OnMouseWheelBackward()
{
    if (!this->Interactor->GetControlKey())
    {
        this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
        return;
    }

    MouseSphereRadius -= 0.15;

    vtkNew<vtkPoints> intersecPoints;  //交互点集合
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(m_polyData,intersecPoints,intersecCells);

    static int i = 0;
    i++;
    if (!bRet /*|| i % 2*/)
    {
        return;
    }
    double position[3];
    intersecPoints->GetPoint(0,position);

    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    m_sphereActor->SetMapper(sphereMapper);
    m_sphereActor->GetProperty()->SetOpacity(0.2);
    m_sphereActor->GetProperty()->SetColor(m_lut->GetTableValue(m_keyPressNumber));
    m_sphereActor->PickableOff();
    m_renderer->AddActor(m_sphereActor);
    this->Interactor->Render();

}

void DesignInteractorStyle::OnKeyPress()
{
    char flag = this->Interactor->GetKeyCode();
    qDebug()<<"flag: "<< flag;

    if ('s' == flag)
    {
        triangleSelectMode = SelectMode::SingleSelect;
        m_polyDataActor->GetProperty()->EdgeVisibilityOn();
        m_renderer->RemoveActor(m_sphereActor);
        return;
    }

    if ('d' == flag)
    {
        triangleSelectMode = SelectMode::MultipleSelect;
        m_polyDataActor->GetProperty()->EdgeVisibilityOff();
        return;
    }

    //shift 按下数字变为0 并保存原有数字
    if (this->Interactor->GetShiftKey())
    {
        m_bShiftKeyIsPress = true;
        m_lastKeyPressNumber = m_keyPressNumber;
        m_keyPressNumber = 0;
        emit sig_keyPressNumber(m_keyPressNumber);
        return;
    }

    if (flag - '0' > 9 || flag - '0' < 0)
    {
        return;
    }

    m_keyPressNumber = flag - '0';
    emit sig_keyPressNumber(m_keyPressNumber);
}


void DesignInteractorStyle::OnKeyRelease()
{
    //shift 松开恢复原有数字
    if (m_bShiftKeyIsPress && !this->Interactor->GetShiftKey())
    {
        m_bShiftKeyIsPress = false;
        m_keyPressNumber = m_lastKeyPressNumber;
        emit sig_keyPressNumber(m_keyPressNumber);
        this->vtkInteractorStyleTrackballCamera::OnKeyRelease();
    }
}

void DesignInteractorStyle::setPolyData(vtkPolyData *newPolyData)
{
    m_polyData = newPolyData;
}

void DesignInteractorStyle::setRenderer(vtkRenderer *newRenderer)
{
    m_renderer = newRenderer;

    //关键一步 平行投影 obbTree才可以正常使用
    m_renderer->GetActiveCamera()->SetParallelProjection(1);
}

/**************************************************************************************************
 *函数名： getOBBTreeIntersectWithLine
 *时间：   2022-09-25 01:34:41
 *用户：   李旺
 *参数：   vtkPolyData *m_polyData, 与之相交的对象
 *        vtkPoints *intersecPoints    与polydata 相交点集合
 *        vtkIdList *intersecCells     与cell相交的集合
 *返回值： 有相交的cell true  无相交的 false
 *描述：   获取 鼠标当前位置与模型相交的points 和 cellIDs的集合（points 和 cells可能有多个)
*************************************************************************************************/
bool DesignInteractorStyle::getOBBTreeIntersectWithLine(vtkPolyData *m_polyData, vtkPoints *intersecPoints, vtkIdList *intersecCells)
{
    int* pEvtPos      = this->Interactor->GetEventPosition();
    auto renderer     = this->Interactor->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    double* pViewDir  = renderer->GetActiveCamera()->GetDirectionOfProjection(); // 当前视角向量

    renderer->SetDisplayPoint(pEvtPos[0], pEvtPos[1], 0);
    renderer->DisplayToWorld();

    //通过OBBTree来做
    double worldPos[3];  //屏幕像素投影到零件的实际位置
    double endPos[3];    //投影射线终点
    renderer->GetWorldPoint(worldPos);
    for (int i = 0; i < 3;i++)
    {
        endPos[i] = pViewDir[i] * 1000 + worldPos[i];
    }

    vtkNew<vtkOBBTree> obbTree;
    obbTree->SetDataSet(m_polyData);
    obbTree->BuildLocator();

    int iRet = obbTree->IntersectWithLine(worldPos,endPos,intersecPoints,intersecCells);

    if (0 == iRet || intersecPoints->GetNumberOfPoints() < 0 ||intersecCells->GetNumberOfIds() < 0)
    {
        return false;
    }
    return true;
}

void DesignInteractorStyle::BFS(double *Position, int TriID)
{
    if (CellInSphere(Position,TriID)==0) return;
    if (static_cast<int>(m_polyData->GetCellData()->GetScalars()->GetTuple1(TriID)) == m_keyPressNumber) return;
    m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, m_keyPressNumber);
    auto id0 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(0);
    auto id1 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(1);
    auto id2 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(2);
    vtkNew<vtkIdList> idlist0 ;
    m_polyData->GetPointCells(id0, idlist0);
    vtkNew<vtkIdList> idlist1;
    m_polyData->GetPointCells(id1, idlist1);
    vtkNew<vtkIdList> idlist2;
    m_polyData->GetPointCells(id2, idlist2);
    for (int i = 0; i < idlist0->GetNumberOfIds(); i++)
        BFS(Position,idlist0->GetId(i));
    for (int i = 0; i < idlist1->GetNumberOfIds(); i++)
        BFS(Position, idlist1->GetId(i));
    for (int i = 0; i < idlist2->GetNumberOfIds(); i++)
        BFS(Position, idlist2->GetId(i));
    return;
}

bool DesignInteractorStyle::CellInSphere(double *Position, int TriID)
{
    auto pt0 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(0);
    auto pt1 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(1);
    auto pt2 = m_polyData->GetCell(TriID)->GetPoints()->GetPoint(2);
    //auto f0 = vtkMath::Distance2BetweenPoints(Position, pt0) < Radius ? 1 : 0;
    if (sqrt(vtkMath::Distance2BetweenPoints(Position, pt0)) < MouseSphereRadius)
        return 1;
    if (sqrt(vtkMath::Distance2BetweenPoints(Position, pt1)) < MouseSphereRadius)
        return 1;
    if (sqrt(vtkMath::Distance2BetweenPoints(Position, pt2)) < MouseSphereRadius)
        return 1;
    //auto f1 = vtkMath::Distance2BetweenPoints(Position, pt1) > Radius ? 0 : 1;
    //auto f2 = vtkMath::Distance2BetweenPoints(Position, pt2) > Radius ? 0 : 1;
    return 0;
}

void DesignInteractorStyle::setPolyDataActor(vtkActor *newPolyDataActor)
{
    m_polyDataActor = newPolyDataActor;
}

void DesignInteractorStyle::slot_changeKeyPressNumber(int number)
{
    m_keyPressNumber = number;
    m_sphereActor->GetProperty()->SetColor(m_lut->GetTableValue(m_keyPressNumber));
    this->Interactor->Render();
}

