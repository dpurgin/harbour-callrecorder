/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2016 Dmitriy Purgin <dpurgin@gmail.com>

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

#include "application.h"

#include <QDir>
#include <QTimer>
#include <QQmlContext>
#include <QQuickItem>
#include <QQuickView>
#include <QQmlEngine>

#include <qofonomanager.h>
#include <qofonovoicecallmanager.h>

#include <libcallrecorder/callrecorderexception.h>
#include <libcallrecorder/database.h>
#include <libcallrecorder/eventstablemodel.h>
#include <libcallrecorder/libcallrecorder.h>
#include <libcallrecorder/settings.h>
#include <libcallrecorder/sqlcursor.h>

#include <qpa/qplatformnativeinterface.h>

#include <qtpulseaudio/qtpulseaudioconnection.h>
#include <qtpulseaudio/qtpulseaudiocard.h>
#include <qtpulseaudio/qtpulseaudiocardprofile.h>
#include <qtpulseaudio/qtpulseaudioserver.h>
#include <qtpulseaudio/qtpulseaudiosink.h>
#include <qtpulseaudio/qtpulseaudiosinkport.h>
#include <qtpulseaudio/qtpulseaudiosource.h>

#include "dbusadaptor.h"
#include "uidbusinterface.h"
#include "model.h"
#include "voicecallrecorder.h"

class Application::ApplicationPrivate
{
    Q_DISABLE_COPY(ApplicationPrivate)

    friend class Application;

private:
    explicit ApplicationPrivate()
        : active(false),
          pulseAudioCard(NULL),
          wantPark(false),
          needResetDefaultSource(false)
    {}

private:
    bool active;

    QScopedPointer< Database > database;

    QScopedPointer< DBusAdaptor > dbusAdaptor;
    QScopedPointer< UiDBusInterface > uiInterface;

    QScopedPointer< Model > model;

    QSharedPointer< QtPulseAudioCard > pulseAudioCard;
    QSharedPointer< QtPulseAudioSink > pulseAudioSink;
    QScopedPointer< QtPulseAudioConnection > pulseAudioConnection;

    QScopedPointer< QOfonoManager > qofonoManager;
    QScopedPointer< QOfonoVoiceCallManager > qofonoVoiceCallManager;

    QScopedPointer< Settings > settings;

    QScopedPointer< QTimer > timer;
    QScopedPointer< QTimer > storageLimitTimer;

    QScopedPointer< QQuickView > approvalView;

    // stores object paths and its recorders
    QHash< QString, VoiceCallRecorder* > voiceCallRecorders;

    bool wantPark;
    QString wantPort;

    // workaround for Android mic issue
    bool needResetDefaultSource;

    QScopedPointer< QTranslator > daemonTranslator;
};


Application::Application(int argc, char* argv[])
    : QGuiApplication(argc, argv),
      d(new ApplicationPrivate())
{
    qDebug();

    setApplicationName(QLatin1String("harbour-callrecorder"));
    setOrganizationName(QLatin1String("kz.dpurgin"));

    LibCallRecorder::installMessageHandler();

    d->settings.reset(new Settings());

    d->daemonTranslator.reset(LibCallRecorder::createTranslator("daemon"));
    installTranslator(d->daemonTranslator.data());

    d->dbusAdaptor.reset(new DBusAdaptor(this));
    d->uiInterface.reset(new UiDBusInterface(this));

    connect(d->uiInterface.data(), SIGNAL(SettingsChanged()),
            d->settings.data(), SLOT(reload()));

    d->database.reset(new Database());
    d->model.reset(new Model(d->database.data()));

    d->qofonoManager.reset(new QOfonoManager());

    // Ofono availability check removed.
    // See https://github.com/dpurgin/harbour-callrecorder/pull/22

    // check if modems available. If there are modems, take the first one and initialize app with it
    // If no modems available, wait for first modemAdded and initialize with this one

    QStringList modems = d->qofonoManager->modems();

    if (modems.length() == 0)
    {
        qWarning() << QLatin1String("No modems available! Waiting for availableChanged");

        connect(d->qofonoManager.data(), &QOfonoManager::availableChanged,
                this, &Application::onOfonoAvailableChanged);
    }
    else
        initVoiceCallManager(modems.first());

#ifdef QT_DEBUG
    connect(d->dbusAdaptor.data(), &DBusAdaptor::EmulatedVoiceCallAdded,
            this, &Application::onVoiceCallAdded);
    connect(d->dbusAdaptor.data(), &DBusAdaptor::EmulatedVoiceCallRemoved,
            this, &Application::onVoiceCallRemoved);
#endif // QT_DEBUG

    d->pulseAudioConnection.reset(
        new QtPulseAudioConnection(QtPulseAudio::Card |
                                   QtPulseAudio::Server |
                                   QtPulseAudio::Source |
                                   QtPulseAudio::Sink));

    connect(d->pulseAudioConnection.data(), SIGNAL(connected()),
            this, SLOT(onPulseAudioConnected()));
    connect(d->pulseAudioConnection.data(), SIGNAL(error(QString)),
            this, SLOT(onPulseAudioError(QString)));

    d->pulseAudioConnection->connectToServer();

    d->storageLimitTimer.reset(new QTimer());
    d->storageLimitTimer->setInterval(3600000); // check once in an hour

    connect(d->storageLimitTimer.data(), SIGNAL(timeout()),
            this, SLOT(checkStorageLimits()));

    d->storageLimitTimer->start();

    // restart storage limit timer after settings were changed.
    // this is needed to avoid firing cleanup when user is in
    // process of changing limit values

    connect(d->uiInterface.data(), SIGNAL(SettingsChanged()),
            d->storageLimitTimer.data(), SLOT(start()));

    // The following code block is used for debugging purposes only and MUST be
    // commented out or removed for any usage of the app
//#ifdef QT_DEBUG
//#pragma message "Debugging block is not removed!"
//    QTimer* timer = new QTimer();
//    connect(timer, &QTimer::timeout,
//            [&]() { showApprovalDialog(); });
//    timer->setSingleShot(true);
//    timer->start(3000);
//#endif // QT_DEBUG
}

Application::~Application()
{
    qDebug();
}

void Application::checkStorageAgeLimits()
{
    qDebug();

    if (d->settings->limitStorage() && d->settings->maxStorageAge() > 0)
    {
        // compute time stamp to keep recordings from
        QDateTime sliceDt = QDateTime::currentDateTime().addDays(-d->settings->maxStorageAge());

        // retrieve file names to remove
        static QString selectStmt("SELECT ID, FileName FROM Events WHERE TimeStamp <= :timeStamp");

        Database::SqlParameters params;
        params.insert(":timeStamp", sliceDt);

        QScopedPointer< SqlCursor > cursor(d->database->select(selectStmt, params));

        if (!cursor.isNull())
            removeEvents(cursor.data());
        else
            qWarning() << d->database->lastError();
    }
}

void Application::checkStorageLimits()
{
    qDebug();

    checkStorageAgeLimits();
    checkStorageSizeLimits();

    // this is clumsy but will cause UI to reload its list
    emit d->dbusAdaptor->RecorderStateChanged();

    d->storageLimitTimer->start();
}

void Application::checkStorageSizeLimits()
{
    qDebug();

    if (d->settings->limitStorage() && d->settings->maxStorageSize() > 0)
    {
        // compute limit in bytes. Settings contain it in megabytes
        quint64 byteLimit = d->settings->maxStorageSize() * 1024 * 1024;

        // now select all files that are out of size limit

        static QString selectStmt(
            "\nSELECT"
            "\n    Events.ID,"
            "\n    Events.FileName,"
            "\n    SUM(NextEvents.FileSize) AS AccumulatedFileSize"
            "\nFROM"
            "\n    Events"
            "\n"
            "\n    LEFT JOIN"
            "\n        Events AS NextEvents"
            "\n    ON"
            "\n        Events.ID < NextEvents.ID"
            "\nGROUP BY"
            "\n    Events.ID,"
            "\n    Events.FileName"
            "\nHAVING"
            "\n    SUM(NextEvents.FileSize) > %1"
            "\nORDER BY"
            "\n    Events.ID DESC");

        QScopedPointer< SqlCursor > cursor(d->database->select(selectStmt.arg(byteLimit)));

        if (!cursor.isNull())
            removeEvents(cursor.data());
        else
            qWarning() << d->database->lastError();
    }
}

void Application::createApprovalDialog()
{
    qDebug();

    // taken from libsailfishapp
    QQuickView::setDefaultAlphaBuffer(true);

    d->approvalView.reset(new QQuickView());

    // set transparent background

    d->approvalView->setColor(QColor(0, 0, 0, 0));
    d->approvalView->setClearBeforeRendering(true);

    // set up QML scene

    d->approvalView->engine()->addImportPath("/usr/share/harbour-callrecorder/lib/imports");
    d->approvalView->setSource(
                QUrl::fromLocalFile("/usr/share/harbour-callrecorder/qml/approval.qml"));
    d->approvalView->create();

    // connect to QML signals to react to user actions

    QQuickItem* dialog =
            d->approvalView->rootObject()->findChild< QQuickItem* >("approvalDialogWindow");

    connect(dialog, SIGNAL(askLaterClicked(int)),
            this, SLOT(onApprovalDialogAskLater(int)));

    connect(dialog, SIGNAL(removeClicked(int)),
            this, SLOT(onApprovalDialogRemove(int)));

    connect(dialog, SIGNAL(storeClicked(int)),
            this, SLOT(onApprovalDialogStore(int)));

    // set window category for lipstick compositor

    QPlatformNativeInterface *native = QGuiApplication::platformNativeInterface();
    native->setWindowProperty(d->approvalView->handle(),
                              QLatin1String("CATEGORY"),
                              QLatin1String("notification"));
}

Database* Application::database() const
{
    return d->database.data();
}

DBusAdaptor* Application::dbusAdaptor() const
{
    return d->dbusAdaptor.data();
}

Model* Application::model() const
{
    return d->model.data();
}

/// Connects to Ofono voice call add/remove signals using the modem given
void Application::initVoiceCallManager(const QString& objectPath)
{
    qDebug() << objectPath;

    d->qofonoVoiceCallManager.reset(new QOfonoVoiceCallManager());

    d->qofonoVoiceCallManager->setModemPath(objectPath);

    // connect to voice call signals from Ofono voice call manager

    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callAdded(QString)),
            this, SLOT(onVoiceCallAdded(QString)));
    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callRemoved(QString)),
            this, SLOT(onVoiceCallRemoved(QString)));
    connect(d->qofonoVoiceCallManager.data(), SIGNAL(callRemoved(QString)),
            this, SLOT(checkStorageLimits()));

    // check if there are any active calls on start
    QStringList activeCalls = d->qofonoVoiceCallManager->getCalls();

    foreach (QString call, activeCalls)
        onVoiceCallAdded(call);

    // ofono manager is not needed now
    d->qofonoManager->deleteLater();
}

void Application::maybeSwitchProfile()
{
    if (d->pulseAudioCard.isNull() || d->pulseAudioSink.isNull())
    {
        qWarning() << "Not managing PulseAudio: unexpected card and sink detected (see the startup logs)";
        return;
    }

    qDebug() << QThread::currentThread() <<
                "Card profile: " << d->pulseAudioCard->activeProfile()->name() <<
                ", sink port: " << d->pulseAudioSink->activePort()->name() <<
                ", want park: " << d->wantPark <<
                ", want port: " << d->wantPort;

    if (d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall"))
    {
        qDebug() << "Switching profile to voicecall-record";

        d->pulseAudioCard->setActiveProfile(QLatin1String("voicecall-record"));              

        d->wantPark = true;
        d->wantPort = d->pulseAudioSink->activePort()->name();

        qDebug() << "Now want park and port" << d->wantPort;
    }
    else if (d->pulseAudioCard->activeProfile()->name() == QLatin1String("voicecall-record"))
    {
        if (d->pulseAudioSink->activePort()->name() == QLatin1String("output-parking"))
        {
            qDebug() << "Port parked -- switching to " << d->wantPort;

            d->pulseAudioSink->setActivePort(d->wantPort);
        }
        else if (d->wantPark || d->pulseAudioSink->activePort()->name() != d->wantPort)
        {
            qDebug() << "Now want port " << d->pulseAudioSink->activePort()->name();

            d->wantPark = false;
            d->wantPort = d->pulseAudioSink->activePort()->name();
            d->pulseAudioSink->setActivePort(QLatin1String("output-parking"));
        }
    }
    else
        qDebug() << "Not managing profile " << d->pulseAudioCard->activeProfile()->name();
}

void Application::onApprovalDialogAskLater(int eventId)
{
    Q_UNUSED(eventId);

    qDebug();

    d->approvalView->setVisible(false);
}

void Application::onApprovalDialogRemove(int eventId)
{
    qDebug();

    model()->events()->remove(eventId);

    emit d->dbusAdaptor->RecorderStateChanged();

    showApprovalDialog();
}

void Application::onApprovalDialogStore(int eventId)
{
    qDebug();

    QVariantMap params;
    params.insert("RecordingStateID", static_cast< int >(EventsTableModel::Done));

    model()->events()->update(eventId, params);

    emit d->dbusAdaptor->RecorderStateChanged();

    showApprovalDialog();
}

void Application::onOfonoAvailableChanged(bool available)
{
    if (available && d->qofonoManager->modems().size() > 0)
        initVoiceCallManager(d->qofonoManager->modems().first());
}

void Application::onPulseAudioCardActiveProfileChanged(QString profileName)
{
    if (d->active)
    {
        qDebug() << QThread::currentThread() << ": active profile: " << profileName;

        maybeSwitchProfile();
    }
}

void Application::onPulseAudioConnected()
{
    qDebug() << QThread::currentThread() << ": connected to PulseAudio";

    d->timer.reset(new QTimer());
    d->timer->setInterval(250);
    d->timer->setSingleShot(true);
    connect(d->timer.data(), SIGNAL(timeout()),
            this, SLOT(maybeSwitchProfile()));

    d->pulseAudioCard = d->pulseAudioConnection->cardByIndex(0);
    d->pulseAudioSink = d->pulseAudioConnection->sinkByName("sink.primary");

    if (!d->pulseAudioCard.isNull())
    {
        connect(d->pulseAudioCard.data(), SIGNAL(activeProfileChanged(QString)),
                this, SLOT(onPulseAudioCardActiveProfileChanged(QString)));
    }
    else
        qDebug() << "no PulseAudio cards found!";


    if (!d->pulseAudioSink.isNull())
    {
        connect(d->pulseAudioSink.data(), SIGNAL(activePortChanged(QString)),
                this, SLOT(onPulseAudioSinkActivePortChanged(QString)));
    }
    else
        qDebug() << "no PulseAudio sink named sink.primary found!";

    connect(d->pulseAudioConnection.data(), SIGNAL(sourceAdded(QSharedPointer<QtPulseAudioSource>)),
            this, SLOT(onPulseAudioSourceAdded(QSharedPointer<QtPulseAudioSource>)));

    connect(d->pulseAudioConnection.data(), SIGNAL(sourceRemoved(QSharedPointer<QtPulseAudioSource>)),
            this, SLOT(onPulseAudioSourceRemoved(QSharedPointer<QtPulseAudioSource>)));
}

void Application::onPulseAudioError(QString error)
{
    qDebug() << QThread::currentThread() << ": error connecting to PulseAudio: " << error;

    quit();
}

void Application::onPulseAudioSinkActivePortChanged(QString portName)
{
    if (d->active)
    {
        qDebug() << QThread::currentThread() << ": active port: " << portName;

        maybeSwitchProfile();
    }
}

void Application::onPulseAudioSourceAdded(QSharedPointer< QtPulseAudioSource > source)
{
    qDebug() << QThread::currentThread() << source->index() << source->name();

    // Workaround for Android mic issue.
    // If recording is active, default source is being reset constantly.
    // After recording is done, the app leaves "needResetDefaultSource" flag in case if
    // source.primary would be recreated after recording is done
    if ((d->active || d->needResetDefaultSource) &&
            source->name() == QLatin1String("source.primary"))
    {
        d->pulseAudioConnection->server()->setDefaultSource("source.primary");

        if (!d->active && d->needResetDefaultSource)
            d->needResetDefaultSource = false;
    }
}

void Application::onPulseAudioSourceRemoved(QSharedPointer< QtPulseAudioSource > source)
{
    qDebug() << QThread::currentThread() << source->index() << source->name();
}

/// Creates the recorder for a voice call appeared in the system
void Application::onVoiceCallAdded(const QString& objectPath)
{
    qDebug() << objectPath;

    QScopedPointer< VoiceCallRecorder > voiceCallRecorder(new VoiceCallRecorder(objectPath));

    connect(voiceCallRecorder.data(), SIGNAL(stateChanged(VoiceCallRecorder::State)),
            this, SLOT(onVoiceCallRecorderStateChanged(VoiceCallRecorder::State)));
    connect(voiceCallRecorder.data(), SIGNAL(stateChanged(VoiceCallRecorder::State)),
            d->dbusAdaptor.data(), SIGNAL(RecorderStateChanged()));

    d->voiceCallRecorders.insert(objectPath, voiceCallRecorder.take());        
}

void Application::onVoiceCallRecorderStateChanged(VoiceCallRecorder::State state)
{
    qDebug() << state;

    if (state == VoiceCallRecorder::Active)
    {
        d->active = true;

        maybeSwitchProfile();
    }
    else
        d->active = false;
}

/// Checks whether recorder is active and cleans it up after the voice call was removed from the system
void Application::onVoiceCallRemoved(const QString& objectPath)
{
    qDebug() << __PRETTY_FUNCTION__ << objectPath;

    VoiceCallRecorder* voiceCallRecorder = d->voiceCallRecorders.value(objectPath, NULL);

    // clean up if call recorder exists

    if (voiceCallRecorder)
    {
        delete voiceCallRecorder;
        d->voiceCallRecorders.remove(objectPath);

        // workaround for Android mic issue
        d->needResetDefaultSource = true;

        // show approval dialog if needed
        if (settings()->requireApproval())
            showApprovalDialog();
    }
}

void Application::removeEvents(SqlCursor* cursor)
{
    qDebug() << "events to remove:" << cursor->size();

    QDir outputLocationDir(d->settings->outputLocation());

    while (cursor->next())
    {
        bool removeFromDb = true;

        QString fileName = cursor->value("FileName").toString();

        QFile file(outputLocationDir.absoluteFilePath(fileName));

        if (!fileName.isEmpty() && file.exists())
        {
            qDebug() << "removing" << fileName << "due to limits";

            if (!file.remove())
            {
                qWarning() << "unable to remove" << file.fileName() << ": " << file.errorString();
                removeFromDb = false;
            }
        }

        if (removeFromDb)
        {
            // remove from database
            static QString deleteStmt("DELETE FROM Events WHERE ID = :id");

            Database::SqlParameters params;
            params.insert(":id", cursor->value("ID").toInt());

            if (!d->database->execute(deleteStmt, params))
                qWarning() << d->database->lastError();
        }
    }
}

Settings* Application::settings() const
{
    return d->settings.data();
}

void Application::showApprovalDialog()
{
    qDebug();

    if (d->approvalView.isNull())
        createApprovalDialog();

    // select next recording requiring approval

    static QString stmt(
                "\nSELECT"
                "\n    Events.ID,"
                "\n    Events.EventTypeID,"
                "\n    Events.TimeStamp,"
                "\n    Events.Duration,"
                "\n    Events.FileSize,"
                "\n    PhoneNumbers.LineIdentification"
                "\nFROM"
                "\n    Events"
                "\n    LEFT JOIN"
                "\n        PhoneNumbers"
                "\n    ON"
                "\n        PhoneNumbers.ID = Events.PhoneNumberID"
                "\nWHERE"
                "\n    Events.RecordingStateID = 5"
                "\nORDER BY"
                "\n    Events.TimeStamp DESC"
                "\nLIMIT"
                "\n    1");

    QScopedPointer< SqlCursor > cursor(d->database->select(stmt));

    if (cursor.isNull())
    {
        qDebug() << d->database->lastError();
    }
    else if (cursor->next())
    {
        QQuickItem* window =
                d->approvalView->rootObject()->findChild< QQuickItem* >("approvalDialogWindow");

        window->setProperty("eventId", cursor->value("ID"));
        window->setProperty("eventTypeId", cursor->value("EventTypeID"));
        window->setProperty("timeStamp", cursor->value("TimeStamp"));
        window->setProperty("duration", cursor->value("Duration"));
        window->setProperty("fileSize", cursor->value("FileSize"));
        window->setProperty("lineIdentification", cursor->value("LineIdentification"));
        window->setProperty("shouldBeVisible", true);

        qDebug() << "approvalView visible: " << d->approvalView->isVisible();

        d->approvalView->setVisible(true);
    }
    else
    {
        qDebug() << "no events to approve";

        d->approvalView->setVisible(false);
    }
}
