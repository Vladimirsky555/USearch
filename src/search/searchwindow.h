#ifndef SEARCHWINDOW_H
#define SEARCHWINDOW_H

#include <QWidget>
#include <QRegExp>
#include <QMessageBox>

#include "helpers/qregexphighlighter.h"
#include "search/booksselector.h"
#include "search/catalogsselector.h"

namespace Ui {
class SearchWindow;
}

class QAction;

//Структура, формирующаяся по результатам поиска в одном цикле
struct searchItem
{
    Catalog *_catalog;
    BookItem *_book;
    ListItem *_chapter;
    TextItem *_section;
    int textCount;
};

//Структура, описывающая номер строки, в которой найдено искомое слово
struct textItem
{
    int id;
    QString line;
};

class SearchWindow : public QWidget
{
    Q_OBJECT

    Storage *s;
    QList<BookItem*> currentBooks;//Массив книг каталога для загрузки
    Catalog *currentCatalog;
    BookItem* currentBook;
    ListItem* currentChapter;
    TextItem* currentSection;
    QString currentTitle;
    QString currentText;

    QStringList nameList;
    QStringList pathList;

    bool resource;

    QList<Catalog*> catalogsList;//для поиска по каталогам или одному каталогу
    QList<BookItem*> booksList;//для поиска по книгам или в отдельной книге

    QList<searchItem> searchItems; //Массив структур, формирующийся по результатам поиска
    QList<textItem> textItems;

    QList<QAction*> listActions;
    QAction *exportTextToFile;
    QAction *exportTextToDisplay;
    QAction *exportResultToFile;
    QAction *exportResultToDisplay;

    //Переменные для подсветки
    QRegexpHighlighter *highlighter1;
    QRegexpHighlighter *highlighter2;

    CatalogsSelector *catalogs_selector;
    BooksSelector *books_selector;

    Ui::SearchWindow *ui;

private:
    bool checkRegExp(QRegExp rx);

public:
    explicit SearchWindow(QWidget *parent = 0);
    ~SearchWindow();

    void loadFromFile(QString path);

    void findInCatalogs();//поиск по всем каталогам
    void findInBooks();

    //Вспомогательные для поиска функции
    void sortResult();
    int findInOneText(int *c, QString txt);
    void addSearchItem(int cnt);

private slots:
    void contextMenuRequsted(const QPoint &p);
    void shutdown();

    //Поиск
    void find();
    void findInChapters();//экшен, привязанный к toolButton
    void on_edtSearch_returnPressed();//поиск по всем каталогам

    //Выбор каталогов, книг для поиска
    void selectBooks();//вызов BooksSelector
    void selectedBooks(QList<BookItem*> selectedBooks);//список книг, результат из chooseFinder
    void selectCatalogs();//вызов CatalogSelector
    void selectedCatalogs(QList<Catalog*> catalogs);
    void chooseFont();

    //Клики по спискам
    void on_lstResults_clicked(const QModelIndex &index);
    void on_lstText_clicked(const QModelIndex &index);

    void text_display_Export();//экспорт цитат только из текста, по которому кликнули мышкой
    void text_file_Export();
    void result_display_Export();//экспорт цитат из всего списка, где она встречается
    void result_file_Export();

    //подсветка
    void on_edtSearch_editingFinished();
};

#endif // SEARCHWINDOW_H
