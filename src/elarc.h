#ifndef ELARC_H
#define ELARC_H
#include <QtCore>
#include <QtSql>
#include "libtable.h"

class elarc
{
public:
    elarc();
    static bool lastOpGood;
    static QString Table;
    static QString qSqlCreateTable;
    static QString qSqlInsert(LibTable *tab);
};

#endif // ELARC_H
