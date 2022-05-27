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
    void on_startButton_clicked();

    void on_actionCsv_triggered();

    void on_action_5_triggered();

    void on_action_4_triggered();

    void on_action_2_triggered();

    void on_action_triggered();

    void on_actionExit_triggered();

    void on_action_6_triggered();

private:
    Ui::MainWindow *ui;

    QString findStartLine(QStringList *text, QString key);
    QStringList getBlock(QStringList *text, QString key);
};

#endif // MAINWINDOW_H
