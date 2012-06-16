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
#include <QtCore/QDir>
#include <QtCore/QFileInfo>
#include <QtGui/QColor>

#ifdef Q_WS_WIN
#  include "windows.h"
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <math.h>

#include "REditor.h"
#include "REditObj.h"
#include "RGrid.h"
#include "RObjRoom.h"
#include "RObjSelection.h"
#include "RCamera.h"
#include "GUI/REditWnd.h"
#include "RModel3DS.h"
#include "RConfig.h"

namespace reditor
{

struct sceneObject
{
        float x,y,ang;
        QString fileName,sceneName;
};

typedef QVector<sceneObject> objectsType;
objectsType objects;

//*******************   
/*void REditor::rysujModel(QString file_name, int tex_num)
{
    QFileInfo finfo(file_name);
    skladTypeCit model_tmp = sklad_modeli.find(file_name);
    if (model_tmp != sklad_modeli.end()) {
        if (tex_num == -1)
        {
            model_tmp.value()->draw();
        }
        else
        {
            model_tmp.value()->draw(tex_num, false);
        }
    }
    else
    {
        qWarning() << "No model with the name " << finfo.absoluteFilePath();
    }
} */
//*******************
    
    
#define PI 3.14159265

// camera moving
double dkX,dkY,dkZ;

REditor::REditor() : mmouseX(0), mmouseY(0), mangle(0.1*PI/180.0), mcellSize(0.5f), mmode(DEFAULT)
{  
    mcam = new RCamera();
    
    addObject(new RGrid(40, QColor(Qt::white), mcellSize));
    mroom = new RObjRoom();
    msel = new RObjSelection();
}

REditor::~REditor()
{   
    foreach(REditObj* obj, mobjs)
    {
        delete obj;
    }
}

void REditor::attachTo(REditWnd* wnd)
{
    mwnd = wnd;
    
    // attach handlers for editor window events
    connect(mwnd, SIGNAL(mousePressed(Qt::MouseButton,int,int)), this, SLOT(hmousePressed(Qt::MouseButton,int,int)));
    connect(mwnd, SIGNAL(mouseReleased(Qt::MouseButton,int,int)), this, SLOT(hmouseReleased(Qt::MouseButton,int,int)));
    connect(mwnd, SIGNAL(mouseMoved(int,int)), this, SLOT(hmouseMoved(int,int)));
    connect(mwnd, SIGNAL(keyPressed(int)), this, SLOT(hkeyPressed(int)));
}

void REditor::addObject(reditor::REditObj* obj, bool showInExplorer)
{
    Q_ASSERT_X(mobjs.indexOf(obj) == -1, "object list", "object is already on the list");
    qDebug() << "Add " << obj->name();
    mobjs.append(obj);
    if(showInExplorer)
    {
        // TODO add to the object explorer
    }
}

REditObj * REditor::removeObject(REditObj* obj)
{
    Q_ASSERT_X(mobjs.indexOf(obj) != -1, "object list", "object is not on the list");
    qDebug() << "Remove " << obj->name();
    mobjs.removeOne(obj);
    return obj;
}

void REditor::hmouseMoved(int x, int y)
{
    if (mmode == VIEW)
    {
        int dx = mmouseX - x;
        int dy = mmouseY - y;
        switch (mcam->mode)
        {
        case RCamera::NORMAL:
            mcam->kX = mcam->kX*cos(-dx*mangle) - mcam->kZ*sin(-dx*mangle);
            mcam->kZ = mcam->kX*sin(-dx*mangle) + mcam->kZ*cos(-dx*mangle);
            dkY = mcam->kY - 0.05 * dy;
            if (dkY > 5.0 && dkY < 20.0)
            {
                mcam->kY = dkY;
            }
            break;
        case RCamera::FLAT:
            mcam->kX += 0.02 * dx;
            mcam->kZ += 0.02 * dy;
            mcam->pX = mcam->kX;
            mcam->pZ = mcam->kZ;
            break;
        case RCamera::INSIDE:
            mcam->pX = mcam->pX*cos(-dx*mangle) - mcam->pZ*sin(-dx*mangle);
            mcam->pZ = mcam->pX*sin(-dx*mangle) + mcam->pZ*cos(-dx*mangle);
            mcam->kX += dy * 0.001 * (mcam->pX - mcam->kX);
            mcam->kZ += dy * 0.001 * (mcam->pZ - mcam->kZ);
            break;
        default:
            Q_ASSERT_X(false, "hmouseMoved", "bad camera mode");
        };
    }
    if (mmode == SELECTION)
    {
        updateSelection(x, y, mendCorner);
        msel->updateCorners(mbeginCorner, mendCorner);
    }
    mmouseX = x;
    mmouseY = y;
    
    mwnd->repaint();
}

void REditor::hmousePressed(Qt::MouseButton b, int x, int y)
{
    qDebug() << "REditor::hmousePressed " << b << ", x = " << x << ", y = " << y; 
    switch (b)
    {
    case Qt::RightButton:
        mmode = VIEW;
        break;

    case Qt::LeftButton:
        updateSelection(x, y, mbeginCorner);
        updateSelection(x, y, mendCorner);
        msel->updateCorners(mbeginCorner, mendCorner);
        if(mobjs.indexOf(mroom) != -1)
        {
            removeObject(mroom);
        }
        addObject(msel);
        mmode = SELECTION;
        break;

    default:
        Q_ASSERT_X(false, "hmousePressed", "bad code number for the button");
    }
    
    mmouseX = x;
    mmouseY = y;
    mwnd->repaint();
}

void REditor::hmouseReleased(Qt::MouseButton b, int x, int y)
{
    qDebug() << "REditor::hmouseReleased " << b << ", x = " << x << ", y = " << y;
    
    if(b == Qt::LeftButton)
    {
        mroom->updateCorners(mbeginCorner, mendCorner);
        removeObject(msel);
        addObject(mroom);
    }
    mmode = DEFAULT;
    
    mwnd->repaint();
}

void REditor::hkeyPressed(int keyCode)
{
    qDebug() << "REditor::hkeyPressed " << keyCode;
    switch (keyCode)
    {
    case 49: // 1
        if (mcam->mode != RCamera::NORMAL)
        {
            mcam->kX = 0.0;
            mcam->kY = 15.0;
            mcam->kZ = 25.0;
            mcam->pX = 0.0;
            mcam->pY = 0.0;
            mcam->pZ = 0.0;
            mcam->upX = 0;
            mcam->upY = 1;
            mcam->upZ = 0;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-mwnd->width()/64.0, mwnd->width()/64.0, -mwnd->height()/64.0, mwnd->height()/64.0, 1.0, 60.0);
            mcam->mode = RCamera::NORMAL;
        }
        break;
    case 50: // 2
        if (mcam->mode != RCamera::FLAT)
        {
            mcam->kX = 0.0;
            mcam->kY = 10.0;
            mcam->kZ = 0.0;
            mcam->pX = 0.0;
            mcam->pY = 0.0;
            mcam->pZ = 0.0;
            mcam->upX = 0;
            mcam->upY = 0;
            mcam->upZ = -1;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-mwnd->width()/64.0, mwnd->width()/64.0, -mwnd->height()/64.0, mwnd->height()/64.0, 1.0, 60.0);
            mcam->mode = RCamera::FLAT;
        }
        break;
    case 51: // 3
        if (mcam->mode != RCamera::INSIDE)
        {
            mcam->kX = 0.0;
            mcam->kY = 1.72;
            mcam->kZ = 0.0;
            mcam->pX = 0.0;
            mcam->pY = 2.0;
            mcam->pZ = 100.0;
            mcam->upX = 0;
            mcam->upY = 1;
            mcam->upZ = 0;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0, (double)mwnd->width()/mwnd->height(), 1.0, 60.0);
            mcam->mode = RCamera::INSIDE;
        }
        break;
    case 27:
        exit(1);
        break;
    case 71:
    case 103:
        qDebug() << "Grid";
        break;
    default:
        break;
    }
    
    mwnd->repaint();
}

void REditor::hsave()
{
    qDebug() << "REditor::hsave()";
    // TODO implementation
}

void REditor::hload()
{
    qDebug() << "REditor::hload()";
    // TODO implementation
}

void REditor::updateSelection(int x, int y, float point[])
{
    GLdouble model[16];
    glGetDoublev(GL_MODELVIEW_MATRIX, model);
    GLdouble proj[16];
    glGetDoublev(GL_PROJECTION_MATRIX, proj);
    GLint view[4];
    glGetIntegerv(GL_VIEWPORT, view);
    GLdouble end[3], begin[3];
    GLint realy;
    realy = view[3] - (GLint) y - 1;
    gluUnProject(x, realy, 0.0, model, proj, view, begin+0, begin+1, begin+2);
    gluUnProject(x, realy, 1.0, model, proj, view, end+0, end+1, end+2);
    end[1] -= 1.5;
    
    // smalles selection step is the size of a single cell
    double s = end[1]/(begin[1] - end[1]);
    int i;
    point[0] = end[0] + (end[0] - begin[0])*s;
    i = point[0] / mcellSize;
    point[0] = i * mcellSize;
    point[1] =  (end[2] + (end[2] - begin[2])*s);
    i = point[1] / mcellSize;
    point[1] = i * mcellSize;
}

} /* namespace reditor */
