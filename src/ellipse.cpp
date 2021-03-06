#include "ellipse.h"

ellipse::ellipse()
{

}

bool ellipse::lastOpGood = false;

QString ellipse::Table("Ellipses");

QString ellipse::qSqlCreateTable("CREATE TABLE Ellipses( \
                             \"RECORD\"                     SMALLINT UNSIGNED, \
                             \"ISNOTACCESIBLE\"             TINYINT UNSIGNED, \
                             \"INDEXINSHEET\"               INT PRIMARY KEY NOT NULL, \
                             \"OWNERPARTID\"                SMALLINT UNSIGNED, \
                             \"OWNERPARTDISPLAYMODE\"       SMALLINT UNSIGNED, \
                             \"LOCATION.X\"                 INT, \
                             \"LOCATION.X_FRAC\"            INT, \
                             \"LOCATION.Y\"                 INT, \
                             \"LOCATION.Y_FRAC\"            INT, \
                             \"RADIUS\"                     INT UNSIGNED, \
                             \"RADIUS_FRAC\"                INT UNSIGNED, \
                             \"SECONDARYRADIUS\"            INT UNSIGNED, \
                             \"SECONDARYRADIUS_FRAC\"       INT UNSIGNED, \
                             \"LINEWIDTH\"                  TINYINT UNSIGNED, \
                             \"COLOR\"                      INT UNSIGNED, \
                             \"AREACOLOR\"                  INT UNSIGNED, \
                             \"ISSOLID\"                    TINYINT UNSIGNED \
                             );");


QString ellipse::qSqlInsert(LibTable *tab)
{
    lastOpGood = false;

    if (tab->type != RECORD_ELLIPSE) return QString("");

    QString columns("");
    QString values("");

    bool flag = false;

    for (int j = 0; j < tab->size(); j++) {
        QStringList pair = tab->at(j).split('=', Qt::SkipEmptyParts);
        pair[1].replace("'", "''");
        columns.append("\"" + pair[0] + "\"");

        if (pair[0] == "INDEXINSHEET") flag = true;

        if (pair[0] == "ISNOTACCESIBLE"
         || pair[0] == "ISSOLID") {
            if (pair[1] == 'T') values.append("1");
            else values.append("0");
        } else values.append(pair[1]);

        if (j < (tab->size() - 1)) columns.append(",");
        if (j < (tab->size() - 1)) values.append(",");
    }

    if (flag == false) {
        columns.append(",\"INDEXINSHEET\"");
        values.append(",0");
    }

    lastOpGood = true;

    return QString("INSERT INTO Ellipses (%1) VALUES (%2);").arg(columns).arg(values);
}

