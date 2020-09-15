#ifndef LIBQUERY_H
#define LIBQUERY_H
#include <QtCore>
#include <QtSql>


class LibQuery : private QSqlQuery
{
public:
    LibQuery( QSqlDatabase db = QSqlDatabase());
    using QSqlQuery::exec;
    QString getStringValue(const QString& table, const QString& name);
    int getIntValue(const QString& table, const QString& name, bool *ok=NULL);
    bool isTableExist(const QString& table);
};

#endif // LIBQUERY_H
