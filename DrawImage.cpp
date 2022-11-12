#include "DrawImage.h"
#include <vtkCamera.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkStripper.h>
#include <vtkTubeFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <moc_mainwindow.cpp>

DrawImage::DrawImage(QObject* parent)
    : QObject{ parent }
{
    m_mouseRightButtonIsPress = false;
    m_bIsPut = false;
}

void DrawImage::setPolyData(vtkPolyData* newPolyData)
{
    m_polyData = newPolyData;
}

void DrawImage::setRenderer(vtkRenderer* newRenderer)
{
    m_renderer = newRenderer;

    //关键一步 平行投影 obbTree才可以正常使用
    m_renderer->GetActiveCamera()->SetParallelProjection(1);
}

void DrawImage::setPolyDataActor(vtkActor* newPolyDataActor)
{
    m_polyDataActor = newPolyDataActor;

}

void DrawImage::OnLeftButtonDown()
{
    if (m_mouseRightButtonIsPress)
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonDown();
        return;
    }
}
void DrawImage::OnLeftButtonUp()
{
    if (m_mouseRightButtonIsPress)
    {
        vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
        return;
    }
    //renderSphere();

}

void DrawImage::OnRightButtonDown()
{
    m_mouseRightButtonIsPress = true;
    vtkInteractorStyleTrackballCamera::OnLeftButtonDown();

}

void DrawImage::OnRightButtonUp()
{
    m_mouseRightButtonIsPress = false;
    vtkInteractorStyleTrackballCamera::OnLeftButtonUp();
}

void DrawImage::OnMiddleButtonDown()
{
    vtkInteractorStyleTrackballCamera::OnMiddleButtonDown();
}

void DrawImage::OnMiddleButtonUp()
{
    vtkInteractorStyleTrackballCamera::OnMiddleButtonUp();
}

void DrawImage::OnMouseMove()
{

    //renderSphere();

    vtkInteractorStyleTrackballCamera::OnMouseMove();
}

void DrawImage::OnMouseWheelForward()
{
    vtkInteractorStyleTrackballCamera::OnMouseWheelForward();
}

void DrawImage::OnMouseWheelBackward()
{
    vtkInteractorStyleTrackballCamera::OnMouseWheelBackward();
}

void DrawImage::OnKeyPress()
{
    const char ch = this->Interactor->GetKeyCode();
    if ('m' == ch)
    {
        m_bIsPut = true;
    }
}

void DrawImage::OnKeyRelease()
{
    const char ch = this->Interactor->GetKeyCode();
    if ('m' == ch)
    {
        m_bIsPut = false;
    }
}

void DrawImage::OnChar()
{
}

#if 0
void DrawImage::renderSphere()
{
    vtkNew<vtkNamedColors> colors;

    vtkSmartPointer<vtkSphereSource> sphere = vtkSmartPointer<vtkSphereSource>::New();
    vtkNew<vtkPoints> points;
    points->InsertPoint(0, 0.01, 0.0, 0.0);
    points->InsertPoint(1, 1.5, 0.0, 0.0);
    points->InsertPoint(2, 1.5, 0.0, 3.5);
    points->InsertPoint(3, 1.25, 0.0, 3.75);
    points->InsertPoint(4, 0.75, 0.0, 4.00);
    points->InsertPoint(5, 0.6, 0.0, 4.35);
    points->InsertPoint(6, 0.7, 0.0, 4.65);
    points->InsertPoint(7, 1.0, 0.0, 4.75);
    points->InsertPoint(8, 1.0, 0.0, 5.0);
    points->InsertPoint(9, 0.2, 0.0, 5.0);

    vtkNew<vtkCellArray> lines;
    lines->InsertNextCell(10); // number of points
    lines->InsertCellPoint(0);
    lines->InsertCellPoint(1);
    lines->InsertCellPoint(2);
    lines->InsertCellPoint(3);
    lines->InsertCellPoint(4);
    lines->InsertCellPoint(5);
    lines->InsertCellPoint(6);
    lines->InsertCellPoint(7);
    lines->InsertCellPoint(8);
    lines->InsertCellPoint(9);

    vtkNew<vtkPolyData> profile;
    profile->SetPoints(points);
    profile->SetLines(lines);

    vtkNew<vtkStripper> stripper;
    stripper->SetInputData(profile);

    //在线路上生成 管道
    vtkNew<vtkTubeFilter> tubes;
    tubes->SetInputConnection(stripper->GetOutputPort());
    tubes->SetNumberOfSides(11);
    tubes->SetRadius(0.05);

    vtkNew<vtkPolyDataMapper> profileMapper;
    profileMapper->SetInputConnection(tubes->GetOutputPort());

    vtkNew<vtkActor> profileActor;
    profileActor->SetMapper(profileMapper);
    profileActor->GetProperty()->SetColor(colors->GetColor3d("Tomato").GetData());
    m_renderer->AddActor(profileActor);

}
#endif

