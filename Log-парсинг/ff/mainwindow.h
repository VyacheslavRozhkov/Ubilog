#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include <QString>
#include <QStringList>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_choosePathButton_clicked();
    void on_csvFileButton_clicked();
    void on_startButton_clicked();

    void on_chooseFileButton_clicked();

    void on_HelpButton_clicked();

private:
    Ui::MainWindow *ui;

    QString findStartLine(QStringList *text, QString key);
    QStringList getBlock(QStringList *text, QString key);
};

#endif // MAINWINDOW_H
