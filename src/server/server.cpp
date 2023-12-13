#include "../common/utils.h"
#include "sv_processes.h"
#include <cstring>
#include <iostream>

using namespace std;

//call this func right after socket chosen and messaeg received 
int send_verbose_msg(string uid, string req, string ip, string port){
  string msg = "User "+uid+" made a "+req+" request from "+ip+":"+port;
  cout << msg;
  return 0;
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
  string sv_info = "190256 password toy 100 1000 foto.txt size data";

  sv_login_process(uid,pass,port,ip);
  sv_logout_process(uid,pass,ip,port);
  sv_login_process(uid,pass,ip,port);
  sv_open_process(sv_info,ip,port);
  sv_bid_process(uid,pass,"001","1001",port,ip);
  sv_myauctions_process(uid,port,ip);
  sv_login_process(uid1,pass,port,ip);
  sv_bid_process(uid1,pass,"001","200",port,ip);
  sv_show_record_process("001", port,ip);
  sv_mybids_process(uid1,port,ip);
  sv_close_process(uid,pass,"001",port,ip);
  sv_list_process(port,ip);
  
  return 0;
}