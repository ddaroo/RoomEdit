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

#ifndef RCONFIG_H
#define RCONFIG_H

#include <QtCore/QString>
#include <QtCore/QCoreApplication>

namespace reditor
{

/**
 * Stores various configuration parameters.
 */

class RConfig
{        
    public:
        static const char * dataDir(const char * path)
        {
            return QCoreApplication::applicationDirPath().append("/data/").append(path).toStdString().c_str();
        }
        
        static const char * texturesDir(const char * path)
        {
            return QCoreApplication::applicationDirPath().append("/data/textures/").append(path).toStdString().c_str();
        }
        
        static const char * modelsDir(const char * path)
        {
            return QCoreApplication::applicationDirPath().append("/data/models/").append(path).toStdString().c_str();
        }
        /**
         * Extension for the saved project files
         */
        static QString fileEditExt;
};
    
} // namespace reditor

#endif /* RCONFIG_H */