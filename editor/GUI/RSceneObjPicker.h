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

#ifndef RSCENE_OBJ_PICKER_H
#define RSCENE_OBJ_PICKER_H

#include <QGLWidget>

namespace reditor
{

class RModel3DS;
    
class RSceneObjPicker : public QGLWidget
{
    Q_OBJECT

public:
    RSceneObjPicker(const QString& modelName, QWidget *parent = 0);
    virtual ~RSceneObjPicker();

signals:
    void selected(QString modelName);

protected:
    void initializeGL();
    void paintGL();
    void enterEvent(QEvent * event);
    void leaveEvent(QEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    
private:
    // starts/stops animation
    void animate(bool animate);
    
    RModel3DS * mmodel;
    QString mmodelName;
};

} /* namespace reditor */

#endif /* RSCENE_OBJ_PICKER_H */