#include <string>

using namespace std;

int sv_login_process(string uid, string pass, string port, string ip, bool verbose);

int sv_logout_process(string uid, string pass, string port, string ip, bool verbose);

int sv_unregister_process(string uid, string pass, string port, string ip, bool verbose);

int sv_myauctions_process(string uid, string port, string ip, bool verbose);

int sv_mybids_process(string uid, string port, string ip, bool verbose);

int sv_list_process(string port, string ip, bool verbose);

int sv_show_record_process(string aid, string port, string ip, bool verbose);

int sv_open_process(char* received,int size, string port, string ip, int socket_fd, bool verbose);

int sv_close_process(string uid, string pass, string aid, string port, string ip, int socket_fd, bool verbose);

int sv_bid_process(string uid, string pass, string aid, string bid, string port, string ip, int socket_fd, bool verbose);

void sv_show_asset(char* input, string port, string ip, int socket_fd, bool verbose);