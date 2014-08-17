#ifndef HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H
#define HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H

#include <QAudio>
#include <QObject>

class VoiceCallRecorder : public QObject
{
    Q_OBJECT
    Q_DISABLE_COPY(VoiceCallRecorder)

public:
    enum State
    {
        Armed,
        Active,
        Inactive
    };

public:
    explicit VoiceCallRecorder(const QString& dbusObjectPath);
    virtual ~VoiceCallRecorder();

    State state() const;

signals:

public slots:

private slots:
    void onAudioInputDeviceReadyRead();
    void onAudioInputStateChanged(QAudio::State state);
    void onVoiceCallStateChanged(const QString& state);

private:
    void processOfonoState(const QString& state);

    void setState(State state);

private:
    class VoiceCallRecorderPrivate;
    QScopedPointer< VoiceCallRecorderPrivate > d;
};

#endif // HARBOUR_CALLRECORDERD_VOICECALLRECORDER_H
