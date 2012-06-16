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

#include <QtCore/QFileInfo>
#include <QtCore/QDir>
#include <QtCore/QDebug>
#include "RResourceDB.h"
#include "RModel3DS.h"
#include "RTexture.h"

namespace reditor
{

RResourceDB::RResourceDB(const QString& textDir, const QString& modDir) :
    mtextDir(textDir), mmodDir(modDir), mtextures()
{ 

}

RResourceDB::~RResourceDB()
{
    for(TexsIt it = mtextures.begin(); it != mtextures.end(); ++it)
    {
        delete it.value();
    }
    for(ModsIt it = mmodels.begin(); it != mmodels.end(); ++it)
    {
        delete it.value();
    }
}


void RResourceDB::load()
{
    // load textures from the directory
    RTexture * tex;
    QDir tdir(mtextDir);
    foreach(QFileInfo finfo, tdir.entryInfoList())
    {
        if (finfo.isFile())
        {
            tex = new RTexture(finfo.absoluteFilePath());
            mtextures.insert(finfo.baseName(), tex);
            qDebug() << "Texture " << finfo.baseName() << "[" <<  tex->id() << "]" << " loaded from the file " << finfo.absoluteFilePath();
        }
    }
    // load models from the directory
    model3DS * mod;
    QDir mdir(mmodDir);
    foreach(QFileInfo finfo, mdir.entryInfoList())
    {
        if (finfo.isFile() && finfo.suffix().toLower() == "3ds")
        {
            mod = new model3DS(finfo.absoluteFilePath().toStdString().c_str(), 1.0f);
            mmodels.insert(finfo.baseName(), mod);
            qDebug() << "Model " << finfo.baseName() << " loaded from the file " << finfo.absoluteFilePath();
        }
    }
}

RTexture * RResourceDB::texture(const QString& name) const
{
    TexsCit cit = mtextures.constFind(name);
    if(cit == mtextures.end())
    {
        // TODO it would be better to use some default texture instead of hard crash
        return 0;
    }
    return cit.value();
}

model3DS * RResourceDB::model(const QString& name) const
{
    ModsCit cit = mmodels.constFind(name);
    if(cit == mmodels.end())
    {
        // TODO it would be better to use some default model instead of hard crash
        return 0;
    }
    return cit.value();
}
    
} // namespace reditor