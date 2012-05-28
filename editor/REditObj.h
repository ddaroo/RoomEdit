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

#ifndef REDIT_OBJ_H
#define REDIT_OBJ_H

#include <QtCore/QString>

namespace reditor
{
/**
 * Base class for all objects which can be painted in the editor window.
 */
class REditObj
{

public:
    REditObj();
    REditObj(QString name);
    virtual ~REditObj();
    /**
     * Paint object in the editor window
     */
    virtual void paintGL() const = 0;
    /**
     * Returns human readable name of the object
     */
    QString name() const
    {
        return mname;
    }

private:
    // human readable name of the object
    QString mname;
};

} // namespace reditor

#endif /* REDIT_OBJECT_H */
