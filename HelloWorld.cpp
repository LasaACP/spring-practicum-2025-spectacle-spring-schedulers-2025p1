#include <iostream>
#include "structures.h"
#include "DataRead.h"
using namespace std;

int main() {
    vector<User> userList = readUsers();
    vector<Task> tasks;
}

bool compareTasks(const Task& a, const Task& b) {
    int aPeople = 0;
    int bPeople = 0;
    for(int i=0;i<sizeof(a.users);i++){
        aPeople++;
    }
    for(int i=0;i<sizeof(b.users);i++){
        bPeople++;
    }
    return aPeople > bPeople;
}

