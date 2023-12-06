#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/socket.h>

#include "utils.h"
#include "processes.h"

using namespace std;
using namespace std::filesystem;

void login_process(vector<string> args, string& log_uid, string& log_pass){

  string uid = args[1];
  string password = args[2];
  string received;

  if(!valid_uid(uid)){
    cout << "ERROR: INVALID UID\n";
    return;
  }

  if(!valid_password(password)){
    cout << "ERROR: INVALID PASSWORD\n";
    return;
  }
  
  received = send_message(UDP_SOCKET,"LIN " + uid + " " + password + "\n");

  if(received == "RLI NOK\n"){            //maybe change
    cout << "Login Failed\n";
    return;
  }
  else if (received == "RLI OK\n"){
    cout << "Login Successful\n";
  }
  else cout << "User registered\n";

  log_uid = uid;                          
  log_pass = password;                    
}

void logout_process(string& log_uid, string& log_pass){
  string received = send_message(UDP_SOCKET,"LOU " + log_uid + " " + log_pass + "\n");
  if(received == "RLO NOK\n"){            //maybe change
    cout << "Logout Failed\n";
    return;
  } else cout << "Logout Successful\n";
  
  log_uid = "";
  log_pass = "";
}

void unregister_process(string& log_uid, string& log_pass){
  string received = send_message(UDP_SOCKET,"UNR " + log_uid + " " + log_pass + "\n");
  if(received == "RUR NOK\n"){            //maybe change
    cout << "Unregistered Failed\n";
    return;
  } else cout << "User " + log_uid + "unregistered\n";
  log_uid = "";
  log_pass = "";
}

void open_auction_process(vector<string> args, string& uid, string& pass){
  if(!valid_auction_name(args[1])){
    cout << "ERROR: INVALID AUCTION NAME\n";
    return;
  }

  if(!valid_filename(args[2])){
    cout << "ERROR: INVALID FILENAME\n";
    return;
  }
  
  if(!valid_start_value(args[3])){
    cout << "ERROR: INVALID START VALUE\n";
    return;
  }
  
  if(!valid_duration(args[4])){
    cout << "ERROR: INVALID DURATION\n";
    return;
  }
  
  FILE* file = fopen(&args[2][0], "rb");
  if(!file){
    cout << "ERROR: INVALID FILE\n";
    return;
  }  

  uintmax_t size = file_size(args[2]);    //to string quando for para enviar no tcp
  if (!valid_filesize(to_string(size))){
    cout << "ERROR: FILE TOO BIG\n";
    fclose(file);
    return;
  }
  
  char *fdata = (char*) malloc(size * sizeof(char) + 1);
  fread(fdata, sizeof(char),size, file);
  fclose(file);
  fdata[size] = '\0';

  printf("%s", fdata);
  
  string aux, received;
  received = send_message(TCP_SOCKET,"OPA " + uid + " " + pass + " " + args[1] + " "
     + args[3] + " " + args[4] + " "  + args[2] + " "  + to_string(size) + " " + fdata + " " + "\n");
  
  istringstream iss(received);
  iss >> aux;
  if (aux != "ROA"){
    cout << "Unexpected server response\n";
    return;
  }
  iss >> aux;
  if (aux == "NOK") cout << "Auction couldn't be started\n";
  else if (aux == "NLG") cout << "No user logged in\n";
  else if (aux == "OK"){
    iss >> aux;
    cout << "Auction " + aux + " was started\n";
  }
  free(fdata);
  
}

void close_auction_process(vector<string> args, string& uid, string& pass){
  string aid = args[1];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  string aux, received = send_message(TCP_SOCKET,"CLS " + uid + " " + pass + " " + aid + "\n");
  istringstream iss(received);
  iss >> aux;
  if (aux != "RCL"){
    cout << "Unexpected server response\n";
    return;
  }
  iss >> aux;
  if (aux == "NOK\n") cout << "There was an error closing the auction\n";
  else if (aux == "NLG\n") cout << "No user logged in\n";
  else if (aux == "EAU\n") cout << "The auction " + aid + " doesn't exist\n";
  else if (aux == "EOW\n") cout << "This auction isnt' owned by " + uid +"\n";
  else if (aux == "END\n") cout << "This auctios has already finished\n";
  else if (aux == "OK\n") cout << "The auction " + aid + " was closed\n";
  cout << "CLOSED AUCTION "+aid+"\n";
}

void my_auctions_process(string uid){
  string received = send_message(UDP_SOCKET,"LMA " + uid + "\n");
  if(received == "RMA NOK\n") 
    cout << "The user " + uid + " doesn't have any ongoing auctions\n";
  else{
    string aid,state;                     
    istringstream iss(received);
    iss >> aid;
    iss >> aid;
    while(iss >> aid){
      iss >> state;
      if(state == "1") state = "Active";
      else state = "Closed";
      cout << aid + ": " + state + "\n";
    }
  }
}

void my_bids_process(string uid){
  string received = send_message(UDP_SOCKET,"LMB " + uid + "\n");
  if(received == "RMB NOK\n") cout << "The user " + uid + " doesn't have any ongoing bids\n";
  else{
    string aid,state;                     
    istringstream iss(received);
    iss >> aid;
    iss >> aid;
    while(iss >> aid){
      iss >> state;
      if(state == "1") state = "Active";
      else state = "Closed";
      cout << aid + ": " + state + "\n";
    }
  }
}

void list_process(){
  string received = send_message(UDP_SOCKET,"LST\n");
  if(received == "RMB NOK\n") cout << "No auction was yet started\n";
  else{
    string aid,state;                     
    istringstream iss(received);
    iss >> aid;
    iss >> aid;
    while(iss >> aid){
      iss >> state;
      if(state == "1") state = "Active";
      else state = "Closed";
      cout << aid + ": " + state + "\n";
    }
  }
}

void show_asset_process(string aid){
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  string code, status, file_name, file_size, file_data;
  string received = send_message(TCP_SOCKET, "SAS "+ aid +"\n");
  istringstream iss(received);
  iss >> code;
  if (code != "RSA"){
    cout << "ERROR: UNEXPECTED SERVER RESPONSE\n";
    return;
  }
  iss >> status;
  if(status == "NOK"){
    cout << "ERROR: NO SUCH FILE TO BE SENT";
    return;
  }
  iss >> file_name >> file_size >> file_data;
  //store file

  cout << "test\n";
  cout << file_name << " " << file_size << " " << file_data << "\n";
  int size = atoi(&file_size[0]);
  FILE* fd = fopen(&file_name[0], "w");
  fwrite(&file_data[0], sizeof(char), size, fd);
  cout << "File +"+file_name+" was stored with "+file_size+" size\n";
  fclose(fd);
  
}

void bid_process(vector<string> args, string uid, string pass){
  string aid = args[1];
  string bid = args[2];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }

  if (!valid_bid(bid)){
    cout << "ERROR: INVALID BID\n";
    return;
  }
  
  string aux,received = send_message(TCP_SOCKET,"BID " + uid + " " + pass + " " + aid + " " + bid +"\n");
  istringstream iss(received);
  iss >> aux;
  if (aux != "RBD"){
    cout << "Unexpected server response\n";
    return;
  }
  iss >> aux;
  if (aux == "NOK") cout << "Auction not active\n";
  else if (aux == "NLG") cout << "No user logged in\n";
  else if (aux == "ACC") cout << "The bid was accepted\n";
  else if (aux == "REF") cout << "A larger bid was placed previously\n";
  else if (aux == "ILG") cout << "Can't bid on your own auction\n";
  
}

void show_record_process(string aid){
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  string host_UID, auction_name, asset_fname, start_value, start_date, start_time, timeactive;
  string bidder_UID, bid_value, bid_date, bid_time, bid_sec_time;
  string end_date, end_time, end_sec_time;
  string aux,received = send_message(UDP_SOCKET,"SRC " + aid + "\n");
  istringstream iss(received);

  if(received == "RRC NOK\n")
    cout << "There was an error displaying the record\n";

  else {
    cout << received;
    iss >> aux >> aux;      //saltar code and status
    iss >> host_UID >> auction_name >> asset_fname >> start_value >> start_date >> start_time >> timeactive;

    cout << "Auction "+auction_name+" hosted by user "+host_UID+"\n"
           "File name: "+asset_fname+"\n"
           "Starting value: "+start_value+"\n"
           "Starting time "+start_date+" "+start_time+"\n"
           "Auction duration: "+timeactive+"\n";
    cout << "---------------------------------------------------\n"
            "                   BID HISTORY                     \n"
            "---------------------------------------------------\n";

    while(iss >> aux){
      if (aux == "B"){
        iss >> bidder_UID >> bid_value >> bid_date >> bid_time >> bid_sec_time;

        cout << bid_date+" "+bid_time+" : User "+bidder_UID+" bid "+bid_value+" credits \n"+
                "                      Time since start: "+bid_sec_time+" seconds\n"
                "---------------------------------------------------\n";
      }
      else if (aux == "E"){
        
        iss >> end_date >> end_time >> end_sec_time;
        cout << "Auction ended at "+end_date+" "+end_time+"\n"
                +end_sec_time+"seconds since start\n";
      }
      else break;
    }    
  }
  
  
}