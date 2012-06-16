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

#include <QtCore/qglobal.h>
#ifdef Q_WS_WIN
#  include <windows.h>
#endif

#ifdef __APPLE__
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

#include "RObjSelection.h"

namespace reditor
{
    
RObjSelection::RObjSelection() : RGridObj("Selection")
{

}

RObjSelection::RObjSelection(int x, int y, int width, int height): RGridObj("Selection", x, y, width, height)
{
    
}

RObjSelection::~RObjSelection()
{

}

void RObjSelection::paintGL() const
{
    glDisable(GL_TEXTURE_2D);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColor4f(0.0f, 1.0f, 0.0f, 0.4f);
    glBegin(GL_QUADS);
        glVertex3f(mbeginCorner[0], 0.07f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mbeginCorner[1]);
    glEnd();

    glLineWidth(3.0f);
    glColor4f(0.4f, 1.0f, 0.0f, 1.0f);
    glBegin(GL_LINES);
        glVertex3f(mbeginCorner[0], 0.07f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mbeginCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mbeginCorner[1]);
        glVertex3f(mendCorner[0], 0.07f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 0.07f, mbeginCorner[1]);
    glEnd();

    glPointSize(5.0f);
    glColor3f(1.0f, 0.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_POINTS);
            glVertex3f(mbeginCorner[0], 0.05f, mbeginCorner[1]);
        glEnd();
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_POINTS);
            glVertex3f(mendCorner[0], 0.05f, mendCorner[1]);
        glEnd();
    glPopMatrix();
    glDisable(GL_BLEND);
    glDisable(GL_COLOR_MATERIAL);
    glEnable(GL_TEXTURE_2D);
}

void RObjSelection::updateCorners(float beginCorner[2], float endCorner[2])
{
    mbeginCorner[0] = beginCorner[0];
    mbeginCorner[1] = beginCorner[1];
    mendCorner[0] = endCorner[0];
    mendCorner[1] = endCorner[1];
}

} // namespace reditor
