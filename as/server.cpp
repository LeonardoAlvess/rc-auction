#include "../user/utils.h"
#include "sv_processes.h"
#include <cstring>
#include <iostream>

using namespace std;

//call this func right after socket chosen and messaeg received 
int send_verbose_msg(string uid, string req, string ip, string port){
  string msg = "User "+uid+" made a "+req+" request from "+ip+":"+port;
  cout << msg;
}

int main(int argc, char *argv[]){ //adicionar args e processar
    string port = PORT;
    bool verbose = false;
    for(int i = 1; i < argc; i++){
      if (!strcmp(argv[i],"-p")){
        i++;
        port = argv[i];
      }
      else if (!strcmp(argv[i],"-v")){
        verbose = true; 
      }
    }
    string uid = "190256";
    string uid1 = "123123";
    string uid2 = "999999";
    string pass = "password";
    string ip = "ipaddr";
    string aid = "001";
    string sv_info = "toy 100 1000 foto.txt size data";


    sv_login_process(uid, pass,port,ip);                //ok
    sv_login_process(uid1, pass,port,ip);               //ok
    sv_login_process(uid2, pass,port,ip);               //ok
    sv_open_process(uid, sv_info, port,ip);             //ok 001
    sv_bid_process(uid1,pass, aid,"150", port, ip);    //OK
    sv_bid_process(uid2,pass, aid,"200", port, ip);    //REF
    sv_bid_process(uid1,pass, aid,"300", port, ip);    //REF
    sv_bid_process(uid1,pass, aid,"350", port, ip);    //REF
    sv_show_record_process(aid,port,ip);
  
    return 0;
}