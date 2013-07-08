#include "guip0f.h"
#include <QApplication>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <QNetworkInterface>
#include <QMessageBox>

#include "qlist.h"

extern "C"{
#include "p0f.h"
}

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    GUIp0f w;

    QNetworkInterface* network = new QNetworkInterface();
    QString strNetworkInterfaces;

    foreach(QNetworkInterface interface,network->allInterfaces())
         {
         strNetworkInterfaces  = strNetworkInterfaces +
           "\nInterface Name: " +interface.humanReadableName()+
           "\n---------------------------";
         foreach(QHostAddress addr,network->allAddresses())
         {
               strNetworkInterfaces  = strNetworkInterfaces +
           "\nAddress Entry : " +addr.toString();
         }

       strNetworkInterfaces  = strNetworkInterfaces +
       "\nHardware Address: " +interface.hardwareAddress()+
       "\nIs UP:"+(interface.flags().testFlag(QNetworkInterface::IsUp)?"YES":"NO")+
       "\nIs Running:"+(interface.flags().testFlag(QNetworkInterface::IsRunning)?"YES":"NO")+
       "\nCan Broadcast:"+(interface.flags().testFlag(QNetworkInterface::CanBroadcast)?"YES":"NO")+
       "\nIs LoopBack:"+(interface.flags().testFlag(QNetworkInterface::IsLoopBack)?"YES":"NO")+
       "\nIs PointToPoint:"+(interface.flags().testFlag(QNetworkInterface::IsPointToPoint)?"YES":"NO")+
       "\nCan Multicast:"+(interface.flags().testFlag(QNetworkInterface::CanMulticast)?"YES":"NO")+"\n";
         }

         QMessageBox::information(NULL,"Network Interfaces..",strNetworkInterfaces);

    printf("%d\n",setuid(0));

    printf("%d",getuid());


    printf("%d\n",setuid(0));

    printf("%d",getuid());
    char stringa[10]="eth0";



    printf("Prova passando array caratteri");
    set_up_iface(stringa);
   // list_interfaces();
    w.show();
    
    return a.exec();
}