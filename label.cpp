#include <stdio.h>
#include <tchar.h>
#include <vtkNamedColors.h>
#include <vtkSmartPointer.h>
#include <vtkNew.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballActor.h>
#include <vtkPointData.h>
#include <vtkUnsignedCharArray.h>
#include <map>
#include <vtkXMLPolyDataReader.h>
#include <vtkXMLPolyDataWriter.h>
#include <vtkInformation.h>
#include<vtkPolyDataNormals.h>
#include <vtkXMLGenericDataObjectReader.h>
#include<vtkXMLDataParser.h>
#include <vtkXMLDataElement.h>
#include <vtkXMLReader.h>
#include <vtkCellData.h>
#include<vtkDataArraySelection.h>
#include<vtkInteractorStyleTrackballCamera.h>
#include <algorithm>
#include <vtkCell.h>
#include <vtkTriangle.h>
#include<vtkLineSource.h>
#include<vtkOBBTree.h>
#include <vtkGlyph3DMapper.h>
#include <vtkKochanekSpline.h>
#include <vtkParametricFunctionSource.h>
#include <vtkParametricSpline.h>
#include <vtkSphereSource.h>
#include<vtkPlaneSource.h>
#include<vtkGPUVolumeRayCastMapper.h>
#include<vtkPiecewiseFunction.h>
#include<vtkColorTransferFunction.h>
#include<vtkVolumeProperty.h>
#include<vtkImagePlaneWidget.h>
#include<vtkCellArray.h>
#include<vtkDataArray.h>
#include <vtkCallbackCommand.h>
#include <vtkCellPicker.h>
#include <vtkMapper.h>
#include <vtkSTLReader.h>
#include <vtkCellData.h>
#include <vtkFloatArray.h>
#include <vtkLookupTable.h>
#include<vtkCellPicker.h>
#include <vtkFeatureEdges.h>
#include <vtkDecimatePro.h>
#include <vtkAutoInit.h>
#include <QString>


VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)
VTK_MODULE_INIT(vtkRenderingFreeType)
using namespace std;


vtkNew<vtkCallbackCommand> LeftButtonPressCallback;
vtkNew<vtkCallbackCommand> LeftButtonReleaseCallback;
vtkNew<vtkCallbackCommand> MiddleButtonPressCallback;
vtkNew<vtkCallbackCommand> MiddleButtonReleaseCallback;
vtkNew<vtkCallbackCommand> RightButtonPressCallback;
vtkNew<vtkCallbackCommand> RightButtonReleaseCallback;
vtkNew<vtkCallbackCommand> KeyPressCallback;
vtkNew<vtkCallbackCommand> KeyReleaseCallback;
vtkNew<vtkCallbackCommand> MouseMoveCallback;
vtkNew<vtkCallbackCommand> MouseWheelForwardCallback;
vtkNew<vtkCallbackCommand> MouseWheelBackwardCallback;
vtkNew<vtkRenderWindow> m_vtkRenderWin;
vtkNew<vtkRenderer> m_vtkRender;
vtkNew<vtkLookupTable> lut;
vtkSmartPointer<vtkPolyData>polydata;
//vtkSmartPointer<vtkActor> polydataActor;
vtkNew<vtkActor>polydataActor;
vtkSmartPointer<vtkActor> sphereActor = vtkSmartPointer<vtkActor>::New();
vtkNew<vtkNamedColors> vtkColor;
int PressFlag = 0;//标签
int Press = 0;//点击
int EditMode = 0;//绘制方式
QString m_inputFileName;//{"000.stl"};  //输入文件名称
QString m_outputFileName; //输出文件名称
double Radius = 2.5;

namespace zxk
{
void initCallbackCommand();
void LeftButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void LeftButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void MiddleButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void MiddleButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void RightButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void RightButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void KeyPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void KeyReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void MouseMoveFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void MouseWheelForwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);
void MouseWheelBackwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData);



bool CellInSphere(double*Position,int TriID)
{
    auto pt0 = polydata->GetCell(TriID)->GetPoints()->GetPoint(0);
    auto pt1 = polydata->GetCell(TriID)->GetPoints()->GetPoint(1);
    auto pt2 = polydata->GetCell(TriID)->GetPoints()->GetPoint(2);
    //auto f0 = vtkMath::Distance2BetweenPoints(Position, pt0) < Radius ? 1 : 0;
    if (vtkMath::Distance2BetweenPoints(Position, pt0) < Radius)
        return 1;
    if (vtkMath::Distance2BetweenPoints(Position, pt1) < Radius)
        return 1;
    if (vtkMath::Distance2BetweenPoints(Position, pt2) < Radius)
        return 1;
    //auto f1 = vtkMath::Distance2BetweenPoints(Position, pt1) > Radius ? 0 : 1;
    //auto f2 = vtkMath::Distance2BetweenPoints(Position, pt2) > Radius ? 0 : 1;
    return 0;
}

void BFS(double*Position,int TriID)
{
    if (CellInSphere(Position,TriID)==0) return;
    if (static_cast<int>(polydata->GetCellData()->GetScalars()->GetTuple1(TriID)) == PressFlag) return;
    polydata->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
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

void LeftButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    Press = 1;
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    int* pEvtPos = vtkInter->GetEventPosition();
    vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
    vtkInter->SetPicker(vtkCurPicker);
    vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, m_vtkRender.GetPointer());
    double Position[3];
    vtkCurPicker->GetPickPosition(Position);
    int TriID = vtkCurPicker->GetCellId();
    polydata->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
    polydata->GetCellData()->Modified();
    polydata->GetCellData()->GetScalars()->Modified();
    m_vtkRenderWin->Render();
}

void LeftButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    Press = 0;
}

void MiddleButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
 

}

void MiddleButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
}

void RightButtonPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
}

void RightButtonReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
}

void KeyPressFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    char flag = vtkInter->GetKeyCode();
    if (flag == 's') { EditMode = 1;    polydataActor->GetProperty()->EdgeVisibilityOn(); m_vtkRender->RemoveActor(sphereActor); return; }
    if (flag == 'r') { EditMode = 0;  polydataActor->GetProperty()->EdgeVisibilityOff(); return; }
    if (flag - '0' > 9 || flag - '0' < 0)return;
    PressFlag = flag - '0';
}

void KeyReleaseFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{

}

void MouseMoveFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    if (EditMode == 0) {
        vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
        int* pEvtPos = vtkInter->GetEventPosition();
        vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

        vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
        vtkInter->SetPicker(vtkCurPicker);
        vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, m_vtkRender.GetPointer());
        double Position[3];
        vtkCurPicker->GetPickPosition(Position);
        int TriID = vtkCurPicker->GetCellId();
        if (TriID == -1) return;

        vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
        sphere->SetCenter(Position);
        sphere->SetRadius(Radius);
        sphere->SetPhiResolution(36);
        sphere->SetThetaResolution(36);
        sphere->Update();

        vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
        spheremapper->SetInputConnection(sphere->GetOutputPort());
        sphereActor->SetMapper(spheremapper);
        sphereActor->GetProperty()->SetOpacity(0.2);
        sphereActor->GetProperty()->SetColor(lut->GetTableValue(PressFlag));
        sphereActor->PickableOff();
        m_vtkRender->AddActor(sphereActor);
        m_vtkRenderWin->Render();

        if (Press == 0)return;

        BFS(Position, TriID);
        polydata->GetCellData()->Modified();
        polydata->GetCellData()->GetScalars()->Modified();
        m_vtkRenderWin->Render();
    }
    if (EditMode == 1)
    {
        vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
        int* pEvtPos = vtkInter->GetEventPosition();
        vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);

        vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
        vtkInter->SetPicker(vtkCurPicker);
        vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, m_vtkRender.GetPointer());
        int TriID = vtkCurPicker->GetCellId();
        if (TriID == -1) return;
        if (Press == 0)return;
        polydata->GetCellData()->GetScalars()->SetTuple1(TriID, PressFlag);
        polydata->GetCellData()->GetScalars()->Modified();
        m_vtkRenderWin->Render();
    }
}

void MouseWheelForwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (vtkInter->GetControlKey())
        Radius += 0.15;
    int* pEvtPos = vtkInter->GetEventPosition();
    vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
    vtkInter->SetPicker(vtkCurPicker);
    vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, m_vtkRender.GetPointer());
    double Position[3];
    vtkCurPicker->GetPickPosition(Position);
    int TriID = vtkCurPicker->GetCellId();
    if (TriID == -1) return;

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetCenter(Position);
    sphere->SetRadius(Radius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spheremapper->SetInputConnection(sphere->GetOutputPort());
    sphereActor->SetMapper(spheremapper);
    sphereActor->GetProperty()->SetOpacity(0.2);
    sphereActor->GetProperty()->SetColor(lut->GetTableValue(PressFlag));
    sphereActor->PickableOff();
    m_vtkRender->AddActor(sphereActor);
    m_vtkRenderWin->Render();
}

void MouseWheelBackwardFunction(vtkObject* caller, long unsigned int eventId, void* clientData, void* callData)
{
    vtkRenderWindowInteractor* vtkInter = vtkRenderWindowInteractor::SafeDownCast(caller);
    if (vtkInter->GetControlKey())
        if(Radius>0.15)
            Radius -= 0.15;
    int* pEvtPos = vtkInter->GetEventPosition();
    vtkInter->FindPokedRenderer(pEvtPos[0], pEvtPos[1]);
    vtkSmartPointer<vtkCellPicker> vtkCurPicker = vtkSmartPointer<vtkCellPicker>::New();
    vtkInter->SetPicker(vtkCurPicker);
    vtkInter->GetPicker()->Pick(pEvtPos[0], pEvtPos[1], 0, m_vtkRender.GetPointer());
    double Position[3];
    vtkCurPicker->GetPickPosition(Position);
    int TriID = vtkCurPicker->GetCellId();
    if (TriID == -1) return;

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    sphere->SetCenter(Position);
    sphere->SetRadius(Radius);
    sphere->SetPhiResolution(36);
    sphere->SetThetaResolution(36);
    sphere->Update();

    vtkSmartPointer<vtkPolyDataMapper> spheremapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    spheremapper->SetInputConnection(sphere->GetOutputPort());
    sphereActor->SetMapper(spheremapper);
    sphereActor->GetProperty()->SetOpacity(0.2);
    sphereActor->GetProperty()->SetColor(lut->GetTableValue(PressFlag));
    sphereActor->PickableOff();
    m_vtkRender->AddActor(sphereActor);
    m_vtkRenderWin->Render();
}

}
void saveVTP()
{
    //导出文件
    polydata->GetCellData()->GetScalars()->SetName("Label");
    polydata->GetCellData()->GetScalars()->Modified();
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputData(polydata);
    writer->SetFileName(m_outputFileName.toLocal8Bit().data());
    writer->Write();
}
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

    virtual void OnLeftButtonDown() {}
    virtual void OnLeftButtonUp() {}
    virtual void OnRightButtonDown() { this->StartRotate(); } // 避免vtk的GrabFocus接口占用交互命令
    virtual void OnRightButtonUp() { this->vtkInteractorStyleTrackballCamera::OnLeftButtonUp(); }
    virtual void OnMouseMove() { this->vtkInteractorStyleTrackballCamera::OnMouseMove(); }
    virtual void OnMouseWheelForward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelForward(); }
    virtual void OnMouseWheelBackward() { if (!this->Interactor->GetControlKey() && !this->Interactor->GetShiftKey()) this->vtkInteractorStyleTrackballCamera::OnMouseWheelBackward(); }
};

//int _tmain(int argc, _TCHAR* argv[])
int showVTK()
{
    if (m_inputFileName.isEmpty())
    {
        return -1;
    }

    m_vtkRender->RemoveAllViewProps();
//    vtkSmartPointer<vtkSTLReader> STLReader= vtkSmartPointer<vtkSTLReader>::New();
    vtkNew<vtkSTLReader> STLReader;
    STLReader->SetFileName(m_inputFileName.toLocal8Bit().data()); //修复中文路径无法打开文件的bug
    STLReader->Update();
    polydata = STLReader->GetOutput();

    vtkNew<vtkFeatureEdges> featureEdges;
    featureEdges->SetInputData(polydata);
    featureEdges->BoundaryEdgesOff();
    featureEdges->FeatureEdgesOn();
    featureEdges->SetFeatureAngle(20);
    featureEdges->ManifoldEdgesOff();
    featureEdges->NonManifoldEdgesOff();
    featureEdges->ColoringOff();
    featureEdges->Update();


    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(featureEdges->GetOutputPort());
    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(vtkColor->GetColor3d("Red").GetData());
    edgeActor->GetProperty()->SetLineWidth(3.0);
    edgeActor->GetProperty()->SetRenderLinesAsTubes(0.5);
    edgeActor->PickableOff();
    m_vtkRender->AddActor(edgeActor);

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

//    vtkSmartPointer<vtkPolyDataMapper> polydataMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vtkNew<vtkPolyDataMapper> polydataMapper;
    polydataMapper->SetInputData(polydata);
    polydataMapper->SetScalarRange(0, 19);
    polydataMapper->SetLookupTable(lut);
    polydataMapper->Update();

//    polydataActor = vtkSmartPointer<vtkActor>::New();
    polydataActor->GetProperty()->EdgeVisibilityOff();
    polydataActor->SetMapper(polydataMapper);
    polydataActor->GetProperty()->SetOpacity(1);
    m_vtkRender->AddActor(polydataActor);

    m_vtkRender->SetBackground(vtkColor->GetColor3d("AliceBlue").GetData());

    m_vtkRenderWin->AddRenderer(m_vtkRender);

    m_vtkRenderWin->Render();
    vtkNew<vtkRenderWindowInteractor> vtkInter;
    m_vtkRenderWin->SetSize(1920, 1080);
//    vtkSmartPointer<DesignInteractorStyle> vtkStyle = vtkSmartPointer<DesignInteractorStyle>::New();
    vtkNew<DesignInteractorStyle> vtkStyle;
    vtkInter->SetInteractorStyle(vtkStyle);
    vtkInter->SetRenderWindow(m_vtkRenderWin);

    zxk::initCallbackCommand();


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
//    vtkInter->Start();
    m_vtkRenderWin->Render();

    m_vtkRender->ResetCamera();
    return EXIT_SUCCESS;
}

namespace zxk{

void initCallbackCommand()
{
    LeftButtonPressCallback->SetCallback(zxk::LeftButtonPressFunction);
    LeftButtonPressCallback->InitializeObjectBase();
    LeftButtonReleaseCallback->SetCallback(zxk::LeftButtonReleaseFunction);
    LeftButtonReleaseCallback->InitializeObjectBase();
    MiddleButtonPressCallback->SetCallback(zxk::MiddleButtonPressFunction);
    MiddleButtonPressCallback->InitializeObjectBase();
    MiddleButtonReleaseCallback->SetCallback(zxk::MiddleButtonReleaseFunction);
    MiddleButtonReleaseCallback->InitializeObjectBase();
    RightButtonPressCallback->SetCallback(zxk::RightButtonPressFunction);
    RightButtonPressCallback->InitializeObjectBase();
    RightButtonReleaseCallback->SetCallback(zxk::RightButtonReleaseFunction);
    RightButtonReleaseCallback->InitializeObjectBase();
    KeyPressCallback->SetCallback(zxk::KeyPressFunction);
    KeyPressCallback->InitializeObjectBase();
    KeyReleaseCallback->SetCallback(zxk::KeyReleaseFunction);
    KeyReleaseCallback->InitializeObjectBase();
    MouseMoveCallback->SetCallback(zxk::MouseMoveFunction);
    MouseMoveCallback->InitializeObjectBase();
    MouseWheelForwardCallback->SetCallback(zxk::MouseWheelForwardFunction);
    MouseWheelForwardCallback->InitializeObjectBase();
    MouseWheelBackwardCallback->SetCallback(zxk::MouseWheelBackwardFunction);
    MouseWheelBackwardCallback->InitializeObjectBase();
}
}
