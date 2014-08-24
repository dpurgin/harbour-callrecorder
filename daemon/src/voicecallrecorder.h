#ifndef HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H
#define HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H

#include <QAudio>
#include <QDateTime>
#include <QObject>

class VoiceCallRecorder : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(VoiceCallRecorder)

public:
    enum CallType
    {
        Incoming,
        Outgoing
    };

    enum State
    {
        Inactive,
        Armed,
        Active,
        Suspended,
        WaitingForFinish,
        Finish
    };

public:
    explicit VoiceCallRecorder(const QString& dbusObjectPath);
    virtual ~VoiceCallRecorder();

    CallType callType() const;
    State state() const;
    QDateTime timeStamp() const;

signals:

public slots:

private slots:
    void onAudioInputDeviceReadyRead();
    void onAudioInputStateChanged(QAudio::State state);
    void onVoiceCallStateChanged(const QString& state);

private:
    void arm();

    void processOfonoState(const QString& state);

    void setCallType(CallType callType);
    void setState(State state);
    void setTimeStamp(const QDateTime& timeStamp);

private:
    class VoiceCallRecorderPrivate;
    QScopedPointer< VoiceCallRecorderPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H
