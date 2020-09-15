#ifndef LINE_H
#define LINE_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class line
{
public:
    line();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // LINE_H
