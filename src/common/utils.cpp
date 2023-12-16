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
    /**
     * @brief Returns the command type based on the string passed as argument
     * @param command string to be compared
     * 
     */
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

int getProtocolType(string protocol){
    /**
     * @brief Returns the protocol type based on the string passed as argument
     * @param protocol string to be compared
     *
    */ 
    if (protocol == "LIN") {
        return LOGIN;
    } else if (protocol == "LOU") {
        return LOGOUT;
    } else if (protocol == "UNR") {
        return UNREGISTER;
    } else if (protocol == "OPA") {
        return OPEN_AUCTION;
    } else if (protocol == "CLS") {
        return CLOSE_AUCTION;
    } else if (protocol == "LMA") {
        return MY_AUCTIONS;
    } else if (protocol == "LMB") {
        return MY_BIDS;
    } else if (protocol == "LST") {
        return LIST;
    } else if (protocol == "SAS") {
        return SHOW_ASSET;
    } else if (protocol == "BID") {
        return BID;
    } else if (protocol == "SRC") {
        return SHOW_RECORD;                          
    } else {
        return UNKNOWN_COMMAND;
    }
}

bool valid_N_args(int code, int n_args){
    /**
     * @brief Returns true if the number of arguments is valid for the command type
     * @param code command type
     * @param n_args number of arguments
     * 
    */
    int good_n_ars[13] = {3,1,1,1,5,2,1,1,1,2,3,2,0};
    if (code == 12) return true;
    else return good_n_ars[code]==n_args;
}

/**
 * @brief Validates uid
 * @param uid 
 * @return true if uid is valid
 * @return false otherwise
*/
bool valid_uid(string uid) {return uid.size() == 6 && all_of(uid.begin(), uid.end(), ::isdigit);}

/**
 * @brief Validates aid
 * @param aid 
 * @return true if aid is valid
 * @return false otherwise
*/
bool valid_aid(string aid) {return aid.size() == 3 && all_of(aid.begin(), aid.end(), ::isdigit);}

/**
 * @brief Validates password
 * @param pass 
 * @return true if password is valid
 * @return false otherwise
*/
bool valid_password(string pass) { return pass.size() == 8 && all_of(pass.begin(), pass.end(), ::isalnum);}


bool valid_auction_name(string name) {
    /**
     * @brief Validates auction name
     * @param name
     * @return true if auction name is valid
     * @return false otherwise
    */
    if (name.size() > 10) return false;

    for( char c : name){
        if (c != '_' && c != '-' && !isdigit(c) && !isalnum(c)) return false;
    }
    return true;
}

/**
 * @brief Validates the auction start value
 * @param value 
 * @return true if start value is valid
 * @return false otherwise
*/
bool valid_start_value(string value) { return value.size() <= 6 && all_of(value.begin(), value.end(), ::isdigit);}

/**
 * @brief Validates the auction duration
 * @param value 
 * @return true if duration is valid
 * @return false otherwise
*/
bool valid_duration(string value) { return value.size() <= 5 && all_of(value.begin(), value.end(), ::isdigit);}


bool valid_filename(string filename){
    /**
     * @brief Validates filename
     * @param filename
     * @return true if filename is valid
     * @return false otherwise
    */
    if (filename.size() > 24) return false;

    for( char c : filename){
        if (c != '_' && c != '-' && c != '.' && !isdigit(c) && !isalnum(c)) return false;
    }
    return true;
}

/**
 * @brief Validates filesize
 * @param filesize 
 * @return true if filesize is valid
 * @return false otherwise
*/
bool valid_filesize(string filesize){ return filesize.size() <= 8;}

/**
 * @brief Validates bid
 * @param bid 
 * @return true if bid is valid
 * @return false otherwise
*/
bool valid_bid(string bid) {return bid.size() <= 6 && all_of(bid.begin(), bid.end(), ::isdigit);}


string send_message_udp(string port, string ip,string message){
    /**
     * @brief Sends a message through udp and returns the response
     * @param port port to connect
     * @param ip ip to connect
     * @param message message to send
     * @param mode mode of the call
     * @return string response from server
    */
    int fd; 
    ssize_t size;
    socklen_t addrlen;
    struct addrinfo hints,*res;
    struct sockaddr_in addr;
    char buffer[6015];                               
    memset(buffer,'\0',sizeof(buffer));

    // Create UDP socket             
    if ((fd=socket(AF_INET,SOCK_DGRAM,0)) == -1) exit(1); //socket

    // Set hints
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_DGRAM; //udp socket

    // Get hostname
    if(getaddrinfo(&ip[0],&port[0],&hints,&res) == -1) exit(1);
    
    // Send message
    if(sendto(fd,&message[0],message.size(),0,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    addrlen=sizeof(addr);

    // Receive message in user mode
    if((size=recvfrom(fd,buffer,6015,0,(struct sockaddr*)&addr,&addrlen)) == -1) exit(1);     
    
    // End connection
    freeaddrinfo(res);
    close(fd);
    
    return buffer;
}

string send_single_message_tcp(string port, string ip, string message){
    /**
     * @brief Sends a single message through tcp and returns the response,
     * opening and closing the connection in the process
     * @param port port to connect
     * @param ip ip to connect
     * @param message message to send
     * @return string response from server
    */
    int fd;
    struct addrinfo *res;
    res = connect_tcp(&fd,port,ip);    
    send_message_tcp(fd,message);
    message = receive_message_tcp(fd);
    end_tcp(fd,res);
    return message;
}

struct addrinfo* connect_tcp(int* fd, string port,string ip){
    /**
     * @brief Establishes a tcp connection
     * @param fd file descriptor
     * @param port port to connect
     * @param ip ip to connect
     * @return struct addrinfo* res
    */
    struct addrinfo hints,*res;      

    // Create TCP socket                        
    if ((*fd=socket(AF_INET,SOCK_STREAM,0)) == -1) exit(1); //socket

    // Set hints
    memset(&hints,0,sizeof hints);
    hints.ai_family=AF_INET; //IPv4
    hints.ai_socktype=SOCK_STREAM; //udp socket

    // Get hostname
    if(getaddrinfo(&ip[0],&port[0],&hints,&res) == -1) exit(1);

    // Connect
    if(connect(*fd,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    return res;
}

void send_message_tcp(int fd, string message){
    /**
     * @brief sends a message through tcp
     * @param fd file descriptor
     * @param message message to send
    */
    if(write(fd,&message[0], message.size()) == -1) exit(1);
}

void send_message_tcp(int fd,const char* message, int size){
    /**
     * @brief sends a message through tcp
     * @param fd file descriptor
     * @param message message to send
     * @param size size of the message
    */
    if(write(fd,message, size) == -1) exit(1);
}

string receive_message_tcp(int fd){
    /**
     * @brief receives a message through tcp
     * @param fd file descriptor
     * @return string message received
    */
    char buffer[BUFFER_SIZE]; 
    memset(buffer,'\0',sizeof(buffer));  
    if(read(fd,buffer,BUFFER_SIZE) == -1) exit(1);
    return buffer;
}

int receive_message_tcp(int fd, char* buffer){
    /**
     * @brief receives a message through tcp
     * @param fd file descriptor
     * @param buffer buffer to store the message
     * @return size read
    */
    int size;
    memset(buffer,'\0', BUFFER_SIZE);  
    if((size = read(fd,buffer,BUFFER_SIZE)) == -1) exit(1);
    return size;
}



void end_tcp(int fd,struct addrinfo *res){
    /**
     * @brief closes a tcp connection
     * @param fd file descriptor
     * @param res struct addrinfo* res
    */
    freeaddrinfo(res);
    close(fd);
}

int get_file_size( FILE* fd){
    long int size;
    fseek(fd,0,SEEK_END);
    size = ftell(fd);
    fseek(fd,0, SEEK_SET);
    return size; 
}