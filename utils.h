#include <string>
using namespace std;

#define PORT "58011"     
#define IP "tejo.tecnico.ulisboa.pt"

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


int getCommandType(const string& command);

bool valid_N_args(int code, int n_args);

bool valid_uid(string uid);

bool valid_aid(string aid);

bool valid_password(string pass);

bool valid_auction_name(string name);

bool valid_start_value(string value);

bool valid_duration(string value);

bool valid_filename(string filename);

bool valid_filesize(string filesize);

bool valid_bid(string bid);

string send_message_udp(string port,string ip, string message);

string send_single_message_tcp( string port, string ip, string message);

struct addrinfo* connect_tcp(int* fd, string port,string ip);

void send_message_tcp(int fd, string message);

void send_message_tcp(int fd, const char* message, int size);

string receive_message_tcp(int fd);

void end_tcp(int fd,struct addrinfo *res);