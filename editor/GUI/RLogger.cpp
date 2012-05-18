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

#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QTime>
#include <QtGui/QTextEdit>

#include "RLogger.h"

QStringList logMsgs;

void msgOutput(QtMsgType type, const char *msg)
{
    QString smsg = QTime::currentTime().toString("hh:mm:ss");
    switch (type) {
    case QtDebugMsg:
        smsg.append(" |DEBUG| ");
        break;
    case QtWarningMsg:
        smsg.append(" <span style=\"color: #fbdd00;\">|WARNING|</span> ");
        break;
    case QtCriticalMsg:
        smsg.append(" <span style=\"color: #f00;\">|CRITICAL|</span> ");
        break;
    case QtFatalMsg:
        smsg.append(" <span style=\"color: #f00;\">|FATAL|</span> ");
        break;
    }
    smsg.append(msg);
    // this function can be called by many threads, so separate GUI from it
    logMsgs.append(smsg);
} 

namespace reditor
{

RLogger::RLogger(QWidget * parent) : QTextEdit(parent)
{
    setReadOnly(true);
    mtimer = new QTimer(this);
    // update logger window every 100 msec
    connect(mtimer, SIGNAL(timeout()), this, SLOT(hlogMsgs()));
    mtimer->start(100);
}

RLogger::~RLogger()
{
    delete mtimer;
}

void RLogger::hlogMsgs()
{
    // display pending messages
    foreach(QString msg, logMsgs)
    {
        append(msg);
    }
    logMsgs.clear();
}

} /* namespace reditor */