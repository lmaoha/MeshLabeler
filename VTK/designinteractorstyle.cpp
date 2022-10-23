#include "designinteractorstyle.h"
#include "vtkCamera.h"
#include "vtkCellData.h"
#include "vtkProperty.h"
#include "vtkRendererCollection.h"

#include <QElapsedTimer>
#include <QTime>
#include <vtkOBBTree.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkIdFilter.h>
#include <vtkCellCenters.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkLabeledDataMapper.h>
#include <vtkNamedColors.h>
#include <set>
#include <vtkImageCanvasSource2D.h>
#include <vtkImageActor.h>
#include <vtkImageMapper3D.h>
#include <array>
#include <vtkSelectEnclosedPoints.h>
#include <vtkDataArray.h>
#include <vtkFloatArray.h>
#include <vtkLineSource.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkPointData.h>
#include <iostream>

DesignInteractorStyle::DesignInteractorStyle(QObject *parent)
    : QObject{parent}
{
    connect(this,&DesignInteractorStyle::sig_keyPressNumber,this,&DesignInteractorStyle::slot_changeKeyPressNumber);
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
//    cell2DShow();
    showCellID();   //使用
    SelectVisiblePointsStyle_SelectCell();
    
    
    //showCircle(); //显示圆


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
        m_visit.clear();
        m_visit.resize(m_polyData->GetNumberOfCells(),false);
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
    if ("s" == key || "w" == key || "3" == key || "f" == key)
    {
        return;
    }
    vtkInteractorStyleTrackballCamera::OnChar();
}


void DesignInteractorStyle::OnMouseWheelForward()
{
    //键盘ctrl + shift 一起按下加滚轮 选择颜色
    if(this->Interactor->GetControlKey() && this->Interactor->GetShiftKey())
    {
        //防止数字超过最大与最小
        if (++m_keyPressNumber > m_lut->GetNumberOfTableValues())
        {
            m_keyPressNumber = m_lut->GetNumberOfTableValues();
        }
        emit sig_keyPressNumber(m_keyPressNumber);
        return;
    }

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
    if(this->Interactor->GetControlKey() && this->Interactor->GetShiftKey())
    {
        if (--m_keyPressNumber <= 0)
        {
            m_keyPressNumber = 1;
        }
        emit sig_keyPressNumber(m_keyPressNumber);
        return;
    }

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

    //shift 按下数字变为0 并保存原有数字
    if (this->Interactor->GetShiftKey() && !this->Interactor->GetControlKey())
    {
        m_bShiftKeyIsPress = true;
        m_lastKeyPressNumber = m_keyPressNumber;
        m_keyPressNumber = 0;
        emit sig_keyPressNumber(m_keyPressNumber);
        return;
    }

    if ('f' == flag)
    {
        m_bFKeyIsPress = true;
    }

    if ('g' == flag)
    {
        m_b_G_keyIsPress = true;
    }


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
        //this->vtkInteractorStyleTrackballCamera::OnKeyRelease();
        return;
    }

    if ('f' == this->Interactor->GetKeyCode())
    {
        m_bFKeyIsPress = false;
        return;
    }
    if ('g' == this->Interactor->GetKeyCode())
    {
        m_b_G_keyIsPress = false;
        return;
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
 *用户：
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

    int iRet = obbTree->IntersectWithLine(worldPos,endPos,intersecPoints,intersecCells);

    if (0 == iRet || intersecPoints->GetNumberOfPoints() < 0 ||intersecCells->GetNumberOfIds() < 0)
    {
        return false;
    }
    return true;
}

void DesignInteractorStyle::BFS(const double *Position, const int TriID)
{
    if (!CellInSphere(Position,TriID) || m_visit[TriID])
    {
        return;
    }

    m_visit[TriID] = true;

    m_polyData->GetCellData()->GetScalars()->SetTuple1(TriID, m_keyPressNumber);

    //获取当前三角面片的点ID
    auto id0 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(0);
    auto id1 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(1);
    auto id2 = m_polyData->GetCell(TriID)->GetPointIds()->GetId(2);

    //根据点ID获取 cellID
    vtkNew<vtkIdList> idlist0 ;
    m_polyData->GetPointCells(id0, idlist0);
    idlist0->DeleteId(TriID);

    vtkNew<vtkIdList> idlist1;
    m_polyData->GetPointCells(id1, idlist1);
    idlist1->DeleteId(TriID);

    vtkNew<vtkIdList> idlist2;
    m_polyData->GetPointCells(id2, idlist2);
    idlist2->DeleteId(TriID);

    vtkNew<vtkIdList> idListCount;

    for (int i = 0; i < idlist0->GetNumberOfIds(); i++)
    {
        idListCount->InsertUniqueId(idlist0->GetId(i));
    }
    for (int i = 0; i < idlist1->GetNumberOfIds(); i++)
    {
        idListCount->InsertUniqueId(idlist1->GetId(i));
    }
    for (int i = 0; i < idlist2->GetNumberOfIds(); i++)
    {
        idListCount->InsertUniqueId(idlist2->GetId(i));
    }

    for (int i = 0; i < idListCount->GetNumberOfIds(); i++)
    {
        BFS(Position, idListCount->GetId(i));
    }
    return;
}


/**************************************************************************************************
 *函数名：CellInSphere
 *时间：   2022-10-01 18:15:42
 *用户：
 *参数： double *Position   鼠标点击下的位置
 *      int TriID  鼠标点击下的三角面ID号
 *返回值： 在球内 true   在球外false
 *描述：cell的3个点 只要有个一个点 在是在球内 就返回true
*************************************************************************************************/
bool DesignInteractorStyle::CellInSphere(const double *Position, int TriID)
{
    for(int i = 0;i < m_polyData->GetCell(TriID)->GetPoints()->GetNumberOfPoints();i++)
    {
        auto pt0 = m_polyData->GetPoint(m_polyData->GetCell(TriID)->GetPointId(i));
        if(sqrt(vtkMath::Distance2BetweenPoints(Position, pt0)) < MouseSphereRadius)
        {
            return true;
        }
    }
    return false;
}


/**************************************************************************************************
 *函数名： showCellID
 *时间：   2022-10-01 18:15:42
 *用户：   李旺
 *参数：   无
 *返回值： 在球内 true   在球外false
 *描述：   将数据使用vtkIdFilter  变成全是点的数据  
 *          分为 三角形中心点  和 顶点   
 *          再使用vtkSelectVisiblePoints 提取可见点
*************************************************************************************************/
void DesignInteractorStyle::showCellID()
{
    if (!m_bFKeyIsPress)
    {
        m_renderer->RemoveActor(rectActor);
        m_renderer->RemoveActor(cellLabels);
        m_renderer->RemoveActor(pointLabels);
        m_polyDataActor->GetProperty()->EdgeVisibilityOff();
        return;
    }
    
    m_polyDataActor->GetProperty()->EdgeVisibilityOn();

    //设置显示的区域
    int* pEvtPos      = this->Interactor->GetEventPosition();
    vtkNew<vtkNamedColors> colors;

    int centerX = pEvtPos[0];
    int centerY = pEvtPos[1];

    int xmin = centerX - 100;
    int xmax = centerX + 100;
    int ymin = centerY - 100;
    int ymax = centerY + 100;

    vtkNew<vtkPoints> pts;
    pts->InsertPoint(0, xmin, ymin, 0);
    pts->InsertPoint(1, xmax, ymin, 0);
    pts->InsertPoint(2, xmax, ymax, 0);
    pts->InsertPoint(3, xmin, ymax, 0);

    vtkNew<vtkCellArray> rect;
    rect->InsertNextCell(5);
    rect->InsertCellPoint(0);
    rect->InsertCellPoint(1);
    rect->InsertCellPoint(2);
    rect->InsertCellPoint(3);
    rect->InsertCellPoint(0);

    vtkNew<vtkPolyData> selectRect;
    selectRect->SetPoints(pts);
    selectRect->SetLines(rect);

    vtkNew<vtkPolyDataMapper2D> rectMapper;
    rectMapper->SetInputData(selectRect);
    rectMapper->Update();

    //显示要刷新显示的区域框
    rectActor->SetMapper(rectMapper);
    rectActor->GetProperty()->SetColor(colors->GetColor3d("black").GetData());

    //将polydatad 数据提取出来
    vtkNew<vtkIdFilter> ids;
    ids->SetInputData(m_polyData);
    ids->PointIdsOn();
    ids->CellIdsOn();
    ids->FieldDataOn();
    ids->Update();

    // cell ID显示
    vtkNew<vtkCellCenters> cellCenter;
    cellCenter->SetInputConnection(ids->GetOutputPort());
    cellCenter->VertexCellsOn();  //获取cell的顶点信息
    cellCenter->Update();

    //提取可见的点（基于 z-buffer 计算） 
    vtkNew<vtkSelectVisiblePoints> visCells;
    visCells->SetInputConnection(cellCenter->GetOutputPort());
    visCells->SetRenderer(m_renderer);
    visCells->SelectionWindowOn();
    visCells->SelectInvisibleOff();
    visCells->SetSelection(xmin, xmax, ymin, ymax);
    visCells->Update();

    vtkNew<vtkLabeledDataMapper> cellMapper;
    cellMapper->SetInputConnection(visCells->GetOutputPort());
    cellMapper->SetLabelModeToLabelFieldData();
    cellMapper->GetLabelTextProperty()->SetColor(
        colors->GetColor3d("DarkGreen").GetData());
    cellLabels->SetMapper(cellMapper);


    // Create labels for points
    vtkNew<vtkSelectVisiblePoints> visPts;
    visPts->SetInputConnection(ids->GetOutputPort());
    visPts->SetRenderer(m_renderer);
    visPts->SelectionWindowOn();
    visPts->SetSelection(xmin, xmax, ymin, ymax);

    // Create the mapper to display the point ids.  Specify the
    // format to use for the labels.  Also create the associated actor.
    vtkNew<vtkLabeledDataMapper> ldm;
    ldm->SetInputConnection(visPts->GetOutputPort());
    ldm->SetLabelModeToLabelFieldData();

    pointLabels->SetMapper(ldm);
    pointLabels->GetProperty()->SetColor(colors->GetColor3d("Yellow").GetData());

    m_renderer->AddActor2D(rectActor);
    m_renderer->AddActor2D(cellLabels);
    m_renderer->AddActor2D(pointLabels);
}


//使用vtkSelectVisiblePoints 方式来选择三角面片
void DesignInteractorStyle::SelectVisiblePointsStyle_SelectCell()
{
    if (!m_b_G_keyIsPress)
    {
        return;
    }

    //设置显示的区域
    int* pEvtPos = this->Interactor->GetEventPosition();
    vtkNew<vtkNamedColors> colors;

    int centerX = pEvtPos[0];
    int centerY = pEvtPos[1];

    int xmin = centerX - 100;
    int xmax = centerX + 100;
    int ymin = centerY - 100;
    int ymax = centerY + 100;

    vtkNew<vtkPoints> pts;
    pts->InsertPoint(0, xmin, ymin, 0);
    pts->InsertPoint(1, xmax, ymin, 0);
    pts->InsertPoint(2, xmax, ymax, 0);
    pts->InsertPoint(3, xmin, ymax, 0);

    vtkNew<vtkCellArray> rect;
    rect->InsertNextCell(5);
    rect->InsertCellPoint(0);
    rect->InsertCellPoint(1);
    rect->InsertCellPoint(2);
    rect->InsertCellPoint(3);
    rect->InsertCellPoint(0);

    vtkNew<vtkPolyData> selectRect;
    selectRect->SetPoints(pts);
    selectRect->SetLines(rect);

    vtkNew<vtkPolyDataMapper2D> rectMapper;
    rectMapper->SetInputData(selectRect);
    rectMapper->Update();

    //显示要刷新显示的区域框
    rectActor->SetMapper(rectMapper);
    rectActor->GetProperty()->SetColor(colors->GetColor3d("black").GetData());

    //将polydatad 数据提取出来
    vtkNew<vtkIdFilter> ids;
    ids->SetInputData(m_polyData);
    ids->PointIdsOn();
    ids->CellIdsOn();
    ids->FieldDataOn();
    ids->Update();

    // cell ID显示
    vtkNew<vtkCellCenters> cellCenter;
    cellCenter->SetInputConnection(ids->GetOutputPort());
    cellCenter->VertexCellsOn();  //获取cell的顶点信息
    cellCenter->Update();

    //提取可见的点（基于 z-buffer 计算） 
    vtkNew<vtkSelectVisiblePoints> visCells;
    visCells->SetInputConnection(cellCenter->GetOutputPort());
    visCells->SetRenderer(m_renderer);
    visCells->SelectionWindowOn();
    visCells->SelectInvisibleOff();
    visCells->SetSelection(xmin, xmax, ymin, ymax);
    visCells->Update();

    //当不知道polyData中有什么数据时，使用这个 
    //保存成文本模式查看内部的数据
    //vtkNew<vtkXMLPolyDataWriter> writer;
    //writer->SetInputData(visCells->GetOutput());
    //writer->SetFileName("C:/Users/Administrator/Desktop/VisCellsPolyData.vtp");
    //writer->SetDataModeToAscii();
    //writer->Write();

    //vtkIdFilter_Ids  这个才是真正的 三角形中心ID号
    vtkDataArray* dataArray = visCells->GetOutput()->GetPointData()->GetScalars("vtkIdFilter_Ids");
    for (vtkIdType i = 0; i < dataArray->GetNumberOfValues(); i++)
    {
        auto value = dataArray->GetVariantValue(i);
        m_polyData->GetCellData()->GetScalars()->SetTuple1(value.ToInt(), m_keyPressNumber);
    }
    m_polyData->Modified();
    m_renderer->AddActor2D(rectActor);

}


void DesignInteractorStyle::cell2DShow()
{
    vtkNew<vtkNamedColors> colors;

//    qDebug() <<"0000000000000";
    std::array<double, 3> drawColor1{0, 0, 0};
    std::array<double, 3> drawColor2{0, 0, 0};
    auto color1 = colors->GetColor3ub("SlateGray").GetData();
    auto color2 = colors->GetColor3ub("Tomato").GetData();
    for (auto i = 0; i < 3; ++i)
    {
      drawColor1[i] = color1[i];
      drawColor2[i] = color2[i];
    }

    vtkNew<vtkImageCanvasSource2D> imageSource;
    imageSource->SetScalarTypeToUnsignedChar();
    imageSource->SetNumberOfScalarComponents(3);
    imageSource->SetExtent(0, 20, 0, 50, 0, 0);
    imageSource->SetDrawColor(drawColor1.data());
    imageSource->FillBox(0, 20, 0, 50);
    imageSource->SetDrawColor(drawColor2.data());
    imageSource->FillBox(0, 10, 0, 30);
    imageSource->Update();

    vtkNew<vtkImageActor> actor;
    actor->GetMapper()->SetInputConnection(imageSource->GetOutputPort());

    m_renderer->AddActor(actor);
}

//
void DesignInteractorStyle::selectEnclosePoints()
{
    vtkNew<vtkSphereSource>  sphere;
//    sphere->SetCenter(position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkNew<vtkSelectEnclosedPoints> select;
    select->SetInputData(m_polyData);
    select->SetSurfaceData(sphere->GetOutput());

}


//通过obbTree 显示圆圈
void DesignInteractorStyle::showCircle()
{
    if (!m_bFKeyIsPress)
    {
        //m_polyDataActor->GetProperty()->EdgeVisibilityOff();
        return;
    }

    vtkNew<vtkPoints> intersecPoints;  //交互点集合
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(m_polyData, intersecPoints, intersecCells);
    double* pViewDir = m_renderer->GetActiveCamera()->GetDirectionOfProjection(); // 当前视角向量

    if (!bRet )return;
    double position[3];
    intersecPoints->GetPoint(0, position);
    //vtkIdType TriID = intersecCells->GetId(0);

    const unsigned int numberofpoints = 21;

    vtkNew<vtkPolyData> circle;
    vtkNew<vtkPoints> points;
    vtkNew<vtkCellArray> lines;
    vtkIdType* lineIndices = new vtkIdType[numberofpoints + 1];

    for (unsigned int i = 0; i < numberofpoints; i++)
    {
        const double angle = 2 * vtkMath::Pi() * static_cast<double>(i) / static_cast<double>(numberofpoints);

        //通过OBBTree来做
        double startPos[3];
        double endPos[3];    //投影射线终点

        startPos[0] = position[0] + MouseSphereRadius * cos(angle);
        startPos[1] = position[1] + MouseSphereRadius * sin(angle);
        startPos[2] = 0;


        endPos[0] = pViewDir[0] * 1000 + startPos[0];
        endPos[1] = pViewDir[1] * 1000 + startPos[1];
        endPos[2] = pViewDir[2] * 1000 + startPos[2];

        vtkNew<vtkLineSource> line;
        line->SetPoint1(startPos);
        line->SetPoint2(endPos);
        line->Update();
        vtkNew<vtkPolyDataMapper> mapper;
        mapper->SetInputData(line->GetOutput());

        vtkNew<vtkActor> actor;
        actor->SetMapper(mapper);
        m_renderer->AddActor(actor);

        int iRet = obbTree->IntersectWithLine(startPos, endPos, intersecPoints, intersecCells);
        if (intersecPoints->GetNumberOfPoints())
        {
            //continue;
            intersecPoints->GetPoint(0, position);
        }

        points->InsertPoint(static_cast<vtkIdType>(i), position);
        lineIndices[i] = static_cast<vtkIdType>(i);

        //            qDebug()<<"  angle: "<<angle
        //                   <<"  cos(angle): "<<holeRadius * cos(angle) + holeCenter.x()
        //                  <<"  sin(angle): "<<holeRadius * sin(angle) + holeCenter.y();
    }
    lineIndices[numberofpoints] = 0;
    lines->InsertNextCell(numberofpoints + 1, lineIndices);
    delete[] lineIndices;

    circle->SetPoints(points);
    circle->SetLines(lines);

    //vtkNew<vtkPolyDataMapper> mapper;
    //mapper->SetInputData(circle);
    //
    //vtkNew<vtkActor> actor;
    //actor->SetMapper(mapper);
    //
    //actor->GetProperty()->SetColor(255, 0, 0);

    //m_renderer->AddActor(actor);
    this->Interactor->GetRenderWindow()->Render();

}


void DesignInteractorStyle::setLut(vtkLookupTable *newLut)
{
    m_lut = newLut;
}

void DesignInteractorStyle::setPolyDataActor(vtkActor *newPolyDataActor)
{
    m_polyDataActor = newPolyDataActor;
    obbTree = vtkSmartPointer<vtkOBBTree>::New();
    obbTree->SetDataSet(m_polyData);
    obbTree->SetMaxLevel(5);
    obbTree->BuildLocator();

    //vtkNew<vtkPolyData> obbPolyData;
    //obbTree->GenerateRepresentation(3, obbPolyData);

    //vtkNew<vtkPolyDataMapper> obbMapper;
    //obbMapper->SetInputData(obbPolyData);

    //vtkNew<vtkActor> obbActor;
    //obbActor->SetMapper(obbMapper);
    //obbActor->GetProperty()->SetInterpolationToFlat();
    //obbActor->GetProperty()->SetOpacity(.5);
    //obbActor->GetProperty()->EdgeVisibilityOn();
    ////obbActor->GetProperty()->SetColor(
    ////    colors->GetColor4d("SpringGreen").GetData());
    //this->m_renderer->AddActor(obbActor);
}

void DesignInteractorStyle::slot_changeKeyPressNumber(int number)
{
    m_keyPressNumber = number;
    m_sphereActor->GetProperty()->SetColor(m_lut->GetTableValue(m_keyPressNumber));
    m_sphereActor->GetProperty()->SetOpacity(1);
    this->Interactor->Render();
}

