/*
    Call Recorder for SailfishOS
    Copyright (C) 2015  Dmitriy Purgin <dpurgin@gmail.com>

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
#include <QFile>
#include <QFileInfo>
#include <QMutex>
#include <QMutexLocker>
#include <QStandardPaths>
#include <QStringBuilder>
#include <QThread>

#include <iostream>

namespace LibCallRecorder
{
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

        entry.append(QCoreApplication::applicationName());
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

        QFile f(logLocation);

        if (f.open(QFile::WriteOnly | QFile::Append))
        {
            f.write(entry);
            f.close();
        }
    }

    void installMessageHandler()
    {
        qInstallMessageHandler(messageHandler);
    }
}
