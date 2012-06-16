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

#ifndef TEXTUREBMP_H
#define TEXTUREBMP_H

#ifdef Q_WS_WIN
#  undef UNICODE
#  define WIN32_LEAN_AND_MEAN
#  include <windows.h>
#endif

#include <QtCore/QString>
#include <QtGui/QImage>
#include <iostream>
#include <fstream>
#include "GL/gl.h"
#include "GL/glu.h"

namespace reditor
{

typedef unsigned char byte;

class RTexture{
private:
    int m_width;
    int m_height;
    int m_bpp;
    QImage::Format m_format;
    GLuint m_id;
    
public:
    RTexture(const char * filename);
    RTexture(std::string filename)
    {
        RTexture(filename.c_str());
    }
    RTexture(const QString& filename)
    {
        RTexture(filename.toStdString().c_str());
    }
    
    int getWidth(){return m_width;}
    int getHeight(){return m_width;}
    int getBpp(){return m_bpp;}
    GLuint id(){return m_id;}
};

} // namespace editor

#endif //TEXTUREBMP_H
