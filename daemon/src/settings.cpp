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
#include <QSettings>
#include <QStandardPaths>
#include <QStringBuilder>

class Settings::SettingsPrivate
{
    friend class Settings;

    void readSettings()
    {
        QSettings settings(QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                                   QLatin1String("/callrecorder.ini")),
                           QSettings::IniFormat);

        settings.beginGroup("general");
            inputDeviceName = settings.value("deviceName", "source.primary").toString();
            outputLocation = settings.value("outputLocation",
                                            QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                                                    QLatin1String("/data"))).toString();
        settings.endGroup();

        settings.beginGroup("encoder");
            sampleRate = settings.value("sampleRate", 22050).toInt();
            sampleSize = settings.value("sampleSize", 16).toInt();
        settings.endGroup();
    }

    QAudioDeviceInfo inputDevice;

    QString inputDeviceName;

    QString outputLocation;

    int sampleRate;
    int sampleSize;
};

Settings::Settings()
    : d(new SettingsPrivate)
{
    qDebug() << __PRETTY_FUNCTION__;

    d->readSettings();

    foreach (QAudioDeviceInfo deviceInfo, QAudioDeviceInfo::availableDevices(QAudio::AudioInput))
    {
        if (deviceInfo.deviceName() == d->inputDeviceName)
        {
            d->inputDevice = deviceInfo;
            break;
        }
    }

    if (d->inputDevice.isNull())
    {
        qDebug() << ": unable to find " << d->inputDeviceName;

        d->inputDevice = QAudioDeviceInfo::defaultInputDevice();

        qDebug() << ": fallen back to " << d->inputDevice.deviceName();
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
    format.setSampleRate(d->sampleRate);
    format.setSampleSize(d->sampleSize);
    format.setSampleType(QAudioFormat::SignedInt);

    return d->inputDevice.nearestFormat(format);
}

QAudioDeviceInfo Settings::inputDevice() const
{
    return d->inputDevice;
}

QString Settings::outputLocation() const
{
    return d->outputLocation;
}
