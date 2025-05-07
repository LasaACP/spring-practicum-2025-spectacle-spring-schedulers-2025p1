#include <iostream>

int main() {
    vector<Tasks> tasks;
}

bool compareTasks(const Task& a, const Task& b) {
    int aPeople = 0;
    int bPeople = 0;
    for(int i=0;i<sizeof(a.names);i++){
        aPeople++;
    }
    for(int i=0;i<sizeof(b.names);i++){
        bPeople++;
    }
    return aPeople > bPeople;
}

