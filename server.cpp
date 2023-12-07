#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>

using namespace std;

//--------------------ACTION-----------------------------

int createUser(string uid, string pass){
    int ret;
    string uid_dirname = "USERS/"+uid;
    ret = mkdir(&uid_dirname[0],0700);
    if (ret==-1) return 0;

    string host_dirname = uid_dirname+"/HOSTED";
    string bid_dirname = uid_dirname+"/BIDDED";

    ret = mkdir(&host_dirname[0],0700);
    if (ret==-1){
        rmdir(&uid_dirname[0]);
        return 0;
    }

    ret = mkdir(&bid_dirname[0],0700);
    if (ret==-1){
        rmdir(&uid_dirname[0]);
        rmdir(&host_dirname[0]);
        return 0;
    }

    string filename = "USERS/"+uid+"/"+uid+"_pass.txt";

    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fwrite(&pass[0],sizeof(char),pass.length(),fp);
    fclose(fp);
    return 1;
}

int createAuction(string aid, string start){
    int ret;
    if (!valid_aid(aid)) return 0;
    string aid_dirname = "AUCTIONS/"+aid;
    ret = mkdir(&aid_dirname[0], 0700);
    if (ret==-1) return 0;

    string bids_dirname = "AUCTIONS/"+aid+"/BIDS";
    ret = mkdir(&bids_dirname[0], 0700);
    if (ret==-1){
        rmdir(&aid_dirname[0]);
        return 0;
    }

    string filename = aid_dirname+"/START_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fwrite(&start[0],sizeof(char),start.length(),fp);   
    fclose(fp);

    return 1;
}

int endAuction(string aid, string end_info){
    string filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fwrite(&end_info[0],sizeof(char),end_info.length(),fp);   
    fclose(fp);
    return 1;
}

int createLogin(string uid){
    if (!valid_uid(uid)) return 0;

    string filename = "USERS/"+uid+"/"+uid+"_login.txt";

    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fprintf(fp, "Logged in\n");
    fclose(fp);
    return 1;
}

int eraseLogin(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    unlink(&filename[0]);
    return 1;       
}

int eraseUser(string uid){
    string login_name = "USERS/"+uid+"/"+uid+"_login.txt";
    string pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
    unlink(&login_name[0]);
    unlink(&pass_name[0]);
    return 1;       
}

int makeBid(string aid, string uid, string bid, string bid_info){
    string filename = "AUCTIONS/"+aid+"/BIDS/"+bid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fwrite(&bid_info[0],sizeof(char),bid_info.length(),fp);
    fclose(fp);
    return 1;
}

//-----------------------VERIFY------------------------------------------


string validateBid(string aid, string uid, string bid){
    if(!valid_aid(aid) || !valid_uid(uid)|| !valid_bid(bid)) return "ERR";

    string end_name = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE *fp = fopen(&end_name[0],"r");
    if (fp == NULL) return "NOK";

    string login_name = "USERS/"+uid+"/login.txt";
    fp = fopen(&login_name[0],"r");
    if (fp == NULL) return "NLG";

    string bids_dirname = "AUCTIONS/"+aid+"/BIDS/";
    struct dirent **filelist;
    int n_entries = scandir(&bids_dirname[0],&filelist, 0,alphasort);
    while (n_entries--){
        string bid_value;
        sscanf(filelist[n_entries]->d_name,"%s.txt",&bid_value[0]);
        if (stoi(bid_value) >= stoi(bid)) return "REF";
    }

    string start_name = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&start_name[0], ofstream::in);
    string auction_uid;

    ifs >> auction_uid;
    if (auction_uid == uid) return "ILG";

    return "ACC";
}


int is_registered(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_pass.txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp==NULL) return 0;
    else return 1;
}

int is_logged(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp==NULL) return 0;
    else return 1;
}

int is_owner(string uid, string aid){
    string host_dirname = "USERS/"+uid+"/HOSTED/";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    while (n_entries--){
        string host_aid;
        sscanf(filelist[n_entries]->d_name,"%s.txt",&host_aid[0]);
        if (host_aid == aid) return 1;
    }
    return 0;
}

int exists(string aid){
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS/",&filelist, 0,alphasort);
    while (n_entries--){
        string real_aid;
        sscanf(filelist[n_entries]->d_name,"%s",&real_aid[0]);
        if (real_aid == aid) return 1;
    }
    return 0;
}

int ended(string aid){
    string filename = "AUCTION/"+aid+"/END_"+aid+".txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp==NULL) return 0;
    else return 1;
}


//------------------------PROCESS-----------------------------------------


int login_process(string uid, string pass, string port, string ip){
    string msg, saved_pass;
    if (is_registered(uid)){
        string pass_name = "USERS/"+uid+"/"+uid+"/_pass.txt";
        ifstream ifs(&pass_name[0],ifstream::in);
        ifs >> saved_pass;
        if (saved_pass != pass) msg = "RLI NOK";
        else {
            msg = "RLI OK";
            createLogin(uid);
        }
    }
    else{
        msg = "RLI REG";
        createUser(uid, pass);
    }
    send_message_udp(port,ip,msg);
    return 1;
}

int logout_process(string uid, string pass, string port, string ip){
    string msg;
    if(!is_registered(uid)) msg = "RLO UNR";
    else if(!is_logged(uid)) msg = "RLO NOK";
    else {
        msg = "RLO OK";
        eraseLogin(uid);
    }

    send_message_udp(port, ip, msg);
    return 1;
}

int unregister_process(string uid, string pass, string port, string ip){
    string msg;
    if(!is_registered(uid)) msg = "RUR UNR";
    else if(!is_logged(uid)) msg = "RUR NOK";
    else {
        msg = "RUR OK";
        eraseUser(uid);
    }

    send_message_udp(port, ip, msg);
    return 1;
}

int close_process(string uid, string pass, string aid, string port, string ip){
    string msg, end_info;
    if (!is_logged(uid)) msg = "RCL NLG";
    else if (!exists(aid)) msg = "RCL EAU";
    else if (!is_owner(uid,aid)) msg = "RCL EOW";    
    else if (!ended(aid)) msg = "RCL END";
    else {
        msg = "RCL OK";
        //get endinfo (time etc)
        endAuction(aid, end_info);
    }


}

int bid_process(string uid, string pass, string aid, string bid, string port, string ip){
    string bid_datetime, bid_sec_time, bid_info, status, msg;
    string msg = "RBD " + validateBid(aid, uid, bid);
    send_single_message_tcp(port,ip,msg);
    if (status != "ACC") return 1;

    // get date and time
    bid_info = uid+" "+bid+" "+bid_datetime+" "+bid_sec_time; //complete pls
    makeBid(aid, uid, bid, bid_info);
    return 1;

}



//----------------------------MAIN----------------------------------


int main(int argc, char *argv[]){ //adicionar args e processar
    string port = PORT;
    bool verbose = false;
    for(int i = 1; i < argc; i++){
      if (argv[i] == "-p"){
        i++;
        port = argv[i];
      }
      else if (argv[i] == "-v"){
        verbose = true; 
      }
    }

}