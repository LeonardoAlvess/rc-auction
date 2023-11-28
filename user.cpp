#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <cstring>
using namespace std;
using namespace std::filesystem;

#define FALSE 0
#define TRUE 1
#define SUCCESS 1
#define FAILURE 0
#define PORT "58011"              

enum CommandType {
    LOGIN,
    LOGOUT,
    UNREGISTER,
    EXIT,
    OPEN_AUCTION,
    CLOSE_AUCTION,
    MY_AUCTIONS,  
    MY_BIDS,       
    LIST,         
    SHOW_ASSET,
    BID,
    SHOW_RECORD,  
    UNKNOWN_COMMAND
};

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

//------------------------------------------------------ AUX ------------------------------------------------------//
//passar para um ficheiro auxiliar

int valid_N_args(int code, int n_args){                                                                           
  int good_n_ars[13] = {3,1,1,1,5,2,1,1,1,2,3,2,0};
  if (code == 12) return TRUE;
  else return good_n_ars[code]==n_args;
}

int valid_uid(string uid) {return uid.size() == 6 && all_of(uid.begin(), uid.end(), ::isdigit);}

int valid_aid(string aid) {return aid.size() == 3 && all_of(aid.begin(), aid.end(), ::isdigit);}

int valid_password(string pass) { return pass.size() == 8 && all_of(pass.begin(), pass.end(), ::isalnum);}

int valid_auction_name(string name) { return name.size() <= 10 && all_of(name.begin(), name.end(), ::isalnum);}

int valid_start_value(string value) { return value.size() <= 6 && all_of(value.begin(), value.end(), ::isdigit);}

int valid_duration(string value) { return value.size() <= 5 && all_of(value.begin(), value.end(), ::isdigit);}

string send_message(int type, string message){
  int fd,errcode; 
  ssize_t size;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[128];                               
  memset(buffer,'\0',sizeof(buffer));             
  if ((fd=socket(AF_INET,type,0)) == -1) exit(1); //socket

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=type; //socket type
  if(getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res) == -1) exit(1);

  if(type==SOCK_DGRAM){
    if(sendto(fd,&message[0],message.size(),0,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    addrlen=sizeof(addr);
    if((size=recvfrom(fd,buffer,128,0,(struct sockaddr*)&addr,&addrlen)) == -1) exit(1);          //adicionar while para textos maior que o buffer
  }
  else{
    if(connect(fd,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    if(write(fd,&message[0],message.size()) == -1) exit(1);
    if(size=read(fd,buffer,128) == -1) exit(1);
  }
  freeaddrinfo(res);
  close(fd);
  return buffer;
}


//------------------------------------------------- MAIN FUNCTIONS -------------------------------------------------//


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
  
  received = send_message(SOCK_DGRAM,"LIN " + uid + " " + password + "\n");

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
  string received = send_message(SOCK_DGRAM,"LOU " + log_uid + " " + log_pass + "\n");
  if(received == "RLO NOK\n"){            //maybe change
    cout << "Logout Failed\n";
    return;
  } else cout << "Logout Successful\n";
  
  log_uid = "";
  log_pass = "";
}

void unregister_process(string& log_uid, string& log_pass){
  string received = send_message(SOCK_DGRAM,"UNR " + log_uid + " " + log_pass + "\n");
  if(received == "RUR NOK\n"){            //maybe change
    cout << "Unregistered Failed\n";
    return;
  } else cout << "User " + log_uid + "unregistered\n";
  log_uid = "";
  log_pass = "";
}

void open_auction_process(vector<string> args, string& log_uid, string& log_pass){
  if(!valid_auction_name(args[1])){
    cout << "ERROR: INVALID AUCTION NAME\n";
    return;
  }

  ifstream file(args[2]);   //testar bem
  if(!file.is_open()){
    cout << "ERROR: INVALID FILE\n";
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

  string fdata;                           //testar bem  
  uintmax_t size = file_size(args[2]);    //to string quando for para enviar no tcp
  file.read(&fdata[0], size);

  //sendto etc etc
  file.close();

  cout << "AUCTION OPENED\n";
  
}

void close_auction_process(vector<string> args, string& log_uid, string& log_pass){
  string aid = args[1];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  //sendto etc etc
  cout << "CLOSED AUCTION "+aid+"\n";
}

void my_auctions_process(string uid){
  string received = send_message(SOCK_DGRAM,"LMA " + uid + "\n");
  if(received == "RMA NOK\n") 
    cout << "The user " + uid + " doesn't have any ongoing auctions\n";
  else{
    string aid,state;                     //código mais half-assed de sempre
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
  string received = send_message(SOCK_DGRAM,"LMB " + uid + "\n");
  if(received == "RMB NOK\n") cout << "The user " + uid + " doesn't have any ongoing bids\n";
  else{
    string aid,state;                     //código mais half-assed de sempre
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
  string received = send_message(SOCK_DGRAM,"LST\n");
  if(received == "RMB NOK\n") cout << "No auction was yet started\n";
  else{
    string aid,state;                     //código mais half-assed de sempre
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

  //tcp conn  
}

void bid_process(vector<string> args, string log_uid, string log_pass){
  string aid = args[1];
  string bid = args[2];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }

  if (!all_of(bid.begin(), bid.end(), ::isdigit)){
    cout << "ERROR: INVALID BID\n";
    return;
  }

  //tcp conn
}

void show_record(string aid){
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }
  
  string received = send_message(SOCK_DGRAM,"SRC " + aid + "\n");
  cout << received << "\n";
}

int main(int argc, char *argv[]){ //adicionar args e processar
    string token,line,uid,pass;
    vector<string> args;
    istringstream iss;
    int stay = 1;

    while(stay){

      //get the command input and process it
      cout << "typecommand: ";
      getline(cin,line);        //getting line string from input
      iss.clear();              //resetting line stream
      iss.str(line);            //turning line into a stream

      args.clear();             //resseting argument vector
      while(iss >> token){      
        args.push_back(token);  //reading from line stream and filling argument vector
      }

      int code = getCommandType(args[0]);   //converting command string to int for switch case

      if (!valid_N_args(code, args.size())){            // check if number of arguments is correct
        cout << "ERROR: WRONG NUMBER OF ARGUMENTS\n";
        continue;
      }
      
      switch (code)
      {
      case LOGIN:
        if (uid != ""){  
          cout << "ERROR: USER "+uid+" ALREADY LOGGED IN\n";
          break;
        }

        login_process(args,uid,pass);
        break;

      case LOGOUT:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";       //tirar par fora, usado emt odos os casos menos no final, no exit e no login
          break;
        }

        logout_process(uid,pass);
        break;

      case UNREGISTER:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }

        unregister_process(uid, pass);

        break;

      case EXIT:
        if (uid != ""){  
          cout << "ERROR: USER "+uid+" STILL LOGGED IN, PLEASE USE LOGOUT COMMAND\n";
          break;
        }
        stay = 0;
        cout << "closing...\n";
        break;

      case OPEN_AUCTION:
         if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        
        break;
      
      case CLOSE_AUCTION:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        close_auction_process(args, uid, pass);
        break;
      
      case MY_AUCTIONS:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        my_auctions_process(uid);
        break;

      case MY_BIDS:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        my_bids_process(uid);
        break;

      case LIST:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        list_process();
        break;
      
      case SHOW_ASSET:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        show_asset_process(args[1]);
        break;
      
      case BID:
        if (uid == ""){  
            cout << "ERROR: NO USER LOGGED IN\n";
            break;
          }
        bid_process(args, uid, pass);
        break;
      
      case SHOW_RECORD:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        show_record(args[1]);
        break;

      default:
      cout << "ERROR: INVALID COMMAND\n";
        break;
      }
    
    }
}


