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

#include <tune.h>

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

    void on_btn_delete_clicked(); //only for fm    

    void on_btn_dab_to_tune_clicked();

    void on_btn_tune_to_fm_clicked();

    void on_btn_tune_to_dab_clicked();

    void on_btn_add_favorite_clicked();

    void on_btn_rem_favorite_clicked();    

    void on_btn_fm_to_tune_clicked();

    void on_btn_fm_to_dab_clicked();

    void on_btn_add_clicked(); //only for fm

    void on_btn_dab_to_fm_clicked();

    void on_btn_tuner_mode_clicked();    

    void on_btn_fm_st01_clicked();
    void on_btn_fm_st02_clicked();
    void on_btn_fm_st03_clicked();
    void on_btn_fm_st04_clicked();
    void on_btn_fm_st05_clicked();
    void on_btn_fm_st06_clicked();
    void on_btn_fm_st07_clicked();
    void on_btn_fm_st08_clicked();
    void on_btn_fm_st09_clicked();
    void on_btn_fm_st10_clicked();
    void on_btn_fm_st11_clicked();
    void on_btn_fm_st12_clicked();

    void on_btn_dab_st01_clicked();
    void on_btn_dab_st02_clicked();
    void on_btn_dab_st03_clicked();
    void on_btn_dab_st04_clicked();
    void on_btn_dab_st05_clicked();
    void on_btn_dab_st06_clicked();
    void on_btn_dab_st07_clicked();
    void on_btn_dab_st08_clicked();
    void on_btn_dab_st09_clicked();
    void on_btn_dab_st10_clicked();
    void on_btn_dab_st11_clicked();
    void on_btn_dab_st12_clicked();

    void on_lst_fm_itemSelectionChanged();
    void on_lst_dab_itemSelectionChanged();

    void on_btn_tune_clicked();

    void on_btn_tune_to_mute_clicked();

    void on_btn_dab_to_mute_clicked();

    void on_btn_fm_to_mute_clicked();

    void fm_show_fav_btn();
    void dab_show_fav_btn();

    void tune_to_station(QString freq, QString sid);

    void on_btn_rename_station_clicked();

    void on_btn_scan_clicked();

    void fm_disable_btn();
    void dab_disable_btn();

    void fm_refresh_all();
    void dab_refresh_all();

    void rename();

    void fm_read_file();
    void dab_read_file();
    void fm_write_file();
    void dab_write_file();

    QStringList sort_list(QStringList list);

    void fm_fill_list();
    void dab_fill_list();

    void mute_unmute();

    //QString logo_dab_fav(QString sid);
    //bool dab_logo_exists(QString sid);

    //QObject paintEvent(QPaintEvent *e, QString path_dab_logo);

    //QPixmap logo_dab(QString in);

private:
    Ui::MainWindow *ui;

    Tune tune;

    QVector<QVector<QString>> dab_vec_vec;
    QVector<QVector<QString>> fm_vec_vec;

    QVector<int> fm_found_favs;
    QVector<int> dab_found_favs;

    QString path_dab = "../.dab.txt";
    QString path_fm = "../.fm.txt";
    QString path_settings = "../dab_fm.ini";

    QString path_dab_icons = "../senderlogos/";

    QString tuner_mode;
    QString mute_unmute_state;

    QString dark_theme = "";
    QString default_theme = "";

    //stylesheets
    QString btn_default_rounded = ("border: 1px solid #ababab;border-radius: 20px;background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #efefef);");


};

#endif // MAINWINDOW_H
