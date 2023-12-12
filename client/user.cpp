#include <iostream>
#include <sstream>
#include "processes.h"
#include <cstring>

using namespace std;

int main(int argc, char *argv[]){
    string token,line,uid,pass,ip,port = PORT;
    vector<string> args;
    istringstream iss;
    int stay = 1;
    
    port = PORT;
    ip = IP;

    for(int i = 1; i < argc; i++){
      if (!strcmp(argv[i],"-n")){
        i++;
        ip = argv[i];
      }
      else if (!strcmp(argv[i],"-p")){
        i++;
        port = argv[i];
      }
    }


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

        login_process(port,ip,args,uid,pass);
        break;

      case LOGOUT:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";       //tirar par fora, usado emt odos os casos menos no final, no exit e no login
          break;
        }

        logout_process(port,ip,uid,pass);
        break;

      case UNREGISTER:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }

        unregister_process(port,ip,uid, pass);

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
        open_auction_process(port,ip,args, uid, pass);
        break;
      
      case CLOSE_AUCTION:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        close_auction_process(port,ip,args, uid, pass);
        break;
      
      case MY_AUCTIONS:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        my_auctions_process(port,ip,uid);
        break;

      case MY_BIDS:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        my_bids_process(port,ip,uid);
        break;

      case LIST:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        list_process(port,ip);
        break;
      
      case SHOW_ASSET:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        show_asset_process(port,ip,args[1]);
        break;
      
      case BID:
        if (uid == ""){  
            cout << "ERROR: NO USER LOGGED IN\n";
            break;
          }
        bid_process(port,ip,args, uid, pass);
        break;
      
      case SHOW_RECORD:
        if (uid == ""){  
          cout << "ERROR: NO USER LOGGED IN\n";
          break;
        }
        show_record_process(port,ip,args[1]);
        break;

      default:
      cout << "ERROR: INVALID COMMAND\n";
        break;
      }
    
    }
}


