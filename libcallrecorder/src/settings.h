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

#ifndef LIBCALLRECORDER_SETTINGS_H
#define LIBCALLRECORDER_SETTINGS_H

#include <QAudioFormat>
#include <QAudioDeviceInfo>
#include <QScopedPointer>

#include "config.h"

class LIBCALLRECORDER_DECL Settings : public QObject
{
    Q_OBJECT

    Q_DISABLE_COPY(Settings)

    Q_PROPERTY(int compression
               READ compression
               WRITE setCompression
               NOTIFY compressionChanged)

    Q_PROPERTY(QString locale
               READ locale
               WRITE setLocale
               NOTIFY localeChanged)

    Q_PROPERTY(QString outputLocation
               READ outputLocation
               WRITE setOutputLocation
               NOTIFY outputLocationChanged)

    Q_PROPERTY(int sampleRate
               READ sampleRate
               WRITE setSampleRate
               NOTIFY sampleRateChanged)

    Q_PROPERTY(OperationMode operationMode
               READ operationMode
               WRITE setOperationMode
               NOTIFY operationModeChanged)

    Q_PROPERTY(bool limitStorage
               READ limitStorage
               WRITE setLimitStorage
               NOTIFY limitStorageChanged)

    Q_PROPERTY(int maxStorageAge
               READ maxStorageAge
               WRITE setMaxStorageAge
               NOTIFY maxStorageAgeChanged)

    Q_PROPERTY(int maxStorageSize
               READ maxStorageSize
               WRITE setMaxStorageSize
               NOTIFY maxStorageSizeChanged)
public:
    enum OperationMode
    {
        BlackList,
        WhiteList
    };

    Q_ENUMS(OperationMode)

public:
    Settings(QObject* parent = NULL);
    ~Settings();

    QAudioFormat audioFormat() const;
    QAudioDeviceInfo inputDevice() const;

    int compression() const;
    void setCompression(int compression);

    bool limitStorage() const;
    void setLimitStorage(bool limitStorage);

    QString locale() const;
    void setLocale(const QString& locale);

    int maxStorageAge() const;
    void setMaxStorageAge(int age);

    int maxStorageSize() const;
    void setMaxStorageSize(int size);

    OperationMode operationMode() const;
    void setOperationMode(OperationMode operationMode);

    QString outputLocation() const;
    void setOutputLocation(const QString& outputLocation);

    int sampleRate() const;
    void setSampleRate(int sampleRate);

public slots:
    void reload();
    void save();

signals:
    void compressionChanged(int compression);
    void limitStorageChanged(bool limitStorage);
    void localeChanged(QString locale);
    void maxStorageAgeChanged(int age);
    void maxStorageSizeChanged(int size);
    void operationModeChanged(OperationMode operationMode);
    void outputLocationChanged(QString outputLocation);
    void sampleRateChanged(int sampleRate);
    void settingsChanged();

private:
    class SettingsPrivate;
    QScopedPointer< SettingsPrivate > d;
};

LIBCALLRECORDER_DECL QDebug operator<<(QDebug dbg, Settings::OperationMode state);
LIBCALLRECORDER_DECL void initQml();

#endif // LIBCALLRECORDER_SETTINGS_H
