#include <iostream>
#include "structures.h"
#include "DataRead.h"

using namespace std;
#include <algorithm>
bool compareTasks(const Task& a, const Task& b);

void printTask(vector<Task> t) {
    for(Task task: t){
    cout << "Task: " << task.taskName << "\n";
    cout << "Users: ";
    for (size_t i = 0; i < task.users.size(); ++i) {
        cout << task.users[i];
        if (i != task.users.size() - 1) {
            cout << ", ";
        }
    }
    cout << "\nDuration: " << task.duration << "\n";
}
}

int main() {
    vector<User> userList = readUsers();
    vector<Task> tasks = readTasks();
    sort(tasks.begin(), tasks.end(), compareTasks);
    printTask(tasks);

}
bool compareTasks(const Task& a, const Task& b) {
    int aPeople = a.users.size();
    int bPeople = b.users.size();
    if (aPeople != bPeople) {
        return aPeople > bPeople; 
    }
    return a.duration > b.duration; 
}

