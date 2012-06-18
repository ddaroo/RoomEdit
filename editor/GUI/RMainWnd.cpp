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

#include <QtGui/QDockWidget>
#include <QtGui/QAction>
#include <QtGui/QMenuBar>
#include <QtGui/QToolBar>

#include "RMainWnd.h"
#include "REditWnd.h"
#include "RLogger.h"
#include "REditor.h"

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
    // TODO attach dock window with available objects
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
    
    // View menu
    QMenu* viewMenu = menuBar()->addMenu(tr("&View"));
    QAction* showGridAction = viewMenu->addAction(tr("&Show grid"));
    // TODO implementation
    // - dock widgets visibility
    // - grid visibility
    
    // Help menu
    QMenu* helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction* aboutAction = helpMenu->addAction(tr("&About RoomEdit"), this, SIGNAL(helpAbout()));
    
    // toolbar
    QToolBar* fileToolBar = addToolBar(tr("File"));
    fileToolBar->addAction(newAction);
    fileToolBar->addAction(openAction);
    fileToolBar->addSeparator();
    fileToolBar->addAction(msaveAction);
    fileToolBar->addAction(msaveAsAction);
}

RMainWnd::~RMainWnd()
{
}

void RMainWnd::enableSave(bool enable)
{
    msaveAction->setEnabled(enable);
}

} /* namespace reditor */