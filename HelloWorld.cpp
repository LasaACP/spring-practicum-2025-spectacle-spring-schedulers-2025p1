#include <iomanip>
#include <iostream>
#include <algorithm>
#include "structures.h"
#include "DataRead.h"
#include <unordered_map>

using namespace std;
bool compareTasks(const Task& a, const Task& b);
void printTaskList(vector<Task> t);
void displaySchedule(const vector<Task>& tasks);
void scheduleTasks(vector<Task>& tasks, unordered_map<string, User>& userMap);


int main() {
    vector<User> userList = readUsers("users.csv");
    vector<Task> tasks = readTasks("tasks.csv");
    unordered_map<string, User> userMap;
    for (const auto& user : userList) {
    userMap[user.userName] = user;
    }
    
    sort(tasks.begin(), tasks.end(), compareTasks);

    cout << "Welcome to the Spring Schedule Spectacle program!" << endl;
    string userInput;
    char userInputFirstChar;

    while (userInputFirstChar != 'e') {
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

        if (userInputFirstChar == 'f') {
            cout << endl << "pretend this works" << endl;
        }
        else if (userInputFirstChar == 't') {
            printTaskList(tasks);
            scheduleTasks(tasks, userMap);
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
    cout << setw(20) << left << "Task" << setw(15) << "Date" << "          " << "Assigned Users\n";
    cout << "--------------------------------------------------------\n";
   
    for (const auto& task : sorted) {
        cout << setw(20) << left << task.taskName
             << setw(15) << left << task.date
             << "  Users: ";
       
        for (const auto& user : task.users) {
            cout << user << " ";
        }
        cout << endl;
    }
}

void scheduleTasks(vector<Task>& tasks, unordered_map<string, User>& userMap) {
    sort(tasks.begin(), tasks.end(), compareTasks);

    tm* current = getDate(); 
    char output[50];

    for (auto& task : tasks) {
        bool scheduled = false;

        for (int i = 0; i < 30 && !scheduled; i++) {
            tm testDate = addDaysToDate(*current, i);

            for (int hour = 1; hour <= 24 - task.duration + 1 && !scheduled; hour++) {
                bool allAvailable = true;

                for (const string& userName : task.users) {
                    auto it = userMap.find(userName);
                    if (it == userMap.end()) {
                        allAvailable = false;
                        break;
                    }

                    const vector<vector<int>>& availability = it->second.hours;
                    bool userAvailable = false;

                    for (const auto& interval : availability) {
                        if (interval.size() == 2 && interval[0] <= hour && hour + task.duration - 1 <= interval[1]) {
                            userAvailable = true;
                            break;
                        }
                    }

                    if (!userAvailable) {
                        allAvailable = false;
                        break;
                    }
                }

                if (allAvailable) {
                    formatDate(testDate, output, sizeof(output));
                    task.date = string(output) + " @ hour " + to_string(hour);
                    scheduled = true;

                    for (const string& userName : task.users) {
                        auto& user = userMap[userName];  
                        vector<vector<int>>& availability = user.hours;

                        vector<vector<int>> newAvailability;
                        for (const auto& interval : availability) {
                            if (interval.size() != 2) continue;
                            int start = interval[0], end = interval[1];

                            if (start <= hour && hour + task.duration - 1 <= end) {
                                if (start < hour)
                                    newAvailability.push_back({start, hour - 1});
                                if (hour + task.duration <= end)
                                    newAvailability.push_back({hour + task.duration, end});
                            } else {
                                newAvailability.push_back(interval);  
                            }
                        }

                        availability = newAvailability;
                    }
                }
            }
        }

        if (!scheduled) {
            task.date = "Not Scheduled";
        }
    }
} 