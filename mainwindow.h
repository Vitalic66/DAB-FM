#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QProcess>
#include <QMessageBox>
#include <QDebug>
#include <QVector>
#include <QTextStream>
#include <QStringList>
//#include <QThread>
//#include <QDir>
#include <QInputDialog>
#include <QListWidgetItem>
#include <QCollator>

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

    void fm_read_file();
    void fm_write_file();

    QStringList sort_list(QStringList list);

    void on_btn_delete_clicked();

    void fm_fill_list();

    void on_btn_dab_to_tune_clicked();

    void on_btn_tune_to_fm_clicked();

    void on_btn_tune_to_dab_clicked();

    void on_btn_add_favorite_clicked();

    void on_btn_rem_favorite_clicked();

    void fm_show_fav_btn();

    void on_btn_fm_to_tune_clicked();

    void on_btn_fm_to_dab_clicked();

private:
    Ui::MainWindow *ui;

    QVector<QVector<QString>> dab_vec_vec;
    QVector<QVector<QString>> fm_vec_vec;

    QString path_dab = "../.dab.txt";
    QString path_fm = "../.fm.txt";

};

#endif // MAINWINDOW_H
