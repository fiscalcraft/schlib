#include "libquery.h"

LibQuery::LibQuery(QSqlDatabase db) : QSqlQuery(db){}

QString LibQuery::getStringValue(const QString& table, const QString& name) {

    QString select_q("SELECT * FROM %1 WHERE name='%2';");

    if (exec(select_q.arg(table).arg(name)) == false) return QString();

    if (next()) {
        return value("value").toString();
    }
    else return QString();
}

int LibQuery::getIntValue(const QString& table, const QString& name, bool *ok) {

    return getStringValue(table, name).toInt(ok);
}

bool LibQuery::isTableExist(const QString& table) {
    QString select_q("SELECT name FROM sqlite_master WHERE type='table' AND name='%1';");
    if (exec(select_q.arg(table)) == false) {
            qDebug() << "ERROR:Invalid request" << select_q.arg(table);
            return false;
    }
    return next();
}
