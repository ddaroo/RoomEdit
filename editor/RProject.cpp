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

#include "RProject.h"

namespace reditor
{

RProject::RProject(QString name) : pname(name)
{
}

void RProject::load( REditor *editor ){
	editor->clearProject();

	QFile file( pname );
	file.open( QIODevice::ReadOnly );
	QDataStream in( &file );

	qDebug() << "Loading data from file";

	QString name;
	qint32 val;
	qint32 rot;

	float pos[2];
	float mbeginCorner[2];
	float mendCorner[2];

	for( int i = 0; i < 1000; i++ ){ // maximum number of elements is 1000
		in >> name;

		if( name.isEmpty() )
			break;

		// positions
		in >> val >> val >> val >> val;

		// room
		if( name == "Room" ){
			qDebug() << "Creating room object...";
			in >> mbeginCorner[0] >> mbeginCorner[1] >> mendCorner[0] >> mendCorner[1];

			RObjRoom *room = new RObjRoom();
			room->updateCorners(mbeginCorner, mendCorner);

			editor->addObject(room);
		}
		// grid object
		else if( name == "Grid" ){
			// qDebug() << "Creating grid object...";
			// dont import grid
		}
		// model
		else{
			qDebug() << "Creating model object (" << name << ") ...";
			in >> pos[0] >> pos[1] >> rot;

			RSceneObj *obj = new RSceneObj( name );
			obj->updatePosition( pos );
			obj->updateRotation( rot );

			editor->addObject(obj);
		}
	}
}

void RProject::save( REditor *editor ){
	QFile file( pname );
	file.open( QIODevice::WriteOnly );
	QDataStream out( &file );

	QList<REditObj *> list = ( *editor->objects() );

	foreach(REditObj* obj, list )
	{
		if( editor->inObjectsMode() && obj == list.last() )
			break;

		// TODO: Objects refactoring
		RSceneObj *sobj = dynamic_cast<RSceneObj *>( obj );
		RGridObj  *gobj  = dynamic_cast<RGridObj *>( obj );
		RObjRoom  *robj  = dynamic_cast<RObjRoom *>( obj );

		if( gobj != NULL ){
			out << gobj->name();
			out << (qint32) gobj->x() << (qint32) gobj->y();
			out << (qint32) gobj->width() << (qint32) gobj->height();

			if( robj != NULL ){
				const float * bpos = robj->beginCorner();
				const float * epos = robj->endCorner();

				out << bpos[0] << bpos[1] << epos[0] << epos[1];

			}
			else if( sobj != NULL ){
				const float * pos = sobj->position();
				out << pos[0] << pos[1] << (qint32) sobj->rotation();
			}
		}
	}
}

} // namespace reditor
