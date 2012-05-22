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

#include <math.h>
#include <GL/glu.h>

#include "REditorWnd.h"

#ifndef GL_MULTISAMPLE
#define GL_MULTISAMPLE  0x809D
#endif

namespace reditor
{

REditorWnd::REditorWnd(QWidget *parent)
    : QGLWidget(QGLFormat(QGL::SampleBuffers), parent)
{
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
}

REditorWnd::~REditorWnd()
{
}

#define PI 3.14159265

int windowHeight = 600;
int windowWidth = 800;

//po³o¿enie kamery
double  kX = 0;
double  kY = 15;
double  kZ = 25;
//punkt skupienia kamery
double pX;
double pY;
double pZ;
//wektor góry kamery
int upX,upY,upZ;
//tryb widoku kamery
int camMode;
//przemieszczenie kamery
double dkX,dkY,dkZ;
double angle = 0.1*PI/180;
//pozycja myszy
int mouseX,mouseY;
//obszar zaznaczenia pod³ogi
struct ground
{
        float beginCorner[2]; // wierzcho³ek pocz¹tkowy
        float endCorner[2]; // wierzcho³ek koñcowy
        float select[2]; // wierzcho³ek koñcowy podczas przeci¹gania
        bool set; // czy mamy rysowaæ pod³ogê
};

struct ground gr;

bool viewMove = false; // czy obracamy widok
bool selectRect = false; // do rysowania obszaru zaznaczenia
// rozmiar siatki
float gridSize = 0.5;

void REditorWnd::liczWsp(GLdouble begin[3],GLdouble end[3], float point[2])
{
    double s = end[1]/(begin[1] - end[1]);
    int i;
    point[0] = end[0] + (end[0] - begin[0])*s;
    i = point[0] / gridSize;
    point[0] = i * gridSize;
    point[1] =  (end[2] + (end[2] - begin[2])*s);
    i = point[1] / gridSize;
    point[1] = i * gridSize;
}

void REditorWnd::drawFloor()
{
    glColor3f(1,0.4,0);
    glBegin(GL_QUADS);
    glVertex3f(gr.beginCorner[0],0,gr.beginCorner[1]);
    glVertex3f(gr.beginCorner[0],0,gr.endCorner[1]);
    glVertex3f(gr.endCorner[0],0,gr.endCorner[1]);
    glVertex3f(gr.endCorner[0],0,gr.beginCorner[1]);
    glEnd();
}

void REditorWnd::walls()
{
    glColor3f(0.8,0.8,0.8);
    if (camMode == 1)
    {
        GLdouble cutPlane[4] = {-kX, -kY, -kZ, 4.0};
        glClipPlane(GL_CLIP_PLANE0,cutPlane);
        glEnable(GL_CLIP_PLANE0);
    }
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3f(gr.beginCorner[0],0,gr.beginCorner[1]);
    glVertex3f(gr.beginCorner[0],2.5,gr.beginCorner[1]);
    glVertex3f(gr.endCorner[0],0,gr.beginCorner[1]);
    glVertex3f(gr.endCorner[0],2.5,gr.beginCorner[1]);
    glVertex3f(gr.endCorner[0],0,gr.endCorner[1]);
    glColor3f(0.3,0.3,0.3);
    glVertex3f(gr.endCorner[0],2.5,gr.endCorner[1]);
    glVertex3f(gr.beginCorner[0],0,gr.endCorner[1]);
    glVertex3f(gr.beginCorner[0],2.5,gr.endCorner[1]);
    glColor3f(0.8,0.8,0.8);
    glVertex3f(gr.beginCorner[0],0,gr.beginCorner[1]);
    glVertex3f(gr.beginCorner[0],2.5,gr.beginCorner[1]);
    glEnd();
    glDisable(GL_CLIP_PLANE0);
}

void REditorWnd::rect()
{
    glPointSize( 5 );
    glColor4f(0,1,0,0.5);
    glBegin(GL_QUADS);
    glVertex3f(gr.beginCorner[0],0,gr.beginCorner[1]);
    glVertex3f(gr.beginCorner[0],0,gr.select[1]);
    glVertex3f(gr.select[0],0,gr.select[1]);
    glVertex3f(gr.select[0],0,gr.beginCorner[1]);
    glEnd();

    glColor3f(1,0,1);
    glPushMatrix();
    glBegin(GL_POINTS);
    glVertex3f(gr.beginCorner[0],0,gr.beginCorner[1]);
    glEnd();
    glPopMatrix();

    glColor3f(0,1,1);
    glPushMatrix();
    glBegin(GL_POINTS);
    glVertex3f(gr.select[0],0,gr.select[1]);
    glEnd();
    glPopMatrix();
}

void REditorWnd::grid()
{
    glColor3f(1,1,1);
    for (int j = 1;j<20/gridSize;j++)
    {
        glBegin(GL_LINES);
        glVertex3f(-10,0.01,-10 + j*gridSize);
        glVertex3f(10,0.01,-10 + j*gridSize);
        glEnd();
    }
    for (int i = 1;i<20/gridSize;i++)
    {
        glBegin(GL_LINES);
        glVertex3f(-10 + i*gridSize,0.01,-10);
        glVertex3f(-10 + i*gridSize,0.01,10);
        glEnd();
    }
}

void REditorWnd::initializeGL()
{
    kX = 0;
    kY = 15;
    kZ = 25;
    pX = 0;
    pY = 0;
    pZ = 0;
    upX = 0;
    upY = 1;
    upZ = 0;
    camMode = 1;
    gr.set = false;
}

void REditorWnd::paintGL()
{
    glClearColor(0.0,0.0,0.0,1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glShadeModel(GL_SMOOTH);
    glLoadIdentity();
    gluLookAt(kX,kY,kZ,pX,pY,pZ,upX,upY,upZ);
    glColor4f(0.4,0.4,0.4,0.5);
    glBegin(GL_QUADS);
    glVertex3i(-20,0,-20);
    glVertex3i(20,0,-20);
    glVertex3i(20,0,20);
    glVertex3i(-20,0,20);
    glEnd();
    grid();
    if (selectRect)
        rect();
    if (gr.set)
    {
        drawFloor();
        walls();
    }
    glFlush();
}

void REditorWnd::resizeGL(int width, int height)
{
    if (width==0) width++;
    if (height==0) height++;
    windowHeight = height;
    windowWidth = width;
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glViewport(0,0,windowWidth,windowHeight+24);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (camMode == 3)
        gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,1.0f,60.0f);
    else
        glOrtho(-(GLfloat)windowWidth/64,(GLfloat)windowWidth/64,-(GLfloat)windowHeight/64,(GLfloat)windowHeight/64,1,60);
    //draw();
}

void REditorWnd::mouseMoveEvent(QMouseEvent *event)
{
    if (viewMove)
    {
        int dx = mouseX-event->x();
        int dy = mouseY-event->y();
        switch (camMode)
        {
        case 1:
            kX = kX*cos(-dx*angle) - kZ*sin(-dx*angle);
            kZ = kX*sin(-dx*angle) + kZ*cos(-dx*angle);
            dkY = kY - 0.05 * dy;
            if (dkY>5 && dkY<20)
                kY = dkY;
            break;
        case 2:
            kX += 0.02 * dx;
            kZ += 0.02 * dy;
            pX = kX;
            pZ = kZ;
            break;
        case 3:
            pX = pX*cos(-dx*angle) - pZ*sin(-dx*angle);
            pZ = pX*sin(-dx*angle) + pZ*cos(-dx*angle);
            kX += dy * 0.001 * (pX - kX);
            kZ += dy * 0.001 * (pZ - kZ);
            break;
        };
        mouseX = event->x();
        mouseY = event->y();
    }
    if (selectRect)
    {
        mouseX = event->x();
        mouseY = event->y();

        GLdouble model[16];
        glGetDoublev(GL_MODELVIEW_MATRIX,model);
        GLdouble proj[16];
        glGetDoublev(GL_PROJECTION_MATRIX,proj);
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT,view);
        GLdouble end[ 3 ], begin[3];
        GLint realy;
        realy = view[3] - (GLint) mouseY - 1;
        gluUnProject(mouseX,realy,0,model,proj,view,begin+0,begin+1,begin+2);
        gluUnProject(mouseX,realy,1,model,proj,view,end+0,end+1,end+2);
        end[1]-=1.5;
        liczWsp(begin,end,gr.select);
    }
    repaint();
    //glutPostRedisplay();
}

void REditorWnd::mousePressEvent(QMouseEvent *event)
{
    switch (event->button())
    {
    //case GLUT_RIGHT_BUTTON:
   case Qt::RightButton:
        //if (state == GLUT_DOWN)
        //{
            viewMove = true;
            selectRect = false;
            mouseX = event->x();
            mouseY = event->y();
            break;
        //}
        //else
        //    viewMove = false;
        //break;
    //case GLUT_LEFT_BUTTON:
    case Qt::LeftButton:
        mouseX = event->x();
        mouseY = event->y();
        GLdouble model[16];
        glGetDoublev(GL_MODELVIEW_MATRIX,model);
        GLdouble proj[16];
        glGetDoublev(GL_PROJECTION_MATRIX,proj);
        GLint view[4];
        glGetIntegerv(GL_VIEWPORT,view);
        GLdouble end[ 3 ], begin[3];
        GLint realy;
        realy = view[3] - (GLint) mouseY - 1;
        gluUnProject(mouseX,realy,0,model,proj,view,begin+0,begin+1,begin+2);
        gluUnProject(mouseX,realy,1,model,proj,view,end+0,end+1,end+2);
        end[1]-=1.5;
//         if (state == GLUT_DOWN)
//         {

            liczWsp(begin,end,gr.beginCorner);
            liczWsp(begin,end,gr.select);
            gr.set = false;
            viewMove = false;
            selectRect = true;
//         }
//         else if (state == GLUT_UP)
//         {
//             liczWsp(begin,end, gr.endCorner);
//             gr.set = true;
//             selectRect = false;
//         }
//         else
//             selectRect = false;
        break;
    default:
        viewMove = false;
        selectRect = false;
    }
    repaint();
    //glutPostRedisplay();
}

void REditorWnd::mouseReleaseEvent(QMouseEvent* event)
{
    GLdouble end[ 3 ], begin[3];
    
    if(event->button() == Qt::LeftButton)
    {
        liczWsp(begin,end, gr.endCorner);
        gr.set = true;
    }
    selectRect = false;
    viewMove = false;
}

void REditorWnd::keyPressEvent(QKeyEvent* event)
{
    //std::cout<<key<<std::endl;
    qDebug() << "Number of the pressed key: " << event->key();
    switch (event->key())
    {
    case 49:
        if (camMode != 1)
        {
            kX = 0;
            kY = 15;
            kZ = 25;
            pX = 0;
            pY = 0;
            pZ = 0;
            upX = 0;
            upY = 1;
            upZ = 0;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-(GLfloat)windowWidth/64,(GLfloat)windowWidth/64,-(GLfloat)windowHeight/64,(GLfloat)windowHeight/64,1,60);
            camMode = 1;
        }
        break;
    case 50:
        if (camMode != 2)
        {
            kX = 0;
            kY = 10;
            kZ = 0;
            pX = 0;
            pY = 0;
            pZ = 0;
            upX = 0;
            upY = 0;
            upZ = -1;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            glOrtho(-(GLfloat)windowWidth/64,(GLfloat)windowWidth/64,-(GLfloat)windowHeight/64,(GLfloat)windowHeight/64,1,60);
            camMode = 2;
        }
        break;
    case 51:
        if (camMode != 3)
        {
            kX = 0;
            kY = 1.72;
            kZ = 0;
            pX = 0;
            pY = 2;
            pZ = 100;
            upX = 0;
            upY = 1;
            upZ = 0;
            glMatrixMode(GL_PROJECTION);
            glLoadIdentity();
            gluPerspective(45.0f,(GLfloat)windowWidth/(GLfloat)windowHeight,1.0f,60.0f);
            camMode = 3;
        }
        break;
    case 27:
        exit(1);
        break;
    case 71:
    case 103:
        //std::cout<<"Grid"<<std::endl;
        qDebug() << "Grid";
        break;
    default:
        break;
    }
    updateGL();
    //glutPostRedisplay();
}

} /* namespace reditor */
