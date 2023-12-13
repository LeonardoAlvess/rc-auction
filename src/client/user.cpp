#include <iostream>
#include <sstream>
#include "processes.h"
#include <cstring>

using namespace std;

int main(int argc, char *argv[]){
    string token,line,uid,pass;
    vector<string> args;
    istringstream iss;
    int stay = 1;

    //default values for ip and port
    string ip = IP,port = PORT;       

    //reading ip and port from command line
    for(int i = 1; i < argc; i++){    
      // if -n is found, next argument is ip 
      if (!strcmp(argv[i],"-n")){     
        i++;
        ip = argv[i];             
      }
      //if -p is found, next argument is port
      else if (!strcmp(argv[i],"-p")){ 
        i++;
        port = argv[i];
      }
    }


    while(stay){

      //get the command input and process it
      if(uid != "")
        cout << "["+uid+"]: ";
      else
        cout << "[No User]: ";
      
      //getting line string from input
      getline(cin,line);

      //resetting line stream and argument vector       
      iss.clear();              
      iss.str(line);            
      args.clear();

      //reading from line stream and filling argument vector
      while(iss >> token){      
        args.push_back(token);  
      }

      //converting command string to int for switch case
      int code = getCommandType(args[0]);   

      // check if number of arguments is correct
      if (!valid_N_args(code, args.size())){   
        cout << "Wrong number of arguments\n";
        continue;
      }
      
      //switch case for each command  
      switch (code)          
      {
      case LOGIN:                                     //login command
        if (uid != ""){  
          cout << "User "+uid+" is already logged in\n";
          break;
        }
        login_process(port,ip,args,uid,pass);
        break;

      case LOGOUT:                                    //logout command
        if (uid == ""){  
          cout << "No user logged in\n";    
          break;
        }

        logout_process(port,ip,uid,pass);
        break;

      case UNREGISTER:                                //unregister command
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }

        unregister_process(port,ip,uid, pass);

        break;

      case EXIT:                                      //exit command
        if (uid != ""){  
          cout << "User is "+uid+" still logged in, use log out command\n";
          break;
        }
        stay = 0;
        cout << "User closing\n";
        break;

      case OPEN_AUCTION:                              //open auction command
         if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        open_auction_process(port,ip,args, uid, pass);
        break;
      
      case CLOSE_AUCTION:                             //close auction command
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        close_auction_process(port,ip,args, uid, pass);
        break;
      
      case MY_AUCTIONS:                               //my auctions command
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        my_auctions_process(port,ip,uid);
        break;

      case MY_BIDS:                                   //my bids command          
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        my_bids_process(port,ip,uid);
        break;

      case LIST:                                      //list command             
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        list_process(port,ip);
        break;
      
      case SHOW_ASSET:                                //show asset command     
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        show_asset_process(port,ip,args[1]);
        break;
      
      case BID:                                       //bid command         
        if (uid == ""){  
            cout << "No user logged in\n";
            break;
          }
        bid_process(port,ip,args, uid, pass);
        break;
      
      case SHOW_RECORD:                               //show record command          
        if (uid == ""){  
          cout << "No user logged in\n";
          break;
        }
        show_record_process(port,ip,args[1]);
        break;

      default:                                        //default case for invalid command      
      cout << "Invalid command\n";
        break;
      }
    
    }
}

