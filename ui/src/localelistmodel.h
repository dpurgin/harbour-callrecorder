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

#ifndef HARBOUR_CALLRECORDER_UI_LOCALELISTMODEL_H
#define HARBOUR_CALLRECORDER_UI_LOCALELISTMODEL_H

#include <QAbstractListModel>

class LocaleListModel : public QAbstractListModel
{
    Q_OBJECT

    Q_PROPERTY(int rowCount READ rowCount NOTIFY rowCountChanged)

public:
    explicit LocaleListModel(QObject* parent = 0);

    Q_INVOKABLE QVariant data(const QModelIndex& index, int role) const;

    Q_INVOKABLE QString nativeName(const QString& locale) const;

    Q_INVOKABLE int rowCount(const QModelIndex& = QModelIndex()) const { return mLocales.size(); }

    Q_INVOKABLE QHash< int, QByteArray > roleNames() const;

signals:
    int rowCountChanged();

public slots:

private:
    // list of available locales.
    // List of pairs where "first" is locale code, "second" is its native name
    QList< QPair< QString, QString > > mLocales;
};

#endif // HARBOUR_CALLRECORDER_UI_LOCALELISTMODEL_H
