#ifndef ELLIPSE_H
#define ELLIPSE_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class ellipse
{
public:
    ellipse();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // ELLIPSE_H
