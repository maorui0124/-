#include "Task.h"
#include "Util.h"

Task::Task() : id(0), startTime(0), priority(MEDIUM), category(LIFE), remindTime(0) {}

Task::Task(int id, const std::string& name, std::time_t startTime, Priority priority, Category category, std::time_t remindTime)
    : id(id), name(name), startTime(startTime), priority(priority), category(category), remindTime(remindTime) {}

std::string Task::toString() const {
    return std::to_string(id) + " " + name + " " + timeToString(startTime) + " " + std::to_string(priority) + " " + std::to_string(category) + " " + timeToString(remindTime);
}
