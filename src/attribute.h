#ifndef ATTRIBUTE_H
#define ATTRIBUTE_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class attr
{
public:
    attr();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // ATTRIBUTE_H
