#ifndef CLASSES
#define CLASSES 
#include<string>
#include<vector>
struct User{
    std::string userName;
    std::vector<std::vector<int>> hours;
};
struct Task{
    std::string taskName;
    std::vector<std::string> users;
    std::string date;
    std::string startTime;
    std::string endTime;
    int duration;
};
struct TimeSlot{
    int hours;
    bool occupied;
};
#endif