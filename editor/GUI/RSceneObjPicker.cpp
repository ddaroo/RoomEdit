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

#include <QtGui/QMouseEvent>
#ifdef Q_WS_WIN
#  include <windows.h>
#endif

#ifdef Q_WS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "RSceneObjPicker.h"
#include "RResourceDB.h"
#include "global.h"
#include "RModel3DS.h"

namespace reditor
{
    
RSceneObjPicker::RSceneObjPicker(const QString& modelName, QWidget *parent) : QGLWidget(parent),
       mmodel(0), mmodelName(modelName)
{
    mrscDB = new RResourceDB();
}

RSceneObjPicker::~RSceneObjPicker()
{

}

void RSceneObjPicker::initializeGL()
{
    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glShadeModel(GL_SMOOTH);
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_LIGHTING);
    GLfloat ambient[4] = {1.0f, 1.0f, 1.0f, 1.0f};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
    GLint position[4] = {0, 10, 0, 1};
    glLightiv(GL_LIGHT0, GL_POSITION, position);
    glEnable(GL_LIGHT0);
    
    QList<QString> models;
    models.append(mmodelName);
    mrscDB->loadModels(reditor::RConfig::texturesDir(""), reditor::RConfig::modelsDir(""), models);
    mmodel = mrscDB->model(mmodelName);
}

void RSceneObjPicker::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(0.0, 15.0, 25.0, 0.0, 0.0, 0.0, 0, 1, 0);
    glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
    
    glEnable(GL_TEXTURE_2D);
    
    mmodel->draw();

    glDisable(GL_TEXTURE_2D);
    glFlush();
}

void RSceneObjPicker::resizeGL(int width, int height)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height + 24);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-width/64.0, width/64.0, -height/64.0, height/64.0, 1.0, 60.0);
}

void RSceneObjPicker::enterEvent(QEvent * event)
{
    // TODO change background
}

void RSceneObjPicker::leaveEvent(QEvent * event)
{
    // TODO change background
}

void RSceneObjPicker::mouseReleaseEvent(QMouseEvent * event)
{
    emit selected(mmodelName);
}

} // namespace reditor