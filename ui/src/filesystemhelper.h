/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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

#ifndef HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H
#define HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H

#include <QObject>

class FileSystemHelper : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemHelper(QObject *parent = 0);

    Q_INVOKABLE bool exists(const QString& filePath) const;
    Q_INVOKABLE bool isRemovable(const QString& filePath) const;
    Q_INVOKABLE bool isWritable(const QString& filePath) const;

    Q_INVOKABLE bool rename(const QString& filePath, const QString& newName) const;
    Q_INVOKABLE bool remove(const QString& filePath) const;

signals:

public slots:

};

#endif // HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H
