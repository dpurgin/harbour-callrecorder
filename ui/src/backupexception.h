/*
    Call Recorder for SailfishOS
    Copyright (C) 2016 Dmitriy Purgin <dpurgin@gmail.com>

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

#ifndef HARBOUR_CALLRECORDER_BACKUPEXCEPTION_H
#define HARBOUR_CALLRECORDER_BACKUPEXCEPTION_H

#include <libcallrecorder/callrecorderexception.h>

#include "backuphelper.h"

class BackupException : public CallRecorderException
{
public:
    explicit BackupException(BackupHelper::ErrorCode errorCode, QString what)
        : CallRecorderException(what),
          mErrorCode(errorCode)
    {
    }

    BackupHelper::ErrorCode errorCode() const { return mErrorCode; }

private:
    BackupHelper::ErrorCode mErrorCode = BackupHelper::ErrorCode::None;
};

#endif
