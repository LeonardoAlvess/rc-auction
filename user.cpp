#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
#include <fstream>
#include <iostream>
#include <filesystem>
using namespace std;

#define FALSE 0
#define TRUE 1
#define SUCCESS 1
#define FAILURE 0

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

int valid_N_args(int code, int n_args){
  int good_n_ars[13] = {3,1,1,1,5,2,1,1,1,2,3,2,0};
  if (code == 12) return TRUE;
  else return good_n_ars[code]==n_args;
}

int valid_uid(string uid) {return uid.size() == 6 && all_of(uid.begin(), uid.end(), ::isdigit);}

int valid_aid(string uid) {return uid.size() == 3 && all_of(uid.begin(), uid.end(), ::isdigit);}

int valid_password(string pass) { return pass.size() == 8 && all_of(pass.begin(), pass.end(), ::isalnum);}

int valid_auction_name(string name) { return name.size() <= 10 && all_of(name.begin(), name.end(), ::isalnum);}

int valid_start_value(string value) { return value.size() <= 6 && all_of(value.begin(), value.end(), ::isdigit);}

int process_duration(string& value) { 
  
  if( value.size() <= 5 && all_of(value.begin(), value.end(), ::isdigit)){
    float duration = stof(value);
    duration = duration/60;
    value = to_string(duration);
    return SUCCESS;
      
  cout << "ERROR: INVALID DURATION\n";
  return FAILURE;
  }
}

void process_file(){
  
}

//------------------------------------------------- MAIN FUNCTIONS -------------------------------------------------//


void login_process(vector<string> args, string& log_uid, string& log_pass){

  string uid = args[1];
  string password = args[2];

  if(!valid_uid(uid)){
    cout << "ERROR: INVALID UID\n";
    return;
  }

  if(!valid_password(password)){
    cout << "ERROR: INVALID PASSWORD\n";
    return;
  }

  //sendto/receivto
  //se pintar guardar uid e pass
  log_uid = uid;                          //for now
  log_pass = password;                    
  cout << "Login Successful\n";
}

void logout_process(string& log_uid, string& log_pass){
  //sendinfo to server

  //for now
  log_uid = "";
  log_pass = "";
  return;
}

void unregister_process(string& log_uid, string& log_pass){
  //send info to server

  //for now
  log_uid = "";
  log_pass = "";
}

void open_auction_process(vector<string> args, string& log_uid, string& log_pass){
  //send info to server

  //for now
  if(!valid_auction_name(args[1])){
    cout << "ERROR: INVALID AUCTION NAME\n";
    return;
  }

  ifstream file(args[2]);
  if(!file.is_open()){
    cout << "ERROR: INVALID FILE\n";
    return;
  }
  
  if(!valid_start_value(args[3])){
    cout << "ERROR: INVALID START VALUE\n";
    return;
  }
  
  if(!process_duration(args[4])){
    cout << "ERROR: INVALID DURATION\n";
    return;
  }

  string file_size = to_string(file_size(args[2]));

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

void show_asset_process(vector<string> args){
  string aid = args[1];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }

  //tcp conn  
}

void bid_process(vector<string> args, string log_uid, string log_pass){
  string aid = args[1];
  if (!valid_aid(aid)){
    cout << "ERROR: INVALID AID\n";
    return;
  }

  //tcp conn
}

int main(int argc, char *argv[]){ //adicionar args e processar
    //ligacacao sockets
    
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
          cout << "ERROR: NO USER LOGGED IN\n";
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
        break;

      case MY_BIDS:
        break;

      case LIST:
        break;
      
      case SHOW_ASSET:
      show_asset_process();
        break;
      
      case BID:
      bid_process();
        break;
      
      case SHOW_RECORD:
        break;

      default:
      cout << "ERROR: INVALID COMMAND\n";
        break;
      }
    
    }

    
}


