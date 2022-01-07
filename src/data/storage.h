#ifndef STORAGE_H
#define STORAGE_H

#include <QObject>
#include "catalog.h"

class Storage : public QObject
{
    Q_OBJECT

    QList<Catalog*> catalogs;

public:
    QStringList nameList;
    QStringList pathList;

    QList<Catalog*> catalogsList;//для поиска по каталогам или одному каталогу
    QList<BookItem*> booksList;//Список книг для поиска

    QList<BookItem*> currentBooks;//Массив книг каталога для загрузки

public:
    Storage(QObject *parent = nullptr);

    void loadFromFile(QString path);

    Catalog* getCatalogById(int id);
    Catalog* getCatalogByPath(QString path);
    Catalog* getCatalogByBook(BookItem* book);
    Catalog* getCatalogByName(QString name);
    BookItem* getBookByName(QString name);
    QList<Catalog*> Catalogs();
    void addCatalog(Catalog *catalog);
    QStringList getPathList();
    QStringList getNameList();
    int getCount();

};

#endif // STORAGE_H
