#ifndef DrawImage_H
#define DrawImage_H

#include <QObject>
#include <QDebug>

//vtk
#include <vtkPolyData.h>
#include <vtkActor.h>
#include <vtkActor2D.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkLookupTable.h>
#include <vtkLookupTable.h>
#include <vtkOBBTree.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>


class DrawImage : public QObject, public vtkInteractorStyleTrackballCamera
{
	Q_OBJECT
public:
	explicit DrawImage(QObject* parent = nullptr);

	static DrawImage* New()
	{
		return new DrawImage;
	}
	vtkTypeMacro(DrawImage, vtkInteractorStyleTrackballCamera);

	virtual ~DrawImage() {}

	void setPolyData(vtkPolyData* newPolyData);
	void setRenderer(vtkRenderer* newRenderer);
	void setPolyDataActor(vtkActor* newPolyDataActor);

signals:

public slots:

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
	//void renderSphere();

private:
	bool m_mouseRightButtonIsPress;
	bool m_bIsPut;
private:
	vtkPolyData* m_polyData;
	vtkActor* m_polyDataActor;
	vtkRenderer* m_renderer;

};

#endif // DrawImage_H

