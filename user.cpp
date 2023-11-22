#include <string>
#include <iostream>
#include <sstream>
#include <cctype>
#include <algorithm>
#include <vector>
using namespace std;

#define FALSE 0
#define TRUE 1

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

int getCommandType(const std::string& command) {
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

//------------------------------------------------------ AUX


int valid_uid(string uid) {return uid.size() == 6 && all_of(uid.begin(), uid.end(), ::isdigit);}

int valid_password(string pass) { return pass.size() == 8 && all_of(pass.begin(), pass.end(), ::isalnum);}

void login_process(vector<string> args, string log_uid, string log_pass){

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
  cout << "Login Successful\n";
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
      iss.clear();      
      iss.str(line);            //turning line into a stream

      args.clear();
                   //resseting argument vector
      while(iss >> token){      
        args.push_back(token);  //reading from line stream and filling argument vector
      }

      int code = getCommandType(args[0]);   //converting command string to int for switch case

      
      switch (code)
      {
      case LOGIN:
        if (args.size() != 3) cout << "ERROR: WRONG NUMBER OF ARGUMENTS\n";
        login_process(args,uid,pass);
        break;
      
      case LOGOUT:
        cout << "logout\n";
        break;

      case UNREGISTER:
        cout << "unreg\n";
        break;

      case EXIT:
        stay = 0;
        cout << "closing...\n";
        break;

      case OPEN_AUCTION:
        cout << "open\n";
        break;
      
      case CLOSE_AUCTION:
        cout << "close\n";
        break;

      
      default:
      cout << "wrong\n";
        break;
      }
    
    }

    
}


