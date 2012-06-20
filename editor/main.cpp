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
#include <QtGui/QApplication>

#include "global.h"
#include "REditor.h"
#include "RResourceDB.h"
#include "RConfig.h"
#include "GUI/RMainWnd.h"
#include "GUI/REditWnd.h"
#include "GUI/RLogger.h"

reditor::RResourceDB rscDB;

int main(int argc, char *argv[])
{
    qInstallMsgHandler(msgOutput);
    
    QApplication app(argc, argv); 
   
    reditor::REditor * edit = new reditor::REditor();  
    reditor::RMainWnd * mwnd = new reditor::RMainWnd(edit);     
    edit->attachTo( static_cast<reditor::REditWnd *>(mwnd->centralWidget()) );
    edit->attachToMainWnd(mwnd); // TODO create reditor manager class and implement all slots there to make possible multiple editor windows
    mwnd->resize(800, 600);
    mwnd->show();
    // can't be moved earlier because it requires initiated OpenGL engine to load textures to GPU memory
    rscDB.load(reditor::RConfig::texturesDir(""), reditor::RConfig::modelsDir(""));
    mwnd->initObjPickers();
    
    return app.exec();
}
