#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QFile>
#include <QDir>
#include <QRegExp>
#include <QDebug>
#include <QMessageBox>
#include <QString>
#include <QStringList>


const QString DATE_REGULAR = "\\d{1,2}\\.\\d{2}\\.\\d{2}";
const QString TIME_REGULAR = "\\d{2}\\:\\d{2}\\:\\d{2}\\.\\d{3}";
const QString CARD_REGULAR = "(#?[A-Fa-f0-9]){6}";

const QString SEARCH_REGULAR = ")(\\d)";

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_startButton_clicked()
{

    QString RESULT_REGULAR =ui->resultEdit->text();
    QString dirStr = ui->pathEdit->text();

    QDir dir = dirStr;
    QStringList files ;
    int r;
    for (int i = 0; i < ui->filesWidget->count(); ++i)
    {
       files <<  ui->filesWidget->item(i)->text();
      r=i;
    }
    r++;
    ui->CountfEdit->setText(QString::number(r));
    QFile csvFile(ui->csvFileEdit->text());
    if (!csvFile.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Что-то не так с файлом"));
        return;
    }

    QTextStream csv(&csvFile);
    csv.setCodec("Windows-1251");
    csv << QString::fromUtf8("Дата;Время;Карта;Левый;Правый;"
                             "Код поиска;Имя файла\n");

    for (int i = 0; i < files.size(); ++i)
    {
        QString fileName = files[i];


        QFile file(dirStr + "/" + fileName); // создаем объект класса QFile
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) // Проверяем, возможно ли открыть наш файл для чтения
        {
            //message
             QMessageBox::critical(NULL,QObject::tr("Ошибка"),tr("Повреждены файлы"));
            return; // если это сделать невозможно, то завершаем функцию
        }
        QStringList text;
        while (!file.atEnd())
        {
            text.append(file.readLine());
        }
        file.close();

        while (true)
        {
            QString startKey = ui->startKeyEdit->text();
            QString startString = findStartLine(&text, startKey);

            QRegExp dateReg(DATE_REGULAR);
            if (dateReg.indexIn(startString) != 0)
            {
                break;
            }
            QString dateStr = dateReg.cap();

            QRegExp timeReg(TIME_REGULAR);
            if (timeReg.indexIn(startString) < 0)
            {
                return;
            }
            QString timeStr = timeReg.cap();

            QRegExp cardReg(CARD_REGULAR);
            if (cardReg.indexIn(startString) < 0)
            {
                return;
            }
            QString cardStr = cardReg.cap();

            QString finishKey = ui->finishKeyEdit->text();
            QStringList block = getBlock(&text, finishKey);
            QString blockText = block.join('\n');

            QString searchString = ui->searchEdit->text();
            QRegExp searchReg("(" + searchString + SEARCH_REGULAR);
            if (searchReg.indexIn(blockText) < 0)
            {
                return;
            }
            QString searchStr = searchReg.cap(2);

            QString result1String = ui->result1KeyEdit->text();
            QRegExp result1Reg("(" + result1String + ")" + RESULT_REGULAR);
            QString result1Str = QString::fromUtf8("пусто");
            if (result1Reg.indexIn(blockText) >= 0)
            {
                result1Str = result1Reg.cap(2);
            }

            QString result2String = ui->result2KeyEdit->text();
            QRegExp result2Reg("(" + result2String + ")" + RESULT_REGULAR);
            QString result2Str = QString::fromUtf8("пусто");
            if (result2Reg.indexIn(blockText) >= 0)
            {
                result2Str = result2Reg.cap(2);
            }

            csv << dateStr << ";" << timeStr << ";" << cardStr << ";" <<
                   result1Str << ";" << result2Str << ";" <<
                   searchStr << ";" << fileName << "\n";
        }
    }
}

QString MainWindow::findStartLine(QStringList *text, QString key)
{
    for (int i = 0; i < text->size(); ++i)
    {
        QString s = text->at(i);
        if (s.indexOf(key) != -1)
        {
            text->erase(text->begin(), text->begin() + i + 1);
            return s;
        }
    }
    return "";
}

QStringList MainWindow::getBlock(QStringList *text, QString key)
{
    QStringList sl;
    for (int i = 0; i < text->size(); ++i)
    {
        QString s = text->at(i);
        if (s.indexOf(key) != -1)
        {
           return sl;
        }
        sl << s;
    }
    return QStringList();
}

void MainWindow::on_actionCsv_triggered()
{

    ui->csvFileEdit->clear();
    QString fileStr;
    fileStr = QFileDialog::getSaveFileName(this, "Выбор csv-файла", QString(),
                                           "CSV (*.csv)");
    ui->csvFileEdit->setText(fileStr);

    ui->startButton->setEnabled(!ui->pathEdit->text().trimmed().isEmpty() &&
                                !ui->csvFileEdit->text().trimmed().isEmpty());
}

void MainWindow::on_action_5_triggered()
{
    QMessageBox::information(NULL,QObject::tr("Инструкция"),tr("В поле <<Ключевые слова>> можно задавать параметры поиска. Программа рассчитана на поиск двух результатов. Поиск происходит по блокам. Дата, время и номер карты всегда выписываются с начала блока. Маска результата должна в себя включать регулярное выражение(для упрощение данной задачи есть файл regexp.png).""\n В таблице (в файле с расширением .csv) есть обозначение <<пусто>> - это значит, что результат отсутствует."));
}

void MainWindow::on_action_4_triggered()
{
    QMessageBox::information(NULL,QObject::tr("Информация о программе"),tr("Программа читает информацию из Log-Файлов. Log-Файлы  читаться поблочно. Из  начало блока выписывается дата, время и шестизначный номер карты.  Программа рассчитана на 2 результата. Результаты делится на:  левый и правый. Поиск результата происходит по ключевой функции и маски значения результата.  Также выписывается Код а поиска – в котором  указывается в Log-Файлах код выполнения процесса (выполнен процесс или нет) .  В конечный файл выписывается все выше написанные значение  и имя файла откуда они взяты. "));
}

void MainWindow::on_action_2_triggered()
{
    ui->CountfEdit->clear();
    ui->AllfEdit->clear();
    QStringList FilesStr;
    FilesStr = QFileDialog::getOpenFileNames(this, tr("Выбор Файла(-ов)"),"","*.*");
    ui->filesWidget->clear();
    ui->filesWidget->addItems(FilesStr);
    if (ui->filesWidget->count()<=0)
    {
        ui->pathEdit->clear();
        QMessageBox::warning(NULL,QObject::tr("Ой..."),tr("Не выбран файл(-ы)"));
        return;
    }
    QString filen ;
    filen =  ui->filesWidget->item(0)->text();
    QString path = QFileInfo(filen).path();
    ui->pathEdit->clear();
    ui->pathEdit->setText(path);
    QDir dir;
    dir = path;
    QStringList items = dir.entryList(QStringList("*.*"));
    ui->AllfEdit->setText(QString::number(items.count()-2));
    QStringList files ;
    QString filenames ;
    for (int i = 0; i < ui->filesWidget->count(); ++i)
    {
        QString filen ;
        filen =  ui->filesWidget->item(i)->text();
        QFileInfo fi(filen);
        filenames = fi.fileName();
        files << filenames;
    }
    ui->filesWidget->clear();
    ui->filesWidget->addItems(files);
    ui->startButton->setEnabled(!ui->pathEdit->text().trimmed().isEmpty() &&
                                !ui->csvFileEdit->text().trimmed().isEmpty());
}

void MainWindow::on_action_triggered()
{
    ui->CountfEdit->clear();
    ui->AllfEdit->clear();
    QString dirStr;
    dirStr = QFileDialog::getExistingDirectory(this, tr ("Выбор директории"));
    ui->pathEdit->clear();
    ui->pathEdit->setText(dirStr);
    QDir dir = dirStr;
    QStringList files = dir.entryList();
    files.removeAt(0);
    files.removeAt(0);
    ui->filesWidget->clear();
    ui->filesWidget->addItems(files);
    if (ui->filesWidget->count()<=0)
    {
        QMessageBox::warning(NULL,QObject::tr("Ой..."),tr("Пустая папка"));
    }
    ui->AllfEdit->setText(QString::number(ui->filesWidget->count()));
    ui->startButton->setEnabled(!ui->pathEdit->text().trimmed().isEmpty() &&
                                !ui->csvFileEdit->text().trimmed().isEmpty());
}

void MainWindow::on_actionExit_triggered()
{
    int n = QMessageBox::warning(0,
                                 "Еxit",
                                 "Вы действительно хотите выйти?",
                                 "Да",
                                 "Нет",
                                 QString(),
                                 0,
                                 1
                                );
    if(!n) {
        exit(0);
    }

}

void MainWindow::on_action_6_triggered()
{
    ui->CountfEdit->clear();
    ui->AllfEdit->clear();
    ui->csvFileEdit->clear();
    ui->filesWidget->clear();
    ui->pathEdit->clear();
}
