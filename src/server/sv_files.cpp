#include "sv_files.h"
#include "sv_verifs.h"

#include <sys/stat.h>
#include <unistd.h>
#include <cstring>
#include <dirent.h>
#include <time.h>
#include <fstream>

using namespace std;


/**
 * @brief Registers a user, creating the necessary files
 * @param uid - the uid to be registered
 * @param pass - the password chosen by the user
 * @return 0 if user sucessfuly registered
 * @return -1 otherwise
*/
int createUser(string uid, string pass){
    int ret;
    string uid_dirname = "USERS/"+uid;
    struct stat buf;

    if (stat(&uid_dirname[0], &buf) != 0){         //if directory USERS/aid not found 

        ret = mkdir(&uid_dirname[0],0700);         //create USERS/aid directory
        if (ret==-1) return -1;

        string host_dirname = uid_dirname+"/HOSTED";
        string bid_dirname = uid_dirname+"/BIDDED";


        //Creating HOSTED and BIDDED folders
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
    
    //Creating password file
    string filename = "USERS/"+uid+"/"+uid+"_pass.txt";

    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&pass[0],sizeof(char),pass.length(),fp);
    fclose(fp);
    return 0;
}


/**
 * @brief Creates an auction, creating the necessary files
 * @param aid - the aid to be associated with the auction
 * @param asset_fname - the filename of the asset sold in the auction
 * @param start_info - a string to store in the START file
 * @return a pointer to aid/asset_fname file if successful
 * @return NULL otherwise
*/
FILE* createAuction(string aid, string asset_fname, string start_info){
    int ret;
    string aid_dirname = "AUCTIONS/"+aid;
    ret = mkdir(&aid_dirname[0], 0700);
    if (ret==-1) return NULL;

    string bids_dirname = "AUCTIONS/"+aid+"/BIDS";
    ret = mkdir(&bids_dirname[0], 0700);
    if (ret==-1){
        rmdir(&aid_dirname[0]);
        return NULL;
    }

    string filename = aid_dirname+"/START_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "wb");    
    if (fp==NULL) return NULL;
    fwrite(&start_info[0],sizeof(char),start_info.size(),fp);   
    fclose(fp);
    filename = aid_dirname+"/"+asset_fname;
    FILE *asset = fopen(&filename[0], "wb");
    return asset;
}


/**
 * @brief Atributes an auction to a user, creating the necessary file
 * @param aid - the id of the auction
 * @param uid - the user id to be the host
 * @return 0 if successful
 * @return -1 otherwise
*/
int hostAuction(string aid, string uid){
    
    string filename = "USERS/"+uid+"/HOSTED/"+aid+".txt";
    FILE* fp = fopen(&filename[0], "w");
    if (fp == NULL) return -1;
    fclose(fp);
    return 0;

}

/**
 * @brief Closes an auction, creating the necessary file
 * @param aid - the id of the auction to close
 * @param end_info - a string to store in END file
 * @return 0 if closing successful
 * @return -1 otherwise
*/
int endAuction(string aid, string end_info){
    string filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&end_info[0],sizeof(char),end_info.length(),fp);   
    fclose(fp);
    return 0;
}

/**
 * @brief Logs a user in, creating the necessary file
 * @param uid - user id to log in
 * @return 0 if successful
 * @return -1 otherwise
*/
int createLogin(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fprintf(fp, "Logged in\n");
    fclose(fp);
    return 0;
}

/**
 * @brief Logs a user out, deleting the necessary file
 * @param uid - user id to log out
 * @return 0 if successful
 * @return -1 otherwise
*/
int eraseLogin(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    if (unlink(&filename[0]) == 0);
    return 0;     
}

/**
 * @brief Unregisters a user, deleting the necessary files
 * @param uid - user id to log out
 * @return 0 if successful
 * @return -1 otherwise
*/
int eraseUser(string uid){
    string login_name = "USERS/"+uid+"/"+uid+"_login.txt";
    string pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
    if (unlink(&login_name[0]) == -1) return -1;
    if (unlink(&pass_name[0]) == -1) return -1;
    return 0;       
}

/**
 * @brief Makes a bid on an auction, creating the necessary files
 * @param aid - auction to bid on
 * @param uid - user that is making the bid
 * @param bid - bidded amount
 * @param bid_info - string to store in the bids file
 * @return 0 if successful
 * @return -1 otherwise
*/
int makeBid(string aid, string uid, string bid, string bid_info){

    //converting the bid to a 6 character, left padded with 0s format
    char new_bid[7];
    int number = atoi(bid.c_str());
    sprintf(new_bid,"%06d",number); 

    //creating and writing to the bid file
    string filename = "AUCTIONS/"+aid+"/BIDS/"+ new_bid + ".txt";
    FILE *fp = fopen(&filename[0], "w");    
    if (fp==NULL) return -1;
    fwrite(&bid_info[0],sizeof(char),bid_info.length(),fp);
    fclose(fp);

    //creating a bid file in the uid's BIDDED folder
    filename = "USERS/"+uid+"/BIDDED/"+aid+".txt";
    FILE *fp1 = fopen(&filename[0], "w");    
    if (fp1==NULL) return -1;
    fclose(fp1);
    return 0;
}

/**
 * @brief Gets all of a user's hosted auctions
 * @param uid - user id
 * @return a vector of the user's hosted auctions' aids
*/
vector<string> get_hosted(string uid){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    string host_dirname = "USERS/"+uid+"/HOSTED/";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue;     //skipping parent and self directory

        aid = aid.substr(0,3);                          //discarding .txt part of string
        aid_list.push_back(aid);
    }
    return aid_list;
}

/**
 * @brief Gets all of a user's bidded auctions
 * @param uid - user id
 * @return a vector of the user's bidded auctions' aids
*/
vector<string> get_bidded(string uid){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    string host_dirname = "USERS/"+uid+"/BIDDED/";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue; //skipping parent and self directory
        
        aid = aid.substr(0 ,3);                     //discarding .txt part of string
        aid_list.push_back(aid);
    }
    return aid_list;
}

/**
 * @brief Gets all auctions
 * @return a vector of all auctions' aids
*/
vector<string> get_all_auctions(){
    string aid, self = ".", parent = "..";
    vector<string> aid_list;
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS",&filelist, 0,alphasort);
    for(int i = 1; i<n_entries; i++){
        aid = filelist[i]->d_name;
        if (aid == self || aid == parent) continue; //skipping parent and self directory
        aid_list.push_back(aid);
    }
    return aid_list;
}

/**
 * @brief Gets all of an auctions's bids
 * @param aid - auction id
 * @return a vector of the auction's bid values
*/
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
        bid = bid.substr(0, cutoff);             //discarding .txt part of string
        
        bids_list.push_back(bid);
    }
    return bids_list;
}

/**
 * @brief Gets the next available aid
 * @return the next available aid
*/
string get_unique_aid(){
    char aid[4];
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS/",&filelist,0,alphasort) - 1; // next availale aid
    sprintf(aid,"%03d",n_entries);                                  // 3 character, left padded with 0
    return aid;
}

/**
 * @brief Gets the current time and returns in a readable format
 * @return a string with (YYYY-MM-DD hh:mm:ss fulltime) format
*/
string get_current_time(){
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

/**
 * @brief Gets the current time and the time in seconds since an auction started
 * @param aid - The auction to consider
 * @return a string with (YYYY-MM-DD hh:mm:ss time_since_start) format
*/
string get_elapsed_time(string aid){
    string ret, aux;
    int start_time;
    char buf[25];

    // getting current time in readable format
    time_t t = time(NULL);
    struct tm *tt;
    tt = gmtime(&t);
    strftime(buf,25,"%Y-%m-%d %X",tt);
    ret = buf;

    // getting the auctions start time
    string filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(filename, ifstream::in);
    for(int i=0; i<7; i++) ifs >> aux;      //discarding irrelevant fields in start file
    ifs >> start_time;

    ret += " "+to_string(t-start_time);     // appending time since start to string
    return ret;
}

/**
 * @brief Gets the time an auction ended by exceeding time active
 * @param aid - The auction to consider
 * @return a string with (YYYY-MM-DD hh:mm:ss timeactive) format
*/
string get_ended_time(string aid){
    string aux, timeactive, start_time, ret;

    // getting the start time
    string filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(filename, ifstream::in);
    for(int i=0; i<4; i++) ifs >> aux;
    ifs >> timeactive >> aux >> aux >> start_time;

    //getting the ended time in a readable format
    time_t end_time = stoi(start_time)+stoi(timeactive);
    struct tm *tt;
    tt = gmtime(&end_time);
    char buf[25];
    strftime(buf,25,"%Y-%m-%d %X",tt);
    ret = buf;

    ret += " "+timeactive;
    return ret;
}