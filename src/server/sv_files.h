#include <string>
#include <vector>

using namespace std;

int createUser(string uid, string pass);

int createAuction(string aid, string start_info);

int loadAsset(string aid, string asset_fname, char* asset_buf, int asset_size);

int hostAuction(string aid, string uid);

int endAuction(string aid, string end_info);

int createLogin(string uid);

int eraseLogin(string uid);

int eraseUser(string uid);

int makeBid(string aid, string uid, string bid, string bid_info);

vector<string> get_hosted(string uid);

vector<string> get_bidded(string uid);

vector<string> get_all_auctions();

vector<string> get_auction_bids(string aid);

string get_unique_aid();

string get_current_time();

string get_elapsed_time(string aid);

string get_ended_time(string aid);