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
#include <QtGui>
#include <QtOpenGL>
#ifdef Q_WS_WIN
#  include <windows.h>
#endif

#ifdef Q_WS_MAC
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "REditWnd.h"
#include "REditObj.h"
#include "RCamera.h"
#include "global.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

namespace reditor
{

REditWnd::REditWnd(const QList< reditor::REditObj* > * objs, const RCamera * cam, QWidget* parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent), mobjs(objs), mcam(cam)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

REditWnd::~REditWnd()
{
}

void REditWnd::initializeGL()
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
}

void REditWnd::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(mcam->kX, mcam->kY, mcam->kZ, mcam->pX, mcam->pY, mcam->pZ, mcam->upX, mcam->upY, mcam->upZ);
    glColor4f(0.4f, 0.4f, 0.4f, 0.5f);
    
    glEnable(GL_COLOR_MATERIAL);
    glBegin(GL_QUADS);
        glVertex3i(-15, 0, -15);
        glVertex3i(15, 0, -15);
        glVertex3i(15, 0, 15);
        glVertex3i(-15, 0, 15);
    glEnd();
    glDisable(GL_COLOR_MATERIAL);

    glEnable(GL_TEXTURE_2D);
    foreach(const REditObj * obj, *mobjs)
    {
        obj->paintGL();
    }
    glDisable(GL_TEXTURE_2D);

    glFlush();
}

void REditWnd::resizeGL(int width, int height)
{
    Q_ASSERT(width > 0);
    Q_ASSERT(height > 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0, 0, width, height + 24);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (mcam->mode == RCamera::INSIDE)
    {
        gluPerspective(45.0, (double)width/height, 1.0, 60.0);
    }
    else
    {
        glOrtho(-width/64.0, width/64.0, -height/64.0, height/64.0, 1.0, 60.0);
    }
}

void REditWnd::mouseMoveEvent(QMouseEvent *event)
{
    emit mouseMoved(event->x(), event->y());
}

void REditWnd::mousePressEvent(QMouseEvent *event)
{
    emit mousePressed(event->button(), event->x(), event->y());
}

void REditWnd::mouseReleaseEvent(QMouseEvent* event)
{
    emit mouseReleased(event->button(), event->x(), event->y());
}

void REditWnd::keyPressEvent(QKeyEvent* event)
{
    emit keyPressed(event->key());
}

} /* namespace reditor */
