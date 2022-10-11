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
#include <vtkXMLPolyDataReader.h>
#include <vtkPLYReader.h>
#include <vtkIdFilter.h>
#include <vtkCellCenters.h>
#include <vtkBillboardTextActor3D.h>
#include <vtkTextProperty.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkActor2D.h>
#include <vtkProperty2D.h>
#include <vtkSelectVisiblePoints.h>
#include <vtkLabeledDataMapper.h>
#include <vtkDataArray.h>
#include <QFileInfo>
#include "UI/colortablewidget.h"
#include <QMessageBox>
#include <vtkDataArray.h>
#include <vtkDecimatePro.h>
#include <vtkOBJReader.h>

VtkShow::VtkShow(QWidget *parent)
    : QWidget{parent}
{
    iniColorTable();
    m_reductionCount = 21000;
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

    //关键一步 设置平行投影 OBBTree 才可以起作用
    m_renderer->GetActiveCamera()->SetParallelProjection(1);

    m_renderWindow = vtkSmartPointer<vtkRenderWindow>::New();
    m_renderWindow->AddRenderer(m_renderer);
    m_renderWindow->SetWindowName("lableTest");

    m_vtkWidget->SetRenderWindow(m_renderWindow);

    //特征边缘
    m_featureEdges = vtkSmartPointer<vtkFeatureEdges>::New();

}

void VtkShow::setReductionCount(int reductionCount)
{
    if (0 == m_reductionCount)
    {
        m_reductionCount = 21000;
    }
    m_reductionCount = reductionCount;
}

/**************************************************************************************************
 *函数名：showVtk
 *时间：   2022-09-12 21:24:49
 *用户：
 *参数：   const QString fileName 打开文件名
 *返回值： 正常0 错误非0
 *描述：显示Vtk 模型主程序
*************************************************************************************************/
int VtkShow::showVtk(const QString fileName)
{
    if (fileName.isEmpty() || !QFileInfo(fileName).isFile())
    {
        return -1;
    }

    m_renderer->RemoveAllViewProps();
    if (0 == fileName.right(4).compare(".stl",Qt::CaseInsensitive))
    {
        openSTLFile(fileName);
    }
    else if(0 == fileName.right(4).compare(".vtp",Qt::CaseInsensitive))
    {
        openVTPFile(fileName);
    }
    else if (0 == fileName.right(4).compare(".ply",Qt::CaseInsensitive))
    {
        openPLYFile(fileName);
    }
    else if (0 == fileName.right(4).compare(".obj",Qt::CaseInsensitive))
    {
        openOBJFile(fileName);
    }
    else
    {
        return -1;
    }

    if (0 == m_polyData->GetNumberOfCells())
    {
        qDebug()<<"文件无效";
        QMessageBox::warning(this,"读取文件","读取文件错误","确定");
        return -1;
    }

    //显示特征边缘操作
    m_featureEdges->BoundaryEdgesOff();
    m_featureEdges->FeatureEdgesOn();
    m_featureEdges->SetFeatureAngle(20);
    m_featureEdges->ManifoldEdgesOff();
    m_featureEdges->NonManifoldEdgesOff();
    m_featureEdges->ColoringOff();
    m_featureEdges->Update();

    vtkNew<vtkNamedColors> vtkColor;

    vtkNew<vtkPolyDataMapper> edgeMapper;
    edgeMapper->SetInputConnection(m_featureEdges->GetOutputPort());

    vtkNew<vtkActor> edgeActor;
    edgeActor->SetMapper(edgeMapper);
    edgeActor->GetProperty()->SetColor(vtkColor->GetColor3d("Red").GetData());
    edgeActor->GetProperty()->SetLineWidth(3.0);
    edgeActor->GetProperty()->SetRenderLinesAsTubes(true);
    edgeActor->PickableOff();
    m_renderer->AddActor(edgeActor);

    vtkNew<vtkPolyDataMapper> polydataMapper;
    polydataMapper->SetInputData(m_polyData);


    polydataMapper->SetScalarRange(0,lut->GetNumberOfTableValues());  //设置颜色表范围
    polydataMapper->SetLookupTable(lut);    //设置颜色表
    polydataMapper->Update();

    polydataActor->GetProperty()->EdgeVisibilityOff();
    polydataActor->SetMapper(polydataMapper);
    m_renderer->AddActor(polydataActor);

    m_renderer->SetBackground(vtkColor->GetColor3d("AliceBlue").GetData());

    m_renderWindow->AddRenderer(m_renderer);

    vtkNew<vtkRenderWindowInteractor> vtkInter;
    m_vtkStyle->setRenderer(m_renderer);
    m_vtkStyle->setPolyData(m_polyData);
    m_vtkStyle->setPolyDataActor(polydataActor);
    m_vtkStyle->setLut(lut);

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
    m_polyData->GetCellData()->GetScalars()->SetName("Label");
    m_polyData->GetCellData()->GetScalars()->Modified();
    vtkNew<vtkXMLPolyDataWriter> writer;
    writer->SetInputData(m_polyData);
    writer->SetFileName(vtpFileName.toLocal8Bit().data());
    return writer->Write();
}

void VtkShow::openSTLFile(QString fileName)
{
    vtkNew<vtkSTLReader> STLReader;
    STLReader->SetFileName(fileName.toLocal8Bit().data()); //修复中文路径无法打开文件的bug
    STLReader->Update();
    m_polyData = STLReader->GetOutput();

    m_featureEdges->SetInputData(m_polyData);

    decimatePro(m_reductionCount);
    iniScalars();
}

void VtkShow::openPLYFile(QString fileName)
{
    vtkNew<vtkPLYReader> plyReader;
    plyReader->SetFileName(fileName.toLocal8Bit().data());
    plyReader->GetOutput()->GetCellData()->SetNumberOfTuples(lut->GetNumberOfTableValues()+1);
    plyReader->GetOutput()->BuildLinks();
    plyReader->Update();
    m_polyData = plyReader->GetOutput();
    m_featureEdges->SetInputData(m_polyData);

    decimatePro(m_reductionCount);
    iniScalars();
}

void VtkShow::openVTPFile(QString fileName)
{
    vtkNew<vtkXMLPolyDataReader> vtpReader;
    vtpReader->SetFileName(fileName.toLocal8Bit().data());
    vtkDataArray * dataArray = vtpReader->GetOutput()->GetCellData()->GetScalars("Label");

    //将原有的颜色显示出来 （数据原始标量设置进去）
    vtpReader->GetOutput()->GetCellData()->SetScalars(dataArray);
    vtpReader->GetOutput()->BuildLinks();
    vtpReader->Update();
    m_polyData = vtpReader->GetOutput();
    m_featureEdges->SetInputData(m_polyData);

}

void VtkShow::openOBJFile(QString fileName)
{
    vtkNew<vtkOBJReader> objReader;
    objReader->SetFileName(fileName.toLocal8Bit().data());
    objReader->GetOutput()->GetCellData()->SetNumberOfTuples(lut->GetNumberOfTableValues()+1);
    objReader->GetOutput()->BuildLinks();
    objReader->Update();
    m_polyData = objReader->GetOutput();
    m_featureEdges->SetInputData(m_polyData);

    decimatePro(m_reductionCount);
    iniScalars();
}

/**************************************************************************************************
 *函数名：iniColorTable
 *时间：   2022-10-05 00:13:13
 *用户：   李旺
 *参数：  无
 *返回值：无
 *描述： 初始化颜色表
*************************************************************************************************/
void VtkShow::iniColorTable()
{
    lut->SetNumberOfTableValues(labelColorList.count()+1);
    double Background[4] = {255/255.0, 162/255.0, 143/255.0,1.0};
    lut->SetTableValue(0, Background);

    int i = 1;
    for(const auto &color : labelColorList)
    {
        int r,g,b;
        color.getRgb(&r,&g,&b);
        lut->SetTableValue(static_cast<vtkIdType>(i),r/255.0,g/255.0,b/255.0);
        i++;
    }
}

/**************************************************************************************************
 *函数名： decimatePro
 *时间：   2022-10-09 19:37:49
 *用户：
 *参数：  const int &triangleCount 最终简化的三角形个数
 *返回值：无
 *描述： 简化三角形
*************************************************************************************************/
void VtkShow::decimatePro(const int &triangleCount)
{
    //简化三角面
    double target = 1.0 - (static_cast<double>(triangleCount) / m_polyData->GetNumberOfCells());
    vtkNew<vtkDecimatePro> deci;
    deci->SetInputData(m_polyData);
    deci->SetTargetReduction(target);
    deci->Update();
    m_polyData = deci->GetOutput();


}

/**************************************************************************************************
 *函数名： iniScalars
 *时间：   2022-10-09 19:45:26
 *用户：   李旺
 *参数：   无
 *返回值： 无
 *描述：    初始化标量数据
*************************************************************************************************/
void VtkShow::iniScalars()
{
    //初始化标量（不初始化，会造成崩溃）
    vtkNew<vtkFloatArray> cellData;
    for (int i = 0; i < m_polyData->GetNumberOfCells(); i++)
        cellData->InsertTuple1(i, 0);
    m_polyData->GetCellData()->SetScalars(cellData);
}

