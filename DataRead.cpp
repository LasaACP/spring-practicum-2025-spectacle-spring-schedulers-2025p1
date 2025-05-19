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

        // Task name
        if (commaIndex == 0) {
            task.taskName += toFormat[i];
        }

        // Users
        else if (commaIndex == 1) {
            if (toFormat[i] == '{') continue;
            if (toFormat[i] == '}' || toFormat[i] == ';') {
                task.users.push_back(readBuffer);
                readBuffer = "";
            } else {
                readBuffer += toFormat[i];
            }
        }

        // Duration
        else if (commaIndex == 2) {
            readBuffer += toFormat[i];
        }

        // Day
        else if (commaIndex == 3) {
            task.duration = stoi(readBuffer);
            readBuffer = "";
            readBuffer += toFormat[i];  // collect day number
        }
    }

    task.day = stoi(readBuffer);  // readBuffer now contains day (e.g., "3")

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


        if (commaIndex == 0) {
            user.userName += toFormat[i];
        }


        if (commaIndex == 1) {
            if (toFormat[i] == '{') {
                hoursBuffer.clear();
                continue;
            }


            if (toFormat[i] == '}') {
                hoursBuffer.push_back(stoi(readBuffer)); // <-- Push final number
                readBuffer = "";
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


vector<Task> readTasks(string inputFile) {
    ifstream taskInput (inputFile);
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

vector<User> readUsers(string inputFile) {
    ifstream userInput (inputFile);
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

bool fileExists(string inputFile) {
    ifstream file (inputFile);
    bool exists = file.is_open();
    file.close();
    return exists;
}