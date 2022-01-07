#ifndef SEARCHITEM_H
#define SEARCHITEM_H

#include <QObject>

#include "data/storage.h"


class searchItem : public QObject
{
    Q_OBJECT

public:
    Catalog *_catalog;
    BookItem *_book;
    ListItem *_chapter;
    TextItem *_section;
    int textCount;//количество повторов в тексте
    int textId;//порядковый номер текста
    int c;//порядковый номер элемента

public:
    searchItem(QObject *parent = nullptr);
};

#endif // SEARCHITEM_H
