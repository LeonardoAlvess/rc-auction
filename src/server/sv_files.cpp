#include "sv_files.h"
#include "sv_verifs.h"

#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <fstream>
#include <iostream>

using namespace std;

int createUser(string uid, string pass){
    int ret;
    string uid_dirname = "USERS/"+uid;
    struct stat buf;

    if (stat(&uid_dirname[0], &buf) != 0){         //if directory USERS/aid not found 
        ret = mkdir(&uid_dirname[0],0700);          
        if (ret==-1) return -1;

        string host_dirname = uid_dirname+"/HOSTED";
        string bid_dirname = uid_dirname+"/BIDDED";

        ret = mkdir(&host_dirname[0],0700);
        if (ret==-1){
            rmdir(&uid_dirname[0]);
            return -1;
        }

        ret = mkdir(&bid_dirname[0],0700);
        if (ret==-1){
            rmdir(&uid_dirname[0]);
            rmdir(&host_dirname[0]);
            return -1;
        }
    }
    

    string filename = "USERS/"+uid+"/"+uid+"_pass.txt";

    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&pass[0],sizeof(char),pass.length(),fp);
    fclose(fp);
    return 0;
}

int createAuction(string aid, string start_info){
    int ret;
    string aid_dirname = "AUCTIONS/"+aid;
    ret = mkdir(&aid_dirname[0], 0700);
    if (ret==-1) return -1;

    string bids_dirname = "AUCTIONS/"+aid+"/BIDS";
    ret = mkdir(&bids_dirname[0], 0700);
    if (ret==-1){
        rmdir(&aid_dirname[0]);
        return -1;
    }

    string filename = aid_dirname+"/START_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&start_info[0],sizeof(char),start_info.size(),fp);   
    fclose(fp);

    return 0;
}

int loadAsset(string aid, string asset_fname, char* asset_buf){
    string filename = "AUCTIONS/"+aid+"/"+asset_fname;
    FILE* fp = fopen(&filename[0],"ab");
    fwrite(asset_buf, sizeof(char), strlen(asset_buf),fp);
    fclose(fp);
    return 0;
}

int hostAuction(string aid, string uid){
    
    string filename = "USERS/"+uid+"/HOSTED/"+aid+".txt";
    FILE* fp = fopen(&filename[0], "w");
    if (fp == NULL) return -1;
    fclose(fp);
    return 0;

}

int endAuction(string aid, string end_info){
    string filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&end_info[0],sizeof(char),end_info.length(),fp);   
    fclose(fp);
    return 0;
}

int createLogin(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fprintf(fp, "Logged in\n");
    fclose(fp);
    return 0;
}

int eraseLogin(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    unlink(&filename[0]);
    return 0;       
}

int eraseUser(string uid){
    string login_name = "USERS/"+uid+"/"+uid+"_login.txt";
    string pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
    unlink(&login_name[0]);
    unlink(&pass_name[0]);
    return 0;       
}

int makeBid(string aid, string uid, string bid, string bid_info){
    string filename = "AUCTIONS/"+aid+"/BIDS/"+bid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&bid_info[0],sizeof(char),bid_info.length(),fp);
    fclose(fp);

    filename = "USERS/"+uid+"/BIDDED/"+aid+".txt";
    FILE *fp1 = fopen(&filename[0], "w");    
    if (fp1==NULL) return -1;
    fclose(fp1);
    return 0;
}

vector<string> get_hosted(string uid){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    string host_dirname = "USERS/"+uid+"/HOSTED/";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue;

        aid = aid.substr(0,3);
        aid_list.push_back(aid);
    }
    return aid_list;
}

vector<string> get_bidded(string uid){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    string host_dirname = "USERS/"+uid+"/BIDDED/";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue;
        
        aid = aid.substr(0 ,3);
        aid_list.push_back(aid);
    }
    return aid_list;
}

vector<string> get_all_auctions(){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS",&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue;
        aid_list.push_back(aid);
    }
    return aid_list;
}

vector<string> get_auction_bids(string aid){
    string bid, self = ".", parent = "..";
    vector<string> bids_list;
    string bids_dirname = "AUCTIONS/"+aid+"/BIDS/";
    struct dirent **filelist;
    int n_entries = scandir(&bids_dirname[0],&filelist, 0,alphasort);
    for(int i=0; i<n_entries; i++){
        bid = filelist[i]->d_name;
        if (bid == self || bid == parent) continue;
        int cutoff = bid.size()-4;
        bid = bid.substr(0, cutoff);
        
        bids_list.push_back(bid);
    }
    return bids_list;
}

string get_unique_aid(){
    char aid[4];
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS/",&filelist,0,alphasort) - 1;
    sprintf(aid,"%03d",n_entries);
    return aid;
}

string get_current_time(){  //returns: (YYYY-MM-DD hh:mm:ss fulltime)
    string ret;
    time_t t = time(NULL);
    struct tm *tt;
    tt = gmtime(&t);
    char buf[25];
    strftime(buf,25,"%Y-%m-%d %X",tt);
    ret = buf;
    ret +=" "+to_string(t);
    return ret;
}

string get_bid_time(string aid){
    string ret, aux;
    int start_time;
    char buf[25];
    time_t t = time(NULL);
    struct tm *tt;
    tt = gmtime(&t);
    strftime(buf,25,"%Y-%m-%d %X",tt);
    ret = buf;

    string filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(filename, ifstream::in);

    for(int i=0; i<7; i++) ifs >> aux;

    ifs >> start_time;
    ret += " "+to_string(t-start_time);

    return ret;
}