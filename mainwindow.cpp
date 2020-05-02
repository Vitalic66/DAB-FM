#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFile>

using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    tuner_mode = "DAB";

    //start mediaclient in case it did not...
    QProcess::execute("/opt/bin/mediaclient --start");
    //QThread::msleep(3000);

    MainWindow::fm_read_file(); //read file to fm_vec_vec
    MainWindow::fm_fill_list(); //fm_vec_vec to fm_list

    //init gui
    //fm favs
    ui->btn_fm_st01->setEnabled(false);
    ui->btn_fm_st02->setEnabled(false);
    ui->btn_fm_st03->setEnabled(false);
    ui->btn_fm_st04->setEnabled(false);

    //tune_butoons_fm
    ui->btn_add->setEnabled(false);
    ui->btn_rename_station->setEnabled(false);
    ui->ln_add_station->setEnabled(false);

    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);

    MainWindow::fm_show_fav_btn(); //activate fm fav buttons

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
    int marked = ui->lst_fm->currentRow();

    fm_vec_vec[marked].insert(2, "fav");

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();

    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);
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

    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);
}

void MainWindow::fm_show_fav_btn(){

    //QVector<int> fm_found_favs;

    fm_found_favs.clear();

    for(int i = 0; i < fm_vec_vec.size(); i++){
        if(fm_vec_vec[i][2] == "fav"){

            fm_found_favs.push_back(i);
        }
    }

    qDebug() << "found favs" << fm_found_favs;
    qDebug() << "found size" << fm_found_favs.size();



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
}

void MainWindow::on_btn_add_clicked()
{

    QString add_station = ui->ln_add_station->text();

    if(!add_station.isEmpty()){
        float add_station_float = add_station.toFloat();
        int to_mhz = add_station_float * 1000000;
        QString station_conv_string = (QString::number(to_mhz));

        if(add_station.contains(",")){
            add_station = add_station.replace(",", ".");
        }
        if(!add_station.contains(".")){
            add_station = add_station.append(".0");
        }

        QVector<QString> fm_vec;

        fm_vec.push_back("man Station@" + add_station + "MHz");
        fm_vec.push_back(station_conv_string);
        fm_vec.push_back(",");

        fm_vec_vec.push_back(fm_vec);

        qDebug() << " add station fm vecvec:" << fm_vec_vec;
    }

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();

    ui->ln_add_station->setText(""); //empty line for new entrie
}



void MainWindow::on_btn_tuner_mode_clicked()
{
    if(ui->btn_tuner_mode->text() == "FM\nMODE"){
        ui->btn_tuner_mode->setText("DAB\nMODE");
        tuner_mode = "FM";
        //ui->ls_dab->setVisible(false);
        ui->lst_fm->setVisible(true);
        //ui->btn_rename->setVisible(true);
        //ui->ln_man_tune->setVisible(true);
        //ui->label->setVisible(true);
        ui->btn_add->setEnabled(true);
        //ui->btn_man_tune->setVisible(true);
        ui->btn_add->setEnabled(true);
        ui->btn_rename_station->setEnabled(true);
        ui->ln_add_station->setEnabled(true);
    } else {
        ui->btn_tuner_mode->setText("FM\nMODE");
        tuner_mode = "DAB";
        //ui->ls_dab->setVisible(true);
        ui->lst_fm->setVisible(false);
        //ui->btn_rename->setVisible(false);
        //ui->ln_man_tune->setVisible(false);
        //ui->label->setVisible(false);
        ui->btn_add->setEnabled(false);
        //ui->btn_man_tune->setVisible(false);
        ui->btn_add->setEnabled(false);
        ui->btn_rename_station->setEnabled(false);
        ui->ln_add_station->setEnabled(false);
    }

    qDebug() << "tuner mode btn mode clicked" << tuner_mode;
    qDebug() << "text btn tuner mode" << ui->btn_tuner_mode->text();

    //enable/disable tune button depending on station selected or not
    /*
    int init_fm_list = ui->ls_fm->currentRow();
    int init_dab_list = ui->ls_dab->currentRow();

    if(init_fm_list == -1 && tgl_state == "FM"){
        ui->btn_tune->setDisabled(true);
    }

    if(init_fm_list != -1 && tgl_state == "FM"){
        ui->btn_tune->setDisabled(false);
    }

    if(init_dab_list == -1 && tgl_state == "DAB"){
        ui->btn_tune->setDisabled(true);
    }

    if(init_dab_list != -1 && tgl_state == "DAB"){
        ui->btn_tune->setDisabled(false);
    }
    */
}

void MainWindow::tune_to_station(QString freq){

    if(tuner_mode == "FM"){

        QString radio_dab_type = "RADIO";
        //QString freq;
        QString serv_id;

        QProcess::execute("/opt/bin/mediaclient --start");
        QProcess::execute("/opt/bin/mediaclient -m" + radio_dab_type + " -f" + freq);
        if(tuner_mode == "DAB"){
            QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -f " + freq + " --sid " + serv_id);
        }
        QProcess::execute("/opt/bin/mediaclient -m " + radio_dab_type + " -g off");
    }
}

void MainWindow::on_btn_fm_st01_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(0);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(0)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune);

}

void MainWindow::on_btn_fm_st02_clicked()
{
    tuner_mode = "FM";
    //int fav_fm_01 = fm_found_favs.at(1);
    QString fm_station_to_tune = fm_vec_vec[fm_found_favs.at(1)][1];
    //QStringList list {MainWindow::sort_list(unsort_list)};
    MainWindow::tune_to_station(fm_station_to_tune);

}

void MainWindow::on_lst_fm_itemSelectionChanged()
{
    ui->btn_add_favorite->setEnabled(true);
    ui->btn_rem_favorite->setEnabled(true);
    if(tuner_mode == "FM"){
        ui->btn_delete->setEnabled(true);
    }
}

void MainWindow::rename(){

    int marked = ui->lst_fm->currentRow();
    QString name_marked = ui->lst_fm->currentItem()->text();

    bool ok;

    QString new_name = QInputDialog::getText(this, "enter new name", "new name: ",QLineEdit::Normal,name_marked, &ok);

    fm_vec_vec[marked].replace(0, new_name);

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();

    ui->btn_add_favorite->setEnabled(false);
    ui->btn_rem_favorite->setEnabled(false);
    ui->btn_delete->setEnabled(false);

}

void MainWindow::on_btn_rename_station_clicked()
{
    MainWindow::rename();
}

void MainWindow::on_btn_scan_clicked()
{
//    if(!add_station.isEmpty()){
//        float add_station_float = add_station.toFloat();
//        int to_mhz = add_station_float * 1000000;
//        QString station_conv_string = (QString::number(to_mhz));

//        if(add_station.contains(",")){
//            add_station = add_station.replace(",", ".");
//        }
//        if(!add_station.contains(".")){
//            add_station = add_station.append(".0");
//        }


    if (tuner_mode == "FM"){
        //clear list
        //ui->ls_fm->clear();
        //clear vectors
        fm_vec_vec.clear();
        qDebug() << "content vector fm after clear:" << fm_vec_vec;

        //start scanning fm ###############################################################################################################
        QProcess process_fm;
        process_fm.close();
        process_fm.start("/opt/bin/mediaclient --scanfmfrequencies /dev/radio0");
        process_fm.waitForFinished();
        //process.close();
        //QByteArray scanned_dab(process.readAllStandardOutput());
        QString scanned_fm(process_fm.readAllStandardOutput());
        process_fm.close();

        QTextStream console_count_lines_fm(&scanned_fm);
        QTextStream console_find_freq(&scanned_fm);
        //QString line_fm;

        QString line_lock = console_count_lines_fm.readLine();
        int line_count_fm = 0;

        while (!console_count_lines_fm.atEnd()) {
            QString line_fm = console_count_lines_fm.readLine();
            line_count_fm++;
        }

        for(int i = 0; i < line_count_fm; i++){
            QVector<QString> fm_vec;
            QString tmp = console_find_freq.readLine();
            //if(tmp != "" && tmp != "Scan completed" && tmp != "SCAN SETUP"){
            //QString match = "LOCKED";
            if(tmp.contains("LOCKED")){
                tmp.replace(" [LOCKED]", "000");
                //float mhz = tmp.toFloat() / 1000000;
                //QString mhz_str = QString::number(mhz).right();
                //tmp = "Station " + QString::number(i-1) + " " + QString::number(mhz) + " MHz," + tmp + ",";

                //out << tmp << "\n";

                //unsort_scan_fm.append(tmp);
                //qDebug() << "unsort list:" << unsort_scan_fm;
                fm_vec.push_back("Station" + QString::number(i-1));
                fm_vec.push_back(tmp);
                fm_vec.push_back(",");

                fm_vec_vec.push_back(fm_vec);
            }
        }

        qDebug() << " add station fm vecvec after scan:" << fm_vec_vec;
//    }

    MainWindow::fm_write_file();
    ui->lst_fm->clear();
    MainWindow::fm_read_file();
    MainWindow::fm_fill_list();
    MainWindow::fm_show_fav_btn();
    }
}
