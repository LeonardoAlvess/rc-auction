#include <string>

using namespace std;

string validateBid(string aid, string uid, string pass, string bid);

int is_registered(string uid);

int is_logged(string uid);

int authenticated(string uid, string pass);

int is_owner(string uid, string aid);

int exists(string aid);

int ended(string aid);