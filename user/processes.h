#include <string>
#include <vector>

#include "utils.h"

using namespace std;

void login_process(string port, string ip, vector<string> args, string& log_uid, string& log_pass);

void logout_process(string port, string ip, string& log_uid, string& log_pass);

void unregister_process(string port, string ip, string& log_uid, string& log_pass);

void open_auction_process(string port, string ip, vector<string> args, string& uid, string& pass);

void close_auction_process(string port, string ip, vector<string> args, string& uid, string& pass);

void my_auctions_process(string port, string ip, string uid);

void my_bids_process(string port, string ip, string uid);

void list_process(string port, string ip);

void show_asset_process(string port, string ip, string aid);

void bid_process(string port, string ip, vector<string> args, string uid, string pass);

void show_record_process(string port, string ip, string aid);