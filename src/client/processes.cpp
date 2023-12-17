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


using namespace std;  

void login_process(string port, string ip, vector<string> args, string& log_uid, string& log_pass){
  /**
   * @brief  This function handles the login process, validates the arguments
   * and if they are valid sends the login command to the server
   * @param  port: the port to connect to
   * @param  ip: the ip to connect to
   * @param  args: the arguments passed to the login command
   * @param  log_uid: the user id of the logged in user, that will be updated
   * @param  log_pass: the password of the logged in user, that will be updated
   * 
  */

  // Get the user id and password from the arguments
  string uid = args[1];
  string password = args[2];
  
  // Check if the user id is valid
  if(!valid_uid(uid)){
    cout << "Invalid uid\n";
    return;
  }

  // Check if the password is valid
  if(!valid_password(password)){
    cout << "Invalid password\n";
    return;
  }
  
  // Send the login command to the server threw UDP
  string received = send_message_udp(port, ip, "LIN " + uid + " " + password + "\n");
  
  // Parse the response
  istringstream iss(received);
  string code, status;
  iss >> code >> status;
  
  // Check if the server response is valid
  if(code != "RLI"){
    cout << "Unexpected server response\n";
    return;
  }

  // Print the response
  if( status == "NOK"){
    cout << "Login Failed\n";
    return;
  }
  else if (status == "OK"){
    cout << "Login Successful\n";
  }
  else cout << "User registered\n";

  // Save the user id and password in the main program
  log_uid = uid;                          
  log_pass = password;                    
}

void logout_process(string port, string ip, string& log_uid, string& log_pass){
  /**
   * @brief  This function handles the logout process
   * @param  port: the port to connect to
   * @param  ip: the ip to connect to
   * @param  log_uid: the user id of the logged in user
   * @param  log_pass: the password of the logged in user
   * 
  */


  //Send the logout command to the server
  string received = send_message_udp(port, ip, "LOU " + log_uid + " " + log_pass + "\n");
  
  //Parse the response
  istringstream iss(received);
  string code,status;
  iss >> code >> status;
  if(code != "RLO"){
    cout << "Unexpected server response\n";
    return;
  }
  
  //Print the response
  if(status == "NOK"){            
    cout << "Logout Failed\n";
    return;
  }
  cout << "Logout Successful\n";
  
  
  //Reset the user id and password in the main program
  log_uid = "";
  log_pass = "";

}

void unregister_process(string port, string ip, string& log_uid, string& log_pass){
  /**
   * @brief  This function handles the unregister process
   * @param  port: the port to connect to
   * @param  ip: the ip to connect to
   * @param  log_uid: the user id of the logged in user
   * @param  log_pass: the password of the logged in user
   * 
  */

  //Send the unregister command to the server
  string received = send_message_udp(port, ip, "UNR " + log_uid + " " + log_pass + "\n");
  
  //Parse the response
  istringstream iss(received);
  string code,status;
  iss >> code >> status;

  // Check if the server response is valid
  if(code != "RUR"){
    cout << "Unexpected server response\n";
    return;
  }

  //Print the response
  if(status == "NOK"){
    cout << "Unregistered Failed\n";
    return;
  }
  cout << "User " + log_uid + " unregistered\n";
  
  //Reset the user id and password in the main program
  log_uid = "";
  log_pass = "";
}

void open_auction_process(string port, string ip, vector<string> args, string& uid, string& pass){
  /**
   * @brief  This function handles the open auction process, validates the arguments
   * and if they are valid sends the open auction command to the server
   * @param  port: the port to connect to
   * @param  ip: the ip to connect to
   * @param  args: the arguments passed to the open auction command
   * @param  uid: the user id of the logged in user
   * @param  pass: the password of the logged in user
   * 
  */

  // Check if the auction name is valid
  if(!valid_auction_name(args[1])){
    cout << "Invalid auction name\n";
    return;
  }

  // Check if the file name is valid
  filesystem::path file_path(args[2]);
  string file_name = file_path.filename();
  if(!valid_filename(file_name)){
    cout << "Invalid file name\n";
    return;
  }
  
  // Check if the auction start value is valid
  if(!valid_start_value(args[3])){
    cout << "Invalid auction start value\n";
    return;
  }
  
  // Check if the auction duration is valid
  if(!valid_duration(args[4])){
    cout << "Invalid auction duration\n";
    return;
  }
  
  // Check if the file exists
  FILE *file = fopen(&args[2][0], "rb");
  if(!file){
    cout << "Invalid file\n";
    return;
  }  

  // Get/Check if the file size is valid
  int total = 0, fsize = get_file_size(file);    
  if (!valid_filesize(to_string(fsize))){
    cout << "File too big\n";
    fclose(file);
    return;
  }
  
  char buffer[BUFFER_SIZE];
  string received,code,status,aux;
  int fd,size;
  struct addrinfo *res;

  // Create the message to send
  aux ="OPA " + uid + " " + pass + " " + args[1] + " "
     + args[3] + " " + args[4] + " "  + file_name + " "  + to_string(fsize) + " ";
  
  // Send the message to the server
  res = connect_tcp(&fd,port,ip);
  send_message_tcp(fd,aux.c_str(),aux.size());
  
  // Read/Send the file to the server
  while(total != fsize){
    if((size = fread(buffer, sizeof(char),BUFFER_SIZE,file)) == -1) exit(1);
    total += size;
    send_message_tcp(fd,buffer,size);
  }
  
  // Send the end of file message
  send_message_tcp(fd,"\n",1);
  

  // Receive the response
  received = receive_message_tcp(fd);
  cout << received;
  // Close the connection
  end_tcp(fd,res);   
  fclose(file);
  
  
  // Parse the response
  istringstream iss(received);
  iss >> code >> status;

  // Check if the server response is valid
  if (code != "ROA"){
    cout << "Unexpected server response\n";
    return;
  }

  // Print the response
  if (status == "NOK") cout << "Auction couldn't be started\n";
  else if (status == "NLG") cout << "No user logged in\n";
  else {
    iss >> aux;
    cout << "Auction " + aux + " was started\n";
  }
  
}

void close_auction_process(string port, string ip, vector<string> args, string& uid, string& pass){
  /**
   * @brief This function handles the close auction process, validates the aid
   * and if it is valid sends the close auction command to the server
   * @param  port: the port to connect to
   * @param  ip: the ip to connect to
   * @param  args: the arguments passed to the close auction command
   * @param  uid: the user id of the logged in user
   * @param  pass: the password of the logged in user
   * 
   */

  string aid = args[1];
  string received, code,status;

  // Check if the auction aid is valid
  if (!valid_aid(aid)){
    cout << "Invalid AID\n";
    return;
  }

  // Send command to server
  received = send_single_message_tcp(port, ip, "CLS " + uid + " " + pass + " " + aid + "\n");

  // Parse response
  istringstream iss(received);
  iss >> code >> status;

  // Check if the server response is valid
  if (code != "RCL"){
    cout << "Unexpected server response\n";
    return;
  }

  // Print response
  if (status == "NOK") cout << "There was an error closing the auction\n";
  else if (status == "NLG") cout << "No user logged in\n";
  else if (status == "EAU") cout << "The auction " + aid + " doesn't exist\n";
  else if (status == "EOW") cout << "This auction isnt' owned by " + uid +"\n";
  else if (status == "END") cout << "This auction has already finished\n";
  else if (status == "OK") cout << "The auction " + aid + " was closed\n";
}

void my_auctions_process(string port, string ip, string uid){
  /**
   * @brief This function handles the my auctions process
   * @param port: the port to connect to
   * @param ip: the ip to connect to
   * @param uid: the user id of the logged in user
   * 
   */

  // Send command to server
  string received = send_message_udp(port, ip, "LMA " + uid + "\n");
  
  // Parse the first part of the response
  istringstream iss(received);
  string code,status;
  iss >> code >> status;

  // Check if the server response is valid
  if(code != "RMA"){
    cout << "Unexpected server response\n";
    return;
  }

  // Prints the response
  if(status == "NOK") 
    cout << "The user " + uid + " doesn't have any ongoing auctions\n";
  else{
    string aid,state;                     
    // Parse the rest of the response
    while(iss >> aid){
      iss >> state;
      // Transforms the state from 1/0 to Active/Closed
      if(state == "1") state = "Active";
      else state = "Closed";
      cout << aid + ": " + state + "\n";
    }
  }
}

void my_bids_process(string port, string ip, string uid){
  /**
   * @brief This function handles the my bids process, validates the uid
   *  and if it is valid sends the my bids command to the server
   *  @param  port: the port to connect to
   *  @param  ip: the ip to connect to
   *  @param  uid: the user id of the logged in user
   */

  //sends a message to the server
  string received = send_message_udp(port, ip, "LMB " + uid + "\n");

  // Parses the first part of the response  
  istringstream iss(received);
  string code,status;
  iss >> code >> status;

  // Check if the server response is valid
  if(code != "RMB"){
    cout << "Unexpected server response\n";
    return;
  }

  //Prints the response
  if(status == "NOK") 
    cout << "The user " + uid + " doesn't have any ongoing bids\n";
  else{
    string aid,state;           

    // If there are bids, parses the rest of the response          
    while(iss >> aid){
      iss >> state;
      // Transforms the state from 1/0 to Active/Closed
      if(state == "1") state = "Active";
      else state = "Closed";
      cout << aid + ": " + state + "\n";
    }
  }
}

void list_process(string port, string ip){
  /**
   * @brief This function handles the list process
   * @param port: the port to connect to
   * @param ip: the ip to connect to
  */

  //sends a message to the server
  string received = send_message_udp(port, ip, "LST\n");
  
  // Parses the first part of the response
  istringstream iss(received);
  string code,status;
  iss >> code >> status;

  // Check if the server response is valid
  if(code != "RLS"){
    cout << "Unexpected server response\n";
    return;
  }

  //Prints the response
  if(status == "NOK") {
    cout << "No auction was yet started\n";
    return;
  }

  // If there are auctions, parses the rest of the response
  string aid,state;
  while(iss >> aid){
    iss >> state;
    // Transforms the state from 1/0 to Active/Closed
    if(state == "1") state = "Active";
      else state = "Closed";

    cout << aid + ": " + state + "\n";
  }
}

void show_asset_process(string port, string ip, string aid){
  /**
   *  @brief This function handles the show asset process, validates the aid
   *  and if it is valid sends the show asset command to the server
   *  @param  port: the port to connect to
   *  @param  ip: the ip to connect to
   *  @param  aid: the auction id of the auction to show the asset
   */

  // Check if the auction aid is valid
  if (!valid_aid(aid)){
    cout << "Invalid AID\n";
    return;
  }

  string code = "", status = "", file_name = "";
  char received[BUFFER_SIZE];
  int fd, size, file_size, bytes_read;
  struct addrinfo *res;

  // Create the message to send
  string aux = "SAS " + aid + "\n";

  // Connection/Send the message to the server
  res = connect_tcp(&fd, port, ip);
  send_message_tcp(fd, aux.c_str(), aux.size());  

  // Parse the response
  istringstream iss;
  string tokens[4];
  int i = 0;
  while((size = receive_message_tcp(fd, received)) != 0){
    // Reset bytes_read
    bytes_read = 0;
    
    // Reset the istringstream
    iss.clear();
    iss.str(received);
    
    // while there are tokens to read
    while(i <= 3){
      iss >> tokens[i];
      // in case there are no more tokens to read
      if(tokens[i] == "")
        break;

      // Update the bytes_read
      bytes_read += tokens[i].size() + BLANK_SPACE;
      i++; 
    }
    // in case there are no more tokens to read
    if (i == 4)
      break;
  }

  code = tokens[0];
  status = tokens[1];

  // Check if the server response is valid
  if (code != "RSA")
  {
    cout << "Unexpected server response\n";
    return;
  }

  // Print the response
  if (status == "NOK")
  {
    cout << "There was an error displaying the asset\n";
    return;
  }

  file_name = tokens[2];
  file_size = stoi(tokens[3]);
  

  // Store total size of the file data received
  int total = size - bytes_read;

  // Create the file
  FILE *file = fopen(file_name.c_str(), "wb");
  
  // In case the file size was smaller than the buffer size
  if ( total > file_size ){
    cout << "here";
    fwrite(received + bytes_read, sizeof(char), file_size , file);
  }
  else
  {
    // Write the first part of the file´
    fwrite(received + bytes_read, sizeof(char), size - bytes_read, file);

    // Read from the server and write the rest of the file
    while (total + BUFFER_SIZE < file_size)
    {
      memset(received, '\0', sizeof(received));

      // Read from the server
      size = receive_message_tcp(fd, received);
      total += size;

      // Write to the file
      fwrite(received, sizeof(char), size, file);
    }

    // Last read/write, excludes any extra bytes
    size = receive_message_tcp(fd, received);
    total += size;
    fwrite(received, sizeof(char), size + (file_size - total), file);
  }

  // Close the connection
  end_tcp(fd, res);
  fclose(file);

  cout << "File " + file_name + " received\n"+
          "The file was stored in the executable's directory\n";
}

void bid_process(string port, string ip, vector<string> args, string uid, string pass){ 
  /**
   * @brief This function handles the bid process, validates the aid and bid
   *  and if they are valid sends the bid command to the server
   *  @param  port: the port to connect to
   *  @param  ip: the ip to connect to
   *  @param  args: the arguments passed to the bid command
   *  @param  uid: the user id of the logged in user
   *  @param  pass: the password of the logged in user
   */

  string aid = args[1];
  string bid = args[2];

  // check if the aid is valid
  if (!valid_aid(aid)){
    cout << "Invalid AID\n";
    return;
  }

  // check if the bid is valid
  if (!valid_bid(bid)){
    cout << "Invalid bid\n";
    return;
  }
  
  string received, code, status;
  // Send the bid command to the server
  received = send_single_message_tcp(port, ip, "BID " + uid + " " + pass + " " + aid + " " + bid +"\n");
  
  // Parse the response
  istringstream iss(received);
  iss >> code >> status;

  // Check if the server response is valid
  if (code != "RBD"){
    cout << "Unexpected server response\n";
    return;
  }

  // Print the response
  if (status == "NOK") cout << "Auction not active\n";
  else if (status == "NLG") cout << "No user logged in\n";
  else if (status == "ACC") cout << "The bid was accepted\n";
  else if (status == "REF") cout << "A larger bid was placed previously\n";
  else if (status == "ILG") cout << "Can't bid on your own auction\n";
  
}

void show_record_process(string port, string ip, string aid){
  /**
   * @brief This function handles the show record process, validates the aid
   *  and if it is valid sends the show record command to the server
   *  @param  port: the port to connect to
   *  @param  ip: the ip to connect to
   *  @param  aid: the auction id of the auction to show the record
   */

  //Check if the auction ID is valid
  if (!valid_aid(aid)){
    cout << "Invalid AID\n";
    return;
  }

  string host_UID, auction_name, asset_fname, start_value, start_date, start_time, timeactive;
  string bidder_UID, bid_value, bid_date, bid_time, bid_sec_time;
  string end_date, end_time, end_sec_time;
  string code,status,received = send_message_udp(port, ip, "SRC " + aid + "\n");

  // Parse the first part of the response
  istringstream iss(received);
  iss >> code >> status;

  // Check if the server response is valid
  if(code != "RRC"){
    cout << "Unexpected server response\n";
    return;
  }

  // Print the response in case of error
  if(status == "NOK")
    cout << "There was an error displaying the record\n";

  // Display the auction information
  else {

    // Parse the auction information
    iss >> host_UID >> auction_name >> asset_fname >> start_value >> start_date >> start_time >> timeactive;

    // Display the auction information
    cout << "\nAuction "+auction_name+" hosted by user "+host_UID+"\n"
           "File name: "+asset_fname+"\n";

    cout << "-----------------------------------------------------\n"
            "                  AUCTION HISTORY                    \n"
            "-----------------------------------------------------\n";

    cout << start_date+" "+start_time+" : Auction started at "+start_value+" credits\n"+
                "                      Expected duration: "+timeactive+" seconds\n"
                "-----------------------------------------------------\n";  

    //Go through the information and display the bids and the end of the auction
    while(iss >> code){
      if (code == "B"){
        iss >> bidder_UID >> bid_value >> bid_date >> bid_time >> bid_sec_time;

        cout << bid_date+" "+bid_time+" : User "+bidder_UID+" bid "+bid_value+" credits\n"+
                "                      Time since start: "+bid_sec_time+" seconds\n"
                "-----------------------------------------------------\n";
      }
      else if (code == "E"){
        iss >> end_date >> end_time >> end_sec_time;
        cout << end_date + " " + end_time + " : Auction was closed\n"+
                "                      Time since start: "+end_sec_time+" seconds\n";
      }
      else break;
    }    
  }
  
  
}