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
    m_vtkStyle->setRenderer(m_renderer);
    m_vtkStyle->setPolyData(polydata);
    m_vtkStyle->setPolyDataActor(polydataActor);

    vtkInter->SetInteractorStyle(m_vtkStyle);
    vtkInter->SetRenderWindow(m_renderWindow);

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

