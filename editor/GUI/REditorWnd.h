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

#ifndef REDITOR_WND_H
#define REDITOR_WND_H

#include <QGLWidget>

namespace reditor
{

class REditorWnd : public QGLWidget
{
    Q_OBJECT

public:
    REditorWnd(QWidget *parent = 0);
    virtual ~REditorWnd();

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);
    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent *event);
    void keyPressEvent(QKeyEvent * event);
    
private:
    void liczWsp(GLdouble begin[3],GLdouble end[3], float point[2]);
    void drawFloor();
    void walls();
    void rect();
    void grid();
};

} /* namespace reditor */

#endif /* REDITOR_WND_H */
