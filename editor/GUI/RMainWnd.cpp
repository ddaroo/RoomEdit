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

#include "RMainWnd.h"
#include "REditorWnd.h"
#include "RLogger.h"

namespace reditor
{

RMainWnd::RMainWnd(QWidget* parent, Qt::WindowFlags f) : QMainWindow(parent, f)
{
    meditWnd = new REditorWnd(this);
    setCentralWidget(meditWnd);
    
    mloggerDock = new QDockWidget(this);
    mloggerDock->setWindowTitle(tr("Editor Log"));
    mlogger = new RLogger(mloggerDock);
    mloggerDock->setWidget(mlogger);
    addDockWidget(Qt::BottomDockWidgetArea, mloggerDock);
    // TODO attach dock window with available objects
    // TODO menu
}

RMainWnd::~RMainWnd()
{
}


} /* namespace reditor */