#include "libtable.h"

LibTable::LibTable(stStreams &s) : QStringList()
{
    ULONG uCount;
    UINT32 recSize;
    QByteArray *b;
    type = RECORD_UNKNOWN;

    if(FAILED(s.Data->Read(&recSize, sizeof(recSize), &uCount))) {
        throw "Can't read size of table";
    }

    if (recSize >= 0x01000000L) {
        LARGE_INTEGER step;
        step.QuadPart = -4;
        type = RECORD_PIN;
        recSize -= 0x01000000L;
        s.Data->Seek(step, STREAM_SEEK_CUR, NULL);
        qDebug() << "extract Pin" << s.pnum;
        append(static_cast<QStringList>(PinTable(s)));
    } else {
        type = RECORD_UNKNOWN;

        b = new QByteArray(recSize, 0);

        if(FAILED(s.Data->Read(b->data(), recSize, &uCount))) {
            delete b;
            throw "Can't read table";
        }

        QString fHeader(QString::fromLocal8Bit(b->data()));
        delete b;

        QStringList lst = fHeader.split('|', Qt::SkipEmptyParts);
        type = static_cast<RECORD_TYPE>( lst.first().split('=', Qt::SkipEmptyParts)[1].toInt() );
        QString record;
        switch (type) {
        case RECORD_COMPONENT:
            record = QString("Component"); break;
        case RECORD_TEXT:
            record = QString("Text"); break;
//        case RECORD_BEZIER:
//            record = QString("Bezier"); break;
//        case RECORD_POLYLINE:
//            record = QString("Polyline"); break;
//        case RECORD_POLYGONE:
//            record = QString("Polygone"); break;
        case RECORD_ELLIPSE:
            record = QString("Ellipse"); break;
        case RECORD_ELARC:
            record = QString("Elliptical Arc"); break;
        case RECORD_ARC:
            record = QString("Arc"); break;
        case RECORD_LINE:
            record = QString("Line"); break;
        case RECORD_RECTANGLE:
            record = QString("Rectangle"); break;
        case RECORD_DESIGNATOR:
            record = QString("Designator"); break;
        case RECORD_ATTRIBUTE:
            record = QString("Attribute"); break;
        case RECORD_END:
            record = QString("End"); break;
        default:
            record = lst.first();
        }

        qDebug() << "extract" << record;
        append(lst);

        bool isInsheet = false;
        for (int j = 0; j < lst.size(); j++) {
            QStringList pair = lst.at(j).split('=', Qt::SkipEmptyParts);
            if (pair[0] == "INDEXINSHEET") {
                isInsheet = true;
                s.insheet = pair[1].toInt()+1;
            }
        }
        if (isInsheet == false) s.insheet = 1;
    }
}


PinTable::PinTable(stStreams &s) : QStringList()
{

    ULONG uCount;
    UINT32 recSize;
    QByteArray *b;
    int index;

    if(FAILED(s.Data->Read(&recSize, sizeof(recSize), &uCount))) {
        throw "Can't read size of pin";
    }
    if (recSize >= 0x01000000L) {
        recSize -= 0x01000000L;
    } else return;

    b = new QByteArray(recSize,0);

    if(FAILED(s.Data->Read(b->data(), recSize, &uCount))) {
        delete b;
        throw "Can't read table";
    }

    index = 0;
    recSize = *(UINT32*)b->mid(index, sizeof(UINT32)).data();
    append(QString("RECORD=%1").arg(recSize));
    index += sizeof(UINT32);

    append(QString("INDEXINSHEET=%1").arg(s.insheet++));

    append(QString("UNKNOWN1=%1").arg((UCHAR)b->at(index++)));

    append(QString("OWNERPARTID=%1").arg(*(UINT16*)b->mid(index,sizeof(UINT16)).data()));
    index += sizeof(UINT16);

    if (b->at(index) != 0) {
        append(QString("OWNERPARTDISPLAYMODE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) != 0) {
        append(QString("INSIDEEDGE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) != 0) {
        append(QString("OUTSIDEEDGE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) != 0) {
        append(QString("INSIDE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) != 0) {
        append(QString("OUTSIDE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) != 0) {
        QString descr = QString::fromLocal8Bit(b->mid(13, b->at(index)));
        append(QString("DESCRIPTION=").append(descr));
        index += b->at(index);
    }
    index += 1;

    append (QString("UNKNOWN2=%1").arg((UCHAR)b->at(index)));
    index += 1;

    if (b->at(index) != 0) {
        append(QString("ELECTRICALTYPE=%1").arg((UCHAR)b->at(index)));
    }
    index += 1;

    if (b->at(index) & 0x10) {
        append(QString("SHOWDESIGNATOR=T"));
    }
    if (b->at(index) & 0x08) {
        append(QString("SHOWNAME=T"));
    }
    if (b->at(index) & 0x03) {
        append(QString("ROTATION=%1").arg(b->at(index) & 0x03));
    }
    index += 1;

    QByteArray slw = s.PinFrac->getData(s.pnum);

    UINT16 wd = *(UINT16*)b->mid(index, sizeof(UINT16)).data();
    append(QString("LENGTH=%1").arg(wd));
    index += sizeof(UINT16);

    if (slw.size() == 12) {
        INT32 frac = *reinterpret_cast<INT32*>(slw.right(sizeof(INT32)).data());
        append(QString("LENGTH_FRAC=%1").arg(frac));
    }

    INT16 swd = *(INT16*)b->mid(index, sizeof(INT16)).data();
    append(QString("LOCATION.X=%1").arg(swd));
    index += sizeof(INT16);

    if (slw.size() == 12) {
        INT32 frac = *reinterpret_cast<INT32*>(slw.left(sizeof(INT32)).data());
        append(QString("LOCATION.X_FRAC=%1").arg(frac));
    }

    swd = *(INT16*)b->mid(index, sizeof(INT16)).data();
    append(QString("LOCATION.Y=%1").arg(swd));
    index += sizeof(INT16);

    if (slw.size() == 12) {
        INT32 frac = *reinterpret_cast<INT32*>(slw.mid(sizeof(INT32), sizeof(INT32)).data());
        append(QString("LOCATION.Y_FRAC=%1").arg(frac));
    }

    recSize = *(UINT32*)b->mid(index, sizeof(UINT32)).data();
    append(QString("COLOR=%1").arg(recSize));
    index += sizeof(UINT32);

    struct custom csName, csDes;
    int idx = 0;
    slw = s.PinTextData->getData(s.pnum);

    idx = setCustoms(slw, idx, csName);
    idx = setCustoms(slw, idx, csDes);

    recSize = b->at(index);
    index += 1;

    if (recSize > 0) {
        QString name = QString::fromLocal8Bit(b->mid(index, recSize));
        append(QString("NAME=").append(name));
    }
    index += recSize;

    if (csName.customSettings) {
        append(QString("NAME.CUSTOMSETTINGS=T"));
        append(QString("NAME.FONTID=%1").arg(csName.fontNum));
        append(QString("NAME.COLOR=%1").arg(csName.color));
    }

    if (csName.customPosition) {
        append(QString("NAME.CUSTOMPOSITION=T"));
        append(QString("NAME.TO=%1").arg(csName.to));
        append(QString("NAME.MARGIN=%1").arg(csName.margin));
        append(QString("NAME.ORIENTATION=%1").arg(csName.orientation));
    }

    recSize = b->at(index);
    index += 1;

    if (recSize > 0) {
        QString des = QString::fromLocal8Bit(b->mid(index, recSize));
        append(QString("DESIGNATOR=").append(des));
    }
    index += recSize;

    if (csDes.customSettings) {
        append(QString("DESIGNATOR.CUSTOMSETTINGS=T"));
        append(QString("DESIGNATOR.FONTID=%1").arg(csDes.fontNum));
        append(QString("DESIGNATOR.COLOR=%1").arg(csDes.color));
    }

    if (csDes.customPosition) {
        append(QString("DESIGNATOR.CUSTOMPOSITION=T"));
        append(QString("DESIGNATOR.TO=%1").arg(csDes.to));
        append(QString("DESIGNATOR.MARGIN=%1").arg(csDes.margin));
        append(QString("DESIGNATOR.ORIENTATION=%1").arg(csDes.orientation));
    }

    recSize = b->at(index);
    index += 1;

    if (recSize > 0) {
        QString des = QString::fromLocal8Bit(b->mid(index, recSize));
        append(QString("OBSOLETE1=").append(des));
    }
    index += recSize;

    recSize = b->at(index);
    index += 1;

    if (recSize > 0) {
        QString des = QString::fromLocal8Bit(b->mid(index, recSize));
        append(QString("OBSOLETE2=").append(des));
    }
    index += recSize;

    recSize = b->at(index);
    index += 1;

    if (recSize > 0) {
        QString des = QString::fromLocal8Bit(b->mid(index, recSize));
        append(QString("OBSOLETE3=").append(des));
    }
    index += recSize;

    delete b;

    slw = s.PinPackageLength->getData(s.pnum);
    if (slw.size() > 4) {
        recSize = *reinterpret_cast<UINT32*>(slw.left(sizeof(UINT32)).data());
        QByteArray wstr = QByteArray(slw.right(slw.size() - sizeof(UINT32)));

        append (QString::fromUtf16(reinterpret_cast<ushort*>(wstr.data()), recSize/2).remove('|'));
    }

    slw = s.PinSymbolLineWidth->getData(s.pnum);
    if (slw.size() > 4) {
        recSize = *reinterpret_cast<UINT32*>(slw.left(sizeof(UINT32)).data());
        QByteArray wstr = QByteArray(slw.right(slw.size() - sizeof(UINT32)));

        append (QString::fromUtf16(reinterpret_cast<ushort*>(wstr.data()), recSize/2).remove('|'));
    }
    s.pnum++;

}

int PinTable::setCustoms(QByteArray& data, int index, struct custom& dest)
{
    int leave = data.size()-index;
    int start = index;

    if (leave < 0) throw QString("error in PinTextData");

    dest.customSettings = false;
    dest.to = 0;
    dest.orientation = 0;
    dest.customPosition = false;
    dest.color = 0xFFFFFFFFL;
    dest.margin = 0xFFFFFFFFL;
    dest.fontNum = 0xFFFF;

    if (leave == 0) return index;

    index += 1;
    if (data.at(start) == 0x00) return index;

    if (data.at(start) & PIN_CUSTOM_POS_FLAG) {
        if (leave < 4) throw QString("error in PinTextData (positions)");
        dest.customPosition = true;
        if (data.at(start) & PIN_MARGIN_TO_FLAG) {
            dest.to = 1;
        }
        if (data.at(start) & PIN_ORIENTATION_FLAG) {
            dest.orientation = 1;
        }
        dest.margin = *reinterpret_cast<UINT32*>(data.mid(index,sizeof(UINT32)).data());
        index += 4;
    }

    if (data.at(start) & PIN_CUSTOM_SET_FLAG) {
        if (leave < 6) throw QString("error in PinTextData (settings)");
        dest.customSettings = true;
        dest.fontNum = *reinterpret_cast<UINT16*>(data.mid(index,sizeof(UINT16)).data());
        dest.color = *reinterpret_cast<UINT32*>(data.mid(index+2,sizeof(UINT32)).data());
        index += 6;
    }
    return index;
}


PinData::PinData(LPSTORAGE st, const QString& name)
{
    ULONG readed;
    INT32 len;

    s = NULL;
    if (FAILED(st->OpenStream((OLECHAR*)name.utf16(), NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &s))) {
        return;
    }
    if (s ==NULL) return;

    if(FAILED(s->Read(&len, sizeof(INT32), &readed))) {
        throw "error: Can't read size of stream header\n";
    }
    start.QuadPart = len+sizeof(INT32);

    QByteArray *ptr = new QByteArray(len, 0);

    if(FAILED(s->Read(ptr->data(), len, &readed))) {
        throw "error: Can't read stream header\n";
    }

    QString hdr(*ptr);
    weight = hdr.split("|", Qt::SkipEmptyParts)[1].split("=", Qt::SkipEmptyParts)[1].toInt();
}

QByteArray PinData::getData(UINT32 pnum) const
{
    ULONG uCount;
    UINT32 uzSize = 0;

    if (s == NULL) return QByteArray();

    if (FAILED(s->Seek(start, STREAM_SEEK_SET, NULL))) throw "can't read pin data (no seek)";

    for (UINT32 i = 0; i < weight; i++) {

        if (FAILED(s->Read(&uzSize, sizeof(UINT32), &uCount))) throw "can't read pin data (no len)";
        if (uzSize < 0x01000000L) throw "can't read pin data (bad format)";
        uzSize -= 0x01000000L;

        QByteArray *ptr = new QByteArray(uzSize, 0);

        if (FAILED(s->Read(ptr->data(), uzSize, &uCount))) throw "can't read pin data";
        if (ptr->at(0) != (CHAR)0xd0) throw "can't read pin data (no d0)";

        UINT32 num = ptr->mid(2,ptr->at(1)).toInt();

        if (num == pnum) {
            UINT32 len = *reinterpret_cast<UINT32*>(ptr->mid(2+ptr->at(1), sizeof(UINT32)).data());
            QByteArray retval = QByteArray(qUncompress(ptr->right(len).insert(0,sizeof(UINT32),0)));
            delete ptr;
            return retval;
        }

        delete ptr;
    }

    return QByteArray();
}

