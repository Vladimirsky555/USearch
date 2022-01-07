#include "searchstorage.h"

SearchStorage::SearchStorage(QObject *parent) : QObject(parent){}

void SearchStorage::destroySearchList()
{
    for(int i = 0; i < searchItems.count(); i++)
    {
        delete searchItems[i];
    }

    for(int i = 0; i < quoteItems.count(); i++)
    {
        delete quoteItems[i];
    }
}

//Формирование элемента списка текстов, в которых есть хотя бы одно ключевое слово
void SearchStorage::addSearchItem(int cnt, Catalog *cat, BookItem *book,
                                  ListItem *chapter, TextItem *section)
{
    searchItem *item = new searchItem();
    item->_catalog = cat;
    item->_book = book;
    item->_chapter = chapter;
    item->_section = section;
    item->textCount = cnt;//количество повторений фразы
    searchItems.append(item);
}

//Формирование цитаты, на основе списка указателей на тексты
void SearchStorage::addQuoteItem(int cnt, QString str, searchItem *si)
{
    quoteItem *item = new quoteItem();
    item->res += str;
    item->res += "<b><span style=\"color:#483D8B\"><br>(";
    item->res += si->_book->getName() + ", ";
    item->res +=  si->_chapter->getName() + ", ";
    item->res += si->_section->getName();
    item->res +=   ")</span></b>";
    item->id = cnt;
    item->itemSearch = si;
    quoteItems.append(item);
}

void SearchStorage::sortResult()
{
    for(int i = 0; i < searchItems.count(); i++)
    {
        for(int j = 0; j < searchItems.count() - 1; j++)
        {
            if(searchItems[j]->textCount < searchItems[j + 1]->textCount)
            {
                searchItem *tmp = searchItems[j];
                searchItems[j] = searchItems[j + 1];
                searchItems[j + 1] = tmp;
            }
        }
    }
}

//Вспомогательная функция для вывода всех цитат по ключевому слову в отдельное окно
QString SearchStorage::exportToDisplay()
{
    QString result;
    for(int i = 0; i < quoteItems.count(); i++ )
    {
        result += "<b><span style=\"color:#800000\">";
        result += QString::number(i+1);
        result += ": </span></b>";
        result += quoteItems[i]->res;
        result += "<br>";
        result += "<br>";
    }

    return result;
}




