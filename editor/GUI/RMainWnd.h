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

#ifndef RMAIN_WND_H
#define RMAIN_WND_H

#include <QMainWindow>

class QButtonGroup;

namespace reditor
{

class REditWnd;
class RLogger;
class REditor;

class RMainWnd : public QMainWindow
{
    Q_OBJECT

signals:
    void newProject();
    void openProject();
    void saveProject();
    void saveProjectAs();
    void helpAbout();
    void showGrid(bool show);
    void switchCamera(int mode);
    void objSelected(QString modelName);
    void closeProgram();
    
public:
    explicit RMainWnd(REditor * edit);
    virtual ~RMainWnd();
    /**
     * Enable or disable save action
     */
    void enableSave(bool enable);
    void setGridVisible(bool visible);
    void setCamera(int mode);
    
    void initObjPickers();
    
protected:
    virtual void closeEvent(QCloseEvent * event);
    
private:   
    REditor * medit;
    REditWnd * meditWnd;
    QDockWidget * mloggerDock;
    RLogger * mlogger;
    QDockWidget * mpickerDock;
    QMenu* mviewMenu;
    
    QAction * msaveAction;
    QAction * msaveAsAction;
    QAction * mshowGridAction;
    QButtonGroup * mcameraGroup;
};

} /* namespace reditor */

#endif /* RMAIN_WND_H */