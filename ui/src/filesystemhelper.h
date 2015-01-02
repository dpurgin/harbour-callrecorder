#ifndef HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H
#define HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H

#include <QObject>

class FileSystemHelper : public QObject
{
    Q_OBJECT

public:
    explicit FileSystemHelper(QObject *parent = 0);

    Q_INVOKABLE bool exists(const QString& filePath) const;
    Q_INVOKABLE bool isRemovable(const QString& filePath) const;
    Q_INVOKABLE bool isWritable(const QString& filePath) const;

    Q_INVOKABLE bool rename(const QString& filePath, const QString& newName) const;
    Q_INVOKABLE bool remove(const QString& filePath) const;

signals:

public slots:

};

#endif // HARBOUR_CALLRECORDER_UI_FILESYSTEMHELPER_H
