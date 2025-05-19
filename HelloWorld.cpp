//TO GET UI IMPLEMENTATION CONTACT COLIN LIU @ colin.liuht@stu.austinisd.org

#include <iomanip>
#include <iostream>
#include <algorithm>
#include <functional>
#include "structures.h"
#include "DataRead.h"
#include <unordered_map>
#include <map>
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
    if (h == 0) {
        h = 12;
    }
    string suffix;
    if (hour24 < 12) {
        suffix = "AM";
    } else {
        suffix = "PM";
    }
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

string formatDate(const tm& date) {
    char output[50];
    strftime(output, sizeof(output), "%B %e, %Y", &date);
    return string(output);
}

void displaySchedule(const vector<vector<Task>>& tasksByDay) {
    cout << "\nHere's your generated schedule:\n";
    cout << setw(20) << left << "Task" 
         << setw(15) << "Date" 
         << setw(12) << "Start" 
         << setw(12) << "End" 
         << setw(8)  << "Dur(h)" 
         << "Assigned Users\n";
    cout << "-------------------------------------------------------------------\n";

    for (const auto& dayTasks : tasksByDay) {
        for (const auto& task : dayTasks) {
            cout << setw(20) << left << task.taskName
                 << setw(15) << left << task.date
                 << setw(12) << left << task.startTime
                 << setw(12) << left << task.endTime
                 << setw(8)  << left << task.duration;
            cout << "  Users: ";
            for (const auto& u : task.users) cout << u << " ";
            cout << "\n";
        }
    }
}


bool isFree(const User& user, int hour, int duration) {
    for (const auto& interval : user.hours) {
        if (hour + duration <= interval[1] &&
            interval[0] <= hour) {
            return true;
        }
    }
    return false;
}

bool areAllUsersFree(const vector<string>& users, unordered_map<string, User>& userMap, int hour, int duration) {
    for (const auto& name : users) {
        auto i = userMap.find(name);
        if (i == userMap.end() || !isFree(i->second, hour, duration)) {
            return false;
        }
    }
    return true;
}

void reserveTime(const vector<string>& users, unordered_map<string, User>& userMap, int hour, int duration) {
    for (const auto& user : users) {
        auto& i = userMap[user].hours;
        vector<vector<int>> u;

        for (const auto& interval : i) {
            int start = interval[0];
            int end = interval[1];

            if (start <= hour && hour+duration - 1 <= end) {
                if (start < hour)
                    u.push_back({start, hour - 1});
                if (hour + duration <= end)
                    u.push_back({hour + duration, end});
            } else {
                u.push_back(interval);
            }
        }

        i = u;
    }
}

void scheduleTasksByDay(
    vector<vector<Task>>& tasksByDay,
    unordered_map<string, User>& userMap,
    function<bool(const Task&, const Task&)> cmp,
    const unordered_map<string, vector<vector<int>>>& userOriginalHours
)
 {
    tm* current = getDate();

    for (int i = 0; i < (int)tasksByDay.size(); ++i) {
        for (auto& p : userMap) {
            const string& userName = p.first;
            User& user = p.second;
            user.hours = userOriginalHours.at(userName);
        }

        auto& dayTasks = tasksByDay[i];
        sort(dayTasks.begin(), dayTasks.end(), cmp);
        for (auto& task : dayTasks) {
            bool scheduled = false;
            tm testDate = addDaysToDate(*current, i); 
            int hour = 1;
            while (hour <= 24 - task.duration + 1 && !scheduled) {
                if (!areAllUsersFree(task.users, userMap, hour, task.duration)) {
                    ++hour;
                    continue;
                }
                task.date      = formatDate(testDate);
                task.startTime = formatHour(hour);
                task.endTime   = formatHour(hour + task.duration);
                reserveTime(task.users, userMap, hour, task.duration);
                scheduled = true;
                hour++;
            }

            if (!scheduled) {
                task.date = "Not Scheduled";
                task.startTime = task.endTime = "";
            }
        }
    }
}

int main() {
    vector<User> userList = readUsers("users.csv");
    vector<Task> tasks = readTasks("tasks.csv");
    vector<vector<Task>> tasksByDay(31);

    for (const auto &task : tasks)
    {
        tasksByDay[task.day - 1].push_back(task);
    }
    unordered_map<string, User> userMap;
    for (const auto &user : userList)
    {
        userMap[user.userName] = user;
    }
    unordered_map<string, vector<vector<int>>> userOriginalHours;
    for (const auto &user : userList)
    {
        userOriginalHours[user.userName] = user.hours;
    }

    cout << "Welcome to the Spring Schedule Spectacle program!" << endl;
    string userInput;
    char userInputFirstChar;

    function<bool(const Task&, const Task&)> comparator = compareByUserCount;

    while (userInputFirstChar != 'e') {
        cout << endl
                << "What would you like to do?" << endl
                << "- change user file: \"U [filename]\"" << endl
                << "- change task file: \"F [filename]\"" << endl
                << "- change priority method: \"P [option]\"" << endl
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
        else if (userInputFirstChar == 'p') {
            int optIndex = userInput.find(' ');
            string optionStr = userInput.substr(optIndex+1);

            if (optionStr == userInput || optionStr == "") {
                cout << endl
                        << "please input an option. available options:" << endl
                        << "highest participants first: \"P\"" << endl
                        << "lowest task duration first: \"L\"" << endl
                        << "highest task duration first: \"H\"" << endl;
            } else {
                switch (optionStr[0] | 32) {
                    case 'p':
                        comparator = compareByUserCount;
                        break;
                    case 'l':
                        comparator = compareByDurationAsc;
                        break;
                    case 'h':
                        comparator = compareByDurationDesc;
                        break;

                    default:
                        cout << endl << "invalid option." << endl;
                        break;
                }
            }
        }
        else if (userInputFirstChar == 't') {
            scheduleTasksByDay(tasksByDay, userMap, comparator, userOriginalHours);
            displaySchedule(tasksByDay);
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


//Big chungus
