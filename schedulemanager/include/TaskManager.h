#ifndef TASK_MANAGER_H
#define TASK_MANAGER_H

#include "Task.h"
#include <vector>
#include <string>

class TaskManager {
public:
    TaskManager(const std::string& filepath);
    bool addTask(const Task& task);
    bool deleteTask(int taskId);
    std::vector<Task> getTasksByDate(const std::tm& date) const;
    void loadTasks();
    void saveTasks();
    int generateTaskId();
private:
    std::string filepath;
    std::vector<Task> tasks;
    
    bool isUnique(const Task& task) const;
};

#endif // TASK_MANAGER_H