#include "settings.h"

#include <QAudioDeviceInfo>
#include <QAudioFormat>
#include <QDebug>
#include <QStandardPaths>

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
