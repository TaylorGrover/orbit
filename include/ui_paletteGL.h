/********************************************************************************
** Form generated from reading UI file 'paletteGL.ui'
**
** Created by: Qt User Interface Compiler version 5.12.8
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef PALETTEGL_H
#define PALETTEGL_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_PaletteGL
{
public:
    QAction *actionSave;
    QAction *actionOpen;
    QWidget *centralwidget;
    QWidget *layoutWidget;
    QVBoxLayout *verticalLayout;
    QTextEdit *textEdit;
    QPushButton *quitButton;
    QMenuBar *menubar;
    QMenu *menuFile;
    QToolBar *mainToolBar;
    QStatusBar *statusbar;

    void setupUi(QMainWindow *PaletteGL)
    {
        if (PaletteGL->objectName().isEmpty())
            PaletteGL->setObjectName(QString::fromUtf8("PaletteGL"));
        PaletteGL->resize(400, 284);
        actionSave = new QAction(PaletteGL);
        actionSave->setObjectName(QString::fromUtf8("actionSave"));
        actionOpen = new QAction(PaletteGL);
        actionOpen->setObjectName(QString::fromUtf8("actionOpen"));
        centralwidget = new QWidget(PaletteGL);
        centralwidget->setObjectName(QString::fromUtf8("centralwidget"));
        layoutWidget = new QWidget(centralwidget);
        layoutWidget->setObjectName(QString::fromUtf8("layoutWidget"));
        layoutWidget->setGeometry(QRect(10, 0, 350, 200));
        verticalLayout = new QVBoxLayout(layoutWidget);
        verticalLayout->setObjectName(QString::fromUtf8("verticalLayout"));
        verticalLayout->setContentsMargins(0, 0, 0, 0);
        textEdit = new QTextEdit(layoutWidget);
        textEdit->setObjectName(QString::fromUtf8("textEdit"));

        verticalLayout->addWidget(textEdit);

        quitButton = new QPushButton(layoutWidget);
        quitButton->setObjectName(QString::fromUtf8("quitButton"));

        verticalLayout->addWidget(quitButton);

        PaletteGL->setCentralWidget(centralwidget);
        menubar = new QMenuBar(PaletteGL);
        menubar->setObjectName(QString::fromUtf8("menubar"));
        menubar->setGeometry(QRect(0, 0, 400, 22));
        menuFile = new QMenu(menubar);
        menuFile->setObjectName(QString::fromUtf8("menuFile"));
        PaletteGL->setMenuBar(menubar);
        mainToolBar = new QToolBar(PaletteGL);
        mainToolBar->setObjectName(QString::fromUtf8("mainToolBar"));
        PaletteGL->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusbar = new QStatusBar(PaletteGL);
        statusbar->setObjectName(QString::fromUtf8("statusbar"));
        PaletteGL->setStatusBar(statusbar);

        menubar->addAction(menuFile->menuAction());

        retranslateUi(PaletteGL);
        QObject::connect(quitButton, SIGNAL(clicked()), PaletteGL, SLOT(close()));

        QMetaObject::connectSlotsByName(PaletteGL);
    } // setupUi

    void retranslateUi(QMainWindow *PaletteGL)
    {
        PaletteGL->setWindowTitle(QApplication::translate("PaletteGL", "MainWindow", nullptr));
        actionSave->setText(QApplication::translate("PaletteGL", "Save", nullptr));
        actionOpen->setText(QApplication::translate("PaletteGL", "Open", nullptr));
        quitButton->setText(QApplication::translate("PaletteGL", "Quit", nullptr));
        menuFile->setTitle(QApplication::translate("PaletteGL", "File", nullptr));
    } // retranslateUi

};

namespace Ui {
    class PaletteGL: public Ui_PaletteGL {};
} // namespace Ui

QT_END_NAMESPACE

#endif // PALETTEGL_H
