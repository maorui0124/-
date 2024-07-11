#include "TaskManager.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <algorithm>

TaskManager::TaskManager(const std::string& filepath) : filepath(filepath) {
    loadTasks();
}

bool TaskManager::addTask(const Task& task) {
    if (!isUnique(task)) return false;
    tasks.push_back(task);
    saveTasks();
    return true;
}

bool TaskManager::deleteTask(int taskId) {
    auto it = std::remove_if(tasks.begin(), tasks.end(), [taskId](const Task& task) { return task.id == taskId; });
    if (it == tasks.end()) return false;
    tasks.erase(it, tasks.end());
    saveTasks();
    return true;
}

std::vector<Task> TaskManager::getTasksByDate(const std::tm& date) const {
    std::vector<Task> result;
    for (const auto& task : tasks) {
        std::tm taskTime = *std::localtime(&task.startTime);
        if (taskTime.tm_year == date.tm_year && taskTime.tm_mon == date.tm_mon && taskTime.tm_mday == date.tm_mday) {
            result.push_back(task);
        }
    }
    std::sort(result.begin(), result.end(), [](const Task& a, const Task& b) { return a.startTime < b.startTime; });
    return result;
}

void TaskManager::loadTasks() {
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        Task task;
        std::string startTimeStr, remindTimeStr;
        int priority, category;
        if (!(iss >> task.id >> task.name >> startTimeStr >> priority >> category >> remindTimeStr)) { break; }
        task.startTime = stringToTime(startTimeStr);
        task.remindTime = stringToTime(remindTimeStr);
        task.priority = static_cast<Priority>(priority);
        task.category = static_cast<Category>(category);
        tasks.push_back(task);
    }
}

void TaskManager::saveTasks() {
    std::ofstream file(filepath);
    for (const auto& task : tasks) {
        file << task.id << " " << task.name << " " << timeToString(task.startTime) << " "
             << task.priority << " " << task.category << " " << timeToString(task.remindTime) << std::endl;
    }
}

int TaskManager::generateTaskId() {
    static int idCounter = 0;
    return ++idCounter;
}

bool TaskManager::isUnique(const Task& task) const {
    for (const auto& t : tasks) {
        if (t.name == task.name && t.startTime == task.startTime) {
            return false;
        }
    }
    return true;
}
