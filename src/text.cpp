#include "text.h"

text::text()
{

}

bool text::lastOpGood = false;

QString text::Table("Texts");

QString text::qSqlCreateTable("CREATE TABLE Texts( \
                             \"RECORD\"                     SMALLINT UNSIGNED, \
                             \"ISNOTACCESIBLE\"             TINYINT UNSIGNED, \
                             \"INDEXINSHEET\"               INT PRIMARY KEY NOT NULL, \
                             \"OWNERPARTID\"                SMALLINT UNSIGNED, \
                             \"OWNERPARTDISPLAYMODE\"       SMALLINT UNSIGNED, \
                             \"LOCATION.X\"                 INT, \
                             \"LOCATION.X_FRAC\"            INT, \
                             \"LOCATION.Y\"                 INT, \
                             \"LOCATION.Y_FRAC\"            INT, \
                             \"ORIENTATION\"                TINYINT UNSIGNED, \
                             \"JUSTIFICATION\"              TINYINT UNSIGNED, \
                             \"COLOR\"                      INT UNSIGNED, \
                             \"FONTID\"                     SMALLINT UNSIGNED, \
                             \"%UTF8%TEXT\"                 VARCHAR(255), \
                             \"TEXT\"                       VARCHAR(255), \
                             \"URL\"                        VARCHAR(255) \
                             );");


QString text::qSqlInsert(LibTable *tab)
{
    lastOpGood = false;

    if (tab->type != RECORD_TEXT) return QString("");

    QString columns("");
    QString values("");

    bool flag = false;

    for (int j = 0; j < tab->size(); j++) {
        QStringList pair = tab->at(j).split('=', Qt::SkipEmptyParts);
        pair[1].replace("'", "''");
        columns.append("\"" + pair[0] + "\"");

        if (pair[0] == "INDEXINSHEET") flag = true;

        if (pair[0] == "ISNOTACCESIBLE") {
            if (pair[1] == 'T') values.append("1");
            else values.append("0");
        } else if (pair[0] == "%UTF8%TEXT"
                || pair[0] == "TEXT"
                || pair[0] == "URL") {
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

    return QString("INSERT INTO Texts (%1) VALUES (%2);").arg(columns).arg(values);
}

