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

#include "settings.h"

#include <QCoreApplication>
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

    QString configPath()
    {
        return QString(QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) %
                       QLatin1Char('/') % qApp->applicationName() %
                       QLatin1String("/callrecorder.ini"));
    }

    void readSettings()
    {
        QSettings settings(configPath(), QSettings::IniFormat);

        inputDeviceName = settings.value("deviceName", "source.primary").toString();
        outputLocation = settings.value("outputLocation",
                                        QString(QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                                                QLatin1String("/data"))).toString();                

        QString operationModeStr = settings.value("operationMode", "blacklist").toString();
        operationMode = (operationModeStr == "whitelist"? Settings::WhiteList: Settings::BlackList);

        settings.beginGroup("encoder");
            sampleRate = settings.value("sampleRate", 32000).toInt();
            sampleSize = settings.value("sampleSize", 16).toInt();
            compression = settings.value("compression", 8).toInt();
        settings.endGroup();

        settings.beginGroup("ui");
            locale = settings.value("locale", "system").toString();
        settings.endGroup();
    }

    void saveSettings()
    {
        QSettings settings(configPath(), QSettings::IniFormat);

        settings.setValue("deviceName", inputDeviceName);
        settings.setValue("outputLocation", outputLocation);
        settings.setValue("operationMode",
                          operationMode == Settings::WhiteList?
                              "whitelist":
                              "blacklist");

        settings.beginGroup("encoder");
            settings.setValue("sampleRate", sampleRate);
            settings.setValue("sampleSize", sampleSize);
            settings.setValue("compression", compression);
        settings.endGroup();

        settings.beginGroup("ui");
            settings.setValue("locale", locale);
        settings.endGroup();
    }

    QAudioDeviceInfo inputDevice;

    QString inputDeviceName;

    QString locale;

    Settings::OperationMode operationMode;

    QString outputLocation;

    int compression;
    int sampleRate;
    int sampleSize;
};

Settings::Settings(QObject* parent)
    : QObject(parent),
      d(new SettingsPrivate)
{
    qDebug();

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
    qDebug();
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

int Settings::compression() const
{
    return d->compression;
}

QAudioDeviceInfo Settings::inputDevice() const
{
    return d->inputDevice;
}

QString Settings::locale() const
{
    return d->locale;
}

Settings::OperationMode Settings::operationMode() const
{
    return d->operationMode;
}

QString Settings::outputLocation() const
{
    return d->outputLocation;
}

void Settings::reload()
{
    qDebug();

    d->readSettings();
}

int Settings::sampleRate() const
{
    return d->sampleRate;
}

void Settings::save()
{
    qDebug();

    d->saveSettings();
}

void Settings::setCompression(int compression)
{
    if (d->compression != compression)
    {
        d->compression = compression;

        emit compressionChanged(compression);
        emit settingsChanged();
    }
}

void Settings::setLocale(const QString& locale)
{
    if (d->locale != locale)
    {
        d->locale = locale;

        emit localeChanged(locale);
        emit settingsChanged();
    }
}

void Settings::setOperationMode(OperationMode operationMode)
{
    if (d->operationMode != operationMode)
    {
        d->operationMode = operationMode;

        emit operationModeChanged(operationMode);
        emit settingsChanged();
    }
}

void Settings::setOutputLocation(const QString& outputLocation)
{
    if (d->outputLocation != outputLocation)
    {
        d->outputLocation = outputLocation;

        emit outputLocationChanged(outputLocation);
        emit settingsChanged();
    }
}

void Settings::setSampleRate(int sampleRate)
{
    if (d->sampleRate != sampleRate)
    {
        d->sampleRate = sampleRate;

        emit sampleRateChanged(sampleRate);
        emit settingsChanged();
    }
}

QDebug operator<<(QDebug dbg, Settings::OperationMode state)
{
    dbg << (state == Settings::BlackList? "Black List": "White List");

    return dbg;
}
