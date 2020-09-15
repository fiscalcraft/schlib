#ifndef RECTANGLE_H
#define RECTANGLE_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"

class rectangle
{
public:
    rectangle();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // RECTANGLE_H
