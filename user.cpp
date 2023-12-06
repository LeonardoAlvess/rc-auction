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
#include <regex>
using namespace std;
using namespace std::filesystem;

#define PORT "58011"     
#define TCP_SOCKET SOCK_STREAM
#define UDP_SOCKET SOCK_DGRAM        

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

string send_message(int type, string message){
  int fd,errcode; 
  ssize_t size;
  socklen_t addrlen;
  struct addrinfo hints,*res;
  struct sockaddr_in addr;
  char buffer[6015];                               
  memset(buffer,'\0',sizeof(buffer));             
  if ((fd=socket(AF_INET,type,0)) == -1) exit(1); //socket

  memset(&hints,0,sizeof hints);
  hints.ai_family=AF_INET; //IPv4
  hints.ai_socktype=type; //socket type
  if(getaddrinfo("tejo.tecnico.ulisboa.pt",PORT,&hints,&res) == -1) exit(1);

  if(type==UDP_SOCKET){
    if(sendto(fd,&message[0],message.size(),0,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    addrlen=sizeof(addr);
    if((size=recvfrom(fd,buffer,6015,0,(struct sockaddr*)&addr,&addrlen)) == -1) exit(1);          //adicionar while para textos maior que o buffer
  }

  else{
    if(connect(fd,res->ai_addr,res->ai_addrlen) == -1) exit(1);
    cout << message;
    if(write(fd,&message[0],message.size()) == -1) exit(1);
    if(size=read(fd,buffer,6015) == -1) exit(1); //adicionar while
    cout << buffer;
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

void show_record(string aid){
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
        open_auction_process(args, uid, pass);
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


