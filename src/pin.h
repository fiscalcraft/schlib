#ifndef PIN_H
#define PIN_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"


class pin
{
public:
    pin();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // PIN_H
