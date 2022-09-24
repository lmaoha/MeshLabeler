#include "vtkshow.h"

#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>
#include <vtkSphereSource.h>
#include <vtkRendererCollection.h>
#include <vtkFeatureEdges.h>
#include <vtkFloatArray.h>
#include <vtkLineSource.h>
#include <vtkAxesActor.h>
#include <vtkOBBTree.h>
#include <vtkTriangleFilter.h>
#include <vtkPoints.h>

bool VtkShow::LeftButtonIsPress = 0;
int VtkShow::KeyPressFlag = 0;
double VtkShow::MouseSphereRadius = 2.5;
VtkShow::SelectMode VtkShow::triangleSelectMode = SelectMode::MultipleSelect;

vtkNew<vtkCallbackCommand> VtkShow::MouseMoveCallback;
vtkNew<vtkCallbackCommand> VtkShow::LeftButtonPressCallback;
vtkNew<vtkCallbackCommand> VtkShow::LeftButtonReleaseCallback;
vtkNew<vtkCallbackCommand> VtkShow::MiddleButtonPressCallback;
vtkNew<vtkCallbackCommand> VtkShow::MiddleButtonReleaseCallback;
vtkNew<vtkCallbackCommand> VtkShow::RightButtonPressCallback;
vtkNew<vtkCallbackCommand> VtkShow::RightButtonReleaseCallback;
vtkNew<vtkCallbackCommand> VtkShow::KeyPressCallback;
vtkNew<vtkCallbackCommand> VtkShow::KeyReleaseCallback;
vtkNew<vtkCallbackCommand> VtkShow::MouseWheelForwardCallback;
vtkNew<vtkCallbackCommand> VtkShow::MouseWheelBackwardCallback;
vtkNew<vtkLookupTable> VtkShow::lut;              //不同色差
vtkNew<vtkActor> VtkShow::sphereActor;            //鼠标跟随球
vtkNew<vtkActor> VtkShow::polydataActor;          //载入模型actor
vtkSmartPointer<vtkPolyData> VtkShow::polydata;   //载入模型的polyData



VtkShow::VtkShow(QWidget *parent)
    : QWidget{parent}
{

}

/**************************************************************************************************
 *函数名： setWidget
 *时间：   2022-09-19 23:42:32
 *用户：
 *参数：   QVTKWidget *vtkWidget  ui的QVTKwidget
 *返回值： 无
 *描述：   将ui QVTKWidget 加入renderWindow中
*************************************************************************************************/
void VtkShow::setWidget(QVTKWidget *vtkWidget)
{
    m_vtkWidget = vtkWidget;
    m_renderer = vtkSmartPointer<vtkRenderer>::New();
    m_renderer->SetBackground(m_colors->GetColor3d("SkyBlue").GetData());
    m_renderer->GetActiveCamera()->SetParallelProjection(1); //关键一步 平行投影

    m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderWindow->AddRenderer(m_renderer);
    m_renderWindow->SetWindowName("lableTest");

    m_vtkWidget->SetRenderWindow(m_renderWindow);

}

/**************************************************************************************************
 *函数名：showVtk
 *时间：   2022-09-12 21:24:49
 *用户：
 *参数：   const QString stlFileName 打开文件名
 *返回值： 正常0 错误非0
 *描述：显示Vtk 模型主程序
*************************************************************************************************/
int VtkShow::showVtk(const QString stlFileName)
{
    if (stlFileName.isEmpty())
    {
        return -1;
    }

    m_renderer->RemoveAllViewProps();
    vtkNew<vtkSTLReader> STLReader;
    STLReader->SetFileName(stlFileName.toLocal8Bit().data()); //修复中文路径无法打开文件的bug
    STLReader->Update();

    vtkNew<vtkTriangleFilter> triangle;
    triangle->SetInputConnection(STLReader->GetOutputPort());
    triangle->Update();
    polydata = triangle->GetOutput();

    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputData(polydata);
    featureEdges->BoundaryEdgesOff();
    featureEdges->FeatureEdgesOn();
    featureEdges->SetFeatureAngle(20);
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOff();
    featureEdges->Update();

    vtkNew<vtkNamedColors> vtkColor;

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(vtkColor->GetColor3d("Red").GetData());
    edgeActor->GetProperty()->SetLineWidth(3.0);
    edgeActor->GetProperty()->SetRenderLinesAsTubes(true);
    edgeActor->PickableOff();
    m_renderer->AddActor(edgeActor);

    lut->SetNumberOfTableValues(20);
    lut->Build();
    double white[4] = { 1.0,1.0,1.0,1.0 };
    lut->SetTableValue(0, white);
    vtkNew<vtkFloatArray> cellData;
    for (int i = 0; i < polydata->GetNumberOfCells(); i++)
        cellData->InsertTuple1(i, 0);
    polydata->GetCellData()->SetScalars(cellData);
    polydata->GetCellData()->GetScalars()->Print(std::cout);
    polydata->BuildLinks();

    vtkNew<vtkPolyDataMapper> polydataMapper;
    polydataMapper->SetInputData(polydata);
    polydataMapper->SetScalarRange(0, 19);
    polydataMapper->SetLookupTable(lut);
    polydataMapper->Update();

    polydataActor->GetProperty()->EdgeVisibilityOff();
    polydataActor->SetMapper(polydataMapper);
    polydataActor->GetProperty()->SetOpacity(1);
//    polydataActor->GetProperty()->SetColor(lut->GetTableValue(20));
    m_renderer->AddActor(polydataActor);

    m_renderer->SetBackground(vtkColor->GetColor3d("AliceBlue").GetData());

    m_renderWindow->AddRenderer(m_renderer);

    m_renderWindow->Render();
    vtkNew<vtkRenderWindowInteractor> vtkInter;
    m_renderWindow->SetSize(1920, 1080);
    vtkNew<DesignInteractorStyle> vtkStyle;
    vtkInter->SetInteractorStyle(vtkStyle);
    vtkInter->SetRenderWindow(m_renderWindow);


    initCallbackCommand();
    vtkInter->AddObserver(vtkCommand::MouseMoveEvent,MouseMoveCallback);
    vtkInter->AddObserver(vtkCommand::LeftButtonPressEvent, LeftButtonPressCallback);
    vtkInter->AddObserver(vtkCommand::LeftButtonReleaseEvent, LeftButtonReleaseCallback);
    vtkInter->AddObserver(vtkCommand::MiddleButtonPressEvent, MiddleButtonPressCallback);
    vtkInter->AddObserver(vtkCommand::MiddleButtonReleaseEvent, MiddleButtonReleaseCallback);
    vtkInter->AddObserver(vtkCommand::RightButtonPressEvent, RightButtonPressCallback);
    vtkInter->AddObserver(vtkCommand::RightButtonReleaseEvent, RightButtonReleaseCallback);
    vtkInter->AddObserver(vtkCommand::KeyPressEvent, KeyPressCallback);
    vtkInter->AddObserver(vtkCommand::KeyReleaseEvent, KeyReleaseCallback);
    vtkInter->AddObserver(vtkCommand::MouseMoveEvent, MouseMoveCallback);
    vtkInter->AddObserver(vtkCommand::MouseWheelForwardEvent, MouseWheelForwardCallback);
    vtkInter->AddObserver(vtkCommand::MouseWheelBackwardEvent, MouseWheelBackwardCallback);
    m_renderer->ResetCamera();
    m_renderWindow->Render();


    return 0;
}

/**************************************************************************************************
 *函数名：saveVtP
 *时间：   2022-09-19 20:20:38
 *用户：
 *参数：   QString vtpFileName
 *返回值： 保存成功true 保存失败false
 *描述： write 返回 1成功  返回0 失败
*************************************************************************************************/
bool VtkShow::saveVtP(QString vtpFileName)
{
    //导出文件
    polydata->GetCellData()->GetScalars()->SetName("Label");
    polydata->GetCellData()->GetScalars()->Modified();
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputData(polydata);
    writer->SetFileName(vtpFileName.toLocal8Bit().data());
    return writer->Write();
}

bool VtkShow::CellInSphere(double *Position, int TriID)
{
    auto pt0 = polydata->GetCell(TriID)->GetPoints()->GetPoint(0);
    auto pt1 = polydata->GetCell(TriID)->GetPoints()->GetPoint(1);
    auto pt2 = polydata->GetCell(TriID)->GetPoints()->GetPoint(2);
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

void VtkShow::BFS(double*Position,int TriID)
{
    if (CellInSphere(Position,TriID)==0) return;
    if (static_cast<int>(polydata->GetCellData()->GetScalars()->GetTuple1(TriID)) == KeyPressFlag) return;
    polydata->GetCellData()->GetScalars()->SetTuple1(TriID, KeyPressFlag);
    auto id0 = polydata->GetCell(TriID)->GetPointIds()->GetId(0);
    auto id1 = polydata->GetCell(TriID)->GetPointIds()->GetId(1);
    auto id2 = polydata->GetCell(TriID)->GetPointIds()->GetId(2);
    vtkNew<vtkIdList> idlist0 ;
    polydata->GetPointCells(id0, idlist0);
    vtkNew<vtkIdList> idlist1;
    polydata->GetPointCells(id1, idlist1);
    vtkNew<vtkIdList> idlist2;
    polydata->GetPointCells(id2, idlist2);
    for (int i = 0; i < idlist0->GetNumberOfIds(); i++)
        BFS(Position,idlist0->GetId(i));
    for (int i = 0; i < idlist1->GetNumberOfIds(); i++)
        BFS(Position, idlist1->GetId(i));
    for (int i = 0; i < idlist2->GetNumberOfIds(); i++)
        BFS(Position, idlist2->GetId(i));
    return;
}

/**************************************************************************************************
 *函数名： LeftButtonPressFunction
 *时间：   2022-09-25 02:14:02
 *用户：   李旺
 *参数：
 *返回值：
 *描述：  鼠标单击 通过obbtree 选中三角面 设置标量来改变颜色
*************************************************************************************************/
void VtkShow::LeftButtonPressFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{
    LeftButtonIsPress = true;
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    auto renderer = vtkInter->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    vtkNew<vtkPoints> intersecPoints;
    bool bRet = getOBBTreeIntersectWithLine(vtkInter,polydata,intersecPoints,intersecCells);

    if (!bRet || intersecCells->GetNumberOfIds() < 0)
    {
        return;
    }
    vtkIdType TriID =intersecCells->GetId(0);

    polydata->GetCellData()->GetScalars()->SetTuple1(TriID, KeyPressFlag);    //改变向量的值
    polydata->GetCellData()->Modified();
    polydata->GetCellData()->GetScalars()->Modified();
    renderer->GetRenderWindow()->Render();
}

void VtkShow::LeftButtonReleaseFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{
    LeftButtonIsPress = false;
}

void VtkShow::MiddleButtonPressFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{


}

void VtkShow::MiddleButtonReleaseFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{

}

void VtkShow::RightButtonPressFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{

}

void VtkShow::RightButtonReleaseFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{

}

void VtkShow::KeyPressFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    char flag = vtkInter->GetKeyCode();
    if ('s' == flag)
    {
        triangleSelectMode = SelectMode::SingleSelect;
        polydataActor->GetProperty()->EdgeVisibilityOn();
        auto vtkRender = vtkInter->GetRenderWindow()->GetRenderers()->GetFirstRenderer();
        vtkRender->RemoveActor(sphereActor);
        return;
    }

    if ('d' == flag)
    {
        triangleSelectMode = SelectMode::MultipleSelect;
        polydataActor->GetProperty()->EdgeVisibilityOff();
        return;
    }
    if (flag - '0' > 9 || flag - '0' < 0)
    {
        return;
    }
    KeyPressFlag = flag - '0';
}

void VtkShow::KeyReleaseFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{

}


//问题：
//1.回调函数应如何定义？
//2.回调函数应如何使用类的成员变量?
//3.回调函数

void VtkShow::MouseMoveFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{
    vtkRenderWindowInteractor *vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    auto renderer = vtkInter->GetRenderWindow()->GetRenderers()->GetFirstRenderer();

    vtkNew<vtkPoints> intersecPoints;  //交互点集合
    vtkNew<vtkIdList> intersecCells;   //相交cell集合
    bool bRet = getOBBTreeIntersectWithLine(vtkInter,polydata,intersecPoints,intersecCells);

    if (!bRet)
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
    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetOpacity(0.2);
    sphereActor->GetProperty()->SetColor(lut->GetTableValue(KeyPressFlag));
    sphereActor->PickableOff();
    renderer->AddActor(sphereActor);
    renderer->GetRenderWindow()->Render();

    if(!LeftButtonIsPress)
    {
        return;
    }

    if (SelectMode::MultipleSelect == triangleSelectMode)
    {
        BFS(position, TriID);
        polydata->GetCellData()->Modified();
        polydata->GetCellData()->GetScalars()->Modified();
        renderer->GetRenderWindow()->Render();
    }
    else if(SelectMode::SingleSelect == triangleSelectMode)
    {
        polydata->GetCellData()->GetScalars()->SetTuple1(TriID, KeyPressFlag);
        polydata->GetCellData()->GetScalars()->Modified();
        renderer->GetRenderWindow()->Render();
    }

}
void VtkShow::MouseWheelForwardFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (vtkInter->GetControlKey())
        MouseSphereRadius += 0.15;
    int* pEvtPos = vtkInter->GetEventPosition();
    auto vtkRender = vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
    vtkInter->SetPicker(vtkCurPicker);
    vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, vtkRender);
    double Position[3];
    vtkCurPicker->GetPickPosition(Position);
    int TriID = vtkCurPicker->GetCellId();
    if (TriID == -1) return;

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetCenter(Position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spheremapper->SetInputConnection(sphere->GetOutputPort());
    sphereActor->SetMapper(spheremapper);
    sphereActor->GetProperty()->SetOpacity(0.2);
    sphereActor->GetProperty()->SetColor(lut->GetTableValue(KeyPressFlag));
    sphereActor->PickableOff();
    vtkRender->AddActor(sphereActor);
    vtkRender->GetRenderWindow()->Render();
}

void VtkShow::MouseWheelBackwardFunction(vtkObject *caller, unsigned long eventId, void *clientData, void *callData)
{

    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (vtkInter->GetControlKey() && MouseSphereRadius > 0.15)
    {
        MouseSphereRadius -= 0.15;
    }

    int* pEvtPos = vtkInter->GetEventPosition();
    auto vtkRender = vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

    vtkNew<vtkCellPicker> vtkCurPicker;
    vtkInter->SetPicker(vtkCurPicker);
    vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, vtkRender);
    double Position[3];
    vtkCurPicker->GetPickPosition(Position);
    int TriID = vtkCurPicker->GetCellId();
    if (TriID == -1) return;

    vtkNew<vtkSphereSource> sphere;
    sphere->SetCenter(Position);
    sphere->SetRadius(MouseSphereRadius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkNew<vtkPolyDataMapper> sphereMapper;
    sphereMapper->SetInputConnection(sphere->GetOutputPort());

    sphereActor->SetMapper(sphereMapper);
    sphereActor->GetProperty()->SetOpacity(0.2);
    sphereActor->GetProperty()->SetColor(lut->GetTableValue(KeyPressFlag));
    sphereActor->PickableOff();
    vtkRender->AddActor(sphereActor);
    vtkRender->GetRenderWindow()->Render();
}

/**************************************************************************************************
 *函数名： getOBBTreeIntersectWithLine
 *时间：   2022-09-25 01:34:41
 *用户：   李旺
 *参数：   const vtkRenderWindowInteractor *vtkInter,  交互器对象
 *        const vtkPolyData *polyData, 与之相交的对象
 *        vtkPoints *intersecPoints    与polydata 相交点集合
 *        vtkIdList *intersecCells     与cell相交的集合
 *返回值： 有相交的cell true  无相交的 false
 *描述：   获取 鼠标当前位置与模型相交的cellIDs的集合（cells可能有多个)
*************************************************************************************************/
bool VtkShow::getOBBTreeIntersectWithLine(vtkRenderWindowInteractor *vtkInter, vtkPolyData *polyData,
                                  vtkPoints *intersecPoints, vtkIdList *intersecCells)
{
    int* pEvtPos      = vtkInter->GetEventPosition();
    auto renderer     = vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    double* pViewDir  = renderer->GetActiveCamera()->GetDirectionOfProjection(); // 当前视角向量

    renderer->SetDisplayPoint(pEvtPos[0], pEvtPos[1], 0);
    renderer->DisplayToWorld();

    //通过OBBTree来做
    double worldPos[3];  //屏幕像素投影到零件的实际位置
    double endPos[3];   // 投影射线终点
    renderer->GetWorldPoint(worldPos);
    for (int i = 0; i < 3;i++)
    {
        endPos[i] = pViewDir[i] * 1000 + worldPos[i];
    }

    vtkNew<vtkOBBTree> obbTree;
    obbTree->SetDataSet(polyData);
    obbTree->BuildLocator();

    int iRet = obbTree->IntersectWithLine(worldPos,endPos,intersecPoints,intersecCells);

    if (0 == iRet || intersecPoints->GetNumberOfPoints() < 0 ||intersecCells->GetNumberOfIds() < 0)
    {
        return false;
    }
    return true;
}





void VtkShow::initCallbackCommand()
{
    LeftButtonPressCallback->SetCallback(LeftButtonPressFunction);
    LeftButtonPressCallback->InitializeObjectBase();
    LeftButtonReleaseCallback->SetCallback(LeftButtonReleaseFunction);
    LeftButtonReleaseCallback->InitializeObjectBase();
    MiddleButtonPressCallback->SetCallback(MiddleButtonPressFunction);
    MiddleButtonPressCallback->InitializeObjectBase();
    MiddleButtonReleaseCallback->SetCallback(MiddleButtonReleaseFunction);
    MiddleButtonReleaseCallback->InitializeObjectBase();
    RightButtonPressCallback->SetCallback(RightButtonPressFunction);
    RightButtonPressCallback->InitializeObjectBase();
    RightButtonReleaseCallback->SetCallback(RightButtonReleaseFunction);
    RightButtonReleaseCallback->InitializeObjectBase();
    KeyPressCallback->SetCallback(KeyPressFunction);
    KeyPressCallback->InitializeObjectBase();
    KeyReleaseCallback->SetCallback(KeyReleaseFunction);
    KeyReleaseCallback->InitializeObjectBase();
    MouseMoveCallback->SetCallback(MouseMoveFunction);
    MouseMoveCallback->InitializeObjectBase();
    MouseWheelForwardCallback->SetCallback(MouseWheelForwardFunction);
    MouseWheelForwardCallback->InitializeObjectBase();
    MouseWheelBackwardCallback->SetCallback(MouseWheelBackwardFunction);
    MouseWheelBackwardCallback->InitializeObjectBase();



}


void DesignInteractorStyle::OnMouseWheelForward()
{
    if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey())
    {
        this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
    }
}

void DesignInteractorStyle::OnMouseWheelBackward()
{
    if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey())
    {
        this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
    }
}
