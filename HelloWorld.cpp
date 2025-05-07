#include <iostream>
#include "structures.h"
#include "DataRead.h"
using namespace std;

int main() {
    vector<User> userList = readUsers();
    vector<Task> tasks;
}

bool compareTasks(const Task& a, const Task& b) {
    int aPeople = a.users.size();
    int bPeople = b.users.size();
    
    return aPeople > bPeople;
}

