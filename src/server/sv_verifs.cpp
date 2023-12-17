#include "sv_verifs.h"
#include "../common/utils.h"
#include "sv_files.h"

#include <dirent.h>
#include <fstream>
#include <iostream>

using namespace std;


/**
 * @brief Checks if user is registered
 * @param uid - the uid to be tested
 * @return 1 if user is registered
 * @return 0 otherwise
*/
int is_registered(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_pass.txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp==NULL) return 0;
    else return 1;
}

/**
 * @brief Checks if user is logged in
 * @param uid - the uid to be tested
 * @return 1 if user is logged in
 * @return 0 otherwise
*/
int is_logged(string uid){
    string filename = "USERS/"+uid+"/"+uid+"_login.txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp==NULL) return 0;
    else return 1;
}

/**
 * @brief Checks if password is equal to user's registered pass
 * @param uid - the user's id
 * @param pass - the password to be compared
 * @return 1 if passwords are equal
 * @return 0 otherwise
*/
int authenticated(string uid, string pass){
    string saved_pass, pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
    ifstream ifs(&pass_name[0],ifstream::in);
    ifs >> saved_pass;
    return saved_pass == pass;
}

/**
 * @brief Checks if user is host of an auction
 * @param uid - the uid to be tested
 * @param aid - the auction to be tested
 * @return 1 if user is the host of the auction
 * @return 0 otherwise
*/
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

/**
 * @brief Checks if an auction exists
 * @param aid - the aid to be tested
 * @return 1 if the auction exists
 * @return 0 otherwise
*/
int exists(string aid){
    struct dirent **filelist;
    int n_entries = scandir("AUCTIONS/",&filelist, 0,alphasort);
    while (n_entries--){
        string real_aid = filelist[n_entries]->d_name;
        if (real_aid == aid) return 1;
    }
    return 0;
}

/**
 * @brief Checks if an auction is closed
 * @param aid - the auction's id
 * @return 1 if the auction is closed
 * @return 0 otherwise
*/
int ended(string aid){
    string filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
    FILE* fp = fopen(&filename[0],"r");
    if (fp!=NULL) return 1;
    
    string trash, starttime, timeactive;
    filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&filename[0], ifstream::in);
    for (int i=0; i<4; i++) ifs >> trash;
    ifs >> timeactive >> trash >> trash >> starttime;

    if(time(NULL) >= stoi(starttime)+stoi(timeactive)){
        string end_info = get_ended_time(aid);
        endAuction(aid, end_info);
        return 1;
    }
    else return 0;

    
}

/**
 * @brief Checks if a bid is possible to make
 * @param aid - the auction on which to make the bid
 * @param uid - the user to make the bid
 * @param pass - the user's submitted password
 * @param bid - the amount to be bid
 * @return "ACC" if the bid can accepted
 * @return "NOK" if the password is wrong, the auction does not exist or it has ended
 * @return "NLG" if the user is not logged in
 * @return "ILG" if the auction's host is the same user making the bid
 * @return "REF" if the bid's amount is lower than the auction's start value or the latest bid
 * @return "ERR" if the aid, uid or bid amount are invalid
*/
string validateBid(string aid, string uid, string pass, string bid){
    if(!valid_aid(aid) || !valid_uid(uid)|| !valid_bid(bid)) return "ERR";

    if (!authenticated(uid, pass) || !exists(aid) || ended(aid)) return "NOK";

    if (!is_logged(uid)) return "NLG";

    //getting host_uid and start value
    string start_fname = "AUCTIONS/"+aid+"/START_"+aid+".txt";
    ifstream ifs(&start_fname[0], ofstream::in);
    string auction_uid, trash, start_value;
    ifs >> auction_uid >> trash >> trash >> start_value;

    if (auction_uid == uid) return "ILG";

    if (stoi(start_value)>= stoi(bid)) return "REF";

    //getting the auction's largest bid and comparing it to the intended bid
    string bid_value;
    string bids_dirname = "AUCTIONS/"+aid+"/BIDS/";
    struct dirent **filelist;
    int n_entries = scandir(&bids_dirname[0],&filelist, 0,alphasort);

    if (n_entries == 2) return "ACC";   //no bids to compare
    bid_value = filelist[n_entries-1]->d_name;
    if (stoi(bid_value.substr(0,6)) >= stoi(bid)) return "REF"; //discarding .txt and converting to integer to compare

    return "ACC";
}
