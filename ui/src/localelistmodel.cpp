/*
    Call Recorder for SailfishOS
    Copyright (C) 2014-2015 Dmitriy Purgin <dpurgin@gmail.com>

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

#include "localelistmodel.h"

#include <QDir>
#include <QFileInfo>
#include <QLocale>
#include <QStringBuilder>
#include <QStringList>

// Constructor reads all files from TRANSLATIONSDIR and builds human readable list of locales.
//
// System locale is always present in the list.
// All translation files have the following naming convention: ui-<language>_<COUNTRY>.qm.
// <language>_<COUNTRY> part is fed to QLocale to get locale name. Special cases like dialects (see
// ui-de_AT-4.qm) are treated separately.
LocaleListModel::LocaleListModel(QObject* parent)
    : QAbstractListModel(parent)
{
    // system locale is always present on the list
    mLocales.append(QPair< QString, QString >("system", tr("System")));

    // read all .qm files from the following location and transform them into locale list
    QDir dir(QLatin1String(TRANSLATIONSDIR));

    QStringList qmFilter;
    qmFilter << "ui-*.qm";

    foreach (QFileInfo entry,
             dir.entryInfoList(qmFilter, QDir::Files | QDir::Readable, QDir::Name))
    {
        QString baseName = entry.baseName();

        // locale code is separated with dash. Get locale code after dash and retrieve its name
        // using QLocale
        if (baseName.contains(QChar('-')))
        {
            QString localeString = baseName.mid(baseName.indexOf(QLatin1Char('-')) + 1);
            QString nativeName;

            // special case of Lower Austrian dialect (not in QLocale database)
            if (localeString == QLatin1String("de_AT-3"))
            {
                nativeName = QString::fromUtf8("Niadaöstreichisches Deidsch (Östareich)");
            }
            else
            {
                QLocale locale(localeString);

                nativeName = locale.nativeLanguageName() %
                        QString(" (") % locale.nativeCountryName() % QString(")");
            }

            mLocales.append(QPair< QString, QString >(localeString, nativeName));
        }
    }
}

QVariant LocaleListModel::data(const QModelIndex& index, int role) const
{
    QVariant result;

    if (index.row() < rowCount())
    {
        const QPair< QString, QString >& item = mLocales.at(index.row());

        // first item is locale code, second item is native name
        if (role == Qt::UserRole)
            result = item.first;
        else
            result = item.second;
    }

    return result;
}

QString LocaleListModel::nativeName(const QString& localeName) const
{
    QString result;

    for (QList< QPair< QString, QString > >::const_iterator cit = mLocales.begin();
         cit != mLocales.end() && result.isEmpty();
         cit++)
    {
        if (cit->first == localeName)
            result = cit->second;
    }

    if (result.isEmpty())
        result = mLocales.front().second;

    return result;
}

QHash< int, QByteArray > LocaleListModel::roleNames() const
{
    static QHash< int, QByteArray > names;

    if (names.isEmpty())
    {
        names.insert(Qt::UserRole, "locale");
        names.insert(Qt::UserRole + 1, "nativeName");
    }

    return names;
}
