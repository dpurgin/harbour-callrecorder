/*
    Call Recorder for SailfishOS
    Copyright (C) 2016 Dmitriy Purgin <dpurgin@gmail.com>

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

#include "corruptfileslistmodel.h"

#include <QFileInfo>
#include <QDir>

#include "database.h"
#include "settings.h"
#include "sqlcursor.h"

class CorruptFilesListModelPrivate
{
    Q_DECLARE_PUBLIC(CorruptFilesListModel)

private:
    CorruptFilesListModelPrivate(CorruptFilesListModel* q)
        : q_ptr(q)
    {
    }

private:
    bool busy = false;

    QStringList list;
};

CorruptFilesListModel::CorruptFilesListModel(QObject* parent)
    : QAbstractListModel(parent),
      d_ptr(new CorruptFilesListModelPrivate(this))
{
}

CorruptFilesListModel::~CorruptFilesListModel()
{
}

QVariant CorruptFilesListModel::data(const QModelIndex& index, int role) const
{
    Q_D(CorruptFilesListModel);

    Q_UNUSED(role)

    QVariant result;

    if (index.row() < d->list.size())
        result = d->list.at(index.row());

    return result;
}

void CorruptFilesListModel::populate()
{
    Q_D(CorruptFilesListModel);

    QScopedPointer< Settings > settings(new Settings());
    QScopedPointer< Database > db(new Database());

    d->list.clear();

    auto fiList = QDir(settings->outputLocation()).entryInfoList(
                "*.flac", QDir::Files, QDir::Name);

    foreach (auto fi, fiList)
    {
        static QString selectStmt("SELECT ID FROM Events WHERE FileName = :fileName");

        Database::SqlParameters params;
        params.insert(":fileName", fi.fileName());

        QScopedPointer< SqlCursor > cursor(db->select(selectStmt, params));

        if (!cursor.isNull() && cursor->size() == 0)
        {
            d->list.push_back(fi.fileName());
        }
    }

    emit rowCountChanged(d->list.size());
}

//void CorruptFilesListModel::populate()
//{
//    Q_D(CorruptFilesListModel);

//    QScopedPointer< Settings > settings(new Settings());
//    QScopedPointer< Database > db(new Database());

//    d->list.clear();

//    static QString selectStmt("SELECT ID, FileName FROM Events");

//    QScopedPointer< SqlCursor > cursor(db->select(selectStmt));

//    if (!cursor.isNull())
//    {
//        QDir outputLocationDir(settings->outputLocation());

//        while (cursor->next())
//        {
//            QString fileName = cursor->value("FileName").toString();

//            if (!QFileInfo(outputLocationDir.absoluteFilePath(fileName).exists()))
//                d->list.push_back(fileName);
//        }
//    }

//    emit rowCountChanged(d->list.size());
//}

int CorruptFilesListModel::rowCount(const QModelIndex&) const
{
    Q_D(CorruptFilesListModel);

    return d->list.size();
}
