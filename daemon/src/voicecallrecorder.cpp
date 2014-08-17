#include "voicecallrecorder.h"

#include <QAudioInput>
#include <QScopedPointer>

#include <qofono-qt5/qofonovoicecall.h>

#include "application.h"
#include "settings.h"

class VoiceCallRecorder::VoiceCallRecorderPrivate
{
    friend class VoiceCallRecorder;

    VoiceCallRecorderPrivate()
        : state(VoiceCallRecorder::Inactive)
    {
    }

    QScopedPointer< QAudioInput > audioInput;
    QIODevice* audioInputDevice;
    QScopedPointer< QFile > outputFile;

    QString dbusObjectPath;

    QScopedPointer< QOfonoVoiceCall > qofonoVoiceCall;

    VoiceCallRecorder::State state;
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

void VoiceCallRecorder::processOfonoState(const QString& state)
{
    qDebug() << __PRETTY_FUNCTION__ << d->dbusObjectPath << state;

    if (state == QLatin1String("active"))
    {
        // simple system() call for now
        // TODO: replace with normal pulseaudio APIs
        int retval = QProcess::execute("pacmd set-card-profile 0 voicecall-record");

        qDebug() << __PRETTY_FUNCTION__ << ": pacmd returns " << retval;

        if (d->audioInput.isNull())
        {
            QString outputLocation(app->settings()->outputLocation() % QLatin1String("/call.wav"));

            qDebug() << __PRETTY_FUNCTION__ << ": writing to " << outputLocation;

            d->audioInput.reset(new QAudioInput(app->settings()->inputDevice(), app->settings()->audioFormat()));
            connect(d->audioInput.data(), SIGNAL(stateChanged(QAudio::State)),
                    this, SLOT(onAudioInputStateChanged(QAudio::State)));

            d->audioInputDevice = d->audioInput->start();
            connect(d->audioInputDevice, SIGNAL(readyRead()),
                    this, SLOT(onAudioInputDeviceReadyRead()));

            d->outputFile.reset(new QFile(outputLocation));

            if (!d->outputFile->open(QFile::WriteOnly))
                qDebug() << __PRETTY_FUNCTION__ <<
                            ": unable to write to " << outputLocation << ": " << d->outputFile->errorString();
        }
        else
            d->audioInput->resume();

        setState(Active);
    }
    else if (state == QLatin1String("disconnected"))
    {
        if (!d->audioInput.isNull())
            d->audioInput->stop();

        if (!d->outputFile.isNull())
            d->outputFile->close();

        setState(Inactive);
    }
    else if (state == QLatin1String("incoming") || state == QLatin1String("dialing"))
    {
        // check if this number was called some day

//        static QString selectStatement("SELECT LineIdentification FROM PhoneNumbers WHERE PhoneNumber")

        setState(Armed);
    }
    else
    {
        if (!d->audioInput.isNull())
            d->audioInput->suspend();

        setState(Armed);
    }
}

void VoiceCallRecorder::setState(State state)
{
    d->state = state;
}

VoiceCallRecorder::State VoiceCallRecorder::state() const
{
    return d->state;
}
