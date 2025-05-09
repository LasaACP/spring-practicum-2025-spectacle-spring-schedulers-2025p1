#include <iostream>
#include <fstream>
#include "structures.h"
#include "DataRead.h"
using namespace std;

Task formatTask(string toFormat) {
    Task task;
    int commaIndex = 0;
    string readBuffer;

    for (int i = 0; i < toFormat.length(); i++) {
        if (toFormat[i] == ',') {
            commaIndex++;
            continue;
        }
        // reading name
        if (commaIndex == 0) {
            task.taskName += toFormat[i];
            continue;
        }
        // reading users
        if (commaIndex == 1) {
            if (toFormat[i] == '{') continue;
            if (toFormat[i] == '}' || toFormat[i] == ';') {
                task.users.push_back(readBuffer);
                readBuffer = "";
                continue;
            }
            readBuffer += toFormat[i];
            continue;
        }
        // reading duration
        if (commaIndex == 2) {
            readBuffer += toFormat[i];
            continue;
        }
    }
    task.duration = stoi(readBuffer);

    return task;
}

User formatUser(string toFormat) {
    User user;
    int commaIndex = 0;
    string readBuffer;
    
    vector<int> hoursBuffer;

    for (int i = 0; i < toFormat.length(); i++) {
        if (toFormat[i] == ',') {
            commaIndex++;
            continue;
        }
        // reading username
        if (commaIndex == 0) {
            user.userName += toFormat[i];
        }
        // reading availability
        if (commaIndex == 1) {
            if (toFormat[i] == '{') {
                hoursBuffer.clear();
                continue;
            }

            if (toFormat[i] == '}') {
                user.hours.push_back(hoursBuffer);
                continue;
            }

            if (toFormat[i] == ';') {
                hoursBuffer.push_back(stoi(readBuffer));
                readBuffer = "";
            } else {
                readBuffer += toFormat[i];
            }
        }
    }

    return user;
}

vector<Task> readTasks() {
    ifstream taskInput ("tasks.csv");
    vector<Task> tasks;

    string currentLine;
    int taskCount = 0;

    while (getline(taskInput,currentLine)) {
        taskCount++;
        if (taskCount == 1) continue; // skip first line
        tasks.emplace_back(formatTask(currentLine));
    }

    taskInput.close();
    return tasks;
}

vector<User> readUsers() {
    ifstream userInput ("users.csv");
    vector<User> users;

    string currentLine;
    int userCount = 0;

    while (getline(userInput,currentLine)) {
        userCount++;
        if (userCount == 1) continue; // skip first line
        users.emplace_back(formatUser(currentLine));
    }

    userInput.close();
    return users;
}