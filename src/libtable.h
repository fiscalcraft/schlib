#ifndef LIBTABLE_H
#define LIBTABLE_H
#include <QtCore>
#include <QtSql>
#include <coml2api.h>

enum RECORD_TYPE {
    RECORD_UNKNOWN,
    RECORD_COMPONENT,
    RECORD_PIN,
    RECORD_TEXT       = 4,
    RECORD_BEZIER,
    RECORD_POLYLINE,
    RECORD_POLYGONE,
    RECORD_ELLIPSE,
    RECORD_ELARC      = 11,
    RECORD_ARC,
    RECORD_LINE,
    RECORD_RECTANGLE,
    RECORD_DESIGNATOR = 34,
    RECORD_ATTRIBUTE  = 41,
    RECORD_END        = 44
};

#define PIN_CUSTOM_POS_FLAG ((UCHAR)0x01)
#define PIN_MARGIN_TO_FLAG ((UCHAR)0x02)
#define PIN_ORIENTATION_FLAG ((UCHAR)0x04)
#define PIN_CUSTOM_SET_FLAG ((UCHAR)0x10)

class PinData
{
public:
    PinData(LPSTORAGE st, const QString& name);
    ~PinData() {
        if (s != NULL) s->Release();
    }

    QByteArray getData(UINT32 pnum) const;

private:
    LPSTREAM s;
    LARGE_INTEGER start;
    UINT32 weight;
};

struct stStreams {
    LPSTREAM Data;
    PinData *PinFrac;
    PinData *PinPackageLength;
    PinData *PinSymbolLineWidth;
    PinData *PinTextData;
    UINT32 pnum;
    INT32 insheet;
};

class LibTable : public QStringList
{
public:
    LibTable(stStreams& s);
    RECORD_TYPE type;
};


class PinTable : public QStringList
{
    struct custom {
        bool customPosition;    // Custom Position flag
        UINT8 to;    // Margin to pin or component
        UINT8 orientation; // 0/90 degrees
        bool customSettings;
        UINT32 margin; //
        UINT16 fontNum;
        UINT32 color;
    };

public:
    PinTable(stStreams& s);

private:
    int setCustoms(QByteArray& data, int index, struct custom& dest);
};

#endif // LIBTABLE_H
