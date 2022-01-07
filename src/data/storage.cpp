#include "storage.h"

#include <QFile>
#include <QDataStream>

Storage::Storage(QObject *parent) : QObject(parent)
{
    nameList      << "Первый каталог"
                  << "Второй каталог"
                   << "Третий каталог";

    pathList     << ":/doc/catalog_first"
                 << ":/doc/catalog_second"
                 << ":/doc/catalog_third";

    //Грузим из файла названия каталогов и пути к ним
    for(int i = 0; i < pathList.size(); i++)
    {
        Catalog *catalog = new Catalog(nameList[i], pathList[i]);
        loadFromFile(pathList[i]);
        catalog->setBook(currentBooks);
        addCatalog(catalog);
        catalogsList.append(catalog);
    }
}

void Storage::loadFromFile(QString path)
{
    currentBooks.clear();
    QFile f(path);
    if(!f.exists()) return;

    f.open(QFile::ReadOnly);
    QDataStream reader(&f);

    while(!reader.atEnd()){
        QByteArray arr;
        reader >> arr;
        currentBooks.append(new BookItem(arr));
    }

    f.close();
}

Catalog* Storage::getCatalogById(int id)
{
    if(id < catalogs.count()){
        return catalogs[id];
    }
    return NULL;
}

Catalog *Storage::getCatalogByPath(QString path)
{
    for(int i = 0; i < catalogs.size(); i++){
        if(catalogs[i]->getPath() == path){
            return catalogs[i];
        }
    }

    return NULL;
}

Catalog *Storage::getCatalogByBook(BookItem *book)
{
    Catalog *catalog = NULL;
    for(int i = 0; i < catalogs.count(); i++){
        for(int j = 0; j < catalogs[i]->getCount(); j++){
            if(catalogs[i]->getBookById(j) == book){
                return catalogs[i];
            }
        }
    }

    return catalog;
}

Catalog *Storage::getCatalogByName(QString name)
{
    for(int i = 0; i < catalogs.count(); i++){
        if(catalogs[i]->getName() == name)
            return catalogs[i];
    }

    return NULL;
}


BookItem *Storage::getBookByName(QString name)
{
    BookItem *currentBook = NULL;
    for(int i = 0; i < catalogs.count(); i++){
        for(int j = 0; j < catalogs[i]->getCount(); j++){

            currentBook = catalogs[i]->getBookById(j);
            if(currentBook->getName() == name){
                return currentBook;
            }
        }
    }

    return currentBook;
}

QList<Catalog *> Storage::Catalogs()
{
    return this->catalogs;
}

void Storage::addCatalog(Catalog* catalog)
{
    catalogs.append(catalog);
}


QStringList Storage::getPathList()
{
    QStringList pathList;
    for(int i = 0; i < catalogs.count(); i++)
    {
        pathList.append(catalogs.at(i)->getPath());
    }

    return pathList;
}

QStringList Storage::getNameList()
{
    QStringList namesList;
    for(int i = 0; i < catalogs.count(); i++)
    {
        namesList.append(catalogs.at(i)->getName());
    }

    return namesList;
}


int Storage::getCount()
{
    return catalogs.count();
}


