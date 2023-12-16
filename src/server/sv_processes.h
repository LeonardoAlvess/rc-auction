#include <string>

using namespace std;

string sv_login_process(string uid, string pass);

string sv_logout_process(string uid, string pass);

string sv_unregister_process(string uid, string pass);

string sv_myauctions_process(string uid);

string sv_mybids_process(string uid);

string sv_list_process(string port, string ip);

string sv_show_record_process(string aid);

string sv_open_process(char* received,int size, string port, string ip, int socket_fd, bool verbose);

string sv_close_process(string uid, string pass, string aid);

string sv_bid_process(string uid, string pass, string aid, string bid);

string sv_show_asset(char* input, string port, string ip, int socket_fd, bool verbose);