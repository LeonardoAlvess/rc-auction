#include <string>
#include <vector>

#include "utils.h"

using namespace std;

void login_process(vector<string> args, string& log_uid, string& log_pass);

void logout_process(string& log_uid, string& log_pass);

void unregister_process(string& log_uid, string& log_pass);

void open_auction_process(vector<string> args, string& uid, string& pass);

void close_auction_process(vector<string> args, string& uid, string& pass);

void my_auctions_process(string uid);

void my_bids_process(string uid);

void list_process();

void show_asset_process(string aid);

void bid_process(vector<string> args, string uid, string pass);

void show_record_process(string aid);