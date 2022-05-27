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

void MainWindow::on_choosePathButton_clicked()
{
    QString dirStr;
    dirStr = QFileDialog::getExistingDirectory(this, "Выбор директории");
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

void MainWindow::on_csvFileButton_clicked()
{
    QString fileStr;
    fileStr = QFileDialog::getSaveFileName(this, "Выбор csv-файла", QString(),
                                           "CSV (*.csv)");
    ui->csvFileEdit->setText(fileStr);

    ui->startButton->setEnabled(!ui->pathEdit->text().trimmed().isEmpty() &&
                                !ui->csvFileEdit->text().trimmed().isEmpty());
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

void MainWindow::on_chooseFileButton_clicked()
{

    QStringList FilesStr;
    FilesStr = QFileDialog::getOpenFileNames(this, "Выбор Файла",tr("*.*"));

    ui->filesWidget->clear();
    ui->filesWidget->addItems(FilesStr);
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

void MainWindow::on_HelpButton_clicked()
{
    QMessageBox::information(NULL,QObject::tr("Информация"),tr("В поле <<Ключевые слова>> можно задавать параметры поиска. Программа рассчитана на поиск двух результатов. Поиск происходит по блокам. Дата, время и номер карты всегда выписываются с начала блока. Маска результата должна в себя включать регулярное выражение(для упрощение данной задачи есть файл regexp.png).                                                                                                                         В таблице (в файле с расширением .csv) есть обозначение <<пусто>> - это значит, что результат отсутствует."));
}
