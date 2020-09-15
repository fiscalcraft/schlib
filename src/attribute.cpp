#include "attribute.h"

attr::attr()
{

}

bool attr::lastOpGood = false;

QString attr::Table("Attributes");

QString attr::qSqlCreateTable("CREATE TABLE Attributes( \
                             \"RECORD\"                     SMALLINT UNSIGNED, \
                             \"INDEXINSHEET\"               INT NOT NULL, \
                             \"OWNERINDEX\"                 SMALLINT UNSIGNED, \
                             \"OWNERPARTID\"                SMALLINT, \
                             \"OWNERPARTDISPLAYMODE\"       SMALLINT UNSIGNED, \
                             \"LOCATION.X\"                 INT, \
                             \"LOCATION.X_FRAC\"            INT, \
                             \"LOCATION.Y\"                 INT, \
                             \"LOCATION.Y_FRAC\"            INT, \
                             \"COLOR\"                      INT UNSIGNED, \
                             \"FONTID\"                     SMALLINT UNSIGNED, \
                             \"ISHIDDEN\"                   TINYINT UNSIGNED, \
                             \"%UTF8%TEXT\"                 VARCHAR(255), \
                             \"TEXT\"                       VARCHAR(255), \
                             \"NAME\"                       VARCHAR(45), \
                             \"READONLYSTATE\"              TINYINT UNSIGNED, \
                             \"UNIQUEID\"                   VARCHAR(45) \
                             );");


QString attr::qSqlInsert(LibTable *tab)
{
    lastOpGood = false;

    if (tab->type != RECORD_ATTRIBUTE) return QString("");

    QString columns("");
    QString values("");

    bool flag = false;

    for (int j = 0; j < tab->size(); j++) {
        QStringList pair = tab->at(j).split('=', Qt::SkipEmptyParts);
        pair[1].replace("'", "''");
        columns.append("\"" + pair[0] + "\"");

        if (pair[0] == "INDEXINSHEET") flag = true;

        if (pair[0] == "ISHIDDEN") {
            if (pair[1] == 'T') values.append("1");
            else values.append("0");
        } else if (pair[0] == "TEXT"
                || pair[0] == "%UTF8%TEXT"
                || pair[0] == "NAME"
                || pair[0] == "UNIQUEID") {
            values.append("'" + pair[1] + "'");
        } else values.append(pair[1]);

        if (j < (tab->size() - 1)) columns.append(",");
        if (j < (tab->size() - 1)) values.append(",");
    }

    if (flag == false) {
        columns.append(",\"INDEXINSHEET\"");
        values.append(",0");
    }

    lastOpGood = true;

    return QString("INSERT INTO Attributes (%1) VALUES (%2);").arg(columns).arg(values);
}

