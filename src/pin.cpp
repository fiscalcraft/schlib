#include "pin.h"

pin::pin()
{

}

bool pin::lastOpGood = false;

QString pin::Table("Pins");

QString pin::qSqlCreateTable("CREATE TABLE Pins( \
                            \"RECORD\"                     SMALLINT UNSIGNED, \
                            \"INDEXINSHEET\"               INT PRIMARY KEY NOT NULL, \
                            \"UNKNOWN1\"                   TINYINT UNSIGNED, \
                            \"OWNERPARTID\"                SMALLINT UNSIGNED, \
                            \"OWNERPARTDISPLAYMODE\"       TINYINT UNSIGNED, \
                            \"INSIDEEDGE\"                 TINYINT UNSIGNED, \
                            \"OUTSIDEEDGE\"                TINYINT UNSIGNED, \
                            \"INSIDE\"                     TINYINT UNSIGNED, \
                            \"OUTSIDE\"                    TINYINT UNSIGNED, \
                            \"DESCRIPTION\"                VARCHAR(255), \
                            \"UNKNOWN2\"                   TINYINT UNSIGNED, \
                            \"ELECTRICALTYPE\"             TINYINT UNSIGNED, \
                            \"SHOWDESIGNATOR\"             TINYINT UNSIGNED, \
                            \"SHOWNAME\"                   TINYINT UNSIGNED, \
                            \"ROTATION\"                   TINYINT UNSIGNED, \
                            \"LENGTH\"                     INT UNSIGNED, \
                            \"LENGTH_FRAC\"                INT UNSIGNED, \
                            \"LOCATION.X\"                 INT, \
                            \"LOCATION.X_FRAC\"            INT, \
                            \"LOCATION.Y\"                 INT, \
                            \"LOCATION.Y_FRAC\"            INT, \
                            \"COLOR\"                      INT UNSIGNED, \
                            \"NAME\"                       VARCHAR(45), \
                            \"NAME.CUSTOMSETTINGS\"        TINYINT UNSIGNED, \
                            \"NAME.FONTID\"                SMALLINT UNSIGNED, \
                            \"NAME.COLOR\"                 INT UNSIGNED, \
                            \"NAME.CUSTOMPOSITION\"        TINYINT UNSIGNED, \
                            \"NAME.TO\"                    TINYINT UNSIGNED, \
                            \"NAME.MARGIN\"                INT, \
                            \"NAME.ORIENTATION\"           TINYINT UNSIGNED, \
                            \"DESIGNATOR\"                 VARCHAR(45), \
                            \"DESIGNATOR.CUSTOMSETTINGS\"  TINYINT UNSIGNED, \
                            \"DESIGNATOR.FONTID\"          SMALLINT UNSIGNED, \
                            \"DESIGNATOR.COLOR\"           INT UNSIGNED, \
                            \"DESIGNATOR.CUSTOMPOSITION\"  TINYINT UNSIGNED, \
                            \"DESIGNATOR.TO\"              TINYINT UNSIGNED, \
                            \"DESIGNATOR.MARGIN\"          INT, \
                            \"DESIGNATOR.ORIENTATION\"     TINYINT UNSIGNED, \
                            \"OBSOLETE1\"                  TINYINT UNSIGNED, \
                            \"OBSOLETE2\"                  TINYINT UNSIGNED, \
                            \"OBSOLETE3\"                  TINYINT UNSIGNED, \
                            \"PINPACKAGELENGTH\"           INT UNSIGNED, \
                            \"SYMBOL_LINEWIDTH\"           INT UNSIGNED \
                            );");


QString pin::qSqlInsert(LibTable *tab)
{
    lastOpGood = false;

    if (tab->type != RECORD_PIN) return QString("");

    QString columns("");
    QString values("");

    for (int j = 0; j < tab->size(); j++) {
        QStringList pair = tab->at(j).split('=', Qt::SkipEmptyParts);
        pair[1].replace("'", "''");
        columns.append("\"" + pair[0] + "\"");

        if (pair[0] == "DESCRIPTION"
         || pair[0] == "NAME"
         || pair[0] == "DESIGNATOR") {
            values.append("'" + pair[1] + "'");
        }
        else if (pair[0] == "SHOWDESIGNATOR"
              || pair[0] == "SHOWNAME") {
            if (pair[1] == 'T') values.append("1");
            else values.append("0");
        } else values.append(pair[1]);

        if (j < (tab->size() - 1)) columns.append(",");
        if (j < (tab->size() - 1)) values.append(",");
    }

    lastOpGood = true;

    return QString("INSERT INTO Pins (%1) VALUES (%2);").arg(columns).arg(values);
}
