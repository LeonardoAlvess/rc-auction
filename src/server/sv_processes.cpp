#include "sv_processes.h"
#include "sv_files.h"
#include "sv_verifs.h"
#include "../common/utils.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <filesystem>
#include <cstring>


using namespace std;

string sv_login_process(string uid, string pass){
    string msg, saved_pass;

    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if (is_registered(uid)){
        if (authenticated(uid, pass)){
            msg = "RLI OK\n";
            createLogin(uid);
        }
        else{
            msg = "RLI NOK\n";
        }
    }
    else{
        msg = "RLI REG\n";
        createUser(uid, pass);
        createLogin(uid);
    }
    return msg;
}


string sv_logout_process(string uid, string pass){
    string msg;
    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if(!is_registered(uid)) msg = "RLO UNR\n";
    else if(!is_logged(uid) || !authenticated(uid, pass)) msg = "RLO NOK\n";
    else {
        msg = "RLO OK\n";
        eraseLogin(uid);
    }
    return msg;
}

string sv_unregister_process(string uid, string pass){
    string msg;
    if (!valid_uid(uid) || !valid_password(pass)) msg = "ERR\n";
    else if(!is_registered(uid)) msg = "RUR UNR\n";
    else if(!is_logged(uid) || !authenticated(uid, pass)) msg = "RUR NOK\n";
    else {
        msg = "RUR OK\n";
        eraseUser(uid);
    }
    return msg;
}

string sv_myauctions_process(string uid){
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
    return msg;
}

string sv_mybids_process(string uid){
    string msg , state;
    vector<string> aid_list, empty;
    if (!valid_uid(uid)) msg = "ERR";
    else if (!is_logged(uid)) msg = "RMB NLG";
    else if((aid_list = get_bidded(uid)).size() == 0) msg = "RMB NOK";
    else{
        msg = "RMB OK";
        for(vector<string>::iterator it=aid_list.begin(); it != aid_list.end(); ++it){
            if(!ended(*it)) state = "1";
            else state = "0";
            msg += " "+ *it + " " + state;
        }
    }
    msg += "\n";
    return msg;
}

string sv_list_process(string port, string ip){
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
    return msg;
    }

string sv_show_record_process(string aid){
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
    return msg;
}



//input is everything excluding fsize and data (might change to auction info obj)
string sv_open_process(char* received,int size, string port, string ip, int socket_fd, bool verbose){
    string trash = "",msg, aid, uid="", pass="", name="", start_value ="", timeactive ="", asset_fname ="", info;
    size_t bytes_read = 0, fsize = 0;

    // Parse the response
    istringstream iss;
    do{
        // Reset bytes_read
        bytes_read = 0;

        // Reset the istringstream
        iss.clear();
        iss.str(received);
        
        if(trash == ""){
          iss >> trash;
          bytes_read += trash.size() + BLANK_SPACE;
        }


        if(uid == ""){
          iss >> uid;
          // In case the file_name wasn't yet received
          if(uid == "")
            // Read again
            continue;
          if(verbose) cout << "User "+uid+" @ address "+ip+":"+port+" made a OPA request\n";
          bytes_read += uid.size() + BLANK_SPACE;
        }

        if(pass == ""){
          iss >> pass;
          // In case the file_name wasn't yet received
          if(pass == "")
            // Read again
            continue;

          bytes_read += pass.size() + BLANK_SPACE;
        }

        if(name == ""){
          iss >> name;
          // In case the file_name wasn't yet received
          if(name == "")
            // Read again
            continue;

          bytes_read += name.size() + BLANK_SPACE;
        }

        if(start_value == ""){
          iss >> start_value;
          // In case the file_name wasn't yet received
          if(uid == "")
            // Read again
            continue;

          bytes_read += start_value.size() + BLANK_SPACE;
        }

        if(timeactive == ""){
          iss >> timeactive;
          // In case the file_name wasn't yet received
          if(timeactive == "")
            // Read again
            continue;

        bytes_read += timeactive.size() + BLANK_SPACE;
        }

        if(asset_fname == ""){
          iss >> asset_fname;
          // In case the file_name wasn't yet received
          if(asset_fname == "")
            // Read again
            continue;

          bytes_read += asset_fname.size() + BLANK_SPACE;
        }
        
        if(fsize == 0){
          iss >> fsize;
          // In case the file_name wasn't yet received
          if(fsize == 0)
            // Read again
            continue;

          bytes_read += to_string(fsize).size() + BLANK_SPACE;
          break;
        }
        
    }
    while((size = receive_message_tcp(socket_fd, received)) != 0);

    if (!valid_uid(uid) || !valid_password(pass) || !valid_auction_name(name) || !valid_start_value(start_value) || !valid_duration(timeactive)) msg = "ERR";
    else if (!is_logged(uid)) msg = "ROA NLG";
    else if (!authenticated(uid, pass)) msg = "ROA NOK";    
    else{

        aid = get_unique_aid();
        FILE *fp;

        info = uid+" "+name+" "+asset_fname+" "+start_value+" "+timeactive+ " " + get_current_time();
        if (aid == "1000" || (fp = createAuction(aid, asset_fname,info)) == NULL) msg = "ROA NOK";
        else {
            size_t total = size - bytes_read;
            hostAuction(aid, uid);          //verificar password
            fwrite(received + bytes_read, sizeof(char),size - bytes_read,fp);
            while(total < fsize){
                size = receive_message_tcp(socket_fd, received);
                total += size;
                if(total > fsize) size -= total - fsize;
                fwrite(received, sizeof(char),size,fp);
            }
            fclose(fp);           
            msg = "ROA OK "+aid;
        }
    }
    msg += "\n";
    return msg;
}



string sv_close_process(string uid, string pass, string aid){
    string msg, end_info;
    if (!valid_uid(uid) || !valid_password(pass) || !valid_aid(aid)) msg = "ERR";
    if (!is_logged(uid) || !authenticated(uid, pass)) msg = "RCL NLG";
    else if (!exists(aid)) msg = "RCL EAU";
    else if (!is_owner(uid,aid)) msg = "RCL EOW";    
    else if (ended(aid)) msg = "RCL END";
    else {
        msg = "RCL OK";
        end_info = get_elapsed_time(aid);
        endAuction(aid, end_info);
    }
    msg += "\n";
    return msg;
}

string sv_bid_process(string uid, string pass, string aid, string bid){
    string bid_datetime, bid_sec_time, bid_info;
    string status = validateBid(aid, uid,pass, bid);    
    string msg = "RBD " + status;
    if (status == "ERR") msg = "ERR";
    if (status == "ACC"){
        
        bid_info = uid+" "+bid+" "+get_elapsed_time(aid);
        makeBid(aid, uid, bid, bid_info);
    }
    msg += "\n";
    return msg;
}

string sv_show_asset(char* input, string port, string ip, int socket_fd, bool verbose){
    string msg, trash, aid, filename, asset_fname;
    istringstream iss(input);
    iss >> trash >> aid;
    if (!valid_aid(aid)) msg = "ERR";
    else if (!exists(aid)) msg = "RRC NOK";
    else {
        msg = "RSA OK";
        filename = "AUCTIONS/"+aid+"/START_"+aid+".txt";
        ifstream ifs(filename, ifstream::in);
        ifs >> trash >> trash >> asset_fname;
        ifs.close();
        filename = "AUCTIONS/"+aid+"/"+asset_fname;
        FILE *file = fopen(&filename[0], "rb");
        long int fsize = get_file_size(file);
        cout << fsize << endl;  
        msg += " " + asset_fname + " " + to_string(fsize) + " ";
        
        send_message_tcp(socket_fd,msg.c_str(),msg.size());
        int total = 0, size;
        // Read/Send the file to the server
        while(total != fsize){
            memset(input, '\0', BUFFER_SIZE);
            if((size = fread(input, sizeof(char),BUFFER_SIZE,file)) == -1) exit(1);
            total += size;
            send_message_tcp(socket_fd,input,size);
            
        }
        msg = "";
    }
    msg += "\n";
    
    if(verbose) cout << "User unknown @ address "+ip+":"+port+" made a LMB request\n";
    return msg;
}
