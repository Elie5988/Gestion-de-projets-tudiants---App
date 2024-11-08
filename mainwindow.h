#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QtSql>
#include <QDebug>
#include <QMouseEvent>

struct Project {
    int id;
    QString title;
    QString description;
    QString studentName;
    QString status;
};
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
private slots:
    void onProjectSelected();
    void onSearchChanged(const QString &text);


    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_5_clicked();



private:
    Ui::MainWindow *ui;
    void loadProjects();
    bool connectToDatabase();
    QList<Project> projects;
    QSqlDatabase db;

};

#endif // MAINWINDOW_H
