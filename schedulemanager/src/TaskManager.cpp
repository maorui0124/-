#include "TaskManager.h"
#include "Util.h"
#include <fstream>
#include <sstream>
#include <algorithm>
#include <iostream>
#include <nlohmann/json.hpp>

// 使用命名空间以便于操作
using json = nlohmann::json;

TaskManager::TaskManager(const std::string& filepath) : filepath(filepath) {
    loadTasks();
}

bool TaskManager::addTask(const Task& task) {
    // loadTasks();
    if (task.startTime == std::time_t(-1) || task.remindTime == std::time_t(-1)) {
        std::cerr << "Error: Invalid time value for task" << std::endl;
        return false;
    }
    // if (!isStartTimeUnique(task.startTime)) {
    //     std::cerr << "Error: Task start time must be unique" << std::endl;
    //     return false;
    // }
    if (!isUnique(task)) {
        std::cerr << "Error: Task name and start time combination must be unique" << std::endl;
        return false;
    }
    tasks.push_back(task);
    for (const auto& task : tasks) {
        std::cout<<task.name;
    }
    saveTasks();
    return true;
}

bool TaskManager::deleteTask(const std::string& taskName) {
    auto it = std::remove_if(tasks.begin(), tasks.end(), [&taskName](const Task& task) {
        return task.name == taskName;
    });
    if (it == tasks.end()) {
        return false; // 如果没有找到，返回 false
    }
    tasks.erase(it, tasks.end()); // 删除找到的任务
    saveTasks(); // 保存修改后的任务列表
    return true; // 成功删除任务
}


std::vector<Task> TaskManager::getTasksByDate(const std::tm& date) const {
    std::vector<Task> result;
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open task file for reading: " << filepath << std::endl;
        return result;
    }

    json j;
    file >> j;
    // std::cout<<date.tm_year<<std::endl;
    // std::cout<<date.tm_mon<<std::endl;
    // std::cout<<date.tm_mday<<std::endl;

    for (const auto& item : j) {
        Task task;
        task.id = item["id"];
        task.name = item["name"];
        task.startTime = stringToTime(item["startTime"].get<std::string>());
        task.priority = static_cast<Priority>(item["priority"].get<int>());
        task.category = static_cast<Category>(item["category"].get<int>());
        task.remindTime = stringToTime(item["remindTime"].get<std::string>());

        std::tm taskTime = *std::localtime(&task.startTime);
        if((taskTime.tm_year == date.tm_year || date.tm_year == 0) && (taskTime.tm_mon == date.tm_mon || date.tm_mon == 0) && (taskTime.tm_mday == date.tm_mday || date.tm_mday == 0)) {
            result.push_back(task);
        }
    }

    file.close();
    return result;
}




void TaskManager::loadTasks() {
    std::ifstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open task file for reading: " << filepath << std::endl;
        return;
    }

    nlohmann::json j;
    try {
        file >> j;  // 直接从文件流中读取 JSON 数据
    } catch (nlohmann::json::parse_error& e) {
        std::cerr << "JSON parsing error: " << e.what() << std::endl;
        return;
    }

    for (const auto& item : j) {
        Task task;
        try {
            task.id = item.at("id").get<int>();
            task.name = item.at("name").get<std::string>();
            task.startTime = stringToTime(item.at("startTime").get<std::string>());
            task.remindTime = stringToTime(item.at("remindTime").get<std::string>());
            task.priority = static_cast<Priority>(item.at("priority").get<int>());
            task.category = static_cast<Category>(item.at("category").get<int>());
        } catch (nlohmann::json::exception& e) {
            std::cerr << "Error processing tasks: " << e.what() << std::endl;
            continue;  // 跳过错误的数据项
        }
        tasks.push_back(task);
    }
    file.close();
}

void TaskManager::saveTasks() {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        std::cerr << "Error: Could not open task file for writing: " << filepath << std::endl;
        return;
    }
    json j;
    for (const auto& task : tasks) {
        // std::cout<<task.name;
        j.push_back({
            {"id", task.id},
            {"name", task.name},
            {"startTime", timeToString(task.startTime)},
            {"priority", static_cast<int>(task.priority)},
            {"category", static_cast<int>(task.category)},
            {"remindTime", timeToString(task.remindTime)}
        });
    }

    file << j.dump(4); // 输出 JSON，缩进 4 个空格
    file.close();
}

std::vector<Task> TaskManager::getAllTasks() const {
    return tasks;
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

bool TaskManager::isStartTimeUnique(const std::time_t& startTime) const {
    for (const auto& t : tasks) {
        if (t.startTime == startTime) {
            return false;
        }
    }
    return true;
}





