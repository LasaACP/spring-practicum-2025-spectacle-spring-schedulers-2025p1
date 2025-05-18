#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>
#include "structures.h"
#include "DataRead.h"
#include <unordered_map>

using namespace std;
void printTaskList(vector<Task> t);
void displaySchedule(const vector<Task>& tasks);

enum class ScheduleMethod { BY_USERS, SHORTEST_TASKS, LONGEST_TASKS };

bool compareByUserCount(const Task& a, const Task& b) {
    int aPeople = a.users.size();
    int bPeople = b.users.size();
    if (aPeople != bPeople) 
        return aPeople > bPeople;
    return a.duration > b.duration;
}

bool compareByDurationAsc(const Task& a, const Task& b) {
    return a.duration < b.duration;
}

bool compareByDurationDesc(const Task& a, const Task& b) {
    return a.duration > b.duration;
}

static string formatHour(int hour24) {
    int h = hour24 % 12;
    if (h == 0) h = 12;
    string suffix = (hour24 < 12) ? "AM" : "PM";
    return to_string(h) + ":00 " + suffix;
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
    cout << "\nHere's your generated schedule:\n";
    cout << setw(20) << left << "Task" << setw(15) << "Date" << setw(12) << "Start" << setw(12) << "End" << setw(8)  << "Dur(h)" << "Assigned Users\n";
    cout << "-------------------------------------------------------------------\n";

    for (auto& task : tasks) {
        cout << setw(20) << left << task.taskName
             << setw(15) << left << task.date
             << setw(12) << left << task.startTime
             << setw(12) << left << task.endTime
             << setw(8)  << left << task.duration;
        cout << "  Users: ";
        for (auto& u : task.users) cout << u << " ";
        cout << "\n";
    }
}

void scheduleTasks(
    vector<Task>& tasks,
    unordered_map<string, User>& userMap,
    function<bool(const Task&, const Task&)> cmp
) {
    sort(tasks.begin(), tasks.end(), cmp);

    tm* current = getDate();
    char dateBuf[50];

    for (auto& task : tasks) {
        bool scheduled = false;
        for (int dayOffset = 0; dayOffset < 30 && !scheduled; ++dayOffset) {
            tm testDate = addDaysToDate(*current, dayOffset);
            for (int hour = 1; hour <= 24 - task.duration + 1 && !scheduled; ++hour) {
                // check every user’s availability
                bool allFree = true;
                for (auto& uname : task.users) {
                    auto it = userMap.find(uname);
                    if (it == userMap.end()) { allFree = false; break; }
                    bool freeThisSlot = false;
                    for (auto& iv : it->second.hours) {
                        if (iv.size()==2 &&
                            iv[0] <= hour &&
                            hour + task.duration <= iv[1]
                        ) { freeThisSlot = true; break; }
                    }
                    if (!freeThisSlot) { allFree = false; break; }
                }
                if (!allFree) continue;

                // everyone is free
                strftime(dateBuf, sizeof(dateBuf), "%B %e, %Y", &testDate);
                task.date       = string(dateBuf);
                task.startTime  = formatHour(hour);
                task.endTime    = formatHour(hour + task.duration);

                // carve that block out of each user availability
                for (auto& uname : task.users) {
                    auto& avail = userMap[uname].hours;
                    vector<vector<int>> next;
                    for (auto& iv : avail) {
                        int s = iv[0], e = iv[1];
                        if (s <= hour && hour + task.duration - 1 <= e) {
                            if (s < hour)
                                next.push_back({s, hour - 1});
                            if (hour + task.duration <= e)
                                next.push_back({hour + task.duration, e});
                        } else {
                            next.push_back(iv);
                        }
                    }
                    swap(avail, next);
                }

                scheduled = true;
            }
        }
        if (!scheduled) {
            task.date      = "Not Scheduled";
            task.startTime = task.endTime = "";
        }
    }
}

int main() {
    vector<User> userList = readUsers("users.csv");
    vector<Task> tasks = readTasks("tasks.csv");
    unordered_map<string, User> userMap;
    for (const auto& user : userList) {
    userMap[user.userName] = user;
    }

    cout << "Welcome to the Spring Schedule Spectacle program!" << endl;
    string userInput;
    char userInputFirstChar;

    cout << "Choose optimization method:\n"
         << "  1) By most participants per task\n"
         << "  2) By shortest tasks first\n"
         << "  3) By longest tasks first\n"
         << "Enter 1, 2 or 3: ";
    int choice;
    cin  >> choice;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    function<bool(const Task&, const Task&)> comparator;
    switch (choice) {
        case 2: comparator = compareByDurationAsc;  break;
        case 3: comparator = compareByDurationDesc; break;
        case 1:
        default: comparator = compareByUserCount;   break;
    }

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
            scheduleTasks(tasks, userMap, comparator);
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
