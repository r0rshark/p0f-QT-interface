#include "guip0f.h"
#include "ui_guip0f.h"
#include <QDebug>
#include <QNetworkInterface>
#include "p0f_auditor.h"
#include "network_db.h"
#include <QLayout>
#include <QGroupBox>
#include <QMessageBox>
#include <QScrollArea>
#include <QHostInfo>
#include "host.h"
#include <unistd.h>
#include "sys/types.h"
extern "C" {
#include "p0f.h"
}




GUIp0f::GUIp0f(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::GUIp0f)
{
    searched = false;
    ui->setupUi(this);

    create_list_interface();
    ui->widget = new QWidget();
    ui->widget->setLayout(ui->gridLayout);
    ui->scrollArea->setWidget(ui->widget);
    ui->stop_button->setEnabled(false);
    ui->progressBar->setRange(0,100);

    timer_update = new QTimer(this);
    connect(timer_update,SIGNAL(timeout()),this,SLOT(update_gui()));

    line = new QTimeLine(10000,this);
    line->setFrameRange(0,100);
    connect(line,SIGNAL(frameChanged(int)),ui->progressBar,SLOT(setValue(int)));

    connect(this,SIGNAL(clicked(QString)),this,SLOT(see_info_host(QString)));


    model = new QFileSystemModel(this);
    model->setReadOnly(true);
    model->setRootPath("/home");
    ui->treeView->setModel(model);
    ui->treeView->setRootIndex(model->index("/home"));
    ui->treeView->setColumnWidth(0,300);



}



/*
 *Set the name interface to listen to and start the thread
 *and the refresh timer.
 */
void GUIp0f::set_name_interface(){

   QString  name_interface = QString(ui->list_interface->currentText());
   ui->list_interface->setEnabled(false);
   qDebug()<<name_interface;

   QByteArray ba = name_interface.toLatin1();
   char *char_interface = ba.data();
   set_up_iface(char_interface);
   my.start();
   qDebug()<<"thread running...";

   start_timer();
   line->start();

   disconnect(ui->start_button,SIGNAL(clicked()),this,SLOT(set_name_interface()));
   connect(ui->start_button,SIGNAL(clicked()),this,SLOT(start_timer()));

   ui->treeView->setEnabled(false);
   ui->analyze_button->setEnabled(false);

}

/*
 *Set the name of the pcap file to read and load the informations
 */
void GUIp0f::set_name_file(){

    QModelIndex index = ui->treeView->currentIndex();
    if(index.isValid()){
        QString name_file = model->fileInfo(index).absoluteFilePath();
        int lenght = name_file.length();
        if(name_file.indexOf(".pcap",lenght-5)!=-1){
            QByteArray ba = name_file.toLatin1();
            char *char_file = ba.data();
            set_up_file(char_file);
            my.start();
            ui->start_button->setEnabled(false);
            ui->stop_button->setEnabled(false);
            ui->list_interface->setEnabled(false);
            sleep(1);
            set_list_ip();
            ui->analyze_button->setEnabled(false);
        }else{
            QMessageBox::warning(NULL,"Error","Choose a file with .pcap extension!");
        }
    }

}

//Start timer in order to refresh the GUI informations
void GUIp0f::start_timer(){

    if(!timer_update->isActive()){
        timer_update->start(10000);
    }

    ui->start_button->setEnabled(false);
    ui->stop_button->setEnabled(true);

}


void GUIp0f::create_list_interface(){

    QNetworkInterface* network = new QNetworkInterface();

    foreach(QNetworkInterface interface,network->allInterfaces()){
        ui->list_interface->addItem(interface.humanReadableName());
    }
}

//Stops update gui since start_buttun is pressed.
void GUIp0f::stop_p0f(){
    timer_update->stop();
    set_list_ip();

    ui->start_button->setText("Restart");
    ui->start_button->setEnabled(true);
    ui->stop_button->setEnabled(false);
}

void GUIp0f::update_gui(){
    ui->progressBar->setVisible(false);

    if (searched == true){
        search_host();
    }
    else{
        set_list_ip();
    }

}

//Adds and shows all hosts
void GUIp0f::set_list_ip(){

    network_db* data = network_db::get_istance();

    if(data->get_hosts().size()>0){
        delete_item();
        signal_buttons = new QSignalMapper(this);
        for(int i = 0,row = 0,column = 0;i<data->get_hosts().size();i++){
            add_item_net(data->get_hosts()[i],row,column);
            if(column == 4){
                row++;
                column = 0;
            }else{
                column++;
            }
        }
        connect(signal_buttons, SIGNAL(mapped(const QString &)),
                this, SIGNAL(clicked(const QString &)));
    }
}

//Adds a groupbox with an image and a button corresponding
//to the current host
void GUIp0f::add_item_net(host *current_host, int row, int column){

    QString host_ip = current_host->get_ip();

    QLabel *host_image = get_image_host(current_host);
    host_image->setFixedHeight(100);
    host_image->setFixedWidth(100);

    QPushButton *host_name = new QPushButton(host_ip);
    host_name->setFixedHeight(30);
    host_name->setFixedWidth(120);

    //if there is the possibility that host is behind nat
    if(current_host->get_packet(HOST_CHANGE)!=NULL){
        host_name->setStyleSheet("background-color: rgb(255, 0, 0); color: rgb(0, 0, 0); font-size: 10pt;font-weight: bold");
    }

    QFont font( "Arial", 11, QFont::Bold);
    host_name->setFont(font);
    connect(host_name, SIGNAL(clicked()), signal_buttons, SLOT(map()));
    signal_buttons->setMapping(host_name, host_ip);

    QGroupBox *my_group = new QGroupBox(ui->widget);

    QVBoxLayout *vbox = new QVBoxLayout;
    vbox->addWidget(host_image);
    vbox->addWidget(host_name);
    my_group->setLayout(vbox);
    my_group->show();
    my_group->setFixedHeight(130);
    my_group->setFixedWidth(130);

    ui->gridLayout->addWidget(my_group,row,column);
}

//Loads os image
QLabel* GUIp0f::get_image_host(host  *myhost){

    QLabel* image = new QLabel(ui->widget);

    QString os = myhost->get_packet(SYN_INFO)->get_value("os");

    if(os.indexOf("Linux") != -1){
        image->setStyleSheet("border-image:url(images/linux.png);");
    }
    else if(os.indexOf("Windows") != -1){
         image->setStyleSheet("border-image:url(images/windows.png);");
    }
    else if(os.indexOf("Mac") != -1) {
        image->setStyleSheet("border-image:url(images/mac.png);");
    }
    else if(os.indexOf("FreeBSD") != -1) {
        image->setStyleSheet("border-image:url(images/freebsd.png);");
    }
    else if(os.indexOf("OpenBSD") != -1) {
        image->setStyleSheet("border-image:url(images/openbsd.png);");
    }
    else if(os.indexOf("Solaris") != -1) {
        image->setStyleSheet("border-image:url(images/solaris.png);");
    }
    else{
        image->setStyleSheet("border-image:url(images/dont_know.png);");
    }

    return image;
}
//Creates a popup containing all informations about host ip
void GUIp0f::see_info_host(QString host_ip){

    network_db* data = network_db::get_istance();

    for(int i=0;i<data->get_hosts().size();i++){
        if(data->get_hosts()[i]->get_ip().compare(host_ip)==0){
              QHostInfo *info_host = new QHostInfo(QHostInfo::fromName(data->get_hosts()[i]->get_ip()));
              QString info = "IP HOST = "+data->get_hosts()[i]->get_ip()+
                        "\nDOMAIN NAME ="+info_host->hostName()+"\n";
              QMessageBox::information(NULL,"Host Information",info+"\n"+
                                       data->get_hosts()[i]->print_packets());
         }
     }
}

//Searchs and shows host ips corresponding with the user selection
void GUIp0f::search_host(){
    searched = true;
    network_db* data = network_db::get_istance();

    if(data->get_hosts().size()>0){
        delete_item();
        signal_buttons = new QSignalMapper(this);
        for(int i = 0,row = 0,column = 0;i<data->get_hosts().size();i++){
            if(host_correspond(data->get_hosts()[i])){
                add_item_net(data->get_hosts()[i],row,column);
                if(column == 4){
                    row++;
                    column = 0;
                }else{
                    column++;
                }
             }
         }
        connect(signal_buttons, SIGNAL(mapped(const QString &)),
                     this, SIGNAL(clicked(const QString &)));
    }

}

/*
 *Decide if the current host satisfies the search params
 */
bool GUIp0f::host_correspond(host* current_host){
    QString ip_searched;
    QString os_searched;
    QString app_searched;
    bool app_correct = true;
    bool os_correct = true;
    bool nat_correct = true;
    bool ip_correct = true;
    if(ui->check_ip->isChecked()){
        ip_searched = ui->line_ip->text();
        ip_correct = current_host->get_ip().indexOf(ip_searched)!=-1;
    }
    if(ui->check_app->isChecked()){
        app_searched = ui->combo_app->currentText();
        app_correct = current_host->get_app().indexOf(app_searched)!=-1;
    }
    if(ui->check_os->isChecked()){
        os_searched = ui->combo_os->currentText();
        os_correct = current_host->get_os().indexOf(os_searched)!=-1;
    }
    if(ui->check_nat->isChecked()){
        nat_correct = current_host->host_with_nat();
    }
    return ip_correct && app_correct && os_correct && nat_correct;
}

//Deletes previous gui items
void GUIp0f::delete_item(){

    QLayoutItem* eliminate;

    while((eliminate = ui->gridLayout->takeAt(0)) != 0){
        delete eliminate->widget();
    }
}



void GUIp0f::closeEvent(QCloseEvent *event){
    my.exit(0);
    timer_update->stop();
    exit(0);
}


GUIp0f::~GUIp0f()
{
    delete ui;
}
