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

#ifndef HARBOUR_CALLRECORDERD_APPLICATION_H
#define HARBOUR_CALLRECORDERD_APPLICATION_H

#include <QGuiApplication>
#include <QDateTime>
#include <QScopedPointer>
#include <QSharedPointer>

#include "voicecallrecorder.h"

#define daemon (reinterpret_cast< Application* >(qApp))

class Database;
class Model;
class Settings;
class SqlCursor;

class QtPulseAudioSource;

class Application : public QGuiApplication
{
    Q_OBJECT
    Q_DISABLE_COPY(Application)

public:
    // Workaround for https://bugreports.qt-project.org/browse/QTBUG-26161
    static QString getIsoTimeStamp(QDateTime dt)
    {
        QDateTime utc = dt.toUTC();
        utc.setTimeSpec(Qt::LocalTime);

        int utcOffset = utc.secsTo(dt);

        dt.setUtcOffset(utcOffset);

        return dt.toString(Qt::ISODate);
    }

public:
    explicit Application(int argc, char* argv[]);
    virtual ~Application();

    Database* database() const;
    Model* model() const;    
    Settings* settings() const;

public slots:

private slots:
    void checkStorageLimits();

    void initVoiceCallManager(const QString& objectPath);

    void maybeSwitchProfile();

    void onPulseAudioConnected();
    void onPulseAudioError(QString error);

    void onPulseAudioCardActiveProfileChanged(QString profileName);
    void onPulseAudioSinkActivePortChanged(QString portName);
    void onPulseAudioSourceAdded(QSharedPointer< QtPulseAudioSource > source);
    void onPulseAudioSourceRemoved(QSharedPointer< QtPulseAudioSource > source);

    void onVoiceCallAdded(const QString& objectPath);
    void onVoiceCallRecorderStateChanged(VoiceCallRecorder::State state);
    void onVoiceCallRemoved(const QString& objectPath);

private:
    void checkStorageAgeLimits();
    void checkStorageSizeLimits();
    void removeEvents(SqlCursor* cursor);

private:
    class ApplicationPrivate;
    QScopedPointer< ApplicationPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_APPLICATION_H
