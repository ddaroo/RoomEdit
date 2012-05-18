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

#include <QtGui>
#include <QtOpenGL>

#include <math.h>

#include "REditorWnd.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

namespace reditor
{

REditorWnd::REditorWnd(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{

}

REditorWnd::~REditorWnd()
{
}

void REditorWnd::initializeGL()
{
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_MULTISAMPLE);
    static GLfloat lightPosition[4] = { 0.5, 5.0, 7.0, 1.0 };
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
}

void REditorWnd::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0, 0.0, -10.0);
}

void REditorWnd::resizeGL(int width, int height)
{
    int side = qMin(width, height);
    glViewport((width - side) / 2, (height - side) / 2, side, side);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
#ifdef QT_OPENGL_ES_1
    glOrthof(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#else
    glOrtho(-0.5, +0.5, -0.5, +0.5, 4.0, 15.0);
#endif
    glMatrixMode(GL_MODELVIEW);
}

void REditorWnd::mousePressEvent(QMouseEvent *event)
{
    //lastPos = event->pos();
}

void REditorWnd::mouseMoveEvent(QMouseEvent *event)
{
    //int dx = event->x() - lastPos.x();
    //int dy = event->y() - lastPos.y();

    //lastPos = event->pos();
}

} /* namespace reditor */