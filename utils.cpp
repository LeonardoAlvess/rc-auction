#include <string>
#include <algorithm>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>  
#include <stdio.h>
#include <cstring>
#include "utils.h"
using namespace std;        
using namespace std::filesystem;       

int getCommandType(const string& command) {
    if (command == "login") {
        return LOGIN;
    } else if (command == "logout") {
        return LOGOUT;
    } else if (command == "unregister") {
        return UNREGISTER;
    } else if (command == "exit") {
        return EXIT;
    } else if (command == "open") {
        return OPEN_AUCTION;
    } else if (command == "close") {
        return CLOSE_AUCTION;
    } else if (command == "myauctions" || command == "ma") {
        return MY_AUCTIONS;
    } else if (command == "mybids" || command == "mb") {
        return MY_BIDS;
    } else if (command == "list" || command == "l") {
        return LIST;
    } else if (command == "show_asset" || command == "sa") {
        return SHOW_ASSET;
    } else if (command == "bid" || command == "b") {
        return BID;
    } else if (command == "show_record" || command == "sr") {
        return SHOW_RECORD;                          
    } else {
        return UNKNOWN_COMMAND;
    }
}

bool valid_N_args(int code, int n_args){                                                                           
  int good_n_ars[13] = {3,1,1,1,5,2,1,1,1,2,3,2,0};
  if (code == 12) return true;
  else return good_n_ars[code]==n_args;
}

bool valid_uid(string uid) {return uid.size() == 6 && all_of(uid.begin(), uid.end(), ::isdigit);}

bool valid_aid(string aid) {return aid.size() == 3 && all_of(aid.begin(), aid.end(), ::isdigit);}

bool valid_password(string pass) { return pass.size() == 8 && all_of(pass.begin(), pass.end(), ::isalnum);}

bool valid_auction_name(string name) { return name.size() <= 10 && all_of(name.begin(), name.end(), ::isalnum);}

bool valid_start_value(string value) { return value.size() <= 6 && all_of(value.begin(), value.end(), ::isdigit);}

bool valid_duration(string value) { return value.size() <= 5 && all_of(value.begin(), value.end(), ::isdigit);}

bool valid_filename(string filename){
  if (filename.size() > 10) return false;

  for( char c : filename){
    if (c != '_' && c != '-' && c != '.' && !isdigit(c) && !isalnum(c)) return false;
  }
  return true;
}

bool valid_filesize(string filesize){ return filesize.size() <= 8;}

bool valid_bid(string bid) { return bid.size() <= 5 && all_of(bid.begin(), bid.end(), ::isdigit);}

string send_message_udp(string port, string ip,string message){
  int fd; 
  ssize_t size;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[6015];                               
  memset(buffer,'\0',sizeof(buffer));             
  if ((fd=socket(AF_INET,SOCK_DGRAM,0)) == -1) exit(1); //socket
  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=SOCK_DGRAM; //udp socket
  if(getaddrinfo(&ip[0],&port[0],&hints,&res) == -1) exit(1);
  if(sendto(fd,&message[0],message.size(),0,res->ai_addr,res->ai_addrlen) == -1) exit(1);
  addrlen=sizeof(addr);
  if((size=recvfrom(fd,buffer,6015,0,(struct sockaddr*)&addr,&addrlen)) == -1) exit(1);     

  freeaddrinfo(res);
  close(fd);
  return buffer;
}

string send_single_message_tcp(string port, string ip, string message, int size){
    int fd;
    struct addrinfo *res;
    res = connect_tcp(&fd,port,ip);
    send_message_tcp(fd,message,size);
    message = receive_message_tcp(fd);
    end_tcp(fd,res);
    return message;
}

struct addrinfo* connect_tcp(int* fd, string port,string ip){
    struct addrinfo hints,*res;                           
               
    if ((*fd=socket(AF_INET,SOCK_STREAM,0)) == -1) exit(1); //socket
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //udp socket
    if(getaddrinfo(&ip[0],&port[0],&hints,&res) == -1) exit(1);
    if(connect(*fd,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    return res;
}

void send_message_tcp(int fd, string message, int size){
    if(write(fd,&message[0], size) == -1) exit(1);
}

string receive_message_tcp(int fd){
    char buffer[2000]; 
    memset(buffer,'\0',sizeof(buffer));  
    if(read(fd,buffer,2000) == -1) exit(1);
    return buffer;
}

void end_tcp(int fd,struct addrinfo *res){
    freeaddrinfo(res);
    close(fd);
}