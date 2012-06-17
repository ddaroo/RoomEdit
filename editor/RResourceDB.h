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

#ifndef RRESOURCE_DB_H
#define RRESOURCE_DB_H

#include <QtCore/QString>
#include <QtCore/QMap>

namespace reditor
{
    
class RModel3DS;
class RTexture;
  
/**
 * Stores resources - textures and models - which are used in the editor. 
 */
class RResourceDB
{
public:   
    RResourceDB(const QString& textDir, const QString& modDir);
    virtual ~RResourceDB();
    void load();
    RTexture * texture(const QString& name) const;
    RModel3DS * model(const QString& name) const;
    
private:
    QString mtextDir, mmodDir;
    
    typedef QMap<QString, RTexture *> Texs;
    typedef QMap<QString, RTexture *>::const_iterator TexsCit;
    typedef QMap<QString, RTexture *>::iterator TexsIt;
    Texs mtextures;
    
    typedef QMap<QString, RModel3DS *> Mods;
    typedef QMap<QString, RModel3DS *>::const_iterator ModsCit;
    typedef QMap<QString, RModel3DS *>::const_iterator ModsIt;
    Mods mmodels;
};

} // namespace reditor

#endif /* RRESOURCE_DB_H */