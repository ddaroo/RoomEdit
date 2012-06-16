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

#ifndef REDITOR_H
#define REDITOR_H

#include <QtCore/QObject>
#include <QtCore/QList>

namespace reditor
{

class REditObj;
class REditWnd;
class RObjRoom;
struct RCamera;
class RObjSelection;
/**
 * Implements logic of the editor.
 */
class REditor : public QObject
{
    Q_OBJECT
    
public:
    REditor();
    virtual ~REditor();
    /**
     * Attach this editor to the editor window where all changes will be visualised
     */
    void attachTo(REditWnd * wnd);    
    /**
     * Returns the window this editor is attached to.
     */
    const REditWnd * editWnd() const
    {
        return mwnd;
    }
    /**
     * Adds new objects to the editor window.
     */
    void addObject(REditObj * obj, bool showInExplorer = true);
    /**
     * Removes object from the editor window. Caller is responsible for deleting the object.
     */
    REditObj * removeObject(REditObj * obj);
    const QList<REditObj *> * objects() const
    {
        return &mobjs;
    }
    /**
     * Returns camera properties.
     */
    const RCamera * camera() const
    {
        return mcam;
    }
    
public slots:
    /**
     * Handler for a mouse move event in the editor window
     */
    void hmouseMoved(int x, int y);
    /**
     * Handler for a mouse press event in the editor window
     */
    void hmousePressed(Qt::MouseButton b, int x, int y);
    /**
     * Handler for a mouse release event in the editor window
     */
    void hmouseReleased(Qt::MouseButton b, int x, int y);
    /**
     * Handler for a key press event in the editor window
     */
    void hkeyPressed(int keyCode);
    /**
     * Save scene to the file.
     */
    void hsave();
    /**
     * Load scene from the file
     */
    void hload();
    
private:
    // main wnd of the editor
    REditWnd * mwnd;
    // paintable objects
    QList<REditObj *> mobjs;
    // floor and walls of the room
    RObjRoom * mroom;
    // dynamic room size selection
    RObjSelection * msel;
    // camera properties
    RCamera * mcam;
    // previous mouse position
    int mmouseX, mmouseY;
    
    // start point for the room
    float mbeginCorner[2];
    // end point for the room
    float mendCorner[2]; 
    
    // camera moving resolution
    double const mangle;
    // size of the single cell
    float mcellSize;
    
    // selection of the size of the room or rotation of the camera
    enum EditMode {SELECTION, VIEW, DEFAULT};
    EditMode mmode;
    
    // update selection coordinates, (x, y) - position of the mouse
    void updateSelection(int x, int y, float point[]);
    
public:
    void ladujModele();
    void rysujModel(QString file_name, int tex_num = -1);
};

} /* namespace reditor */

#endif /* REDITOR_H */