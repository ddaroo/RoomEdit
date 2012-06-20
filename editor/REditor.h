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
class RSceneObj;
class RMainWnd;
class RGrid;

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
     * Attach signals from the main window to editor's slots
     */
    void attachToMainWnd(RMainWnd * wnd);
    /**
     * Returns the window this editor is attached to.
     */
    const REditWnd * editWnd() const
    {
        return meditorWnd;
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
    void hsaveProject();
    /**
     * "Save as..." scene to the file.
     */
    void hsaveProjectAs();
    /**
     * Load scene from the file
     */
    void hopenProject();
    /**
     * New scene
     */
    void hnewProject();
    /**
     * Ask user to save edited scene and close the program.
     */
    void hcloseProgram();
    void hswitchCamera(int mode);
    // TODO connect to menu signal
    void hshowGrid(bool show);
    void hhelpAbout();
    void hobjSelected(QString modelName);
    
private:
    // main wnd of the editor
    REditWnd * meditorWnd;
    // main wnd of the program
    RMainWnd * mmainWnd;
    // paintable objects
    QList<REditObj *> mobjs;
    
    RGrid * mgrid;
    bool mgridVisible;
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
    // current positon of the mouse
    float mcurPos[2];

    // rotation of element
    int mrotation;

    bool mroomDimensionsPicked;
    // currently selected object
    RSceneObj * mactiveObject;
    
    // camera moving resolution
    double const mangle;
    // size of the single cell
    float mcellSize;
    
    // selection of the size of the room or rotation of the camera
    enum EditMode {SELECTION, VIEW, OBJECTS, DEFAULT};
    EditMode mmode, mprevMode;
    
    // current path to the project file
    QString mdefPath;
    // path to the currently opened project
    QString mopenedProject;
    // true when all changes are saved to the file
    bool msavedProject;
    
    // update selection coordinates, (x, y) - position of the mouse
    void updateCoord(int x, int y, float point[]);
    // make sure user is aware that changes are not saved, return true when user agreed to proceed
    bool ensureSaved();
};

} /* namespace reditor */

#endif /* REDITOR_H */
