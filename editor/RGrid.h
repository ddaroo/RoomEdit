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

#ifndef RGRID_H
#define RGRID_H

#include <QtGui/QColor>

#include "REditObj.h"

namespace reditor 
{
/**
 * Base class for all objects which can be painted in the editor window.
 */
class RGrid : public REditObj
{
    
public:
    /**
     * size - dimensions of the grid: size X size
     * clr - colour of the grid
     * cellSize - size of the single cell in the grid
     */
    RGrid(int size, const QColor& clr, float cellSize);
    virtual ~RGrid();
    virtual void paintGL() const;
    
private:
    // size of the grid
    int msize;
    QColor mclr;
    float mcellSize;
};

} // namespace reditor

#endif /* RGRID_H */