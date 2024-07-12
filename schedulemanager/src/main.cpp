#include "AccountManager.h"
#include "TaskManager.h"
#include "Task.h"
#include "Util.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <sstream>
#include <map>
#include <iomanip>
#include <limits>

void displayHelp() {
    std::cout << "Usage: myschedule <command> [<args>]\n";
    std::cout << "Commands:\n";
    std::cout << "  run                  Run the schedule manager in interactive mode.\n";
    std::cout << "  register <username> <password>  Register a new account.\n";
    std::cout << "  addtask <username> <password> <taskname> <starttime> [<priority>] [<category>] [<remindtime>]\n";
    std::cout << "                       Add a task to the schedule.\n";
    std::cout << "  showtask <username> <password> <date>\n";
    std::cout << "                       Show tasks for a specific date.\n";
    std::cout << "  deltask <username> <password> <taskid>\n";
    std::cout << "                       Delete a task by ID.\n";
    std::cout << "  helptask             Display help information.\n";
}

void checkReminders(TaskManager& taskManager) {
    while (true) {
        auto now = std::time(nullptr);
        for (const auto& task : taskManager.getAllTasks()) {
            if (task.remindTime <= now && task.remindTime > now - 5) {
                std::cout << "Reminder: " << task.name << " is scheduled to start soon!\n";
            }
        }
        std::this_thread::sleep_for(std::chrono::seconds(5));
    }
}

void runInteractiveMode(AccountManager& accountManager) {
    std::string username, password;
    std::cout << "Enter username: ";
    std::cin >> username;
    std::cout << "Enter password: ";
    std::cin >> password;
    if (!accountManager.login(username, password)) {
        std::cout << "Login failed.\n";
        return;
    }

    TaskManager taskManager(accountManager.getUserTaskFilePath(username));
    std::cout << "Debug: User " << username << " logged in." << std::endl;
    std::cout << "Welcome, " << username << "! Type 'helptask' for help.\n";

    // Start reminder checking thread
    std::thread reminderThread(checkReminders, std::ref(taskManager));
    reminderThread.detach(); // Detach the thread to run in the background

    while (true) {
        std::string command;
        std::cout << "myschedule> ";
        std::cin >> command;
        if (command == "exit") {
            // taskManager.saveTasks(); // Ensure tasks are saved before exiting
            break;
        } else if (command == "addtask") {
            std::string taskname, starttime, remindtime;
            int priority = MEDIUM, category = LIFE;
            std::cout << "Enter task name: ";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            std::getline(std::cin, taskname);
            std::cout << "Enter start time (YYYY-MM-DD HH:MM:SS): ";
            std::getline(std::cin, starttime);
            std::cout << "Enter priority (0: LOW, 1: MEDIUM, 2: HIGH) [default is MEDIUM]: ";
            std::string priorityStr;
            std::getline(std::cin, priorityStr);
            if (!priorityStr.empty()) {
                priority = std::stoi(priorityStr);
            }
            std::cout << "Enter category (0: STUDY, 1: ENTERTAINMENT, 2: LIFE) [default is LIFE]: ";
            std::string categoryStr;
            std::getline(std::cin, categoryStr);
            if (!categoryStr.empty()) {
                category = std::stoi(categoryStr);
            }
            std::cout << "Enter remind time (YYYY-MM-DD HH:MM:SS) [default is start time]: ";
            std::getline(std::cin, remindtime);
            if (remindtime.empty()) {
                remindtime = starttime;
            }

            std::time_t startTimeParsed = stringToTime(starttime);
            std::time_t remindTimeParsed = stringToTime(remindtime);
            if (startTimeParsed == std::time_t(-1) || remindTimeParsed == std::time_t(-1)) {
                std::cout << "Error: Invalid time format. Failed to add task." << std::endl;
                continue;
            }

            Task task(taskManager.generateTaskId(), taskname, startTimeParsed, 
                      static_cast<Priority>(priority), static_cast<Category>(category), remindTimeParsed);
            if (taskManager.addTask(task)) {
                std::cout << "Task added successfully.\n";
            } else {
                std::cout << "Failed to add task. Task name and start time must be unique.\n";
            }
        } else if (command == "showtask") {
            std::string dateStr;
            std::cout << "Enter date (YYYY-MM-DD): ";
            std::cin >> dateStr;
            std::tm date = {};
            std::istringstream ss(dateStr);
            ss >> std::get_time(&date, "%Y-%m-%d");
            if (ss.fail()) {
                std::cout << "Failed to parse date.\n";
                continue;
            }

            auto tasks = taskManager.getTasksByDate(date);
            for (const auto& task : tasks) {
                std::cout << task.toString() << std::endl;
            }
        } else if (command == "deltask") {
            int taskId;
            std::cout << "Enter task ID: ";
            std::cin >> taskId;
            if (taskManager.deleteTask(taskId)) {
                std::cout << "Task deleted successfully.\n";
            } else {
                std::cout << "Failed to delete task. Task ID not found.\n";
            }
        } else if (command == "helptask") {
            displayHelp();
        } else {
            std::cout << "Unknown command. Type 'helptask' for help.\n";
        }
    }
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        displayHelp();
        return 1;
    }

    std::string command = argv[1];
    AccountManager accountManager("data/accounts.txt");

    if (command == "run") {
        runInteractiveMode(accountManager);
    } else if (command == "register" && argc >= 4) {
        std::string username = argv[2];
        std::string password = argv[3];
        if (accountManager.registerAccount(username, password)) {
            std::cout << "Account registered successfully.\n";
        } else {
            std::cout << "Account registration failed. Username may already exist.\n";
        }
    } else if (command == "addtask" && argc >= 7) {
        std::string username = argv[2];
        std::string password = argv[3];
        if (!accountManager.login(username, password)) {
            std::cout << "Login failed.\n";
            return 1;
        }
        TaskManager taskManager(accountManager.getUserTaskFilePath(username));
        std::string taskname = argv[4];
        std::string starttime = argv[5];
        int priority = argc > 6 ? std::stoi(argv[6]) : MEDIUM;
        int category = argc > 7 ? std::stoi(argv[7]) : LIFE;
        std::string remindtime = argc > 8 ? argv[8] : starttime;

        std::time_t startTimeParsed = stringToTime(starttime);
        std::time_t remindTimeParsed = stringToTime(remindtime);
        if (startTimeParsed == std::time_t(-1) || remindTimeParsed == std::time_t(-1)) {
            std::cout << "Error: Invalid time format." << std::endl;
            return 1;
        }

        Task task(taskManager.generateTaskId(), taskname, startTimeParsed, 
                  static_cast<Priority>(priority), static_cast<Category>(category), remindTimeParsed);
        if (taskManager.addTask(task)) {
            std::cout << "Task added successfully.\n";
        } else {
            std::cout << "Failed to add task.\n";
        }
    } else if (command == "showtask" && argc >= 5) {
        std::string username = argv[2];
        std::string password = argv[3];
        if (!accountManager.login(username, password)) {
            std::cout << "Login failed.\n";
            return 1;
        }
        TaskManager taskManager(accountManager.getUserTaskFilePath(username));
        std::string dateStr = argv[4];
        std::tm date = {};
        std::istringstream ss(dateStr);
        ss >> std::get_time(&date, "%Y-%m-%d");
        if (ss.fail()) {
            std::cout << "Failed to parse date.\n";
            return 1;
        }

        auto tasks = taskManager.getTasksByDate(date);
        for (const auto& task : tasks) {
            std::cout << task.toString() << std::endl;
        }
    } else if (command == "deltask" && argc >= 5) {
        std::string username = argv[2];
        std::string password = argv[3];
        if (!accountManager.login(username, password)) {
            std::cout << "Login failed.\n";
            return 1;
        }
        TaskManager taskManager(accountManager.getUserTaskFilePath(username));
        int taskId = std::stoi(argv[4]);
        if (taskManager.deleteTask(taskId)) {
            std::cout << "Task deleted successfully.\n";
        } else {
            std::cout << "Failed to delete task.\n";
        }
    } else {
        displayHelp();
    }

    return 0;
}







