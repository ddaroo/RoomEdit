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

#ifndef RCAMERA_H
#define RCAMERA_H

namespace reditor
{

struct RCamera
{
    enum CameraMode {NORMAL = 1, FLAT, INSIDE};
    CameraMode mode;
    double kX , kY, kZ, pX, pY, pZ;
    int upX, upY, upZ;
    
    RCamera() : mode(NORMAL), kX(0.0), kY(15.0), kZ(25.0), pX(0.0), pY(0.0), pZ(0.0), upX(0), upY(1), upZ(0)
    {}
};

} // namespace reditor

#endif /* RCAMERA_H */