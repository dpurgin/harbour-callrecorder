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

#ifndef HARBOUR_CALLRECORDER_BACKUPHELPER_H
#define HARBOUR_CALLRECORDER_BACKUPHELPER_H

#include <QObject>

class BackupHelper : public QObject
{
    Q_OBJECT

    Q_PROPERTY(bool busy
               READ busy
               NOTIFY busyChanged)
    Q_PROPERTY(int progress
               READ progress
               NOTIFY progressChanged)
    Q_PROPERTY(int totalCount
               READ totalCount
               NOTIFY totalCountChanged)

public:
    BackupHelper(QObject* parent = nullptr);
    virtual ~BackupHelper();

    Q_INVOKABLE bool backup(const QString& fileName);
    Q_INVOKABLE void restore(const QString& fileName);

    bool busy() const { return mBusy; }
    int progress() const { return mProgress; }
    int totalCount() const { return mTotalCount; }

signals:
    void busyChanged(bool);
    void totalCountChanged(int);
    void progressChanged(int);

private:
    void setBusy(bool busy)
    {
        if (busy != mBusy)
            emit busyChanged(mBusy = busy);
    }

    void setProgress(int progress)
    {
        if (progress != mProgress)
            emit progressChanged(mProgress = progress);
    }

    void setTotalCount(int totalCount)
    {
        if (totalCount != mTotalCount)
            emit totalCountChanged(mTotalCount = totalCount);
    }

private:
    bool mBusy = false;
    int mProgress = 0;
    int mTotalCount = 0;
};

#endif // HARBOUR_CALLRECORDER_BACKUPHELPER_H
