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

#include <QtOpenGL>

#include "RGrid.h"

namespace reditor
{
    
RGrid::RGrid(int size, const QColor& clr, float cellSize) : 
    REditObj("Grid"), msize(size), mclr(clr), mcellSize(cellSize)
{

}

RGrid::~RGrid()
{

}

void RGrid::paintGL() const
{
    glColor3f(mclr.redF(), mclr.greenF(), mclr.blueF());
    for (int i = 0; i <= msize; ++i)
    {
        glBegin(GL_LINES);
            glVertex3f(-10.0f, 0.01f, -10.0f + i*mcellSize);
            glVertex3f(10.0f , 0.01f, -10.0f + i*mcellSize);
        glEnd();
        
        glBegin(GL_LINES);
            glVertex3f(-10.0f + i*mcellSize, 0.01f, -10.0f);
            glVertex3f(-10.0f + i*mcellSize, 0.01f, 10.0f);
        glEnd();
    }
}

} // namespace reditor