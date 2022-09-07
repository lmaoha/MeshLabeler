#include "mainwindow.h"

#include <QApplication>
#include <vtkOutputWindow.h>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    vtkOutputWindow::SetGlobalWarningDisplay(0);
    MainWindow w;
    w.show();
    return a.exec();
}
