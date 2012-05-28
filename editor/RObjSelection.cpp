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
#include <GL/glu.h>

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
    glPointSize(5.0f);
    glColor4f(0.0f, 1.0f, 0.0f, 0.5f);
    glBegin(GL_QUADS);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 0.0f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mbeginCorner[1]);
    glEnd();

    glColor3f(1.0f, 0.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_POINTS);
            glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glEnd();
    glPopMatrix();

    glColor3f(0.0f, 1.0f, 1.0f);
    glPushMatrix();
        glBegin(GL_POINTS);
            glVertex3f(mendCorner[0], 0.0f, mendCorner[1]);
        glEnd();
    glPopMatrix();
}

void RObjSelection::updateCorners(float beginCorner[2], float endCorner[2])
{
    mbeginCorner[0] = beginCorner[0];
    mbeginCorner[1] = beginCorner[1];
    mendCorner[0] = endCorner[0];
    mendCorner[1] = endCorner[1];
}

} // namespace reditor