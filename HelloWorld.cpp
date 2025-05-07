#include <iostream>
#include "structures.h"
#include "DataRead.h"
using namespace std;

int main() {
    vector<User> userList = readUsers();
    cout << "Hello World!";
    return 0;
}
