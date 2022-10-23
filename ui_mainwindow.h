/********************************************************************************
** Form generated from reading UI file 'mainwindow.ui'
**
** Created by: Qt User Interface Compiler version 5.15.2
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_MAINWINDOW_H
#define UI_MAINWINDOW_H

#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>
#include "QVTKWidget.h"
#include "UI/colortablewidget.h"
#include "UI/mytablewidget.h"

QT_BEGIN_NAMESPACE

class Ui_MainWindow
{
public:
    QWidget *centralwidget;
    QVBoxLayout *verticalLayout_3;
    QHBoxLayout *horizontalLayout;
    QVBoxLayout *verticalLayout;
    QPushButton *inputFile_btn;
    QPushButton *outPut_btn;
    ColorTableWidget *colorTabel;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label;
    QLineEdit *reductionCount_lineEdit;
    QLabel *label_2;
    QLabel *fileName_label;
    QSpacerItem *horizontalSpacer;
    QHBoxLayout *horizontalLayout_2;
    MyTableWidget *tableWidget;
    QVTKWidget *qvtkWidget;
    QLabel *label_3;
    QLabel *label_4;
    QMenuBar *menubar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName(QString::fromUtf8("MainWindow"));
        MainWindow->resize(1270, 786);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        verticalLayout_3 = new QVBoxLayout(centralwidget);
        verticalLayout_3->setObjectName(QString::fromUtf8("verticalLayout_3"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QString::fromUtf8("horizontalLayout"));
        verticalLayout = new QVBoxLayout();
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        inputFile_btn = new QPushButton(centralwidget);
        inputFile_btn->setObjectName(QString::fromUtf8("inputFile_btn"));
        inputFile_btn->setMinimumSize(QSize(0, 50));
        QFont font;
        font.setPointSize(14);
        inputFile_btn->setFont(font);

        verticalLayout->addWidget(inputFile_btn);

        outPut_btn = new QPushButton(centralwidget);
        outPut_btn->setObjectName(QString::fromUtf8("outPut_btn"));
        outPut_btn->setMinimumSize(QSize(0, 50));
        outPut_btn->setFont(font);

        verticalLayout->addWidget(outPut_btn);


        horizontalLayout->addLayout(verticalLayout);

        colorTabel = new ColorTableWidget(centralwidget);
        colorTabel->setObjectName(QString::fromUtf8("colorTabel"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Ignored);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(colorTabel->sizePolicy().hasHeightForWidth());
        colorTabel->setSizePolicy(sizePolicy);
        colorTabel->setMinimumSize(QSize(0, 120));

        horizontalLayout->addWidget(colorTabel);


        verticalLayout_3->addLayout(horizontalLayout);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setObjectName(QString::fromUtf8("horizontalLayout_3"));
        label = new QLabel(centralwidget);
        label->setObjectName(QString::fromUtf8("label"));
        QSizePolicy sizePolicy1(QSizePolicy::Minimum, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(0);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(label->sizePolicy().hasHeightForWidth());
        label->setSizePolicy(sizePolicy1);
        label->setMinimumSize(QSize(60, 0));
        label->setFont(font);

        horizontalLayout_3->addWidget(label);

        reductionCount_lineEdit = new QLineEdit(centralwidget);
        reductionCount_lineEdit->setObjectName(QString::fromUtf8("reductionCount_lineEdit"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Fixed);
        sizePolicy2.setHorizontalStretch(0);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(reductionCount_lineEdit->sizePolicy().hasHeightForWidth());
        reductionCount_lineEdit->setSizePolicy(sizePolicy2);
        reductionCount_lineEdit->setMinimumSize(QSize(30, 0));
        reductionCount_lineEdit->setMaximumSize(QSize(86, 16777215));
        reductionCount_lineEdit->setFont(font);
        reductionCount_lineEdit->setFrame(true);
        reductionCount_lineEdit->setCursorPosition(5);
        reductionCount_lineEdit->setClearButtonEnabled(false);

        horizontalLayout_3->addWidget(reductionCount_lineEdit);

        label_2 = new QLabel(centralwidget);
        label_2->setObjectName(QString::fromUtf8("label_2"));
        QSizePolicy sizePolicy3(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy3.setHorizontalStretch(0);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(label_2->sizePolicy().hasHeightForWidth());
        label_2->setSizePolicy(sizePolicy3);
        label_2->setFont(font);

        horizontalLayout_3->addWidget(label_2);

        fileName_label = new QLabel(centralwidget);
        fileName_label->setObjectName(QString::fromUtf8("fileName_label"));
        fileName_label->setFont(font);

        horizontalLayout_3->addWidget(fileName_label);

        horizontalSpacer = new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

        horizontalLayout_3->addItem(horizontalSpacer);


        verticalLayout_3->addLayout(horizontalLayout_3);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QString::fromUtf8("horizontalLayout_2"));
        tableWidget = new MyTableWidget(centralwidget);
        if (tableWidget->columnCount() < 2)
            tableWidget->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget->setObjectName(QString::fromUtf8("tableWidget"));
        QSizePolicy sizePolicy4(QSizePolicy::Preferred, QSizePolicy::Expanding);
        sizePolicy4.setHorizontalStretch(0);
        sizePolicy4.setVerticalStretch(0);
        sizePolicy4.setHeightForWidth(tableWidget->sizePolicy().hasHeightForWidth());
        tableWidget->setSizePolicy(sizePolicy4);
        tableWidget->setMinimumSize(QSize(100, 20));

        horizontalLayout_2->addWidget(tableWidget);

        qvtkWidget = new QVTKWidget(centralwidget);
        qvtkWidget->setObjectName(QString::fromUtf8("qvtkWidget"));
        QSizePolicy sizePolicy5(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy5.setHorizontalStretch(0);
        sizePolicy5.setVerticalStretch(0);
        sizePolicy5.setHeightForWidth(qvtkWidget->sizePolicy().hasHeightForWidth());
        qvtkWidget->setSizePolicy(sizePolicy5);

        horizontalLayout_2->addWidget(qvtkWidget);


        verticalLayout_3->addLayout(horizontalLayout_2);

        label_3 = new QLabel(centralwidget);
        label_3->setObjectName(QString::fromUtf8("label_3"));
        QFont font1;
        font1.setPointSize(10);
        label_3->setFont(font1);

        verticalLayout_3->addWidget(label_3);

        label_4 = new QLabel(centralwidget);
        label_4->setObjectName(QString::fromUtf8("label_4"));
        label_4->setFont(font1);

        verticalLayout_3->addWidget(label_4);

        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 1270, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        inputFile_btn->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\205\245\346\226\207\344\273\266", nullptr));
        outPut_btn->setText(QCoreApplication::translate("MainWindow", "\345\257\274\345\207\272\346\226\207\344\273\266", nullptr));
        label->setText(QCoreApplication::translate("MainWindow", "\347\256\200\345\214\226\344\270\252\346\225\260", nullptr));
        reductionCount_lineEdit->setText(QCoreApplication::translate("MainWindow", "21000", nullptr));
        label_2->setText(QCoreApplication::translate("MainWindow", "\345\275\223\345\211\215\350\275\275\345\205\245\347\232\204\346\226\207\344\273\266\357\274\232", nullptr));
        fileName_label->setText(QCoreApplication::translate("MainWindow", "TextLabel", nullptr));
        QTableWidgetItem *___qtablewidgetitem = tableWidget->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\346\226\207\344\273\266\345\220\215\347\247\260", nullptr));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QCoreApplication::translate("MainWindow", "\344\277\235\345\255\230\347\212\266\346\200\201", nullptr));
        label_3->setText(QCoreApplication::translate("MainWindow", "\345\277\253\346\215\267\351\224\256\357\274\232 \345\215\225\347\213\254\351\200\211\346\213\251: \"S\"   \350\277\236\347\273\255\351\200\211\346\213\251\344\270\211\350\247\222\351\235\242\347\211\207: \"D\"   \345\244\215\344\275\215\350\247\206\350\247\222: \"R\"    \351\200\211\346\213\251\346\240\207\346\263\250\351\242\234\350\211\262: \"1-9\"   \346\270\205\351\231\244\351\200\211\346\213\251 \"\346\214\211\344\270\213shift\"    \350\256\276\347\275\256\351\200\211\346\213\251\347\220\203\345\244\247\345\260\217:\"\346\214\211\344\270\213Ctrl+\346\273\232\350\275\256\"     Ctrl + Shift + \346\273\232\350\275\256 :\342\200\234\345\210\207\346\215\242\346\240\207\347\255\276\351\242\234\350\211\262\"", nullptr));
        label_4->setText(QCoreApplication::translate("MainWindow", "\345\277\253\346\215\267\351\224\256\357\274\232 \346\237\245\347\234\213\347\202\271ID \345\222\214\344\270\211\350\247\222\345\275\242ID : \"F\"\357\274\233   \344\275\277\347\224\250\345\217\257\350\247\201\347\202\271\346\235\245\351\200\211\346\213\251\344\270\211\350\247\222\351\235\242: \"G\"\357\274\233", nullptr));
    } // retranslateUi

};

namespace Ui {
    class MainWindow: public Ui_MainWindow {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_MAINWINDOW_H
