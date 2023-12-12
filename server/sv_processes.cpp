#include "sv_processes.h"
#include "sv_actions.h"
#include "sv_verifs.h"
#include "../common/utils.h"

#include <fstream>
#include <iostream>
#include <sstream>


using namespace std;

int sv_login_process(string uid, string pass, string port, string ip){
    string msg, saved_pass;
    if (is_registered(uid)){
        string pass_name = "USERS/"+uid+"/"+uid+"_pass.txt";
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
        createLogin(uid);
    }
    //send_message_udp(port,ip,msg);
    cout << msg+"\n"; //for testing
    return 0;
}

int sv_logout_process(string uid, string pass, string port, string ip){
    string msg;
    if(!is_registered(uid)) msg = "RLO UNR";
    else if(!is_logged(uid)) msg = "RLO NOK";
    else {
        msg = "RLO OK";
        eraseLogin(uid);
    }

    //send_message_udp(port, ip, msg);
    cout << msg+"\n"; //for testing
    return 0;
}

int sv_unregister_process(string uid, string pass, string port, string ip){
    string msg;
    if(!is_registered(uid)) msg = "RUR UNR";
    else if(!is_logged(uid)) msg = "RUR NOK";
    else {
        msg = "RUR OK";
        eraseUser(uid);
    }

    //send_message_udp(port, ip, msg);
    cout << msg+"\n"; //for testing
    return 0;
}

int sv_myauctions_process(string uid, string port, string ip){
    string msg = "RMA", state;
    vector<string> aid_list;
    if (!is_logged(uid)) msg+= " NLG";
    else if ((aid_list = get_hosted(uid)).size() == 0) msg += " NOK";
    else{
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    //send_message_udp(port, ip, msg);
    cout << msg+"\n";
    return 0;
}

int sv_mybids_process(string uid, string port, string ip){
    string msg = "RMB", state;
    vector<string> aid_list, empty, used;
    bool dupe;
    if (!is_logged(uid)) msg += " NLG";
    else if((aid_list = get_bidded(uid)).size() == 0) msg += " NOK";
    else{
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
    //send_message_udp(port, ip, msg);
    cout << msg+"\n";
    return 0;
}

int sv_list_process(string port, string ip){
    string msg = "RLS", state;
    vector<string> aid_list = get_all_auctions();
    if (aid_list.size() == 0) msg += " NOK";
    else {
        msg += " OK";
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    //send_message_udp(port,ip,msg);
    cout << msg+"\n";
    return 0;
    }

int sv_show_record_process(string aid, string port, string ip){
    string msg = "RRC", host_UID, name, asset_fname, start_value, start_datetime, timeactive;
    vector<string> bids_list;
    if (!exists(aid)) msg += " NOK";
    else{
        msg += " OK";
        string filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
        ifstream ifs(filename, ifstream::in);
        ifs >> host_UID >> name >> asset_fname >> start_value >> timeactive >> start_datetime;
        msg += " " + host_UID + " " + name + " " + asset_fname + " " + start_value + " " + start_datetime + " " + timeactive;
       
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

    //send_message_udp(port, ip, msg);
    cout << msg+"\n";
    return 0;
}

int sv_open_process(string uid,string input, string port, string ip){
    string msg = "ROA", aid, name, start_value, timeactive, asset_fname, info;
    istringstream iss(input);
    if (!is_logged(uid)) msg += " NLG";
    else{
        aid = get_unique_aid();

        iss >> name >> start_value >> timeactive >> asset_fname;
        info = uid+" "+name+" "+asset_fname+" "+start_value+" "+timeactive+ " " + get_current_time();

        if (aid == "1000" || createAuction(aid, info) == -1) msg += " NOK";
        else {
            hostAuction(aid, uid);
            char txt[8] = "mememan";
            loadAsset(aid, asset_fname, txt);
            msg += " OK "+aid;
        }
    }
    //send_single_message_tcp(port, ip, msg);
    cout << msg + "\n";
    return 0;
}

int sv_close_process(string uid, string pass, string aid, string port, string ip){
    string msg, end_info;
    if (!is_logged(uid)) msg = "RCL NLG";
    else if (!exists(aid)) msg = "RCL EAU";
    else if (!is_owner(uid,aid)) msg = "RCL EOW";    
    else if (ended(aid)) msg = "RCL END";
    else {
        msg = "RCL OK";
        end_info = get_current_time();
        endAuction(aid, end_info);
    }
    //send_single_message_tcp(port, ip, msg);
    cout << msg+"\n";
    return 0;
}

int sv_bid_process(string uid, string pass, string aid, string bid, string port, string ip){
    string bid_datetime, bid_sec_time, bid_info;
    string status = validateBid(aid, uid, bid);
    string msg = "RBD " + status;
    if (status == "ACC"){
        
        bid_info = uid+" "+bid+" "+get_bid_time(aid);
        makeBid(aid, uid, bid, bid_info);
    }
    //send_single_message_tcp(port,ip,msg);
    cout << msg+"\n";
    return 0;
}
