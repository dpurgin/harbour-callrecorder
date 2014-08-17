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
