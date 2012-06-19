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

#include "RGridObj.h"

namespace reditor
{

RGridObj::RGridObj(QString name) : REditObj(name)
{

}
    
RGridObj::RGridObj(QString name, int x, int y, int width, int height) 
    : REditObj(name), mx(x), my(y), mwidth(width), mheight(height)
{

}

RGridObj::~RGridObj()
{

}

void RGridObj::move(int x, int y)
{
    mx = x;
    my = y;
}
    
} // namespace reditor
