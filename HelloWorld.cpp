#include <iomanip>
#include <iostream>
#include <algorithm>
#include "structures.h"
#include "DataRead.h"
#include <unordered_map>

using namespace std;
bool compareTasks(const Task& a, const Task& b);
void printTaskList(vector<Task> t);
void scheduleTasks(vector<Task>& tasks, unordered_map<string, User>& userMap);
void displaySchedule(const vector<Task>& tasks);

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
    cout << setw(20) << left << "Task" << setw(15) << "Date" << "         Assigned Users\n";
    cout << "--------------------------------------------------------\n";
   
    for (const auto& task : sorted) {
        string timeBlock = task.startTime + " - " + task.endTime;
        cout << setw(20) << left << task.taskName
             << setw(15) << left << task.date
             << "   Users: ";
       
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
