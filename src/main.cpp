#include <QtCore>
#include <QtSql>
#include <coml2api.h>
#include "libtable.h"
#include "libquery.h"
#include "pin.h"
#include "text.h"
#include "ellipse.h"
#include "elarc.h"
#include "arc.h"
#include "line.h"
#include "rectangle.h"
#include "attribute.h"

//QTextStream out(stdout);

int main(int argc, char *argv[])
{
    if (argc < 2) {
        qDebug() << "Need filename";
        return 1;
    }

    QString fname = QString::fromLocal8Bit(argv[1]);
    LPSTORAGE s = NULL;
    stStreams p = {NULL, NULL, NULL, NULL, NULL, 0, 0};
    LibTable *tab;

    //---------------------------Открываем файл библиотеки--------------------------
    if (FAILED(StgOpenStorage((OLECHAR*)fname.utf16(), NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, 0, &s)))
    {
        qDebug() << "Can't open file " << fname;
        return 1;
    }

    //---------------------------Открываем и читаем FileHeader-------------------------------

    if (FAILED(s->OpenStream((OLECHAR*)QString("FileHeader").utf16(), NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &p.Data))) {
        qDebug() << "Can't open FileHeader";
        return 1;
    }

    tab = new LibTable(p);

    p.Data->Release();
    //-------------------Проверяем наличие БД. Если есть - удаляем---------------------
    QString hdbName = QFileInfo(fname).baseName().append("_fh").append(".db");

    if (QFileInfo::exists(hdbName)) {
        if (false == QFile::remove(hdbName)) {
            qDebug() << "Can't remove file " << hdbName;
            return 1;
        }
    }

    //----------------------------Создаём БД-------------------------------------------
    QSqlDatabase hdb = QSqlDatabase::addDatabase("QSQLITE", "fh");
    hdb.setDatabaseName(hdbName);

    if (!hdb.open()) {
        qDebug() << hdb.lastError().text();
        return 1;
    }

//-------------------------------------------For Unknown objects-----------------------------------------------
    QString qSqlCreate("CREATE TABLE %1(\"name\" VARCHAR(45) PRIMARY KEY NOT NULL, \"value\" VARCHAR(256));");
    QString qSqlInsert("INSERT INTO %1 (name, value) VALUES('%2', '%3');");
//-------------------------------------------------------------------------------------------------------------

    LibQuery a_query(hdb);
    //---------------------------Создаём таблицу FileHeader----------------------------
    if (a_query.exec(qSqlCreate.arg("FileHeader")) == false) {
        qDebug() << "Can't create table";
        return 1;
    }

    //---------------------------Заполняем таблицу FileHeader------------------------------
    for (int i = 0; i < tab->size(); i++) {
        QStringList pair = tab->at(i).split('=', Qt::SkipEmptyParts);
        if(a_query.exec(qSqlInsert.arg("FileHeader").arg(pair[0]).arg(pair[1])) == false) {
            qDebug() << "Can't create table";
            return 1;
        }
    }

    //--------------------------------------------------------------------------------------
    delete tab;

    QSqlDatabase sdb = QSqlDatabase::addDatabase("QSQLITE");

    int compCount = a_query.getIntValue("FileHeader", "COMPCOUNT");

    for (int compNum = 0; compNum < compCount; compNum++) {

        LPSTORAGE sComp;
    //----------------------------Получаем имя компонента-----------------------------------
        QString compName = a_query.getStringValue("FileHeader", QString("LIBREF%1").arg(compNum));

        qDebug() << "extracting" << compName;
    //--------------------------------------------------------------------------------------
        QString sdbName = compName + ".db";

        if (QFileInfo::exists(sdbName)) {
            if (false == QFile::remove(sdbName)) {
                qDebug() << "Can't remove file " << sdbName;
                return 1;
            }
        }

//        sdb = QSqlDatabase::addDatabase("QSQLITE");
        sdb.setDatabaseName(sdbName);

        if (!sdb.open()) {
            qDebug() << sdb.lastError().text();
            return 1;
        }

        LibQuery b_query(sdb);

        if (FAILED(s->OpenStorage((OLECHAR*)compName.utf16(), NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, NULL, 0, &sComp))) {
            qDebug() << "Can't open component storage";
            return 1;
        }

        if (FAILED(sComp->OpenStream((OLECHAR*)QString("Data").utf16(), NULL, STGM_READWRITE|STGM_SHARE_EXCLUSIVE, 0, &p.Data))) {
            qDebug() << "Can't open Data";
            return 1;
        }

        try {
//----------------- Открываем потоки хранилища--------------------------------
            p.PinFrac = new PinData(sComp, "PinFrac");
            p.PinTextData = new PinData(sComp, "PinTextData");
            p.PinPackageLength = new PinData(sComp, "PinPackageLength");
            p.PinSymbolLineWidth = new PinData(sComp, "PinSymbolLineWidth");
//-----Инициализируем счётчик выводов, счётчик индекса выводов и флаг условия продолжения цикла----------------
            p.pnum = 0;
            p.insheet = 0;

            bool next = true;
//----------------------Цикл создания таблиц примитивов компонента----------------------------
            for (int i = 0; next; i++) {
                tab = new LibTable(p);                          // Создаём очередной примитив из потоков хранилища

                switch (tab->type) {
// Если примитив - вывод, отдельную таблицу для него не создаём, а помещаем в таблицу Pins
                case RECORD_PIN: {
//--------------Создаём пустую таблицу Pins, если не создана-------------------------------
                    if (!b_query.isTableExist(pin::Table)) {
                        if (b_query.exec(pin::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Pins";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем вывод в таблицу-----------------------------------
                    QString req = pin::qSqlInsert(tab);
                    delete tab;
                    if (!pin::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Pins" << req; return 1; }
                    break;
                }
// Если примитив - текст, отдельную таблицу для него не создаём, а помещаем в таблицу Texts
                case RECORD_TEXT: {
//--------------Создаём пустую таблицу Texts, если не создана-------------------------------
                    if (!b_query.isTableExist(text::Table)) {
                        if (b_query.exec(text::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Texts";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем текст в таблицу-----------------------------------
                    QString req = text::qSqlInsert(tab);
                    delete tab;
                    if (!text::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Texts" << req; return 1; }
                    break;
                }
// Если примитив - эллиптическая дуга, отдельную таблицу для него не создаём, а помещаем в таблицу Elarcs
                case RECORD_ELARC: {
//--------------Создаём пустую таблицу Elarcs, если не создана-------------------------------
                    if (!b_query.isTableExist(elarc::Table)) {
                        if (b_query.exec(elarc::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Elarcs";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем эллиптическую дугу в таблицу-----------------------------------
                    QString req = elarc::qSqlInsert(tab);
                    delete tab;
                    if (!elarc::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Elarcs" << req; return 1; }
                    break;
                }
// Если примитив - дуга, отдельную таблицу для него не создаём, а помещаем в таблицу Arcs
                case RECORD_ARC: {
//--------------Создаём пустую таблицу Arcs, если не создана-------------------------------
                    if (!b_query.isTableExist(arc::Table)) {
                        if (b_query.exec(arc::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Arcs";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем дугу в таблицу-----------------------------------
                    QString req = arc::qSqlInsert(tab);
                    delete tab;
                    if (!arc::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Arcs" << req; return 1; }
                    break;
                }
// Если примитив - эллипс, отдельную таблицу для него не создаём, а помещаем в таблицу Ellipses
                case RECORD_ELLIPSE: {
//--------------Создаём пустую таблицу Ellipses, если не создана-------------------------------
                    if (!b_query.isTableExist(ellipse::Table)) {
                        if (b_query.exec(ellipse::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Ellipses";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем эллипс в таблицу-----------------------------------
                    QString req = ellipse::qSqlInsert(tab);
                    delete tab;
                    if (!ellipse::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Ellipses" << req; return 1; }
                    break;
                }
// Если примитив - линия, отдельную таблицу для него не создаём, а помещаем в таблицу Lines
                case RECORD_LINE: {
//--------------Создаём пустую таблицу Lines, если не создана-------------------------------
                    if (!b_query.isTableExist(line::Table)) {
                        if (b_query.exec(line::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Lines";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем линию в таблицу-----------------------------------
                    QString req = line::qSqlInsert(tab);
                    delete tab;
                    if (!line::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Lines" << req; return 1; }
                    break;
                }
// Если примитив - прямоугольник, отдельную таблицу для него не создаём, а помещаем в таблицу Rectangles
                case RECORD_RECTANGLE: {
//--------------Создаём пустую таблицу Lines, если не создана-------------------------------
                    if (!b_query.isTableExist(rectangle::Table)) {
                        if (b_query.exec(rectangle::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Lines";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем линию в таблицу-----------------------------------
                    QString req = rectangle::qSqlInsert(tab);
                    delete tab;
                    if (!rectangle::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Rectangles" << req; return 1; }
                    break;
                }
// Если примитив - аттрибут, отдельную таблицу для него не создаём, а помещаем в таблицу Attributes
                case RECORD_ATTRIBUTE: {
//--------------Создаём пустую таблицу Attributes, если не создана-------------------------------
                    if (!b_query.isTableExist(attr::Table)) {
                        if (b_query.exec(attr::qSqlCreateTable) == false) {
                            qDebug() << "Can't create table Attributes";
                            delete tab;
                            return 1;
                        }
                    }
//--------------------------------Добавляем аттрибут в таблицу-----------------------------------
                    QString req = attr::qSqlInsert(tab);
                    delete tab;
                    if (!attr::lastOpGood) return 1;
                    if (b_query.exec(req) == false) { qDebug() << "Can't fill table Attributes" << req; return 1; }
                    break;
                }
//------------------Для неизвестного примитива создаём отдельную таблицу----------------------------
                default: {
                    QString tabName = QString::asprintf("Data%d",i);
                    //---------------------------Создаём таблицу Data<i>----------------------------
                    if (b_query.exec(qSqlCreate.arg(tabName)) == false) {
                        qDebug() << "Can't create table" << tabName;
                        delete tab;
                        return 1;
                    }

                    //---------------------------Заполняем таблицу----------------------------------
                    for (int j = 0; j < tab->size(); j++) {
                        QStringList pair = tab->at(j).split('=', Qt::SkipEmptyParts);
                        pair[1].replace("'", "''");
                        if(b_query.exec(qSqlInsert.arg(tabName).arg(pair[0]).arg(pair[1])) == false) {
                            qDebug() << "Can't fill table" << tabName << pair[0] << pair[1];
                            delete tab;
                            return 1;
                        }
                    }

                    next = (tab->first() != "RECORD=44");
                    delete tab;
                }
                }
            }

        } catch (const QString& e) {
            qDebug() << e;
        }

        p.Data->Release();
        sComp->Release();
        sdb.close();
    }

    hdb.close();
    s->Release();

    return 0;
}
