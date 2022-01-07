#ifndef SEARCHSTORAGE_H
#define SEARCHSTORAGE_H

#include <QObject>

#include "data/searchitem.h"
#include "data/quoteitem.h"

//Хранилище для текстов с ключевым словом и цитат, на основе этих текстов

class SearchStorage : public QObject
{
    Q_OBJECT

public:
    QList<searchItem*> searchItems; //Массив текстов, формирующийся по результатам поиска
    QList<quoteItem*> quoteItems; // Массив цитат, формирующийся из массива текстов

public:
    SearchStorage(QObject *parent = nullptr);

    //////////// Раздел текстов ////////////
    //Фунции для организации поиска
    void destroySearchList();
    void addSearchItem(int cnt, Catalog *cat, BookItem *book,
                       ListItem *chapter, TextItem *section);


    ///////////// Раздел цитат //////////////
    //Вспомогательная функция для вывода всех цитат на дисплей.
    //Раньше это было событие на кнопку, сейчас функция для передачи в другое окно
    //Функция возвращает список цитат и источников одной строкой в формате html
    QString exportToDisplay();
    void addQuoteItem(int cnt, QString str, searchItem* si);

    void sortResult();
};

#endif // SEARCHSTORAGE_H
