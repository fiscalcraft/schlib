#ifndef ARC_H
#define ARC_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class arc
{
public:
    arc();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // ARC_H
