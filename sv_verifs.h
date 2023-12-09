#include <string>

using namespace std;

string validateBid(string aid, string uid, string bid);

int is_registered(string uid);

int is_logged(string uid);

int is_owner(string uid, string aid);

int exists(string aid);

int ended(string aid);