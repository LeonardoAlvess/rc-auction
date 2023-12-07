#include "utils.h"
#include <unistd.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fstream>
#include <iostream>

using namespace std;

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

string validateBid(string aid, string uid, string bid){
    if(!valid_aid(aid) || !valid_uid(uid)|| !valid_bid(bid)) return "ERR";

    string end_name = "AUCTIONS/"+aid+"/END_"+aid".txt";
    FILE *fp = fopen(&end_name[0],"r");
    if (fp == NULL) return "NOK";

    string login_name = "USERS/"+uid+"/login.txt";
    fp = fopen(&login_name[0],"r");
    if (fp == NULL) return "NLG";

    string bids_dirname = "AUCTIONS/"+aid+"/BIDS/";
    struct dirent **filelist;
    int n_entries = scandir(&bids_dirname[0],&filelist, 0,alphasort);
    string bid_filename;
    while (n_entries--){
        string bid_value;
        sscanf(filelist[n_entries]->d_name,"%s.txt",&bid_value[0]);
        if (stoi(bid_value) >= stoi(bid)) return "REF";
    }

    string start_name = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&start_name[0], ofstream::in);
    string auction_uid << ifs;
    if (auction_uid == uid) return "ILG";
    
    return "ACC";
}

int makeBid(string aid, string uid, string bid, string bid_info){
    string filename = "AUCTIONS/"+aid+"/BIDS/"+bid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return 0;
    fwrite(&bid_info[0],sizeof(char),bid_info.length(),fp);
    fclose(fp);
    return 1;
}





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