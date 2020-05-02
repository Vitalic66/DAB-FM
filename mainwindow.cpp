#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    MainWindow::fm_read_file(); //read file to fm_vec_vec
    MainWindow::fm_fill_list(); //fm_vec_vec to fm_list

    //init gui
    //fm favs
    ui->btn_fm_st01->setEnabled(false);
    ui->btn_fm_st02->setEnabled(false);
    ui->btn_fm_st03->setEnabled(false);
    ui->btn_fm_st04->setEnabled(false);

    MainWindow::fm_show_fav_btn(); //activate fm fav buttons

}

MainWindow::~MainWindow()
{
    delete ui;
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

    qDebug() << "fm vec vec" << fm_vec_vec;

    file_fm.close();
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

QStringList MainWindow::sort_list(QStringList list){

    QCollator coll;
    coll.setNumericMode(true);

    std::sort(list.begin(), list.end(), [&](const QString& s1, const QString& s2){ return coll.compare(s1, s2) < 0; });

    return list;
}



void MainWindow::on_btn_delete_clicked()
{
    int fm_marked_line = ui->lst_fm->currentRow();
    qDebug() << "line_remove" << fm_marked_line;

    fm_vec_vec.remove(fm_marked_line);

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
}

void MainWindow::fm_fill_list(){

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

void MainWindow::on_btn_dab_to_tune_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btn_tune_to_fm_clicked()
{
    ui->stackedWidget->setCurrentIndex(1);
}

void MainWindow::on_btn_tune_to_dab_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btn_fm_to_tune_clicked()
{
    ui->stackedWidget->setCurrentIndex(2);
}

void MainWindow::on_btn_fm_to_dab_clicked()
{
    ui->stackedWidget->setCurrentIndex(0);
}

void MainWindow::on_btn_add_favorite_clicked()
{
    int marked = ui->lst_fm->currentRow();

    fm_vec_vec[marked].insert(2, "fav");

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();
}

void MainWindow::on_btn_rem_favorite_clicked()
{
    int marked = ui->lst_fm->currentRow();

    fm_vec_vec[marked].insert(2, "");

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();
}

void MainWindow::fm_show_fav_btn(){

    //int fm_count_favs = 0;
    QVector<int> fm_found_favs;

    for(int i = 0; i < fm_vec_vec.size(); i++){
        if(fm_vec_vec[i][2] == "fav"){
            //++fm_count_favs;
            fm_found_favs.push_back(i);
        }
    }

    qDebug() << "found favs" << fm_found_favs;
    qDebug() << "found size" << fm_found_favs.size();

    //for(int i = 0; i < fm_found_favs.size(); i++){

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

    //}



}




