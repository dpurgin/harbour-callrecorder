#include "voicecallrecorder.h"

#include <QAudioInput>
#include <QFileInfo>
#include <QScopedPointer>

#include <FLAC/stream_encoder.h>

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
          flacEncoder(NULL),
          state(VoiceCallRecorder::Inactive)
    {
    }

    QScopedPointer< QAudioInput > audioInput;
    QIODevice* audioInputDevice;

    VoiceCallRecorder::CallType callType;

    QString dbusObjectPath;

    int eventId;

    FLAC__StreamEncoder* flacEncoder;

    QString outputLocation;

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

    // if the call was recorded, the processor for "disconnected" signal should have told us
    // to finalize FLAC encoder, as readyRead() could still have been called after audioInput->stop()
    if (state() == WaitingForFinish)
    {
        FLAC__stream_encoder_finish(d->flacEncoder);
        FLAC__stream_encoder_delete(d->flacEncoder);

        // update recording state to Done
        if (d->eventId != -1)
        {
            QFileInfo fi(d->outputLocation);

            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::Done)));
            params.insert(QLatin1String("FileName"), fi.fileName());
            params.insert(QLatin1String("FileSize"), fi.size());

            app->model()->events()->update(d->eventId, params);
        }

        setState(Inactive);
    }

    // the state() should be Inactive. If it isn't, something went wrong
    if (state() != Inactive)
        qWarning() << __PRETTY_FUNCTION__ <<
                      QLatin1String(": dbusObjectPath = ") << d->dbusObjectPath <<
                      QLatin1String(", qofonoVoiceCall->state() = ") << d->qofonoVoiceCall->state() <<
                      QLatin1String(", state() = ") << state();
}

void VoiceCallRecorder::arm()
{
    qDebug() << __PRETTY_FUNCTION__;

    const QAudioFormat audioFormat = app->settings()->audioFormat();

    qDebug() << __PRETTY_FUNCTION__ << "Audio format is: " << audioFormat;

    // create audio input device
    if (d->audioInput.isNull())
    {
        d->audioInput.reset(new QAudioInput(app->settings()->inputDevice(), audioFormat));
        connect(d->audioInput.data(), SIGNAL(stateChanged(QAudio::State)),
                this, SLOT(onAudioInputStateChanged(QAudio::State)));
    }
    else
        qWarning() << __PRETTY_FUNCTION__ << ": d->audioInput expected to be NULL but it wasn't!";

    // initialize FLAC encoder
    if (d->flacEncoder == NULL)
    {
        d->flacEncoder = FLAC__stream_encoder_new();

        if (d->flacEncoder == NULL)
            qCritical() << __PRETTY_FUNCTION__ << ": unable to create FLAC encoder!";

        if (!FLAC__stream_encoder_set_channels(d->flacEncoder, audioFormat.channelCount()))
            qCritical() << __PRETTY_FUNCTION__ <<
                         ": unable to set FLAC channels: " <<
                         FLAC__stream_encoder_get_state(d->flacEncoder);

        if (!FLAC__stream_encoder_set_bits_per_sample(d->flacEncoder, audioFormat.sampleSize()))
            qCritical() << __PRETTY_FUNCTION__ <<
                         ": unable to set FLAC bits per sample: " <<
                         FLAC__stream_encoder_get_state(d->flacEncoder);

        if (!FLAC__stream_encoder_set_sample_rate(d->flacEncoder, audioFormat.sampleRate()))
            qCritical() << __PRETTY_FUNCTION__ <<
                         ": unable to set FLAC sample rate: " <<
                         FLAC__stream_encoder_get_state(d->flacEncoder);

        if (!FLAC__stream_encoder_set_compression_level(d->flacEncoder, 8))
            qCritical() << __PRETTY_FUNCTION__ <<
                         ": unable to set FLAC compression level: " <<
                         FLAC__stream_encoder_get_state(d->flacEncoder);

        // form output location with file name based on current date, phone number, call direction (in/out).
        // if line ID does not exist, we should wait for the corresponding signal from Ofono,
        // but maybe it is always known at this stage. Needs checking.
        // File name is set to "{timestamp}_{phoneNumber}_{type}.flac"
        d->outputLocation = (app->settings()->outputLocation() %
                             QLatin1Char('/') %
                             timeStamp().toString(Qt::ISODate).replace(QChar(':'), QChar('_')) % QLatin1Char('_') %
                             d->qofonoVoiceCall->lineIdentification() % QLatin1Char('_') %
                             (callType() == Incoming? QLatin1String("in"): QLatin1String("out")) %
                             QLatin1String(".flac"));

        qDebug() << __PRETTY_FUNCTION__ << "Writing to " << d->outputLocation;

        FLAC__StreamEncoderInitStatus status = FLAC__stream_encoder_init_file(d->flacEncoder,
                                                                              d->outputLocation.toLatin1().data(),
                                                                              NULL,
                                                                              NULL);
        if (status != FLAC__STREAM_ENCODER_INIT_STATUS_OK)
            qCritical() << __PRETTY_FUNCTION__ <<
                        ": unable to init FLAC file: " << status <<
                        ", current encoder status is " << FLAC__stream_encoder_get_state(d->flacEncoder);
    }
    else
        qWarning() << __PRETTY_FUNCTION__ << ": d->flacEncoder expected to be NULL but it wasn't!";

    // add new event to events table
    if (d->eventId == -1)
    {
        d->eventId = app->model()->events()->add(
                    timeStamp(),                                                // time stamp of recording
                    app->model()->phoneNumbers()->getIdByLineIdentification(    // phone number ref
                        d->qofonoVoiceCall->lineIdentification()),
                    callType() == Incoming?                                     // call type
                        EventsTableModel::Incoming :
                        EventsTableModel::Outgoing,
                    EventsTableModel::Armed);                                   // initial recording state
    }
    else
        qWarning() << __PRETTY_FUNCTION__ << ": d->eventId expected to be -1 but it wasn't!";

    setState(Armed);
}

VoiceCallRecorder::CallType VoiceCallRecorder::callType() const
{
    return d->callType;
}

void VoiceCallRecorder::onAudioInputDeviceReadyRead()
{
    qDebug() << __PRETTY_FUNCTION__;

    QByteArray data = d->audioInputDevice->readAll();

    qDebug() << __PRETTY_FUNCTION__ << ": read bytes: " << data.size();

    const qint16* sampleData = reinterpret_cast< const qint16* >(data.constData());

    quint64 sampleCount = data.size() / (app->settings()->audioFormat().sampleSize() / 8);

    qDebug() << __PRETTY_FUNCTION__ << ": sample count: " << sampleCount;

    FLAC__StreamEncoderState state = FLAC__stream_encoder_get_state(d->flacEncoder);

    if (state != FLAC__STREAM_ENCODER_OK)
        qDebug() << __PRETTY_FUNCTION__ << ": FLAC encoder state is not ok: " << state;
    else
    {
        for (quint64 sampleIdx = 0; sampleIdx < sampleCount; sampleIdx++)
        {
            FLAC__int32 sample = static_cast< FLAC__int32 >(sampleData[sampleIdx]);

            if (!FLAC__stream_encoder_process_interleaved(d->flacEncoder, &sample, 1))
            {
                qDebug() << "Unable to FLAC__stream_encoder_process(): " << FLAC__stream_encoder_get_state(d->flacEncoder);
                break;
            }
        }
    }
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

void VoiceCallRecorder::processOfonoState(const QString& ofonoState)
{
    qDebug() << __PRETTY_FUNCTION__ << d->dbusObjectPath << ofonoState;

    // if a call has just appeared, we arm the recorder before it actually gets into recordable state
    if (ofonoState == QLatin1String("incoming") || ofonoState == QLatin1String("dialing"))
    {
        setCallType(ofonoState == QLatin1String("incoming")? Incoming: Outgoing);
        setTimeStamp(QDateTime::currentDateTime());

        arm();
    }
    // when the call goes into active state, the sound card's profile is set to voicecall-record.
    // recording is started or resumed
    if (ofonoState == QLatin1String("active"))
    {
        // simple shell call for now
        // TODO: replace with normal pulseaudio APIs
        int retval = QProcess::execute("pacmd set-card-profile 0 voicecall-record");

        qDebug() << __PRETTY_FUNCTION__ << ": pacmd returns " << retval;

        // if the recorder was armed, recording was not started yet. start recording and connect to
        // readyRead() signal to retrieve and encode data
        if (state() == Armed)
        {
            d->audioInputDevice = d->audioInput->start();
            connect(d->audioInputDevice, SIGNAL(readyRead()),
                    this, SLOT(onAudioInputDeviceReadyRead()));
        }
        // if the recorder was suspended, just resume the recording
        else if (state() == Suspended)
            d->audioInput->resume();

        // update event state to InProgress
        if (d->eventId != -1)
        {
            QVariantMap params;
            params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::InProgress)));

            app->model()->events()->update(d->eventId, params);
        }

        setState(Active);
    }
    // stop recording if the call was disconnected
    else if (ofonoState == QLatin1String("disconnected"))
    {
        // stop recording if it was ever started
        if (state() == Active || state() == Suspended)
        {
            d->audioInput->stop();

            // we do not call FLAC__stream_encode_finish at this point, as the d->audioInput can still some data
            // to actually cleanup FLAC, we will process WaitingForFinish in destructor

            setState(WaitingForFinish);
        }
        // if the call was never active, remove the record from Events
        else
        {
            app->model()->events()->remove(d->eventId);

            setState(Inactive);
        }
    }
    // if the call is on hold or waiting, suspend recording if it was active
    else if (ofonoState == QLatin1String("held") || ofonoState == QLatin1String("waiting"))
    {
        if (state() == Active)
        {
            d->audioInput->suspend();

            setState(Suspended);

            if (d->eventId != -1)
            {
                QVariantMap params;
                params.insert(QLatin1String("RecordingStateID"), QVariant(static_cast< int >(EventsTableModel::Suspended)));

                app->model()->events()->update(d->eventId, params);
            }
        }
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
