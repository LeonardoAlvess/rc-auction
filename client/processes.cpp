#include <sstream>
#include <cctype>
#include <algorithm>
#include <fstream>
#include <iostream>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/socket.h>
#include <filesystem>
#include <fcntl.h>
#include <cstring>
#include "processes.h"

#define BLANK_SPACE 1
using namespace std;  

void login_process(string port, string ip, vector<string> args, string& log_uid, string& log_pass){

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
  
  received = send_message_udp(port, ip, "LIN " + uid + " " + password + "\n");

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

void logout_process(string port, string ip, string& log_uid, string& log_pass){
  string received = send_message_udp(port, ip, "LOU " + log_uid + " " + log_pass + "\n");
  if(received == "RLO NOK\n"){            //maybe change
    cout << "Logout Failed\n";
    return;
  } else cout << "Logout Successful\n";
  
  log_uid = "";
  log_pass = "";
}

void unregister_process(string port, string ip, string& log_uid, string& log_pass){
  string received = send_message_udp(port, ip, "UNR " + log_uid + " " + log_pass + "\n");
  if(received == "RUR NOK\n"){            //maybe change
    cout << "Unregistered Failed\n";
    return;
  } else cout << "User " + log_uid + "unregistered\n";
  log_uid = "";
  log_pass = "";
}

void open_auction_process(string port, string ip, vector<string> args, string& uid, string& pass){
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
  
  FILE *file = fopen(&args[2][0], "rb");
  if(!file){
    cout << "ERROR: INVALID FILE\n";
    return;
  }  

  uintmax_t total = 0, fsize = filesystem::file_size(args[2]);    
  if (!valid_filesize(to_string(fsize))){
    cout << "ERROR: FILE TOO BIG\n";
    fclose(file);
    return;
  }
  
  char buffer[512];
  string received,aux;
  int fd,size;
  struct addrinfo *res;
  aux ="OPA " + uid + " " + pass + " " + args[1] + " "
     + args[3] + " " + args[4] + " "  + args[2] + " "  + to_string(fsize) + " ";
  
  res = connect_tcp(&fd,port,ip);
  send_message_tcp(fd,aux.c_str(),aux.size());
  while(total != fsize){
    if((size = fread(buffer, sizeof(char),512,file)) == -1) exit(1);
    total += size;
    send_message_tcp(fd,buffer,size);
  }

  send_message_tcp(fd,"\n",1);
  received = receive_message_tcp(fd);
  end_tcp(fd,res);   
  fclose(file);

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
  
}

void close_auction_process(string port, string ip, vector<string> args, string& uid, string& pass){
  string aid = args[1];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  string received, aux;
  received = send_single_message_tcp(port, ip, "CLS " + uid + " " + pass + " " + aid + "\n");
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

void my_auctions_process(string port, string ip, string uid){
  string received = send_message_udp(port, ip, "LMA " + uid + "\n");
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

void my_bids_process(string port, string ip, string uid){
  string received = send_message_udp(port, ip, "LMB " + uid + "\n");
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

void list_process(string port, string ip){
  string received = send_message_udp(port, ip, "LST\n");
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

void show_asset_process(string port, string ip, string aid){
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }

  string code, status, file_name;
  string  aux= "SAS "+ aid +"\n";  
  char received[512];
  int fd,size,file_size,bytes_read;
  struct addrinfo *res;


  res = connect_tcp(&fd,port,ip);
  send_message_tcp(fd,aux.c_str(),aux.size());
  memset(received,'\0',sizeof(received));  
  if((size = read(fd,received,512)) == -1) exit(1);
  istringstream iss(received);
  iss >> code >> status;

  if (code != "RSA"){
      cout << "ERROR: UNEXPECTED SERVER RESPONSE\n";
      return;
  }
  if(status == "NOK"){
    cout << "ERROR: NO SUCH FILE TO BE SENT";
    return;
  }
  bytes_read = code.size() + status.size() + BLANK_SPACE*2;

  if(size<10){
    bytes_read = 0;
    if((size = read(fd,received,512)) == -1) exit(1);
    iss.clear();
    iss.str(received);
  }
  iss >> file_name >> file_size;
  bytes_read += file_name.size() + to_string(file_size).size() + BLANK_SPACE*2;

  file_size -= (size - bytes_read);
  FILE *file = fopen(file_name.c_str(), "wb");
  fwrite(received + bytes_read, sizeof(char), size - bytes_read, file);

  while(file_size > 0){
    memset(received,'\0',sizeof(received));  
    if((size = read(fd,received,512)) == -1) exit(1);
    file_size -= size;
    if(file_size < 0) break;                                  //funciona mas deve haver maneira melhor
    fwrite(received, sizeof(char), size, file);
  }
  fwrite(received, sizeof(char), size + file_size, file);     //write sem \n

  end_tcp(fd,res);   
  fclose(file);
  
  cout << "File " + file_name + " received\n";
}

void bid_process(string port, string ip, vector<string> args, string uid, string pass){
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
  
  string received, aux;
  received = send_single_message_tcp(port, ip, "BID " + uid + " " + pass + " " + aid + " " + bid +"\n");
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

void show_record_process(string port, string ip, string aid){
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  string host_UID, auction_name, asset_fname, start_value, start_date, start_time, timeactive;
  string bidder_UID, bid_value, bid_date, bid_time, bid_sec_time;
  string end_date, end_time, end_sec_time;
  string aux,received = send_message_udp(port, ip, "SRC " + aid + "\n");
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