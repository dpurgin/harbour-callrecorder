/*
    Call Recorder for SailfishOS
    Copyright (C) 2015-2016  Dmitriy Purgin <dpurgin@gmail.com>

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

#include "libcallrecorder.h"

#include <QtGlobal>

#include <QCoreApplication>
#include <QDateTime>
#include <QDebug>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QScopedPointer>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QThread>
#include <QTranslator>

#include <iostream>

#include "settings.h"

namespace LibCallRecorder
{
    QTranslator* createTranslator(const QString& resource, const QString& path)
    {
        QScopedPointer< Settings > settings(new Settings());
        QScopedPointer< QTranslator > translator(new QTranslator());

        // Check if this specific file name exists.
        // Useful for non-existent locales like de_AT-3
        QString fileName(resource % QLatin1Char('-') % settings->locale() % QLatin1String(".qm"));

        qDebug() << "generated file name:" << fileName;

        if (QFile(path + "/" + fileName).exists())
        {
            qDebug() << "loading translations for " << resource <<
                        ", file name " << fileName <<
                        ", translations dir " << path;

            if (!translator->load(fileName, path))
                qWarning() << "unable to load file-based translations";
        }
        // If cannot load from specific file, try QLocale-based algorithm
        else
        {
            QLocale locale;

            if (settings->locale().compare("system", Qt::CaseInsensitive) != 0)
                locale = QLocale(settings->locale());

            qDebug() << "loading translations for " << resource <<
                        ", locale " << locale <<
                        ", translations dir " << path;

            if (!translator->load(locale, resource, "-", path, ".qm"))
                qWarning() <<  "unable to load QLocale-based translation";
        }

        // Fall-back algorithm. Load engineering English
        if (translator->isEmpty())
        {
            fileName = resource % QLatin1String(".qm");

            qWarning() << "FALL-BACK: loading translations for " << fileName <<
                        ", translations dir " << path;

            if (!translator->load(fileName, path))
                qCritical() << "All translation methods failed!";
        }

        return translator.take();
    }

    QString databaseFilePath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                QLatin1String("/callrecorder.db");
    }

    void messageHandler(QtMsgType type, const QMessageLogContext& context, const QString& msg)
    {
        Q_UNUSED(type)

#ifdef __x86_64__
        #define ADDRESS quint64
        const int WIDTH = 16;
#else
        #define ADDRESS quint32
        const int WIDTH = 8;
#endif

        static QMutex mutex;
        QMutexLocker locker(&mutex);

        QString threadId = QLatin1String("[0x") %
                QString::number(
                    (ADDRESS)QThread::currentThread(), 16).rightJustified(WIDTH, QLatin1Char('0')) %
                QLatin1Char(']');

        QString sourceFile(QFileInfo(context.file).fileName() %
                           QLatin1Char(':') %
                           QString::number(context.line));

        QByteArray entry;

        static QString appName;

        if (appName.isEmpty())
        {
            QFileInfo fi(QCoreApplication::applicationFilePath());
            appName = fi.fileName();
        }

        entry.append(appName);
        entry.append(": ", 2);
        entry.append(QDateTime::currentDateTime().toString());
        entry.append(": ", 2);
        entry.append(threadId.toUtf8().data());
        entry.append(": ", 2);
        entry.append(sourceFile.toUtf8().data());
        entry.append(": ", 2);
        entry.append(context.function);
        entry.append(": ", 2);
        entry.append(msg.toUtf8().data());
        entry.append("\n", 1);

        std::cerr << entry.data();

        static QString logLocation =
                QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                QLatin1String("/log.txt");

        // check current log size, rotate if >= 2MB

        QFileInfo logFileInfo(logLocation);

        if (logFileInfo.size() >= 2097152)
        {
            // rotated log name
            static QString rotatedLogLocation =
                    QStandardPaths::writableLocation(QStandardPaths::DataLocation) %
                    QLatin1String("/log.0.txt");

            // if rotated file already exists, remove it
            QFile rotatedLog(rotatedLogLocation);

            if (rotatedLog.exists())
                rotatedLog.remove();

            // now rename current log to rotated log
            QFile::rename(logLocation, rotatedLogLocation);
        }

        QFile log(logLocation);

        if (log.open(QFile::WriteOnly | QFile::Append))
        {
            log.write(entry);
            log.close();
        }
    }

    void installMessageHandler()
    {
        qInstallMessageHandler(messageHandler);
    }

    QString settingsFilePath()
    {
        return QStandardPaths::writableLocation(QStandardPaths::ConfigLocation) %
                QLatin1Char('/') % qApp->applicationName() %
                QLatin1String("/callrecorder.ini");
    }
}
