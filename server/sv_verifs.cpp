#include "sv_verifs.h"
#include "../common/utils.h"
#include "sv_files.h"

#include <dirent.h>
#include <fstream>

using namespace std;

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
    string aid_fname = aid+".txt";
    struct dirent **filelist;
    int n_entries = scandir(&host_dirname[0],&filelist, 0,alphasort);
    while (n_entries--){
        string filename = filelist[n_entries]->d_name;
        if (filename == aid_fname) return 1;
    }
    return 0;
}

int exists(string aid){
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS/",&filelist, 0,alphasort);
    while (n_entries--){
        string real_aid = filelist[n_entries]->d_name;
        if (real_aid == aid) return 1;
    }
    return 0;
}

int ended(string aid){
    string filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp!=NULL) return 1;
    
    string trash, starttime, timeactive;
    filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&filename[0], ifstream::in);
    for (int i=0; i<4; i++) ifs >> trash;
    ifs >> timeactive >> trash >> starttime;
    if(stoi(get_current_time()) < stoi(starttime)+stoi(timeactive)) return 1;
    else return 0;

    
}

string validateBid(string aid, string uid, string bid){
    if(!valid_aid(aid) || !valid_uid(uid)|| !valid_bid(bid)) return "ERR";

    if (!ended(aid)) return "NOK";

    if (is_logged(uid)) return "NLG";

    string start_fname = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&start_fname[0], ofstream::in);
    string auction_uid, trash, start_value;

    ifs >> auction_uid >> trash >> trash >> start_value;
    if (auction_uid == uid) return "ILG";

    if (stoi(start_value)>= stoi(bid)) return "REF";

    string bid_value, self = ".", parent = "..";
    string bids_dirname = "AUCTIONS/"+aid+"/BIDS/";
    struct dirent **filelist;
    int n_entries = scandir(&bids_dirname[0],&filelist, 0,alphasort);
    while (n_entries--){
        bid_value = filelist[n_entries]->d_name;
        int cutoff = bid_value.size() - 4;

        if(bid_value == self || bid_value == parent) continue;
        else if (stoi(bid_value.substr(0,cutoff)) >= stoi(bid)) return "REF";
    }

    return "ACC";
}