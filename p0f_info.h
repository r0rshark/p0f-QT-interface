#include <map>
#include <time.h>
#include <QString>
#ifndef P0F_PACKET_H
#define P0F_PACKET_H

//Abstract class

class p0f_info
{
public:
    enum owner{
        SERVER,
        CLIENT
    };
    p0f_info(QString server){//I create 2 constuctor because the owner field is optional
        server_address=server;
    }
    p0f_info(QString server,owner own){
        server_address=server;
        subject=own;
    }
    owner get_subject(){
        return subject;
    }
    QString get_address(){
        return server_address;
    }
protected:
   QString server_address; //address of the computer which we are trying to identify
   owner subject;          //CLIENT means our machine(not so interesting this info) SERVER means another machine in the network
};



class uptime_info : public p0f_info{
private:
    QString uptime;
    QString raw_freq;
public:
   uptime_info(QString server) :p0f_info(server){}
   uptime_info(QString server,p0f_info::owner own) : p0f_info(server,own){}
   QString  get_uptime(){
        return uptime;
    }
    QString get_raw_freq(){
        return raw_freq;
    }
    void set_uptime(QString up_time){
        uptime=up_time;
    }
    void set_raw_freq(QString rawfreq){
        raw_freq=rawfreq;
    }
};

class mtu_info :public p0f_info{
private:
    QString link;
    QString raw_mtu;
public:
    mtu_info(QString server) :p0f_info(server){}
    mtu_info(QString server,p0f_info::owner own) : p0f_info(server,own){}
    QString get_link(){
        return link;
    }
     QString get_raw_mtu(){
        return raw_mtu;
    }
     void set_link(QString mtu_link){
         link= mtu_link;
     }
     void set_raw_mtu(QString mtu){
         raw_mtu=mtu;
     }
};

class syn_info :public p0f_info{
private:
    QString os;
    QString dist;
    QString params;
    QString raw_sig;
public:
    syn_info(QString server) :p0f_info(server){}
    syn_info(QString server,p0f_info::owner own) : p0f_info(server,own){}

    QString get_os(){
        return os;
    }
    QString get_dist(){
        return dist;
    }
    QString get_params(){
        return params;
    }
    QString get_raw_sig(){
        return raw_sig;
    }
    void set_os(QString SO){
       os=SO;
    }
    void set_dist(QString distance){
        dist=distance;
    }
    void set_params(QString parameters){
        params=parameters;
    }
    void set_raw_sig(QString sig){
        raw_sig=sig;
    }
};
class http_response_info :public p0f_info{
private:
    QString app;
    QString lang;
    QString param;
    QString raw_sig;
public:
    http_response_info(QString server) :p0f_info(server){}
    http_response_info(QString server,p0f_info::owner own) : p0f_info(server,own){}

    QString get_app(){
        return app;
    }
    QString get_lang(){
        return lang;
    }
    QString get_param(){
        return param;
    }
    QString get_raw_sig(){
        return raw_sig;
    }
    void set_app(QString application){
        app=application;
    }
    void set_lang(QString language){
        lang=language;
    }
    void set_param(QString parameters){
        param=parameters;
    }
    void set_raw_sig(QString sig){
        raw_sig=sig;
    }

};

class p0f_info_factory{
public:
    enum info_type{
        HTTP_RESPONSE,
        SYN,
        MTU,
        UPTIME
    };
    static p0f_info *new_p0f_info(p0f_info_factory::info_type type,QString machine_addr);

};


int test();


#endif // P0F_PACKET_H
