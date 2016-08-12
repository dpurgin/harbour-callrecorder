/*
    Call Recorder for SailfishOS
    Copyright (C) 2014  Dmitriy Purgin <dpurgin@gmail.com>

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef LIBCALLRECORDER_CALLRECORDEREXCEPTION_H
#define LIBCALLRECORDER_CALLRECORDEREXCEPTION_H

#include <exception>

#include <QString>

#include "config.h"

class LIBCALLRECORDER_DECL CallRecorderException : public std::exception
{
public:
    CallRecorderException(const QString& what): mWhat(what) { }
    virtual ~CallRecorderException() throw()
    {
    }

    const char* what() const throw() { return mWhat.toUtf8().data(); }
    QString qWhat() { return mWhat; }

private:
    QString mWhat;
};

#endif // LIBCALLRECORDER_CALLRECORDEREXCEPTION_H
