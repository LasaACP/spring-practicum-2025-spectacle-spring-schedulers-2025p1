#include <iostream>
#include <iomanip>
#include <iostream>
#include <iomanip>
#include <iostream>
#include <algorithm>
#include "structures.h"
#include "DataRead.h"

using namespace std;
bool compareTasks(const Task& a, const Task& b);

int main() {
    vector<User> userList = readUsers("users.csv");
    vector<Task> tasks = readTasks("tasks.csv");

    sort(tasks.begin(), tasks.end(), compareTasks);

    cout << "Welcome to the Spring Schedule Spectacle program!" << endl;
    string userInput;
    char userInputFirstChar;

    while (userInputFirstChar != 'E') {
        cout << endl
                << "What would you like to do?" << endl
                << "- change file input: \"F\"" << endl
                << "- print task list: \"T\"" << endl
                << "- exit program: \"E\"" << endl;

        cout << "input... ";
        
        cin >> userInput;
        if (userInput.length() == 0) {
            userInputFirstChar = ' ';
        } else {
            userInputFirstChar = userInput[0] | 32;
        }

        if (userInputFirstChar == 'F') {
            cout << endl << "pretend this works" << endl;
        }
        else if (userInputFirstChar == 'T') {
            printTaskList(tasks);
        }
        else if (userInputFirstChar == 'E') {
            cout << "Bye, have a nice day!" << endl;
            break;
        }
        else {
            cout << endl << "invalid input." << endl;
        }
    }
    
    return 0;
}

void printTaskList(vector<Task> t) {
    for (Task task : t) {
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


tm* getDate() {
    time_t timestamp;
    char output[50];
    struct tm * datetime;
    time(&timestamp);
    datetime = localtime(&timestamp);
    return datetime;
}


tm addDaysToDate(const tm& date, int add) {
    tm newDate = date;
    newDate.tm_mday += add;
    mktime(&newDate);
    return newDate;
}

void formatDate(const tm& date, char* output, size_t size) {
    strftime(output, size, "%B %e, %Y", &date);
}

void displaySchedule(const vector<Task>& tasks) {
    vector<Task> sorted = tasks;
    sort(sorted.begin(), sorted.end(), compareTasks);

    cout << "Here's your generated schedule:\n";
    cout << setw(20) << left << "Task" << setw(15) << "Date" << "Assigned Users\n";
    cout << "--------------------------------------------------------\n";
   
    for (const auto& task : sorted) {
        cout << setw(20) << left << task.taskName
             << setw(15) << left << task.date
             << "Users: ";
       
        for (const auto& user : task.users) {
            cout << user << " ";
        }
        cout << endl;
    }
}

void scheduleTasks(vector<Task>& tasks) {
    tm* current = getDate(); 

    char output[50];
    formatDate(*current, output, sizeof(output)); 
    int add = 0;
    for (auto& task : tasks) {
        tm scheduledDate = addDaysToDate(*current, add); 
        formatDate(scheduledDate, output, sizeof(output));
        task.date = string(output);
        add++;
    }
}