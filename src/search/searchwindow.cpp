#include "searchwindow.h"
#include "ui_searchwindow.h"

#include <QScrollBar>
#include <QFontDialog>
#include <QMenu>
#include <QTextStream>
#include <QFileDialog>


SearchWindow::SearchWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SearchWindow)
{
    ui->setupUi(this);
    setWindowTitle("Поисковик");

    s = new Storage();

    this->currentBook = NULL;
    this->currentChapter = NULL;
    this->currentSection = NULL;

    nameList      << "Первый каталог"
                  << "Второй каталог"
                   << "Третий каталог";

    pathList     << ":/doc/first_catalog"
                 << ":/doc/second_catalog"
                 << ":/doc/third_catalog";


    //Грузим из файла названия каталогов и пути к ним
    for(int i = 0; i < pathList.size(); i++)
    {
        Catalog *catalog = new Catalog(nameList[i], pathList[i]);
        loadFromFile(pathList[i]);
        catalog->setBook(currentBooks);
        s->addCatalog(catalog);
        catalogsList.append(catalog);
    }

    this->resource = true;
    //this->catalogsList = s->Catalogs();//по умолчанию ищем по всем каталогам

    //Подсветка
    highlighter1 = new QRegexpHighlighter(this);
    highlighter1->setDocument(ui->edtText->document());
    highlighter1->setPattern(ui->edtSearch->text());

    ui->edtText->setEnabled(false);
    ui->label->setEnabled(false);
    ui->edtSource->setEnabled(false);
    ui->lstResults->setEnabled(false);
    ui->lstText->setEnabled(false);

    //Подключим ко всем toolButton экшены
    ui->btnFind->setDefaultAction(ui->actionFind);
    connect(ui->actionFind, SIGNAL(triggered()), this, SLOT(find()));

    ui->btnFindChapters->setDefaultAction(ui->actionFindChapters);
    connect(ui->actionFindChapters, SIGNAL(triggered()), this, SLOT(findInChapters()));

    ui->btnBooksSelect->setDefaultAction(ui->action_books_select);
    connect(ui->action_books_select, SIGNAL(triggered()), this, SLOT(selectBooks()));

    ui->btnCatalogsSelect->setDefaultAction(ui->action_catalogs_select);
    connect(ui->action_catalogs_select, SIGNAL(triggered()), this, SLOT(selectCatalogs()));

    ui->btnFont->setDefaultAction(ui->actionFont);
    connect(ui->actionFont, SIGNAL(triggered()), this, SLOT(chooseFont()));

    //Для экспорта текстов с помощью событий
    ui->lstResults->setContextMenuPolicy(Qt::ActionsContextMenu);
    connect(this, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(contextMenuRequsted(QPoint)));

    {
        QAction *A = exportTextToDisplay= new QAction(this);
        QPixmap p(":/images/export-to-display.png");
        A->setIcon(QIcon(p));
        A->setText(tr("Экспорт цитат текста на дисплей"));
        connect(A, SIGNAL(triggered()),this, SLOT(text_display_Export()));
        A->setFont(QFont ("MS Shell Dlg 2", 11));
        ui->lstResults->addAction(A);
        listActions << A;
    }{
        QAction *A = exportResultToDisplay= new QAction(this);
        QPixmap p(":/images/export-to-display.png");
        A->setIcon(QIcon(p));
        A->setText(tr("Экспорт всех цитат на дисплей"));
        connect(A, SIGNAL(triggered()),this, SLOT(result_display_Export()));
        A->setFont(QFont ("MS Shell Dlg 2", 11));
        ui->lstResults->addAction(A);
        listActions << A;
    }{
        QAction *A = exportTextToFile = new QAction(this);
        QPixmap p(":/images/export-in-file.png");
        A->setIcon(QIcon(p));
        A->setText(tr("Экспорт цитат текста в файл"));
        connect(A, SIGNAL(triggered()),this, SLOT(text_file_Export()));
        A->setFont(QFont ("MS Shell Dlg 2", 11));
        ui->lstResults->addAction(A);
        listActions << A;
    }{
        QAction *A = exportResultToFile = new QAction(this);
        QPixmap p(":/images/export-in-file.png");
        A->setIcon(QIcon(p));
        A->setText(tr("Экспорт всех цитат в файл"));
        connect(A, SIGNAL(triggered()),this, SLOT(result_file_Export()));
        A->setFont(QFont ("MS Shell Dlg 2", 11));
        ui->lstResults->addAction(A);
        listActions << A;
    }
}

SearchWindow::~SearchWindow()
{
    delete ui;
}

void SearchWindow::loadFromFile(QString path)
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

void SearchWindow::sortResult()
{
    for(int i = 0; i < searchItems.count(); i++)
    {
        for(int j = 0; j < searchItems.count() - 1; j++)
        {
            if(searchItems[j].textCount < searchItems[j + 1].textCount)
            {
                searchItem tmp = searchItems[j];
                searchItems[j] = searchItems[j + 1];
                searchItems[j + 1] = tmp;
            }
        }
    }
}

int SearchWindow::findInOneText(int *c, QString txt)
{
    int cnt = 0;
    QRegExp rx(ui->edtSearch->text());
    if(!checkRegExp(rx)) return 0;
    int pos = 0;
    while((pos = rx.indexIn(txt, pos)) != -1){
        pos += rx.matchedLength();
        *c += 1;
        cnt++;
    }

    return cnt;
}

void SearchWindow::addSearchItem(int cnt)
{
    searchItem s;
    s._catalog = currentCatalog;
    s._book = currentBook;
    s._chapter = currentChapter;
    s._section = currentSection;
    s.textCount = cnt;
    searchItems.append(s);
}


void SearchWindow::findInCatalogs()
{
    ui->edtText->setEnabled(true);
    ui->edtSource->setEnabled(true);
    ui->label->setEnabled(true);
    ui->lstResults->setEnabled(true);
    ui->lstResults->clear();
    ui->edtSource->clear();
    ui->edtText->clear();
    searchItems.clear();

    int c = 0;
    for(int k = 0; k < catalogsList.count(); k++){

        currentCatalog = catalogsList[k];
        int cnt = 0;

        for(int l = 0; l < currentCatalog->getCount(); l++){
            currentBook = currentCatalog->getBookById(l);

            for(int i = 0; i < currentBook->getCount(); i++){
                currentChapter = currentBook->getChapterById(i);

                for(int j = 0; j < currentChapter->getCount(); j++){
                    currentSection = currentChapter->getSectionById(j);

                    cnt = findInOneText(&c, currentSection->getData());
                    if(cnt != 0){
                        addSearchItem(cnt);
                    }
                    cnt = 0;
                }
            }
        }
    }

    sortResult();

    //Отображение списка результатов в нижнем виджете
    int n = 0;
    for(int i = 0; i < searchItems.size(); i++,n++){
        ui->lstResults->addItem(QString::number(n+1) + ": " + //Порядковый номер
                                "[" + QString::number(searchItems[i].textCount) + "] " + //Число совпадений в тексте
                                searchItems[i]._catalog->getName() + ", " + searchItems[i]._book->getName() + ", " +
                                searchItems[i]._chapter->getName() + ", " + searchItems[i]._section->getName());
        ui->lstResults->item(i)->setIcon(QIcon(":/images/search_2.png"));
    }

    //Отображение информации о результатах
    QString result;
    result += "<b><span style=\"color:#800000\">";
    result += "Режим исследования текстов";
    result += "</span><br><br>";
    result += "<span style=\"color:#483D8B\">";
    result += "В квадратных скобках - число, указывающее на то, "
              "сколько раз в тексте встретились слово или фраза.";
    result += "<br>";
    result += "<br><span style=\"color:#FF0000\">";
    result += QString::number(c) + "</span> повторений фразы (слова) " + "\"" +
            ui->edtSearch->text() + "\" в <span style=\"color:#FF0000\">"  + QString::number(n) +
            "</span> текстах следующих каталогов:";
    result += "</span></b>";
    ui->edtText->append(result);
    ui->edtText->append(" ");
    result = "";

    for(int i = 0; i < catalogsList.count(); i++){
        ui->edtText->append(QString::number(i+1) + ": " + catalogsList[i]->getName());
    }

    result += "<br><br><b><span style=\"color:#483D8B\">";
    result += "Поиск завершен!";
    result += "</span></b><br>";
    ui->edtText->append(result);


    ui->lstResults->addItem(" ");
    ui->lstResults->addItem("Итого: " + QString::number(c) + " повторений фразы (слова) " + "\"" +
                            ui->edtSearch->text() + "\"" + " в " + QString::number(n) + " текстах");
    ui->lstResults->addItem("Поиск завершен!");
}

void SearchWindow::findInBooks()
{
    ui->edtText->clear();
    ui->edtText->setEnabled(true);
    ui->edtSource->setEnabled(true);
    ui->label->setEnabled(true);
    ui->lstResults->setEnabled(true);
    ui->lstResults->clear();
    ui->edtSource->clear();
    ui->edtText->clear();
    searchItems.clear();

    int c = 0;
    int cnt = 0;
    for(int l = 0; l < booksList.count(); l++){
        currentBook = booksList[l];
        currentCatalog = s->getCatalogByBook(currentBook);

        for(int i = 0; i < currentBook->getCount(); i++){
            currentChapter = currentBook->getChapterById(i);

            for(int j = 0; j < currentChapter->getCount(); j++){
                currentSection = currentChapter->getSectionById(j);

                cnt = findInOneText(&c, currentSection->getData());
                if(cnt != 0){
                    addSearchItem(cnt);
                }
                cnt = 0;
            }
        }
    }

    sortResult();

    //Вывод результатов
    int n = 0;
    for(int i = 0; i < searchItems.size(); i++,n++){
        ui->lstResults->addItem(QString::number(n+1) + ": " + //Порядковый номер
                                "[" + QString::number(searchItems[i].textCount) + "] " + //Число совпадений в тексте
                                searchItems[i]._catalog->getName() + ", " +
                                searchItems[i]._book->getName() + ", " +
                                searchItems[i]._chapter->getName() + ", " + searchItems[i]._section->getName());
        ui->lstResults->item(i)->setIcon(QIcon(":/images/search_2.png"));
    }

    //Отображение информации о результатах
    QString result;
    result += "<b><span style=\"color:#800000\">";
    result += "Режим исследования текстов";
    result += "</span><br><br><span style=\"color:#483D8B\">";
    result += "В квадратных скобках - число, указывающее на то, "
              "сколько раз в тексте встретились слово или фраза.";
    result += "<br><br><span style=\"color:#FF0000\">";
    result += QString::number(c) + "</span> повторений фразы (слова) " + "\"" +
            ui->edtSearch->text() + "\" в <span style=\"color:#FF0000\">"  + QString::number(n) +
            "</span> текстах следующих книг:";
    result += "</span></b>";
    ui->edtText->append(result);
    ui->edtText->append(" ");
    result = "";

    for(int i = 0; i < booksList.count(); i++){
        ui->edtText->append(QString::number(i+1) + ": " + booksList[i]->getName());
    }

    result += "<br><br><b><span style=\"color:#483D8B\">";
    result += "Поиск завершен!";
    result += "</span></b><br>";
    ui->edtText->append(result);

    ui->lstResults->addItem(" ");
    ui->lstResults->addItem("Итого: " + QString::number(c) + " повторений фразы (слова) " + "\"" +
                            ui->edtSearch->text() + "\"" + " в " + QString::number(n) + " текстах");
    ui->lstResults->addItem("Поиск завершен!");
}



void SearchWindow::contextMenuRequsted(const QPoint &p)
{
    QMenu M(this);
    M.addActions(listActions);
    M.exec(mapToGlobal(p));
}


//Функция, закрывающая окно поиска, если закрыть главное окно
void SearchWindow::shutdown()
{
    close();
}

//Реализация поиска
void SearchWindow::find()
{
    ui->lstText->clear();
    ui->btnFont->setEnabled(true);

    if(resource){
        findInCatalogs();
    } else {
        findInBooks();
    }
}

void SearchWindow::findInChapters()
{
    ui->btnFont->setEnabled(false);
    ui->edtText->setEnabled(true);
    ui->edtSource->setEnabled(true);
    ui->edtSource->setEnabled(false);
    ui->edtSource->clear();
    ui->label->setEnabled(false);
    ui->lstResults->setEnabled(false);
    ui->lstText->setEnabled(false);

    ui->lstResults->clear();
    ui->lstText->clear();
    searchItems.clear();
    ui->edtText->clear();

    int c = 0;
    for(int k = 0; k < catalogsList.count(); k++){

        currentCatalog = catalogsList.at(k);

        int cnt = 0;

        for(int l = 0; l < currentCatalog->getCount(); l++){

            currentBook = currentCatalog->getBookById(l);

            QRegExp rx(ui->edtSearch->text());
            if(!checkRegExp(rx))return;
            int pos = 0;
            while((pos = rx.indexIn(currentBook->getName(), pos)) != -1){
                pos += rx.matchedLength();
                c++;
                cnt++;
            }

            if(cnt != 0){
                searchItem s;
                s._catalog = currentCatalog;
                s._book = currentBook;
                s.textCount = cnt;
                searchItems.append(s);
            }

            cnt = 0;

            for(int i = 0; i < currentBook->getCount(); i++){
                currentChapter = currentBook->getChapterById(i);

                QRegExp rx(ui->edtSearch->text());
                if(!checkRegExp(rx))return;
                int pos = 0;
                while((pos = rx.indexIn(currentChapter->getName(), pos)) != -1){
                    pos += rx.matchedLength();
                    c++;
                    cnt++;
                }

                if(cnt != 0){
                    searchItem s;
                    s._catalog = currentCatalog;
                    s._book = currentBook;
                    s._chapter = currentChapter;
                    s.textCount = cnt;
                    searchItems.append(s);
                }

                cnt = 0;

                for(int j = 0; j < currentChapter->getCount(); j++){
                    currentSection = currentChapter->getSectionById(j);

                    QRegExp rx(ui->edtSearch->text());
                    if(!checkRegExp(rx))return;
                    int pos = 0;
                    while((pos = rx.indexIn(currentSection->getName(), pos)) != -1){
                        pos += rx.matchedLength();
                        c++;
                        cnt++;
                    }

                    if(cnt != 0){
                        searchItem s;
                        s._catalog = currentCatalog;
                        s._book = currentBook;
                        s._chapter = currentChapter;
                        s._section = currentSection;
                        s.textCount = cnt;
                        searchItems.append(s);
                    }

                    cnt = 0;
                }
            }
        }
    }

    //Отображение информации о результатах в окне поиска
    QString result;

    result += "<b><span style=\"color:#800000\">";
    result += "Режим исследования заголовков текстов";
    result += "<br>";
    result += "</span><br>";
    result += "<span style=\"color:#483D8B\">";
    result += "Итого: " + QString::number(c) + " повторений фразы (слова) " + "\"" +
            ui->edtSearch->text() + "\" в " +  "заголовках следующих каталогов:";
    result += "</span></b>";
    result += "<br>";
    ui->edtText->append(result);
    result = "";

    for(int i = 0; i < catalogsList.count(); i++){
        ui->edtText->append(QString::number(i+1) + ": " + catalogsList[i]->getName());
    }

    result += "<br><b><span style=\"color:#483D8B\">";
    result += "Результат поиска:";
    result += "</span></b><br>";
    ui->edtText->append(result);
    result = "";

    int n = 1;
    for(int i = 0; i < searchItems.size(); i++,n++){
        ui->edtText->append(QString::number(n) + ": " +
                            searchItems[i]._catalog->getName() + ", " +
                            searchItems[i]._book->getName() + ", " +
                            searchItems[i]._chapter->getName() + ", " +
                            searchItems[i]._section->getName());
    }

    result += "<br><br><b><span style=\"color:#483D8B\">";
    result += "Поиск завершен!";
    result += "</span></b><br>";
    ui->edtText->append(result);
}

void SearchWindow::on_lstResults_clicked(const QModelIndex &index)
{
    ui->lstText->setEnabled(true);
    ui->lstText->clear();

    textItems.clear();
    int id = index.row();

    if(id >= searchItems.size()){
        QMessageBox::information(this, "Информация!", "Спасибо!");
        return;
    }

    currentBook = s->getBookByName(searchItems[id]._book->getName());
    currentChapter = currentBook->getChapterByName(searchItems[id]._chapter->getName());
    currentSection = currentChapter->getSectionByName(searchItems[id]._section->getName());
    currentText = searchItems[id]._section->getData();
    ui->edtText->setHtml(currentText);

    //Вводим переменную, считающую сколько раз встречается слово в строке
    //Анализируем текст по клику на предмет наличия искомой фразы или слова
    int cnt = 0;
    for(int i = 0; i < ui->edtText->document()->blockCount(); i++){
        QRegExp rx(ui->edtSearch->text());
        if(!checkRegExp(rx))return;
        int pos = 0;
        QString str = ui->edtText->document()->findBlockByLineNumber(i).text();
        while((pos = rx.indexIn(str, pos)) != -1){
            pos += rx.matchedLength();
            cnt++;

            //В нижнем правом окошке выдаем информацию в каких строках
            //и сколько раз встретилось искомое слово
            ui->lstText->addItem(QString::number(cnt)+ " [" + QString::number(i+1) + " строка" + "] ");

            textItem t;
            t.id = i+1;
            t.line = str;
            textItems.append(t);
        }
    }

    //Выводим всю информацию об источнике искомой фразы
    ui->edtSource->setText(searchItems[id]._book->getName() + ", " +
                           searchItems[id]._chapter->getName() + ", " +
                           searchItems[id]._section->getName());

    currentTitle = searchItems[id]._chapter->getName();
}

//Реализация выбора ресурса для поиска
void SearchWindow::selectBooks()
{
    books_selector = new BooksSelector(s);

    connect(books_selector, SIGNAL(select(QList<BookItem*>)),
            this, SLOT(selectedBooks(QList<BookItem*>)));

    books_selector->exec();
}

void SearchWindow::selectedBooks(QList<BookItem *> selectedBooks)
{
    resource = false;//ищем по книгам
    this->booksList = selectedBooks;
}

void SearchWindow::selectCatalogs()
{
    catalogs_selector = new CatalogsSelector(s);

    connect(catalogs_selector, SIGNAL(select(QList<Catalog*>)),
            this, SLOT(selectedCatalogs(QList<Catalog*>)));

    catalogs_selector->exec();
}

void SearchWindow::selectedCatalogs(QList<Catalog *> catalogs)
{
    resource = true;//ищем по каталогам
    this->catalogsList = catalogs;
}


void SearchWindow::on_edtSearch_returnPressed()
{
    ui->lstText->clear();
    ui->btnFont->setEnabled(true);

    if(resource){
        findInCatalogs();
    } else {
        findInBooks();
    }
}

void SearchWindow::on_lstText_clicked(const QModelIndex &index)
{
    ui->edtText->clear();
    //Помещаю текст в окно, ведь по нему придётся искать
    ui->edtText->setHtml(currentText);
    int id = index.row();

    //Найденные строки помещаю в QStringlist p
    //По другому реализовать пробелы между строками не получилось
    QStringList p;
    for(int i = textItems[id].id - 1; i < ui->edtText->document()->blockCount(); i++){
        p.append(ui->edtText->document()->findBlockByLineNumber(i).text());
    }

    //Очищаю, теперь можно вставлять искомую строку и все остальные
    //Добавляю строчки в темповую переменную, чтобы бегунок был вверху
    //Если сразу писать в окно, то бегунок будет внизу, что неудобно, придётся каждый раз его поднимать вручную
    QString tmp;
    ui->edtText->clear();
    for(int i = 0; i < p.count(); i++){

        tmp += "<span style='color:#B22222'>" +
                QString::number(textItems[id].id + i) + ". "  +
                "</span>" + p[i];

        tmp += "<br>";
        tmp += "<br>";
    }

    ui->edtText->setHtml(tmp);
    p.clear();
}


void SearchWindow::chooseFont()
{
    bool ok;
    QFont font = QFontDialog::getFont(
                &ok, QFont("MS Shell Dlg 2", 12), this);
    if (ok) {
        ui->edtText->setFont(font);

        QRegExp e("font-size");
        QString txt = currentText.replace(e," ");

        ui->edtText->clear();
        ui->edtText->setText(txt);

    } else {
        QMessageBox::information(this,"Сообщение","Шрифт не выбран");
    }
}

void SearchWindow::text_display_Export()
{
    QString result;
    ui->lstText->clear();
    ui->edtText->clear();
    for(int i = 0; i < textItems.count(); i++){

        result += "<b><span style=\"color:#800000\">";
        result += QString::number(i+1);
        result += ": </span></b><br> \"";
        result += textItems[i].line + "\"";
        result += "<br>";
        result += "<b><span style=\"color:#483D8B\">(";
        result += currentBook->getName() + ", ";
        result +=  currentChapter->getName() + ", ";
        result += currentSection->getName();
        result +=   ")</span></b>";
        result += "<br>";
        result += "<br>";
        result += "<br>";
    }



    ui->edtText->setHtml(result);
}

void SearchWindow::text_file_Export()
{
    //        QString filename = QFileDialog::getSaveFileName(this,"Сохранить как");
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить как"), QString(), tr("DOC (*.doc)"));
    if(filename.isEmpty())return;

    QFile file(filename);

    //Open the file
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append))
        return;

    QTextStream out(&file);

    for(int i = 0; i < textItems.count(); i++){
        out << "\"" + textItems[i].line + "\"" << "\n" <<
               "(" << currentBook->getName() << ", " <<
               currentChapter->getName() << ", " <<
               currentSection->getName() << ")" << "\n\n";
    }

    file.close();
}

void SearchWindow::result_display_Export()
{
    ui->edtText->clear();
    ui->lstText->clear();
    QString result;
    QStringList strList;

    int cnt = 0;
    for(int i = 0; i < searchItems.count(); i++){

        currentText = searchItems[i]._section->getData();
        QRegExp rx_("</p>");
        strList = currentText.split(rx_);

        for(int j = 0; j < strList.count(); j++){
            QRegExp rx(ui->edtSearch->text());
            if(!checkRegExp(rx))return;
            int pos = 0;
            QString str = "\"" +strList[j] + "\"";
            while((pos = rx.indexIn(str, pos)) != -1){
                pos += rx.matchedLength();
                cnt++;

                result += "<b><span style=\"color:#800000\">";
                result += QString::number(cnt);
                result += ": </span></b>";
                result += str;
                result += "<b><span style=\"color:#483D8B\"><br>(";
                result += searchItems[i]._book->getName() + ", ";
                result +=  searchItems[i]._chapter->getName() + ", ";
                result += searchItems[i]._section->getName();
                result +=   ")</span></b>";
                result += "<br>";
                result += "<br>";
                result += "<br>";
            }
        }

        strList.clear();
    }

    ui->edtText->setHtml(result);
}


void SearchWindow::result_file_Export()
{
    QStringList strList;

    //    QString filename = QFileDialog::getSaveFileName(this,"Сохранить как");
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранить как"), QString(), tr("DOC (*.doc)"));
    if(filename.isEmpty())return;

    QFile file(filename);
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Append)) return;
    QTextStream out(&file);

    int cnt = 0;
    for(int i = 0; i < searchItems.count(); i++){

        currentText = searchItems[i]._section->getData();
        QRegExp rx_("</p>");
        strList = currentText.split(rx_);

        for(int j = 0; j < strList.count(); j++){
            QRegExp rx(ui->edtSearch->text());
            if(!checkRegExp(rx))return;
            int pos = 0;

            QRegExp e("\<(/?[^>]+)>");
            QString str = strList[j].replace(e," ");

            while((pos = rx.indexIn(str, pos)) != -1){
                pos += rx.matchedLength();
                cnt++;

                out << QString::number(cnt) << ": " << "\"" << str<< "\"" << "\n" <<
                       "(" << currentBook->getName() << ", " <<
                       currentChapter->getName() << ", " <<
                       currentSection->getName() << ")" << "\n\n";
            }
        }

        strList.clear();
    }
    file.close();
}


//Вспомогательные функции
bool SearchWindow::checkRegExp(QRegExp rx)
{
    if(rx.isValid() && !rx.isEmpty() && !rx.exactMatch("")){
        return true;
    } else {
//        QMessageBox::information(this,
//                                 "Информсообщение",
//                                 trUtf8("Некорректный шаблон регулярного выражения!"));
        return false;
    }
}


void SearchWindow::on_edtSearch_editingFinished()
{
    highlighter1->setPattern(ui->edtSearch->text());
    highlighter1->rehighlight();
}

