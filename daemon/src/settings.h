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

#ifndef HARBOUR_CALLRECORDERD_SETTINGS_H
#define HARBOUR_CALLRECORDERD_SETTINGS_H

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QScopedPointer>

class Settings
{
    Q_DISABLE_COPY(Settings)

public:
    Settings();
    ~Settings();

    QAudioFormat audioFormat() const;
    QAudioDeviceInfo inputDevice() const;
    QString outputLocation() const;

private:
    class SettingsPrivate;
    QScopedPointer< SettingsPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_SETTINGS_H