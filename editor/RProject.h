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

#ifndef RProject_H
#define RProject_H

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QDataStream>

#include "REditor.h"
#include "REditObj.h"
#include "RSceneObj.h"
#include "RGridObj.h"
#include "RObjRoom.h"


namespace reditor
{

class RProject
{
public:
	RProject(QString name);
	void save( REditor *editor );
	void load( REditor *editor );

private:
    QString pname;
};
} // namespace reditor

#endif /* RProject_H */
