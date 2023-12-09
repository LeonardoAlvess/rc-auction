#include <string>

using namespace std;

int sv_login_process(string uid, string pass, string port, string ip);

int sv_logout_process(string uid, string pass, string port, string ip);

int sv_unregister_process(string uid, string pass, string port, string ip);

int sv_myauctions_process(string uid, string port, string ip);

int sv_mybids_process(string uid, string port, string ip);

int sv_list_process(string port, string ip);

int sv_show_record_process(string aid, string port, string ip);

int sv_open_process(string uid,string input, string port, string ip);

int sv_close_process(string uid, string pass, string aid, string port, string ip);

int sv_bid_process(string uid, string pass, string aid, string bid, string port, string ip);

