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

#include "settings.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QDebug>
#include <QDir>
#include <QStandardPaths>
#include <QStringBuilder>

class Settings::SettingsPrivate
{
    friend class Settings;

    QAudioDeviceInfo inputDevice;
};

Settings::Settings()
    : d(new SettingsPrivate)
{
    qDebug() << __PRETTY_FUNCTION__;

    foreach (QAudioDeviceInfo deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        if (deviceInfo.deviceName() == QLatin1String("source.primary"))
        {
            d->inputDevice = deviceInfo;
            break;
        }
    }

    if (d->inputDevice.isNull())
    {
        qDebug() << __PRETTY_FUNCTION__ << ": unable to find source.primary";

        d->inputDevice = QAudioDeviceInfo::defaultInputDevice();

        qDebug() << __PRETTY_FUNCTION__ << ": fallen back to " << d->inputDevice.deviceName();
    }

    QDir().mkpath(outputLocation());
}

Settings::~Settings()
{
    qDebug() << __PRETTY_FUNCTION__;
}

QAudioFormat Settings::audioFormat() const
{
    QAudioFormat format;
    format.setByteOrder(QAudioFormat::LittleEndian);
    format.setChannelCount(1);
    format.setCodec("audio/pcm");
    format.setSampleRate(44100);
    format.setSampleSize(16);
    format.setSampleType(QAudioFormat::SignedInt);

    return d->inputDevice.nearestFormat(format);
}

QAudioDeviceInfo Settings::inputDevice() const
{
    return d->inputDevice;
}

QString Settings::outputLocation() const
{
    static QString outputLocation(QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                                  QLatin1String("/data"));

    return outputLocation;
}
