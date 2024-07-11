#ifndef TASK_H
#define TASK_H

#include <string>
#include <ctime>

enum Priority { LOW, MEDIUM, HIGH };
enum Category { STUDY, ENTERTAINMENT, LIFE };

struct Task {
    int id;
    std::string name;
    std::time_t startTime;
    Priority priority;
    Category category;
    std::time_t remindTime;

    Task();
    Task(int id, const std::string& name, std::time_t startTime,
         Priority priority, Category category, std::time_t remindTime);
    std::string toString() const;
};

#endif // TASK_H
