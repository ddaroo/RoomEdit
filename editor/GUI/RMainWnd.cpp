/*
Copyright (C) 2012 RoomEdit Development Team

This file is part of RoomEdit.

RoomEdit is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

RoomEdit is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with RoomEdit. If not, see <http://www.gnu.org/licenses/>.
*/

#include <QtCore/QDebug>
#include <QtGui/QDockWidget>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>
#include <QtGui/QButtonGroup>
#include <QtGui/QPushButton>
#include <QtGui/QCloseEvent>
#include <QtGui/QGridLayout>
#include <QtGui/QLayout>
#include <QtGui/QGroupBox>

#include "RMainWnd.h"
#include "REditWnd.h"
#include "RLogger.h"
#include "REditor.h"
#include "global.h"
#include "RResourceDB.h"
#include "RSceneObjPicker.h"

namespace reditor
{
  
RMainWnd::RMainWnd(reditor::REditor* edit) : QMainWindow(), medit(edit)
{
    setWindowTitle(tr("Room Editor"));
    setWindowIcon(QIcon(":/resources/favicon.ico"));
    
    // central widget
    meditWnd = new REditWnd(medit->objects(), medit->camera(), this);
    setCentralWidget(meditWnd);
    
    // set corners for docks
    setCorner(Qt::TopRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
    setCorner(Qt::TopLeftCorner, Qt::LeftDockWidgetArea);
    setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    
    // docks
    mloggerDock = new QDockWidget(this);
    mloggerDock->setWindowTitle(tr("Editor Log"));
    mlogger = new RLogger(mloggerDock);
    mloggerDock->setWidget(mlogger);
    addDockWidget(Qt::BottomDockWidgetArea, mloggerDock);
    // TODO attach dock window with objects on the scene
    
    // File menu
    QMenu* fileMenu = menuBar()->addMenu(tr("&File"));
    QAction* newAction = fileMenu->addAction(tr("&New Project"), this, SIGNAL(newProject()), QKeySequence::New);
    newAction->setIcon(QIcon(":/resources/new.png"));
    QAction* openAction = fileMenu->addAction(tr("&Open Project.."), this, SIGNAL(openProject()), QKeySequence::Open);
    openAction->setIcon(QIcon(":/resources/open.png"));
    fileMenu->addSeparator();
    msaveAction = fileMenu->addAction(tr("&Save Project"), this, SIGNAL(saveProject()), QKeySequence::Save);
    msaveAction->setIcon(QIcon(":/resources/save.png"));
    msaveAsAction = fileMenu->addAction(tr("S&ave Project As..."), this, SIGNAL(saveProjectAs()), Qt::SHIFT + Qt::CTRL + Qt::Key_S);
    msaveAsAction->setIcon(QIcon(":/resources/saveAs.png"));
    enableSave(false); // activate after user makes some changes, no sense to save empty project
    fileMenu->addSeparator();
    fileMenu->addAction(tr("&Exit.."), this, SLOT(close()), QKeySequence::Quit);
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(tr("&About RoomEdit"), this, SIGNAL(helpAbout()));
    
    // File toolbar
    QToolBar* fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addSeparator();
    fileToolBar->addAction(msaveAction);
    fileToolBar->addAction(msaveAsAction);
    
    // Camera view toolbar
    QToolBar* cameraToolBar = addToolBar(tr("Camera"));
    mcameraGroup = new QButtonGroup(cameraToolBar);
    QPushButton * cam1 = new QPushButton(QString("1"), this);
    QPushButton * cam2 = new QPushButton(QString("2"), this);
    QPushButton * cam3 = new QPushButton(QString("3"), this);
    cam1->setCheckable(true);
    cam2->setCheckable(true);
    cam3->setCheckable(true);
    cam1->setChecked(true);
    mcameraGroup->addButton(cam1, 1);
    mcameraGroup->addButton(cam2, 2);
    mcameraGroup->addButton(cam3, 3);
    mcameraGroup->setExclusive(true);
    connect(mcameraGroup, SIGNAL(buttonClicked(int)), this, SIGNAL(switchCamera(int)));
    cameraToolBar->addWidget(cam1);
    cameraToolBar->addWidget(cam2);
    cameraToolBar->addWidget(cam3);
}

RMainWnd::~RMainWnd()
{
}

void RMainWnd::enableSave(bool enable)
{
    msaveAction->setEnabled(enable);
}

void RMainWnd::setGridVisible(bool visible)
{
    mshowGridAction->setChecked(visible);
}

void RMainWnd::setCamera(int mode)
{
    mcameraGroup->button(mode)->setChecked(true);
}

void RMainWnd::closeEvent(QCloseEvent* event)
{
    event->ignore();
    emit closeProgram();
}

void RMainWnd::initObjPickers()
{
    RResourceDB::Mods mods = rscDB.models();
    RSceneObjPicker * picker;
    
    mpickerDock = new QDockWidget(this);
    mpickerDock->setWindowTitle("Available Objects");
    
    QGridLayout * l = new QGridLayout;
    QGroupBox * g = new QGroupBox(mpickerDock);
    
    // attach models
    int i = 0;
    int k = 0;
    for(RResourceDB::ModsCit cit = mods.begin(); cit != mods.end(); ++cit)
    {
        picker = new RSceneObjPicker(cit.key(), mpickerDock);
        picker->setFixedSize(100, 100);
        connect(picker, SIGNAL(selected(QString)), this, SIGNAL(objSelected(QString)));
        l->addWidget(picker, k, i%2);
        i++;
        if(i%2 == 0)
        {
            ++k;
        }
    }
    
    l->setRowStretch(i, 1000);
    g->setLayout(l);
    
    mpickerDock->setWidget(g);
    mpickerDock->setMinimumWidth(250);
    addDockWidget(Qt::LeftDockWidgetArea, mpickerDock);
    
    
    // View menu
    mviewMenu = menuBar()->addMenu(tr("&View"));
    mshowGridAction = mviewMenu->addAction(tr("&Show grid"));
    mshowGridAction->setCheckable(true);
    mshowGridAction->setChecked(true);
    connect(mshowGridAction, SIGNAL(toggled(bool)), this, SIGNAL(showGrid(bool)));
    // view menu - dockable windows
    QMenu * docksMenu = mviewMenu->addMenu(tr("&Dockable Windows"));
    QAction * editorLogAction = docksMenu->addAction(tr("&Editor Log"));
    editorLogAction->setCheckable(true);
    editorLogAction->setChecked(true);
    connect(editorLogAction, SIGNAL(toggled(bool)), mloggerDock, SLOT(setVisible(bool)));
    connect(mloggerDock, SIGNAL(visibilityChanged(bool)), editorLogAction, SLOT(setChecked(bool)));
    // --
    QAction * pickerAction = docksMenu->addAction(tr("&Available Objects"));
    pickerAction->setCheckable(true);
    pickerAction->setChecked(true);
    connect(pickerAction, SIGNAL(toggled(bool)), mpickerDock, SLOT(setVisible(bool)));
    connect(mpickerDock, SIGNAL(visibilityChanged(bool)), pickerAction, SLOT(setChecked(bool)));
}

} /* namespace reditor */