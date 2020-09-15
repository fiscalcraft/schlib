#ifndef TEXT_H
#define TEXT_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class text
{
public:
    text();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // TEXT_H
