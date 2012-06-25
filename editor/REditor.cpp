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
#include <QtGui/QFileDialog>
#include <QtGui/QMessageBox>
#include <QtGui/QApplication>

#ifdef Q_WS_WIN
#  include "windows.h"
#endif

#ifdef Q_WS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include <math.h>

#include "global.h"
#include "REditor.h"
#include "REditObj.h"
#include "RGrid.h"
#include "RObjRoom.h"
#include "RObjSelection.h"
#include "RCamera.h"
#include "RSceneObj.h"
#include "RConfig.h"
#include "RProject.h"
#include "GUI/REditWnd.h"
#include "GUI/RMainWnd.h"


namespace reditor
{  
    
#define PI 3.14159265
#define ROTATION_STEP 45

// camera moving
double dkX,dkY,dkZ; // TODO get rid of these global variables

REditor::REditor() : mgridVisible(true), mmouseX(0), mmouseY(0), mrotation(0), mroomDimensionsPicked(false), mactiveObject(NULL),
    mangle(0.1*PI/180.0), mcellSize(0.5f), mmode(DEFAULT), mdefPath(QDir::homePath()), mopenedProject(""), msavedProject(false)
{  
    mcam = new RCamera();
    
    mgrid = new RGrid(40, QColor(Qt::white), mcellSize);
    addObject(mgrid);
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
    meditorWnd = wnd;
    
    // attach handlers for editor window signals
    connect(meditorWnd, SIGNAL(mousePressed(Qt::MouseButton,int,int)), this, SLOT(hmousePressed(Qt::MouseButton,int,int)));
    connect(meditorWnd, SIGNAL(mouseReleased(Qt::MouseButton,int,int)), this, SLOT(hmouseReleased(Qt::MouseButton,int,int)));
    connect(meditorWnd, SIGNAL(mouseMoved(int,int)), this, SLOT(hmouseMoved(int,int)));
    connect(meditorWnd, SIGNAL(keyPressed(int)), this, SLOT(hkeyPressed(int)));
}

void REditor::attachToMainWnd(RMainWnd* wnd)
{
    mmainWnd = wnd;
    // attach handlers for editor window signals
    connect(mmainWnd, SIGNAL(newProject()), this, SLOT(hnewProject()));
    connect(mmainWnd, SIGNAL(openProject()), this, SLOT(hopenProject()));
    connect(mmainWnd, SIGNAL(saveProject()), this, SLOT(hsaveProject()));
    connect(mmainWnd, SIGNAL(saveProjectAs()), this, SLOT(hsaveProjectAs()));
    connect(mmainWnd, SIGNAL(helpAbout()), this, SLOT(hhelpAbout()));
    connect(mmainWnd, SIGNAL(showGrid(bool)), this, SLOT(hshowGrid(bool)));
    connect(mmainWnd, SIGNAL(switchCamera(int)), this, SLOT(hswitchCamera(int)));
    connect(mmainWnd, SIGNAL(closeProgram()), this, SLOT(hcloseProgram()));
    connect(mmainWnd, SIGNAL(objSelected(QString)), this, SLOT(hobjSelected(QString)));
}
// FIXME after adding several objects, editor is getting laggy
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
    // calculate current position of the mouse
    updateCoord(x, y, mcurPos);
    
    if(mmode == VIEW)
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
    
    if(mmode == SELECTION)
    {
        mendCorner[0] = mcurPos[0];
        mendCorner[1] = mcurPos[1];
           
        msel->updateCorners(mbeginCorner, mendCorner);
    }
    if((mmode == OBJECTS || mmode == PICKING) && mactiveObject != NULL)
    {   
		if(mactiveObject->name() == "door")
		{
			float dist,best = abs(mcurPos[1] - mroom->beginCorner()[1]);
			float pos[2] = {mcurPos[0] , mroom->beginCorner()[1] };
			mactiveObject->updateRotation(0);
			dist = abs(mcurPos[0] - mroom->beginCorner()[0]);
			if(dist < best)
			{
				best = dist;
				pos[0] = mroom->beginCorner()[0];
				pos[1] = mcurPos[1];
				mactiveObject->updateRotation(90);
			}
			dist = abs(mcurPos[1] - mroom->endCorner()[1]);
			if(dist < best)
			{
				best = dist;
				pos[0] = mcurPos[0];
				pos[1] = mroom->endCorner()[1];
				mactiveObject->updateRotation(0);
			}
			dist = abs(mcurPos[0] - mroom->endCorner()[0]);
			if(dist < best)
			{
				best = dist;
				pos[0] = mroom->endCorner()[0];
				pos[1] = mcurPos[1];
				mactiveObject->updateRotation(90);
			}
			mactiveObject->updatePosition(pos);
		}
		else
		{
			mactiveObject->updatePosition(mcurPos);
		}
    }
    mmouseX = x;
    mmouseY = y;
    
    meditorWnd->repaint();
}

void REditor::hmousePressed(Qt::MouseButton b, int x, int y)
{
    //qDebug() << "REditor::hmousePressed " << b << ", x = " << x << ", y = " << y; 
    switch (b)
    {
    case Qt::RightButton:
        mprevMode = mmode;
        mmode = VIEW;
        break;

    case Qt::LeftButton:
        if(!mroomDimensionsPicked)
        {
            mmode = SELECTION;
        }
        if(mmode == SELECTION)
        {
            updateCoord(x, y, mbeginCorner);
            updateCoord(x, y, mendCorner);
            msel->updateCorners(mbeginCorner, mendCorner);
            addObject(msel);
        }
        break;

    default:
        Q_ASSERT_X(false, "hmousePressed", "bad code number for the button");
    }
    
    mmouseX = x;
    mmouseY = y;
    meditorWnd->repaint();
}

void REditor::hmouseReleased(Qt::MouseButton b, int x, int y)
{
    //qDebug() << "REditor::hmouseReleased " << b << ", x = " << x << ", y = " << y;
    
    if(b == Qt::LeftButton && mmode == SELECTION)
    {
        mroom->updateCorners(mbeginCorner, mendCorner);
        removeObject(msel);
        addObject(mroom);
        mroomDimensionsPicked = true;
        
        mmode = OBJECTS;
    }
    if(b == Qt::LeftButton && mmode == OBJECTS && mactiveObject != NULL)
    {
        mactiveObject = new RSceneObj(mactiveObject->name());
        addObject(mactiveObject);
        updateCoord(x, y, mcurPos);
        mactiveObject->updatePosition(mcurPos);
        mactiveObject->updateRotation(mrotation);
        msavedProject = false;
    }
	if(b == Qt::LeftButton && mmode == PICKING)
	{
		if(mactiveObject != NULL)
		{
			mactiveObject = NULL;
		}
		else
		{
			float best = 2;
		    float dist = 0;
		    updateCoord(x, y, mcurPos);
			foreach(REditObj * obj, mobjs)
			{
				RSceneObj * current = dynamic_cast<RSceneObj *>(obj);
				if(current != 0)
				{
					dist = sqrt( pow(current->position()[0] - mcurPos[0], 2) + pow(current->position()[1] - mcurPos[1], 2) );
					//qDebug() << "best : " << best << " Dist : " << dist;
					if(dist < best)
					{	
						best = dist;
						mactiveObject = current;
						//mmode = OBJECTS;
					}
					//qDebug() << "Current pos : " << current->position()[0] << "  " << current->position()[1];
				}
			}
		}
	}
    if(b == Qt::RightButton && mmode == VIEW)
    {
        mmode = mprevMode;
    }
    
    meditorWnd->repaint();
}

// TODO handle mouse scrolling

void REditor::hkeyPressed(int keyCode)
{
    qDebug() << "REditor::hkeyPressed " << keyCode;
    switch (keyCode)
    {
    case Qt::Key_1:
        hswitchCamera(RCamera::NORMAL);
        break;

    case Qt::Key_2:
        hswitchCamera(RCamera::FLAT);
        break;

    case Qt::Key_3:
        hswitchCamera(RCamera::INSIDE);
        break;

    case Qt::Key_R:
        if ((mmode == OBJECTS || mmode == PICKING) && mactiveObject != NULL)
        {
            mrotation += ROTATION_STEP;
            mrotation = mrotation%360;

            mactiveObject->updateRotation(mrotation);
            mactiveObject->updatePosition(mcurPos);
        }

        break;

    case Qt::Key_Escape:
        QApplication::exit(0);
        break;

    case Qt::Key_G:
        hshowGrid(!mgridVisible);
        break;
	case Qt::Key_P:
		if(mactiveObject != NULL)
		{
		    removeObject(mactiveObject);
			delete mactiveObject;
			mactiveObject = NULL;
		}
		mmode = PICKING;
    default:
        break;
    }

    meditorWnd->repaint();
}

void REditor::hsaveProject()
{
    if(mopenedProject.isEmpty())
    {
        hsaveProjectAs();
    }
    else
    {
        qDebug() << "Save project " << mopenedProject;
        // TODO save scene to the file
        msavedProject = true;

        RProject * project = new RProject( mopenedProject );
        project->save( this );
        delete project;
    } 
}

void REditor::hsaveProjectAs()
{    
    QString filePatch = QFileDialog::getSaveFileName(mmainWnd, 
                                                     tr("Save Project as"), 
                                                     mdefPath + QString("/roomProject.%1").arg(RConfig::fileEditExt),
                                                     tr("Project Files *.%1 (*.%1);;All files *.* (*.*)").arg(RConfig::fileEditExt));
    if (!filePatch.isEmpty())
    {
        mopenedProject = filePatch;
        hsaveProject();
        msavedProject = true;

        RProject * project = new RProject( mopenedProject );
        project->save( this );
        delete project;
    }
    else
    {
        qDebug() << "Save project canceled";
    }
}

void REditor::hopenProject()
{
    if(ensureSaved())
    {
        QString filePatch = QFileDialog::getOpenFileName(mmainWnd, 
                                                        tr("Open Project"),
                                                        mdefPath, 
                                                        tr("Project Files *.%1 (*.%1);;All files *.* (*.*)").arg(RConfig::fileEditExt));
        if (!filePatch.isEmpty())
        {
            mopenedProject = filePatch;
            mdefPath = QFileInfo(mopenedProject).dir().absolutePath();
            msavedProject = true;
            mmode = OBJECTS;
            mactiveObject = NULL;
            mroomDimensionsPicked = true;
            qDebug() << "Open project " << mopenedProject;

            RProject * project = new RProject( mopenedProject );
            project->load( this );
            delete project;
        }
        else
        {
            qDebug() << "Open project canceled";
        }
    }
}

void REditor::hnewProject()
{
    if(ensureSaved())
    {
        qDebug() << "New project";

        clearProject();
    }
}

void REditor::clearProject()
{
    foreach(REditObj * obj, mobjs)
    {
        if (obj != mroom && obj != msel && obj != mgrid)
        {
            delete obj;
        }
    }

    mobjs.clear();
    mactiveObject = NULL;

    addObject(mgrid, false);
    mroomDimensionsPicked = false;
}

void REditor::hcloseProgram()
{
    if(ensureSaved())
    {
        QApplication::exit(0);
    }
}

void REditor::hswitchCamera(int mode)
{
    if(mode == mcam->mode)
    {
        return;
    }
        
    switch(mode)
    {
        case RCamera::NORMAL:
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
            glOrtho(-meditorWnd->width()/64.0, meditorWnd->width()/64.0, -meditorWnd->height()/64.0, meditorWnd->height()/64.0, 1.0, 60.0);
            mcam->mode = RCamera::NORMAL;
            break;
            
        case RCamera::FLAT:
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
            glOrtho(-meditorWnd->width()/64.0, meditorWnd->width()/64.0, -meditorWnd->height()/64.0, meditorWnd->height()/64.0, 1.0, 60.0);
            mcam->mode = RCamera::FLAT;
            break;
            
        case RCamera::INSIDE:
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
            gluPerspective(45.0, (double)meditorWnd->width()/meditorWnd->height(), 1.0, 60.0);
            mcam->mode = RCamera::INSIDE;
            break;
    }
    mmainWnd->setCamera(mode);
    meditorWnd->repaint();
    qDebug() << "Switch camera: " << mode;
}

void REditor::hshowGrid(bool show)
{
    mobjs.removeAll(mgrid);
    if(show)
    {
        addObject(mgrid, false);
    }
    mgridVisible = show;
    mmainWnd->setGridVisible(show);
    qDebug() << "Show grid " << mgridVisible;
}

void REditor::hhelpAbout()
{
    qDebug() << "Help about";
    QMessageBox::about(mmainWnd, tr("About RoomEdit"), tr("The <b>RoomEdit</b> is a 3D room editor. It's a student project created at the University of Science and Technology in Krakow, Poland.<br><br>Authors:<ul><li>Dariusz Jania [dariusz.jania@gmail.com]</li><li>Witold Baran [baranvtek@gmail.com]</li><li>Kacper Stasik [kacper@statis.eu]</li></ul>"));
}

void REditor::hobjSelected(QString modelName)
{
    if(mmode == OBJECTS)
    {
        if(mactiveObject != NULL)
        {
            delete removeObject(mactiveObject);
        }
        mactiveObject = new RSceneObj(modelName);
        addObject(mactiveObject);
    }
}

void REditor::updateCoord(int x, int y, float point[])
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

bool REditor::ensureSaved()
{
    if(!msavedProject)
    {
        QMessageBox msgBox(mmainWnd);
        msgBox.setText(tr("Project has been modified."));
        msgBox.setInformativeText("Do you want to save your changes?");
        msgBox.setIcon(QMessageBox::Warning);
        msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
        msgBox.setDefaultButton(QMessageBox::Save);
        int ret = msgBox.exec();

        switch (ret) {
        case QMessageBox::Save:
            hsaveProject();
            return msavedProject;
            break;
        case QMessageBox::Discard:
            msavedProject = true;
            qDebug() << "ensureSaved Discard changes";
            return true;
            break;
        case QMessageBox::Cancel:
            qDebug() << "ensureSaved canceled";
            return false;
            break;
        default:
            // should never be reached
            return false;
            break;
        }
    }
    
    return true;
}

} /* namespace reditor */
