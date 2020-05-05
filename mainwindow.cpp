#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QThread>
#include <QFileInfo>

using namespace std;

/*todo list
 * settings menu
 * pics for db stations
 * data from dab stations
 * rds fm
 * bugfixes
 *  - when switching from DAB favorites to SCAN list and DAB mode was selected before, wrong list is shown.
 * clear code
 * tune button at scan list             ### done
 * stylesheets
 * dab names (remove \t)                ### done
 * shorten station name to fit button
 * mute button                          ### done
*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tuner_mode = "DAB";
    mute_unmute_state = "unmuted";

    //start mediaclient in case it did not...
    //QProcess::execute("/opt/bin/mediaclient --shutdown");
//    QThread::msleep(2000);
    QProcess::execute("/opt/bin/mediaclient --start");
    QThread::msleep(4000);

    MainWindow::fm_read_file(); //read file to fm_vec_vec
    MainWindow::fm_fill_list(); //fm_vec_vec to fm_list

    MainWindow::dab_read_file(); //read file to dab_vec_vec
    MainWindow::dab_fill_list(); //dab_vec_vec to fm_list

    //tune_buttons_fm
    ui->btn_add->setEnabled(false);
    ui->btn_rename_station->setEnabled(false);
    ui->ln_add_station->setEnabled(false);

    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);
    ui->btn_tune->setEnabled(false);

    MainWindow::fm_show_fav_btn(); //activate fm fav buttons
    MainWindow::dab_show_fav_btn(); //activate dab fav buttons

    if(tuner_mode == "DAB"){
        ui->lst_dab->setVisible(true);
        ui->lst_fm->setVisible(false);
        //QProcess::execute("/opt/bin/mediaclient -m DAB -g off");
        //QProcess::execute("/opt/bin/mediaclient -d /dev/dab0");
    }

    if(tuner_mode == "FM"){
        ui->lst_dab->setVisible(false);
        ui->lst_fm->setVisible(true);
        //QProcess::execute("/opt/bin/mediaclient -m RADIO -g off");
        //QProcess::execute("/opt/bin/mediaclient -d /dev/radio0");
    }

    //qDebug() << "list fm current row" << ui->lst_fm->currentRow();

}

MainWindow::~MainWindow()
{
    delete ui;

    QProcess::execute("/opt/bin/mediaclient -m DAB -g on");
    QProcess::execute("/opt/bin/mediaclient -m RADIO -g on");
    QProcess::execute("/opt/bin/mediaclient --shutdown");
}

//functions #######################################################################################

void MainWindow::fm_read_file(){

    //read lines from fm.txt to qVectorVector

    QFile file_fm(path_fm);
    if(!file_fm.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    fm_vec_vec.clear();

    QTextStream fm_stream_in_file(&file_fm);
    QString fm_stream_in_line;

    while (!fm_stream_in_file.atEnd()) {

       fm_stream_in_line = fm_stream_in_file.readLine();

       QStringList split_fm_stram_in_line = fm_stream_in_line.split(",");

       QVector<QString> fm_vec;

       fm_vec.push_back(split_fm_stram_in_line.at(0)); //== name of station
       fm_vec.push_back(split_fm_stram_in_line.at(1)); //== freq of station
       fm_vec.push_back(split_fm_stram_in_line.at(2)); //== is favorit?
       fm_vec_vec.push_back(fm_vec);
    }

    //qDebug() << "fm vec vec" << fm_vec_vec;

    file_fm.close();
}

void MainWindow::dab_read_file(){

    //read lines from fm.txt to qVectorVector

    QFile file_dab(path_dab);
    if(!file_dab.open(QFile::ReadOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    dab_vec_vec.clear();

    QTextStream dab_stream_in_file(&file_dab);
    QString dab_stream_in_line;

    while (!dab_stream_in_file.atEnd()) {

       dab_stream_in_line = dab_stream_in_file.readLine();

       QStringList split_dab_stram_in_line = dab_stream_in_line.split(",");

       QVector<QString> dab_vec;

       dab_vec.push_back(split_dab_stram_in_line.at(0)); //== name of station
       dab_vec.push_back(split_dab_stram_in_line.at(1)); //== freq of station
       dab_vec.push_back(split_dab_stram_in_line.at(2)); //== sid of station
       dab_vec.push_back(split_dab_stram_in_line.at(3)); //== is favorit?
       dab_vec_vec.push_back(dab_vec);
    }

    //qDebug() << "dab vec vec" << dab_vec_vec;

    file_dab.close();
}

void MainWindow::fm_write_file(){

    //writes lines from qVectorVector to file

    QFile file_fm(path_fm);
    if(!file_fm.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    file_fm.resize(0); //erase content in file before write

    /*
     *  fm_vec_vec to QstringList
    */

    QString fm_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < fm_vec_vec.size(); i++){
        //for(int = j; j < fm_vec.size(); j++){
            fm_vec_vec_to_line = fm_vec_vec[i][0] + "," + fm_vec_vec[i][1] + "," + fm_vec_vec[i][2];
            unsort_list.append(fm_vec_vec_to_line);
        //}
    }

    QTextStream fm_write_to_file(&file_fm);

    QStringList list {MainWindow::sort_list(unsort_list)};

    /*
     *  sort QStringList
    */

    foreach(QString fm_line_sort, list){
        fm_write_to_file << fm_line_sort << "\n";
    }


    file_fm.flush();
    file_fm.close();

}

void MainWindow::dab_write_file(){

    //writes lines from qVectorVector to file

    QFile file_dab(path_dab);
    if(!file_dab.open(QFile::WriteOnly | QFile::Text)){
        //QMessageBox::warning(this,"..","keine datei gefunden");
        //ui->warn_no_dab_list->setVisible(true);
        return;
    }

    file_dab.resize(0); //erase content in file before write

    /*
     *  fm_vec_vec to QstringList
    */

    QString dab_vec_vec_to_line;
    QStringList unsort_list;

    for(int i = 0; i < dab_vec_vec.size(); i++){
        //for(int = j; j < fm_vec.size(); j++){
            dab_vec_vec_to_line = dab_vec_vec[i][0] + "," + dab_vec_vec[i][1] + "," + dab_vec_vec[i][2] + "," + dab_vec_vec[i][3];
            unsort_list.append(dab_vec_vec_to_line);
        //}
    }

    QTextStream dab_write_to_file(&file_dab);

    QStringList list {MainWindow::sort_list(unsort_list)};

    /*
     *  sort QStringList
    */

    foreach(QString dab_line_sort, list){
        dab_write_to_file << dab_line_sort << "\n";
    }


    file_dab.flush();
    file_dab.close();

}

QStringList MainWindow::sort_list(QStringList list){

    QCollator coll;
    coll.setNumericMode(true);

    std::sort(list.begin(), list.end(), [&](const QString& s1, const QString& s2){ return coll.compare(s1, s2) < 0; });

    return list;
}



void MainWindow::on_btn_delete_clicked()
{
    if(tuner_mode == "FM"){
        int fm_marked_line = ui->lst_fm->currentRow();
        //qDebug() << "line_remove fm list" << fm_marked_line;

        fm_vec_vec.remove(fm_marked_line);

        MainWindow::fm_refresh_all();
    }

    if(tuner_mode == "DAB"){
        int dab_marked_line = ui->lst_dab->currentRow();
        //qDebug() << "line_remove dab list" << dab_marked_line;

        dab_vec_vec.remove(dab_marked_line);

        MainWindow::dab_refresh_all();
    }
}

void MainWindow::fm_fill_list()
{

    for(int i = 0; i < fm_vec_vec.size(); i++){
        QString fm_to_list;

        fm_to_list = fm_vec_vec[i][0];

        ui->lst_fm->addItem(fm_to_list);

        //mark fav green row
        if(fm_vec_vec[i][2].contains("fav")){
            ui->lst_fm->setCurrentRow(i);
            ui->lst_fm->currentItem()->setBackgroundColor(Qt::green);
        }
    }

    ui->lst_fm->setCurrentRow(-1);
}

void MainWindow::dab_fill_list()
{

    for(int i = 0; i < dab_vec_vec.size(); i++){
        QString dab_to_list;

        dab_to_list = dab_vec_vec[i][0];

        ui->lst_dab->addItem(dab_to_list);

        //mark fav green row
        if(dab_vec_vec[i][3].contains("fav")){
            ui->lst_dab->setCurrentRow(i);
            ui->lst_dab->currentItem()->setBackgroundColor(Qt::green);
        }
    }

    ui->lst_dab->setCurrentRow(-1);
}

void MainWindow::on_btn_dab_to_tune_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btn_tune_to_fm_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //tuner_mode = "FM";
}

void MainWindow::on_btn_tune_to_dab_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //tuner_mode = "DAB";
}

void MainWindow::on_btn_fm_to_tune_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btn_fm_to_dab_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
    //tuner_mode = "DAB";
}

void MainWindow::on_btn_dab_to_fm_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
    //tuner_mode = "FM";
}

void MainWindow::on_btn_add_favorite_clicked()
{
    if(tuner_mode == "FM"){
        int marked = ui->lst_fm->currentRow();

        fm_vec_vec[marked].insert(2, "fav");

        MainWindow::fm_refresh_all();
    }

    if(tuner_mode == "DAB"){
        int marked = ui->lst_dab->currentRow();

        dab_vec_vec[marked].insert(3, "fav");

        MainWindow::dab_refresh_all();
    }
}

void MainWindow::on_btn_rem_favorite_clicked()
{
    if(tuner_mode == "FM"){
        int marked = ui->lst_fm->currentRow();

        fm_vec_vec[marked].insert(2, "");

        MainWindow::fm_refresh_all();
    }

    if(tuner_mode == "DAB"){
        int marked = ui->lst_dab->currentRow();

        dab_vec_vec[marked].insert(3, "");

        MainWindow::dab_refresh_all();
    }
}

void MainWindow::fm_show_fav_btn()
{

    //QVector<int> fm_found_favs;

    fm_found_favs.clear();

    for(int i = 0; i < fm_vec_vec.size(); i++){
        if(fm_vec_vec[i][2] == "fav"){

            fm_found_favs.push_back(i);
        }
    }

    //qDebug() << "found favs" << fm_found_favs;
    //qDebug() << "found size" << fm_found_favs.size();



    if(fm_found_favs.size() >= 1){
        ui->btn_fm_st01->setEnabled(true);
        ui->btn_fm_st01->setText(fm_vec_vec[fm_found_favs.at(0)][0]);        
    } else if(fm_found_favs.size() < 1){
        ui->btn_fm_st01->setEnabled(false);
        ui->btn_fm_st01->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 2){
        ui->btn_fm_st02->setEnabled(true);
        ui->btn_fm_st02->setText(fm_vec_vec[fm_found_favs.at(1)][0]);
    } else if(fm_found_favs.size() < 2){
        ui->btn_fm_st02->setEnabled(false);
        ui->btn_fm_st02->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 3){
        ui->btn_fm_st03->setEnabled(true);
        ui->btn_fm_st03->setText(fm_vec_vec[fm_found_favs.at(2)][0]);
    } else if(fm_found_favs.size() < 3){
        ui->btn_fm_st03->setEnabled(false);
        ui->btn_fm_st03->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 4){
        ui->btn_fm_st04->setEnabled(true);
        ui->btn_fm_st04->setText(fm_vec_vec[fm_found_favs.at(3)][0]);
    } else if(fm_found_favs.size() < 4){
        ui->btn_fm_st04->setEnabled(false);
        ui->btn_fm_st04->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 5){
        ui->btn_fm_st05->setEnabled(true);
        ui->btn_fm_st05->setText(fm_vec_vec[fm_found_favs.at(4)][0]);
    } else if(fm_found_favs.size() < 5){
        ui->btn_fm_st05->setEnabled(false);
        ui->btn_fm_st05->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 6){
        ui->btn_fm_st06->setEnabled(true);
        ui->btn_fm_st06->setText(fm_vec_vec[fm_found_favs.at(5)][0]);
    } else if(fm_found_favs.size() < 6){
        ui->btn_fm_st06->setEnabled(false);
        ui->btn_fm_st06->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 7){
        ui->btn_fm_st07->setEnabled(true);
        ui->btn_fm_st07->setText(fm_vec_vec[fm_found_favs.at(6)][0]);
    } else if(fm_found_favs.size() < 7){
        ui->btn_fm_st07->setEnabled(false);
        ui->btn_fm_st07->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 8){
        ui->btn_fm_st08->setEnabled(true);
        ui->btn_fm_st08->setText(fm_vec_vec[fm_found_favs.at(7)][0]);
    } else if(fm_found_favs.size() < 8){
        ui->btn_fm_st08->setEnabled(false);
        ui->btn_fm_st08->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 9){
        ui->btn_fm_st09->setEnabled(true);
        ui->btn_fm_st09->setText(fm_vec_vec[fm_found_favs.at(8)][0]);
    } else if(fm_found_favs.size() < 9){
        ui->btn_fm_st09->setEnabled(false);
        ui->btn_fm_st09->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 10){
        ui->btn_fm_st10->setEnabled(true);
        ui->btn_fm_st10->setText(fm_vec_vec[fm_found_favs.at(9)][0]);
    } else if(fm_found_favs.size() < 10){
        ui->btn_fm_st10->setEnabled(false);
        ui->btn_fm_st10->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 11){
        ui->btn_fm_st11->setEnabled(true);
        ui->btn_fm_st11->setText(fm_vec_vec[fm_found_favs.at(10)][0]);
    } else if(fm_found_favs.size() < 11){
        ui->btn_fm_st11->setEnabled(false);
        ui->btn_fm_st11->setText("no favorite\navailable");
    }

    if(fm_found_favs.size() >= 12){
        ui->btn_fm_st12->setEnabled(true);
        ui->btn_fm_st12->setText(fm_vec_vec[fm_found_favs.at(11)][0]);
    } else if(fm_found_favs.size() < 12){
        ui->btn_fm_st12->setEnabled(false);
        ui->btn_fm_st12->setText("no favorite\navailable");
    }
}

void MainWindow::dab_show_fav_btn()
{

    //QVector<int> fm_found_favs;

    dab_found_favs.clear();

    for(int i = 0; i < dab_vec_vec.size(); i++){
        if(dab_vec_vec[i][3] == "fav"){

            dab_found_favs.push_back(i);
        }
    }

    //qDebug() << "found favs dab" << dab_found_favs;
    //qDebug() << "found size dab" << dab_found_favs.size();

    if(dab_found_favs.size() >= 1){
        ui->btn_dab_st01->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(0)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st01->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            //pixmap = pixmap.scaled(120, 90, Qt::IgnoreAspectRatio);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st01->setIcon(ButtonIcon);
            ui->btn_dab_st01->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st01->setText(dab_vec_vec[dab_found_favs.at(0)][0]);
        }
    } else if(dab_found_favs.size() < 1){
        ui->btn_dab_st01->setIcon(QIcon());
        ui->btn_dab_st01->setEnabled(false);
        ui->btn_dab_st01->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 2){
        ui->btn_dab_st02->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(1)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st02->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st02->setIcon(ButtonIcon);
            ui->btn_dab_st02->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st02->setText(dab_vec_vec[dab_found_favs.at(1)][0]);
        }
    } else if(dab_found_favs.size() < 2){
        ui->btn_dab_st02->setIcon(QIcon());
        ui->btn_dab_st02->setEnabled(false);
        ui->btn_dab_st02->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 3){
        ui->btn_dab_st03->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(2)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st03->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st03->setIcon(ButtonIcon);
            ui->btn_dab_st03->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st03->setText(dab_vec_vec[dab_found_favs.at(2)][0]);
        }
    } else if(dab_found_favs.size() < 3){
        ui->btn_dab_st03->setIcon(QIcon());
        ui->btn_dab_st03->setEnabled(false);
        ui->btn_dab_st03->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 4){
        ui->btn_dab_st04->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(3)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st04->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st04->setIcon(ButtonIcon);
            ui->btn_dab_st04->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st04->setText(dab_vec_vec[dab_found_favs.at(3)][0]);
        }
    } else if(dab_found_favs.size() < 4){
        ui->btn_dab_st04->setIcon(QIcon());
        ui->btn_dab_st04->setEnabled(false);
        ui->btn_dab_st04->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 5){
        ui->btn_dab_st05->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(4)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st05->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st05->setIcon(ButtonIcon);
            ui->btn_dab_st05->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st05->setText(dab_vec_vec[dab_found_favs.at(4)][0]);
        }
    } else if(dab_found_favs.size() < 5){
        ui->btn_dab_st05->setIcon(QIcon());
        ui->btn_dab_st05->setEnabled(false);
        ui->btn_dab_st05->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 6){
        ui->btn_dab_st06->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(5)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st06->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st06->setIcon(ButtonIcon);
            ui->btn_dab_st06->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st03->setText(dab_vec_vec[dab_found_favs.at(5)][0]);
        }
    } else if(dab_found_favs.size() < 6){
        ui->btn_dab_st06->setIcon(QIcon());
        ui->btn_dab_st06->setEnabled(false);
        ui->btn_dab_st06->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 7){
        ui->btn_dab_st07->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(6)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st07->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st07->setIcon(ButtonIcon);
            ui->btn_dab_st07->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st07->setText(dab_vec_vec[dab_found_favs.at(6)][0]);
        }
    } else if(dab_found_favs.size() < 7){
        ui->btn_dab_st07->setIcon(QIcon());
        ui->btn_dab_st07->setEnabled(false);
        ui->btn_dab_st07->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 8){
        ui->btn_dab_st08->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(7)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st08->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st08->setIcon(ButtonIcon);
            ui->btn_dab_st08->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st08->setText(dab_vec_vec[dab_found_favs.at(7)][0]);
        }
    } else if(dab_found_favs.size() < 8){
        ui->btn_dab_st08->setIcon(QIcon());
        ui->btn_dab_st08->setEnabled(false);
        ui->btn_dab_st08->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 9){
        ui->btn_dab_st09->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(8)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st09->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st09->setIcon(ButtonIcon);
            ui->btn_dab_st09->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st09->setText(dab_vec_vec[dab_found_favs.at(8)][0]);
        }
    } else if(dab_found_favs.size() < 9){
        ui->btn_dab_st09->setIcon(QIcon());
        ui->btn_dab_st09->setEnabled(false);
        ui->btn_dab_st09->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 10){
        ui->btn_dab_st10->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(9)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st10->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st10->setIcon(ButtonIcon);
            ui->btn_dab_st10->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st10->setText(dab_vec_vec[dab_found_favs.at(9)][0]);
        }
    } else if(dab_found_favs.size() < 10){
        ui->btn_dab_st10->setIcon(QIcon());
        ui->btn_dab_st10->setEnabled(false);
        ui->btn_dab_st10->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 11){
        ui->btn_dab_st11->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(10)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st11->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st11->setIcon(ButtonIcon);
            ui->btn_dab_st11->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st11->setText(dab_vec_vec[dab_found_favs.at(10)][0]);
        }
    } else if(dab_found_favs.size() < 11){
        ui->btn_dab_st11->setIcon(QIcon());
        ui->btn_dab_st11->setEnabled(false);
        ui->btn_dab_st11->setText("no favorite\navailable");
    }

    if(dab_found_favs.size() >= 12){
        ui->btn_dab_st12->setEnabled(true);
        QString btn_sid = dab_vec_vec[dab_found_favs.at(11)][2];
        bool dab_logo_exist {MainWindow::dab_logo_exists(btn_sid)};

        if(dab_logo_exist == true){
            ui->btn_dab_st12->setText("");
            QString dab_logo = path_dab_icons + btn_sid + ".png";
            QPixmap pixmap(dab_logo);
            QIcon ButtonIcon(pixmap);
            ui->btn_dab_st12->setIcon(ButtonIcon);
            ui->btn_dab_st12->setIconSize(pixmap.rect().size());
        } else {
            ui->btn_dab_st12->setText(dab_vec_vec[dab_found_favs.at(11)][0]);
        }
    } else if(dab_found_favs.size() < 12){
        ui->btn_dab_st12->setIcon(QIcon());
        ui->btn_dab_st12->setEnabled(false);
        ui->btn_dab_st12->setText("no favorite\navailable");
    }
}

void MainWindow::on_btn_add_clicked() //only for fm
{

    QString add_station = ui->ln_add_station->text();

    if(!add_station.isEmpty()){

        if(add_station.contains(",")){
            add_station = add_station.replace(",", ".");
        }

        float add_station_float = add_station.toFloat();
        int to_mhz = add_station_float * 1000000;
        QString station_conv_string = (QString::number(to_mhz));

        if(!add_station.contains(".")){
            add_station = add_station.append(".0");
        }

        QVector<QString> fm_vec;

        fm_vec.push_back("man Station@" + add_station + "MHz");
        fm_vec.push_back(station_conv_string);
        fm_vec.push_back("");

        fm_vec_vec.push_back(fm_vec);

        //qDebug() << " add station fm vecvec:" << fm_vec_vec;
    }

    ui->ln_add_station->setText(""); //empty line for new entrie

    MainWindow::fm_refresh_all();
}



void MainWindow::on_btn_tuner_mode_clicked()
{
    if(ui->btn_tuner_mode->text() == "FM\nMODE"){
        ui->btn_tuner_mode->setText("DAB\nMODE");
        tuner_mode = "FM";
        ui->lst_dab->setVisible(false);
        ui->lst_fm->setVisible(true);
        //ui->btn_rename->setVisible(true);
        //ui->ln_man_tune->setVisible(true);
        //ui->label->setVisible(true);
        ui->btn_add->setEnabled(true);
        //ui->btn_man_tune->setVisible(true);
        ui->btn_add->setEnabled(true);
        ui->btn_rename_station->setEnabled(true);
        ui->ln_add_station->setEnabled(true);
        ui->lst_fm->setCurrentRow(-1);

        MainWindow::fm_refresh_all();
    } else {
        ui->btn_tuner_mode->setText("FM\nMODE");
        tuner_mode = "DAB";
        ui->lst_dab->setVisible(true);
        ui->lst_fm->setVisible(false);
        //ui->btn_rename->setVisible(false);
        //ui->ln_man_tune->setVisible(false);
        //ui->label->setVisible(false);
        ui->btn_add->setEnabled(false);
        //ui->btn_man_tune->setVisible(false);
        ui->btn_add->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->ln_add_station->setEnabled(false);
        ui->lst_dab->setCurrentRow(-1);

        MainWindow::dab_refresh_all();
    }

    //qDebug() << "tuner mode btn mode clicked" << tuner_mode;
    //qDebug() << "text btn tuner mode" << ui->btn_tuner_mode->text();
}

void MainWindow::tune_to_station(QString freq, QString sid)
{
/*
    if(tuner_mode == "FM"){

        QString radio_dab_type = "RADIO";
        //QString freq;
        //QString serv_id;

        QProcess::execute("/opt/bin/mediaclient --start");
        QProcess::execute("/opt/bin/mediaclient -m" + radio_dab_type + " -f" + freq);
        if(tuner_mode == "DAB"){
            QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + sid);
        }
        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -g off");
    }

    if(tuner_mode == "DAB"){

        QString radio_dab_type = "DAB";
        //QString freq;
        //QString serv_id;

        QProcess::execute("/opt/bin/mediaclient --start");
        QProcess::execute("/opt/bin/mediaclient -m" + radio_dab_type + " -f" + freq);
        if(tuner_mode == "DAB"){
            QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + sid);
        }
        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -g off");
    }
*/
    QProcess::execute("/opt/bin/mediaclient --start");

    QString radio_dab_type;

    if(tuner_mode == "FM"){
        radio_dab_type = "RADIO";
        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f" + freq);
    }
    if(tuner_mode == "DAB"){
        radio_dab_type = "DAB";
        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq);
        //QProcess dab_tune_freq;
        //dab_tune_freq.start("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq);
        //dab_tune_freq.waitForReadyRead(-1);

        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + sid);

        //QProcess dab_tune_all;
        //dab_tune_all.start("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + sid);
        //dab_tune_all.waitForFinished(-1);




//        QProcess dab_tune;
//        dab_tune.start("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + sid);
//        dab_tune.waitForReadyRead(-1);
    }

    //QThread::msleep(1000);
    QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -g off");

    mute_unmute_state = "unmuted";
}

void MainWindow::on_btn_fm_st01_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(0);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(0)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st02_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(1)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st03_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(2)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st04_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(3)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st05_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(4)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st06_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(5)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st07_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(6)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st08_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(7)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st09_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(8)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st10_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(9)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st11_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(10)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_fm_st12_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(11)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune, "");

}

void MainWindow::on_btn_dab_st01_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(0);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(0)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(0)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st02_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(1)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(1)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st03_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(2)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(2)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st04_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(3)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(3)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st05_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(4)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(4)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st06_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(5)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(5)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st07_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(6)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(6)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st08_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(7)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(7)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st09_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(8)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(8)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st10_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(9)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(9)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st11_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(10)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(10)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_btn_dab_st12_clicked()
{
    tuner_mode = "DAB";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString dab_station_to_tune = dab_vec_vec[dab_found_favs.at(11)][1];
    QString dab_sid_to_tune = dab_vec_vec[dab_found_favs.at(11)][2];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(dab_station_to_tune, dab_sid_to_tune);

}

void MainWindow::on_lst_fm_itemSelectionChanged()
{
    ui->btn_add_favorite->setEnabled(true);
    ui->btn_rem_favorite->setEnabled(true);
    if(tuner_mode == "FM"){
        ui->btn_delete->setEnabled(true);
        ui->btn_rename_station->setEnabled(true);
        ui->btn_tune->setEnabled(true);
    }
}

void MainWindow::on_lst_dab_itemSelectionChanged()
{
    ui->btn_add_favorite->setEnabled(true);
    ui->btn_rem_favorite->setEnabled(true);
    if(tuner_mode == "DAB"){
        ui->btn_delete->setEnabled(true);
        ui->btn_rename_station->setEnabled(false);
        ui->btn_tune->setEnabled(true);
    }
}

void MainWindow::rename() //for fm only
{

    int marked = ui->lst_fm->currentRow();
    QString name_marked = ui->lst_fm->currentItem()->text();

    bool ok;

    QString new_name = QInputDialog::getText(this, "enter new name", "new name: ",QLineEdit::Normal,name_marked, &ok);

    if(new_name.contains(",")){
        new_name = new_name.replace(",", ".");
    }

    fm_vec_vec[marked].replace(0, new_name);

    MainWindow::fm_refresh_all();
}

void MainWindow::on_btn_rename_station_clicked()
{
    MainWindow::rename();
}

void MainWindow::on_btn_scan_clicked()
{
    if (tuner_mode == "FM"){

        fm_vec_vec.clear();
        //qDebug() << "content vector fm after clear:" << fm_vec_vec;

        //start scanning fm ###############################################################################################################
        QProcess process_fm;
        process_fm.close();
        process_fm.start("/opt/bin/mediaclient --scanfmfrequencies /dev/radio0");
        process_fm.waitForFinished();

        QString scanned_fm(process_fm.readAllStandardOutput());
        process_fm.close();

        //qDebug() << "was steht im string nach scan?" << scanned_fm;

        QTextStream console_count_lines_fm(&scanned_fm);
        QTextStream console_find_freq(&scanned_fm);
        //QString line_fm;

        QString line_lock = console_count_lines_fm.readLine();
        int line_count_fm = 0;

        while (!console_count_lines_fm.atEnd()) {
           console_count_lines_fm.readLine();
            line_count_fm++;
        }

        //qDebug() << "line count fm" << line_count_fm;

        for(int i = 0; i < line_count_fm; i++){
            QVector<QString> fm_vec;
            QString tmp = console_find_freq.readLine();

            if(tmp.contains("LOCKED")){
                tmp.replace(" [LOCKED]", "000");              
                fm_vec.push_back("Station" + QString::number(i-1));
                fm_vec.push_back(tmp);
                fm_vec.push_back(",");
                fm_vec_vec.push_back(fm_vec);
            }
        }

        //qDebug() << " add station fm vecvec after scan:" << fm_vec_vec;

        MainWindow::fm_refresh_all();
        }

    if (tuner_mode == "DAB"){

        dab_vec_vec.clear();

        //start scanning for dab frequencies ###################################################################################################

        QProcess get_terminal_out;
        get_terminal_out.close();
        get_terminal_out.start("/opt/bin/mediaclient --scandabfrequencies /dev/dab0");
        get_terminal_out.waitForFinished(-1); //overwrite default 30sec

        QString scanned_dab(get_terminal_out.readAllStandardOutput());
        //qDebug() << "was steht im string nach scan?" << scanned_dab;

        get_terminal_out.close();

        QTextStream console_count_lines_dab(&scanned_dab);
        QTextStream console_find_freq(&scanned_dab);

        QString line_lock = console_count_lines_dab.readLine();
        int line_count_dab = 0;

        while (!console_count_lines_dab.atEnd()) {
            console_count_lines_dab.readLine();
            line_count_dab++;
        }

        //qDebug() << "line count dab" << line_count_dab;

        //search for frequencies above locked ###############################################################################

        QVector<QVector<QString>> all_each_dab_lines; //hold all dab_each_lines in vector

        QVector<QString> dab_freq_vec; //only frequencies in vector which ar found before "LOCKED"

        for(int i = 0; i < line_count_dab; i++){
            QVector<QString> dab_each_line; //all lines splited from console output
            QString tmp = console_find_freq.readLine();            

            dab_each_line.push_back(tmp);            

            all_each_dab_lines.push_back(dab_each_line);

             if(all_each_dab_lines[i][0].contains("[LOCKED]")){

                QString dab_freq = all_each_dab_lines[i-1][0];

                QStringRef dab_freq_cut = dab_freq.leftRef(3);
                if(dab_freq_cut.contains(" ")){
                    dab_freq = dab_freq.remove(0, 3);
                } else {
                    dab_freq = dab_freq.remove(0, 4);
                }

                dab_freq_vec.push_back(dab_freq);
             }
        }

        //qDebug() << "all lines in vec" << all_each_dab_lines;
        qDebug() << "only freq before locked" << dab_freq_vec;

        //start scanning locked frequencies for transponders ###############################################################################


        //QVector<QString> dab_vec;
        QVector<QString> dab_trans_vec;

        for(int i = 0; i < dab_freq_vec.size(); i++){

            QProcess tune_to_locked;
            QThread::msleep(500); //delay else found freq might not be locked
            tune_to_locked.start("/opt/bin/mediaclient -m DAB -f " + dab_freq_vec[i]);
            tune_to_locked.waitForReadyRead(-1);
            tune_to_locked.waitForFinished(-1);
            QString realy_locked(tune_to_locked.readAllStandardOutput());
            //qDebug() << "tune_to_locked exit status" << tune_to_locked.exitStatus();
            //qDebug() << "tune_to_locked exit code" << tune_to_locked.exitCode();
            //qDebug() << "tune_to_locked state" << tune_to_locked.state();
            //tune_to_locked.close();

            //qDebug() << "realy locked" << realy_locked;


            if(realy_locked.contains("LOCKED")){

                //process scan transponders at frequency
                QProcess scan_locked;
                //scan_locked.close();
                scan_locked.start("/opt/bin/mediaclient -m DAB --scandabservices /dev/dab0");
                scan_locked.waitForReadyRead(-1);
                scan_locked.waitForFinished(-1);

                QString output_dab_transponder(scan_locked.readAllStandardOutput());

                //QString output_dab_transponder(scan_locked.readAllStandardOutput());

                QTextStream output_dab_transponder_count_lines(&output_dab_transponder);
                QTextStream data_from_output_dab_transponder(&output_dab_transponder);

                //qDebug() << "transponders" << output_dab_transponder;

                int line_count_dab_trans = 0;

                while (!output_dab_transponder_count_lines.atEnd()) {
                    output_dab_transponder_count_lines.readLine();
                    line_count_dab_trans++;
                }


                for(int j = 0; j < line_count_dab_trans; j++){
                    QVector<QString> dab_vec;

                    QString tmp_trans = data_from_output_dab_transponder.readLine();
                    ////dab_trans_vec.push_back(tmp_trans);
                    if(!tmp_trans.contains("Service Name") && !tmp_trans.contains("failed")){

                        //"0 x char char char char"
                        QRegularExpression sid_re("[0][x][a-f0-9][a-f0-9][a-f0-9][a-f0-9]");
                        QRegularExpressionMatch match = sid_re.match(tmp_trans);
                        QString matched;
                        if (match.hasMatch()) {
                            matched = match.captured(0);
                        }

                        QString service_name = tmp_trans.left(tmp_trans.indexOf(QLatin1String("0x")));

                        service_name = service_name.trimmed(); //remove trailing whitespaces

                        dab_vec.push_back(service_name); //name of station
                        dab_vec.push_back(dab_freq_vec[i]); //frequency of station
                        dab_vec.push_back(matched); //service id of station
                        dab_vec.push_back(""); //fav placeholder
                        dab_vec_vec.push_back(dab_vec);
                    }
                }
                ////qDebug() << "lines count transponder" << line_count_dab_trans;
            } else {
                dab_trans_vec.push_back("lock failed at " + dab_freq_vec[i] + " hit scan again...");
            }
        }
    MainWindow::dab_refresh_all();
    }
}

void MainWindow::fm_disable_btn() //disable some buttons on lciking some other button
{
    ui->lst_fm->setCurrentRow(-1);
    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);
    ui->btn_rename_station->setEnabled(false);
    ui->btn_tune->setEnabled(false);
}

void MainWindow::dab_disable_btn() //disable some buttons on lciking some other button
{
    ui->lst_dab->setCurrentRow(-1);
    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);
    ui->btn_rename_station->setEnabled(false);
    ui->btn_tune->setEnabled(false);
}

void MainWindow::fm_refresh_all()
{
    MainWindow::fm_write_file(); //write vecvec to file
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list(); //write file to vecvec
    MainWindow::fm_show_fav_btn();
    MainWindow::fm_disable_btn();
}

void MainWindow::dab_refresh_all()
{
    MainWindow::dab_write_file(); //write vecvec to file
    ui->lst_dab->clear();
    MainWindow::dab_read_file();
    MainWindow::dab_fill_list(); //write file to vecvec
    MainWindow::dab_show_fav_btn();
    MainWindow::dab_disable_btn();
}

void MainWindow::on_btn_tune_clicked()
{
    QString freq;
    QString sid;

    if(tuner_mode == "FM"){
        int marked = ui->lst_fm->currentRow();
        freq = fm_vec_vec[marked][1];
        sid = "";
    }

    if(tuner_mode == "DAB"){
        int marked = ui->lst_dab->currentRow();
        freq = dab_vec_vec[marked][1];
        sid = dab_vec_vec[marked][2];
    }

    MainWindow::tune_to_station(freq, sid);
}

void MainWindow::on_btn_dab_to_mute_clicked()
{
    MainWindow::mute_unmute();
}

void MainWindow::on_btn_fm_to_mute_clicked()
{
    MainWindow::mute_unmute();
}

void MainWindow::on_btn_tune_to_mute_clicked()
{
    MainWindow::mute_unmute();
}

void MainWindow::mute_unmute()
{
    QString tmp_mute_unmute_state;

    if(mute_unmute_state == "unmuted"){
        //turn mute on

        if(tuner_mode == "FM"){
            QProcess::execute("/opt/bin/mediaclient -m RADIO -g on");
        }

        if(tuner_mode == "DAB"){
            QProcess::execute("/opt/bin/mediaclient -m DAB -g on");
        }

        tmp_mute_unmute_state = "muted";
    }

    if(mute_unmute_state == "muted"){
        //turn mute off

        if(tuner_mode == "FM"){
            QProcess::execute("/opt/bin/mediaclient -m RADIO -g off");
        }

        if(tuner_mode == "DAB"){
            QProcess::execute("/opt/bin/mediaclient -m DAB -g off");
        }

        tmp_mute_unmute_state = "unmuted";
    }

    mute_unmute_state = tmp_mute_unmute_state;
}

QString MainWindow::logo_dab_fav(QString sid) //returns dab station logo if exists
{
    QString dab_logo = path_dab_icons + sid +".png";

    return dab_logo;
}

bool MainWindow::dab_logo_exists(QString sid) {

    QString dab_logo = path_dab_icons + sid +".png";

    QFileInfo check_dab_logo(dab_logo);
    // check if file exists and if yes: Is it really a file and no directory?
    if (check_dab_logo.exists() && check_dab_logo.isFile()) {
        return true;
    } else {
        return false;
    }
}
