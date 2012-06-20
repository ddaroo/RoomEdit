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

#include "global.h"
#include "RSceneObj.h"
#include "RModel3DS.h"

namespace reditor
{

RSceneObj::RSceneObj(QString name) : RGridObj(name), mrotation(0), mmodel(rscDB.model(name))
{
}
    
void RSceneObj::paintGL() const
{
    glPushMatrix();
        glTranslatef(mpos[0], 0.07f, mpos[1]);
        glRotatef(mrotation, 0, 1, 0);
        mmodel->draw();
    glPopMatrix();
}

void RSceneObj::updatePosition(float position[2])
{
    mpos[0] = position[0];
    mpos[1] = position[1];
}

void RSceneObj::updateRotation(int r)
{
    mrotation = r;
}


} // namespace reditor
