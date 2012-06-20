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

#ifndef RGRID_OBJ_H
#define RGRID_OBJ_H

#include "REditObj.h"

namespace reditor 
{
/**
 * Base class for all objects which can be painted in the editor window and placed on the grid.
 */
class RGridObj : public REditObj
{  
public:
    explicit RGridObj(QString name);
    RGridObj(QString name, int x, int y, int width, int height);
    virtual ~RGridObj();
    virtual void paintGL() const = 0;
    int x() const
    {
        return mx;
    }
    int y() const
    {
        return my;
    }
    void move(int x, int y);
    
private:
    // x, y coordinate of the object on the grid
    int mx, my;
    // how many cells is used by the object
    int mwidth, mheight;
};

} // namespace reditor

#endif /* RGRID_OBJ_H */
