/*
    BMP texture loader
    � Keith O'Conor 2005
    keith.oconor @ {cs.tcd.ie, gmail.com}
*/

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

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtGui/QImage>
#include "RTexture.h"

namespace reditor
{

RTexture::RTexture(const char * filename) : 
    m_format(QImage::Format_RGB888), m_id(0)
{    
    QImage img(filename), copy;
    QFileInfo info(filename);
    
    if(img.isNull())
    {
        qCritical() << "Couldn't load texture from the file " << info.absoluteFilePath();
        return;
    }
    
    m_width = img.width();
    m_height = img.height();
    m_bpp = img.depth();
    
    copy = img.convertToFormat(m_format);
    
    glGenTextures(1, &m_id);
    // Bind texture ID to load
    glBindTexture(GL_TEXTURE_2D, m_id);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
    
    // Upload texture to card with bound texture ID
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, m_width, m_height, 0, GL_RGB, GL_UNSIGNED_BYTE, copy.bits());
    gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_width, m_height, GL_RGB, GL_UNSIGNED_BYTE, copy.bits());
    //Q_ASSERT(glIsTexture(m_id));
//     uchar texs[1024*1024];
//     for(int i = 0; i < 1024*1024; ++i)
//         texs[i] = i;
//     gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, m_width, m_height, GL_RGB8, GL_UNSIGNED_BYTE, texs);
}

} // namespace editor