#include "voicecallrecorder.h"

#include <QAudioInput>
#include <QScopedPointer>

#include <qofono-qt5/qofonovoicecall.h>

#include "application.h"
#include "model.h"
#include "eventstablemodel.h"
#include "phonenumberstablemodel.h"
#include "settings.h"

class VoiceCallRecorder::VoiceCallRecorderPrivate
{
    friend class VoiceCallRecorder;

    VoiceCallRecorderPrivate()
        : audioInputDevice(NULL),
          callType(VoiceCallRecorder::Incoming),
          eventId(-1),
          state(VoiceCallRecorder::Inactive)
    {
    }

    QScopedPointer< QAudioInput > audioInput;
    QIODevice* audioInputDevice;

    VoiceCallRecorder::CallType callType;

    QString dbusObjectPath;

    int eventId;

    QScopedPointer< QFile > outputFile;

    QScopedPointer< QOfonoVoiceCall > qofonoVoiceCall;

    VoiceCallRecorder::State state;

    QDateTime timeStamp;
};

VoiceCallRecorder::VoiceCallRecorder(const QString& dbusObjectPath)
    : d(new VoiceCallRecorderPrivate())
{
    qDebug() << __PRETTY_FUNCTION__ << dbusObjectPath;

    d->dbusObjectPath = dbusObjectPath;

    d->qofonoVoiceCall.reset(new QOfonoVoiceCall());
    d->qofonoVoiceCall->setVoiceCallPath(dbusObjectPath);

    connect(d->qofonoVoiceCall.data(), SIGNAL(stateChanged(QString)),
            this, SLOT(onVoiceCallStateChanged(QString)));

    processOfonoState(d->qofonoVoiceCall->state());
}

VoiceCallRecorder::~VoiceCallRecorder()
{
    qDebug() << __PRETTY_FUNCTION__ << d->dbusObjectPath;

    // the state() should be Inactive because the QOfonoVoiceCall becomes "disconnected" before
    // the call is removed from QOfonoVoiceCallManager. If it doesn't, something went wrong
    if (state() != Inactive)
        qWarning() << __PRETTY_FUNCTION__ <<
                      QLatin1String(": dbusObjectPath = ") << d->dbusObjectPath <<
                      QLatin1String(", qofonoVoiceCall->state() = ") << d->qofonoVoiceCall->state() <<
                      QLatin1String(", state() = ") << state();

}

VoiceCallRecorder::CallType VoiceCallRecorder::callType() const
{
    return d->callType;
}

QString VoiceCallRecorder::getOutputLocation(const QDateTime& timeStamp, const QString& lineIdentification, CallType callType) const
{
    return (app->settings()->outputLocation() %
            QLatin1Char('/') %
            timeStamp.toString(Qt::ISODate) % QLatin1Char('_') %
            lineIdentification % QLatin1Char('_') %
            (callType == Incoming? QLatin1String("in"): QLatin1String("out")) %
            QLatin1String(".raw"));

}

void VoiceCallRecorder::onLineIdentificationChanged(const QString& lineIdentification)
{
    qDebug() << __PRETTY_FUNCTION__ << lineIdentification;

    openOutputFile(getOutputLocation(timeStamp(), lineIdentification, callType()));
}

void VoiceCallRecorder::onAudioInputDeviceReadyRead()
{
    qDebug() << __PRETTY_FUNCTION__;

    d->outputFile->write(d->audioInputDevice->readAll());
}

void VoiceCallRecorder::onAudioInputStateChanged(QAudio::State state)
{
    qDebug() << __PRETTY_FUNCTION__ << state;
}

void VoiceCallRecorder::onVoiceCallStateChanged(const QString& state)
{
    qDebug() << __PRETTY_FUNCTION__ << d->dbusObjectPath << state;

    processOfonoState(state);
}

void VoiceCallRecorder::openOutputFile(const QString& outputLocation)
{
    d->outputFile.reset(new QFile(outputLocation));

    if (!d->outputFile->open(QFile::WriteOnly))
        qDebug() << __PRETTY_FUNCTION__ <<
                    ": unable to write to " << outputLocation << ": " << d->outputFile->errorString();
}

void VoiceCallRecorder::processOfonoState(const QString& state)
{
    qDebug() << __PRETTY_FUNCTION__ << d->dbusObjectPath << state;

    // when the call goes into active state, the sound card's profile is set to voicecall-record
    // and the actual recording starts
    if (state == QLatin1String("active"))
    {
        // simple shell call for now
        // TODO: replace with normal pulseaudio APIs
        int retval = QProcess::execute("pacmd set-card-profile 0 voicecall-record");

        qDebug() << __PRETTY_FUNCTION__ << ": pacmd returns " << retval;

        if (!d->audioInput.isNull())
        {
            d->audioInputDevice = d->audioInput->start();
            connect(d->audioInputDevice, SIGNAL(readyRead()),
                    this, SLOT(onAudioInputDeviceReadyRead()));

            // if the file is still not open (e.g. line is not identified), open it with unknown line id
            if (d->outputFile.isNull())
                openOutputFile(getOutputLocation(timeStamp(), QLatin1String("unknown"), callType()));
        }
        else
            d->audioInput->resume();

        if (d->eventId != -1)
        {
            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::InProgress)));

            app->model()->events()->update(d->eventId, params);
        }

        setState(Active);
    }
    // stop recording if the call was disconnected
    else if (state == QLatin1String("disconnected"))
    {
        if (!d->audioInput.isNull())
            d->audioInput->stop();

        if (!d->outputFile.isNull())
        {
            d->outputFile->close();

            // if d->audioInputDevice is NULL, it means the call was never active.
            // call is not recorded, remove empty file
            if (!d->audioInputDevice)
                d->outputFile->remove();
        }

        if (d->eventId != -1)
        {
            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::Done)));

            if (!d->outputFile.isNull())
            {
                params.insert(QLatin1String("FileName"), d->outputFile->fileName());
                params.insert(QLatin1String("FileSize"), d->outputFile->size());
            }

            app->model()->events()->update(d->eventId, params);
        }

        setState(Inactive);
    }
    // if a call has just appeared, we arm the recorder before it actually gets into recordable state
    else if (state == QLatin1String("incoming") || state == QLatin1String("dialing"))
    {
        setCallType(state == QLatin1String("incoming")? Incoming: Outgoing);
        setTimeStamp(QDateTime::currentDateTime());

        if (d->audioInput.isNull())
        {
            d->audioInput.reset(new QAudioInput(app->settings()->inputDevice(), app->settings()->audioFormat()));
            connect(d->audioInput.data(), SIGNAL(stateChanged(QAudio::State)),
                    this, SLOT(onAudioInputStateChanged(QAudio::State)));

            // if line ID exists, open the output file for writing, then add the corresponding event to DB.
            // defer it to lineIdentificationChanged() otherwise
            if (!d->qofonoVoiceCall->lineIdentification().isEmpty())
            {
                openOutputFile(getOutputLocation(timeStamp(),
                                                 d->qofonoVoiceCall->lineIdentification(),
                                                 callType()));

                d->eventId = app->model()->events()->add(
                            timeStamp(),
                            callType() == Incoming?
                                EventsTableModel::Incoming :
                                EventsTableModel::Outgoing);

                QVariantMap params;
                params.insert(QLatin1String("PhoneNumberID"),
                              app->model()->phoneNumbers()->getIdByLineIdentification(
                                  d->qofonoVoiceCall->lineIdentification()));
                app->model()->events()->update(d->eventId, params);

            }
            else
            {
                connect(d->qofonoVoiceCall.data(), SIGNAL(lineIdentificationChanged(QString)),
                        this, SLOT(onLineIdentificationChanged(QString)));
            }
        }
        else
            qWarning() << __PRETTY_FUNCTION__ << ": d->audioInput expected to be NULL but it wasn't!";

        if (d->eventId != -1)
        {
            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::Armed)));

            app->model()->events()->update(d->eventId, params);
        }

        setState(Armed);
    }
    else
    {
        EventsTableModel::RecordingState recordingState;

        if (!d->audioInput.isNull())
        {
            d->audioInput->suspend();
            recordingState = EventsTableModel::Suspended;
        }
        else
            recordingState = EventsTableModel::Armed;

        if (d->eventId != -1)
        {
            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(recordingState)));

            app->model()->events()->update(d->eventId, params);
        }

        setState(Armed);
    }
}

void VoiceCallRecorder::setCallType(CallType callType)
{
    d->callType = callType;
}

void VoiceCallRecorder::setState(State state)
{
    d->state = state;
}

void VoiceCallRecorder::setTimeStamp(const QDateTime& timeStamp)
{
    d->timeStamp = timeStamp;
}

VoiceCallRecorder::State VoiceCallRecorder::state() const
{
    return d->state;
}

QDateTime VoiceCallRecorder::timeStamp() const
{
    return d->timeStamp;
}
