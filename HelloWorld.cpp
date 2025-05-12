#include <iomanip>
#include <iostream>
#include <algorithm>
#include "structures.h"
#include "DataRead.h"

using namespace std;
bool compareTasks(const Task& a, const Task& b);
void printTaskList(vector<Task> t);
void displaySchedule(const vector<Task>& tasks);
void scheduleTasks(vector<Task>& tasks);

int main() {
    vector<User> userList = readUsers("users.csv");
    vector<Task> tasks = readTasks("tasks.csv");

    sort(tasks.begin(), tasks.end(), compareTasks);

    cout << "Welcome to the Spring Schedule Spectacle program!" << endl;
    string userInput;
    char userInputFirstChar;

    while (userInputFirstChar != 'e') {
        cout << endl
                << "What would you like to do?" << endl
                << "- change user file: \"U [filename]\"" << endl
                << "- change task file: \"F [filename]\"" << endl
                << "- print task list: \"T\"" << endl
                << "- exit program: \"E\"" << endl;

        cout << "input... ";
        
        getline(cin, userInput);
        if (userInput.length() == 0) {
            userInputFirstChar = ' ';
        } else {
            userInputFirstChar = userInput[0] | 32;
        }

        if (userInputFirstChar == 'f' || userInputFirstChar == 'u') {
            int fileNameIndex = userInput.find(' ');
            string fileName = userInput.substr(fileNameIndex+1);

            if (fileExists(fileName)) {
                cout << endl << "working... if something goes wrong, you've likely formatted the file wrong";
                if (userInputFirstChar == 'f') {
                    tasks = readTasks(fileName);
                    cout << endl << "updated tasks successfully!" << endl;
                } else if (userInputFirstChar == 'u') {
                    userList = readUsers(fileName);
                    cout << endl << "updated users successfully!" << endl;
                }
            } else {
                if (fileName == userInput || fileName == "") {
                    cout << endl << "please input a file." << endl;
                } else {
                    cout << endl << "file \"" << fileName << "\" doesn't exist." << endl;
                }
            }
            
        }
        else if (userInputFirstChar == 't') {
            scheduleTasks(tasks);
            displaySchedule(tasks);
        }
        else if (userInputFirstChar == 'e') {
            cout << "Bye, have a nice day!" << endl;
            break;
        }
        else {
            cout << endl << "invalid input." << endl;
        }
    }
    
    return 0;
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
    cout << setw(20) << left << "Task" << setw(15) << "Date" << setw(15) << "Duration" << "Assigned Users\n";
    cout << "--------------------------------------------------------\n";
   
    for (const auto& task : sorted) {
        cout << setw(20) << left << task.taskName
             << setw(15) << left << task.date
             << setw(15) << left << task.duration
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