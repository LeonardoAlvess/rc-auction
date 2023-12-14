#include "sv_processes.h"
#include "sv_files.h"
#include "sv_verifs.h"
#include "../common/utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>


using namespace std;

int sv_login_process(string uid, string pass, string port, string ip){
    string msg, saved_pass;

    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if (is_registered(uid)){
        string pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
        ifstream ifs(&pass_name[0],ifstream::in);
        ifs >> saved_pass;
        if (saved_pass != pass) msg = "RLI NOK\n";
        else {
            msg = "RLI OK\n";
            createLogin(uid);
        }
    }
    else{
        msg = "RLI REG\n";
        createUser(uid, pass);
        createLogin(uid);
    }
    send_message_udp(port,ip,msg, SERVER_MODE);
    //cout << msg; //for testing
    return 0;
}

int sv_logout_process(string uid, string pass, string port, string ip){
    string msg;
    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if(!is_registered(uid)) msg = "RLO UNR\n";
    else if(!is_logged(uid)) msg = "RLO NOK\n";
    else {
        msg = "RLO OK\n";
        eraseLogin(uid);
    }
    send_message_udp(port, ip, msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
}

int sv_unregister_process(string uid, string pass, string port, string ip){
    string msg;
    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if(!is_registered(uid)) msg = "RUR UNR\n";
    else if(!is_logged(uid)) msg = "RUR NOK\n";
    else {
        msg = "RUR OK\n";
        eraseUser(uid);
    }
    send_message_udp(port, ip, msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
}

int sv_myauctions_process(string uid, string port, string ip){
    string msg, state;
    vector<string> aid_list;
    if (!valid_uid(uid)) msg = "ERR";
    else if (!is_logged(uid)) msg = "RMA NLG";
    else if ((aid_list = get_hosted(uid)).size() == 0) msg = "RMA NOK";
    else{
        msg = "RMA OK";
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    msg += "\n";
    send_message_udp(port, ip, msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
}

int sv_mybids_process(string uid, string port, string ip){
    string msg , state;
    vector<string> aid_list, empty, used;
    bool dupe;
    if (!valid_uid(uid)) msg = "ERR";
    else if (!is_logged(uid)) msg = "RMB NLG";
    else if((aid_list = get_bidded(uid)).size() == 0) msg = "RMB NOK";
    else{
        msg = "RMB OK";
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            dupe = false;
            for (vector<string>::iterator us=used.begin(); us != used.end(); ++us){
                if (*it == *us) {
                    dupe = true;
                    break;
                }
            }
            if (dupe) continue;
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    msg += "\n";
    send_message_udp(port, ip, msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
}

int sv_list_process(string port, string ip){
    string msg , state;
    vector<string> aid_list = get_all_auctions();
    if (aid_list.size() == 0) msg = "RLS NOK";
    else {
        msg = "RLS OK";
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    msg += "\n";
    send_message_udp(port,ip,msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
    }

int sv_show_record_process(string aid, string port, string ip){
    string msg, host_UID, name, asset_fname, start_value, timeactive, date, time;
    vector<string> bids_list;
    if (!valid_aid(aid)) msg = "ERR";
    else if (!exists(aid)) msg = "RRC NOK";
    else{
        msg = "RRC OK";
        string filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
        ifstream ifs(filename, ifstream::in);
        ifs >> host_UID >> name >> asset_fname >> start_value >> timeactive >> date >> time;
        msg += " " + host_UID + " " + name + " " + asset_fname + " " + start_value + " " + date + " " + time + " " + timeactive;
       
        bids_list = get_auction_bids(aid);

        for(vector<string>::iterator it = bids_list.begin(); it != bids_list.end(); ++it){
            filename = "AUCTIONS/"+aid+"/BIDS/"+*it+".txt";
            ifstream bidstream(filename, ifstream::in);
            string bid_info;
            getline(bidstream,bid_info);
            msg += " B " + bid_info;
        }

        if(ended(aid)){
            filename = "AUCTIONS/"+aid+"/END_"+aid+".txt";
            ifstream endstream(filename, ifstream::in);
            string end_info;
            getline(endstream,end_info);
            msg += " E " + end_info;
        }
    }
    msg += "\n";
    send_message_udp(port, ip, msg, SERVER_MODE);
    //cout << msg+"\n"; //for testing
    return 0;
}

//input is everything excluding fsize and data (might change to auction info obj)
int sv_open_process(string input, string port, string ip, int socket_fd){
    string trash,msg, aid, uid, pass, name, start_value, timeactive, asset_fname, info;
    istringstream iss(input);
    iss >> trash >> uid >> pass >> name >> start_value >> timeactive >> asset_fname;
    if (!valid_uid(uid) || !valid_password(pass) || !valid_auction_name(name) || !valid_start_value(start_value) || !valid_duration(timeactive)) msg = "ERR";
    else if (!is_logged(uid)) msg = "ROA NLG";
    else{
        aid = get_unique_aid();

        info = uid+" "+name+" "+asset_fname+" "+start_value+" "+timeactive+ " " + get_current_time();
        if (aid == "1000" || createAuction(aid, info) == -1) msg = "ROA NOK";
        else {
            hostAuction(aid, uid);          //verificar password    
            char txt[8] = "mememan";        //TO CHANGE INTO READ LOOP FOR FILE
            loadAsset(aid, asset_fname, txt);
            msg = "ROA OK "+aid;
        }
    }
    cout << msg;
    msg += "\n";
    send_message_tcp(socket_fd, msg);
    close(socket_fd);
    //cout << msg;
    return 0;
}

int sv_close_process(string uid, string pass, string aid, string port, string ip, int socket_fd){
    string msg, end_info;
    if (!valid_uid(uid) || !valid_password(pass) || !valid_aid(aid)) msg = "ERR";
    if (!is_logged(uid)) msg = "RCL NLG";
    else if (!exists(aid)) msg = "RCL EAU";
    else if (!is_owner(uid,aid)) msg = "RCL EOW";    
    else if (ended(aid)) msg = "RCL END";
    else {
        msg = "RCL OK";
        end_info = get_current_time();
        endAuction(aid, end_info);
    }
    msg += "\n";
    send_message_tcp(socket_fd, msg);
    close(socket_fd);
    //cout << msg;
    return 0;
}

int sv_bid_process(string uid, string pass, string aid, string bid, string port, string ip, int socket_fd){
    string bid_datetime, bid_sec_time, bid_info;
    string status = validateBid(aid, uid, bid);
    string msg = "RBD " + status;
    if (status == "ERR") msg = "ERR";
    if (status == "ACC"){
        
        bid_info = uid+" "+bid+" "+get_bid_time(aid);
        makeBid(aid, uid, bid, bid_info);
    }
    msg += "\n";
    send_message_tcp(socket_fd, msg);
    close(socket_fd);
    //cout << msg;
    return 0;
}
