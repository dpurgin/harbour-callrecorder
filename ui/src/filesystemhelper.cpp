#include "filesystemhelper.h"

#include <QDir>
#include <QFile>
#include <QFileInfo>

FileSystemHelper::FileSystemHelper(QObject *parent) :
    QObject(parent)
{
}

bool FileSystemHelper::exists(const QString& filePath) const
{
    return QFileInfo(filePath).exists();
}

bool FileSystemHelper::isRemovable(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return fi.isWritable() && (fi.isDir()? QDir(filePath).entryList(QDir::NoDotAndDotDot).size() == 0: true);
}

bool FileSystemHelper::isWritable(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return fi.exists() && fi.isWritable();
}

bool FileSystemHelper::rename(const QString& filePath, const QString& newName) const
{
    QFile f(filePath);

    return (f.exists() && f.rename(newName));
}

bool FileSystemHelper::remove(const QString& filePath) const
{
    QFileInfo fi(filePath);

    return isRemovable(filePath) && (fi.isDir()? QDir().rmdir(filePath): QFile(filePath).remove());
}
