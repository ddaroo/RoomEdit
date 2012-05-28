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

#include "RObjRoom.h"

namespace reditor
{
    
RObjRoom::RObjRoom() : RGridObj("Room")
{

}

RObjRoom::RObjRoom(int x, int y, int width, int height): RGridObj("Room", x, y, width, height)
{

}

RObjRoom::~RObjRoom()
{

}

void RObjRoom::paintGL() const
{
    // paint floor
    glColor3f(1.0f, 0.4f, 0.0f);
    glBegin(GL_QUADS);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 0.0f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mendCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mbeginCorner[1]);
    glEnd();
    // paint walls
    // TODO missing functionality
    glColor3f(0.8f, 0.8f, 0.8f);
//     if (camMode == 1)
//     {
//         GLdouble cutPlane[4] = {-kX, -kY, -kZ, 4.0};
//         glClipPlane(GL_CLIP_PLANE0,cutPlane);
//         glEnable(GL_CLIP_PLANE0);
//     }
    glBegin(GL_TRIANGLE_STRIP);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 2.5f, mbeginCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mbeginCorner[1]);
        glVertex3f(mendCorner[0], 2.5f, mbeginCorner[1]);
        glVertex3f(mendCorner[0], 0.0f, mendCorner[1]);
        glColor3f(0.3f, 0.3f, 0.3f);
        glVertex3f(mendCorner[0], 2.5f, mendCorner[1]);
        glVertex3f(mbeginCorner[0], 0.0f, mendCorner[1]);
        glVertex3f(mbeginCorner[0], 2.5f, mendCorner[1]);
        glColor3f(0.8f, 0.8f, 0.8f);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glVertex3f(mbeginCorner[0], 2.5f, mbeginCorner[1]);
    glEnd();
    glDisable(GL_CLIP_PLANE0);
}

void RObjRoom::updateCorners(float beginCorner[2], float endCorner[2])
{
    mbeginCorner[0] = beginCorner[0];
    mbeginCorner[1] = beginCorner[1];
    mendCorner[0] = endCorner[0];
    mendCorner[1] = endCorner[1];
}

} // namespace reditor