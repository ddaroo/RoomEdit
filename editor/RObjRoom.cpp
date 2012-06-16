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

#include "global.h"
#include "RObjRoom.h"
#include "REditor.h"
#include "RResourceDB.h"
#include "RTexture.h"

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
    int dh = mbeginCorner[0] - mendCorner[0];
    int dw = mbeginCorner[1] - mendCorner[1];

    // paint floor
    GLfloat ambientFloor[4] = {0.5f, 0.5f, 0.5f, 1.0f};
    GLfloat specularFloor[4] = {0.8f, 0.8f, 0.8f, 1.0f};
    glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientFloor);
    glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specularFloor);
    glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);
    glBindTexture(GL_TEXTURE_2D, rscDB.texture("floor3")->id());
    glColor3f(1.0f, 0.4f, 0.0f);
   
    glBegin(GL_QUADS);
        glTexCoord2d(0, 0);
        glVertex3f(mbeginCorner[0], 0.05f, mbeginCorner[1]);
        glTexCoord2d(dh, 0);
        glVertex3f(mbeginCorner[0], 0.05f, mendCorner[1]);
        glTexCoord2d(dh, dw);
        glVertex3f(mendCorner[0], 0.05f, mendCorner[1]);
        glTexCoord2d(0, dw);
        glVertex3f(mendCorner[0], 0.05f, mbeginCorner[1]);
    glEnd();
    
    // paint walls
    GLfloat ambientWalls[4] = {0.95f, 0.95f, 0.95f, 1.0f};
    GLfloat specularWalls[4] = {0.0f, 0.0f, 0.0f, 1.0f};
    glMaterialfv(GL_FRONT, GL_AMBIENT, ambientWalls);
    glMaterialfv(GL_FRONT, GL_SPECULAR, specularWalls);
    glBindTexture(GL_TEXTURE_2D, rscDB.texture("wall4")->id());
    glColor3f(0.8f, 0.8f, 0.8f);
    // TODO missing functionality
//     if (camMode == 1)
//     {
//         GLdouble cutPlane[4] = {-kX, -kY, -kZ, 4.0};
//         glClipPlane(GL_CLIP_PLANE0,cutPlane);
//         glEnable(GL_CLIP_PLANE0);
//     }
    glBegin(GL_TRIANGLE_STRIP);
    glTexCoord2d(0, 0);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glTexCoord2d(0, 1);
        glVertex3f(mbeginCorner[0], 2.5f, mbeginCorner[1]);
        glTexCoord2d(dh, 0);
        glVertex3f(mendCorner[0], 0.0f, mbeginCorner[1]);
        glTexCoord2d(dh, 1);
        glVertex3f(mendCorner[0], 2.5f, mbeginCorner[1]);
        glTexCoord2d(dh+dw, 0);
        glVertex3f(mendCorner[0], 0.0f, mendCorner[1]);
        glTexCoord2d(dh+dw, 1);
        glVertex3f(mendCorner[0], 2.5f, mendCorner[1]);
        glTexCoord2d(2*dh+dw, 0);
        glVertex3f(mbeginCorner[0], 0.0f, mendCorner[1]);
        glTexCoord2d(2*dh+dw, 1);
        glVertex3f(mbeginCorner[0], 2.5f, mendCorner[1]);
        glTexCoord2d(2*dh+ 2*dw, 0);
        glVertex3f(mbeginCorner[0], 0.0f, mbeginCorner[1]);
        glTexCoord2d(2*dh+2*dw, 1);
        glVertex3f(mbeginCorner[0], 2.5f, mbeginCorner[1]);
    glEnd();
    //glDisable(GL_CLIP_PLANE0);
}

void RObjRoom::updateCorners(float beginCorner[2], float endCorner[2])
{
    mbeginCorner[0] = beginCorner[0];
    mbeginCorner[1] = beginCorner[1];
    mendCorner[0] = endCorner[0];
    mendCorner[1] = endCorner[1];
}

} // namespace reditor