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
#include <filesystem>
#include <cstdlib>
namespace fs = std::filesystem;

void displayHelp() {
    std::cout << "账户注册：./schedule_manager register <username> <password>\n";
    std::cout << "注册后可选择直接登录或不直接登陆进行操作：\n";
    std::cout << "若选择登录：\n";
    std::cout << "./schedule_manager run\n";
    std::cout << "随后输入账号密码，并根据提示和需要逐行输入\n";
    std::cout << "添加任务：addtask\n";
    std::cout << "展示任务：showtask\n";
    std::cout << "删除任务：deltask\n";
    std::cout << "退出：exit\n";
    std::cout << "若选择不登陆，可通过以下命令进行操作：\n";
    std::cout << "./schedule_manager addtask <username> <password> <taskname> <starttime> [<priority>] [<category>] [<remindtime>]（其中<starttime>和<remindtime>的格式为”YYYY-MM-DD HH:MM:SS”）\n";
    std::cout << "./schedule_manager showtask <username> <password> <date>\n";
    std::cout << "./schedule_manager deltask <username> <password> <taskid>\n";
}

void checkReminders(TaskManager& taskManager) {
    while (true) {
        auto now = std::time(nullptr);
        for (const auto& task : taskManager.getAllTasks()) {
            if (task.remindTime <= now && task.remindTime > now - 5) {
                std::cout << "Reminder: " << task.name << " is scheduled to start soon!\n";
                std::system("paplay /usr/share/sounds/freedesktop/stereo/alarm-clock-elapsed.oga"); // 播放系统提示音
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
        // std::cout << "Login failed.\n";
        return;
    }

    TaskManager taskManager(accountManager.getUserTaskFilePath(username));
    std::cout << "User " << username << " logged in." << std::endl;
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
            // std::cout << "Press ENTER to continue...";
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');  // 等待并忽略第一次回车

            std::string dateStr;
            std::cout << "Enter date (YYYY-MM-DD) or press ENTER to show all tasks: ";
            std::getline(std::cin, dateStr); // 第二次回车用来接受日期或为空

            std::tm date = {};
            if (dateStr.empty()) {
                // std::cout << "Default date set to 1900-01-01.\n";
                date.tm_year = 0;  
                date.tm_mon = 0;    
                date.tm_mday = 0;  
            } else {
                std::istringstream ss(dateStr);
                ss >> std::get_time(&date, "%Y-%m-%d");
                if (ss.fail()) {
                    std::cout << "Failed to parse date. Please enter a valid date in the format YYYY-MM-DD.\n";
                    continue;
                }
            }

            auto tasks = taskManager.getTasksByDate(date);
            if (tasks.empty()) {
                std::cout << "No tasks found for this date.\n";
            } else {
                for (const auto& task : tasks) {
                    std::cout << task.toString() << std::endl;
                }
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

    AccountManager accountManager("");

    fs::path currentPath = fs::current_path();
    std::string currentPathStr = currentPath.string();
    // std::cout<<"currentPathStr:";
    // std::cout<<currentPathStr;
    if (currentPathStr.find("build") != std::string::npos) {
        // std::cout<<"good!";
        accountManager = AccountManager("data/accounts.txt");
    } else {
        accountManager = AccountManager("build/data/accounts.txt");
    }

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
        int taskId = std::stoi(argv[4]);
        if (!accountManager.login(username, password)) {
            std::cout << "Login failed.\n";
            return 1;
        }
        TaskManager taskManager(accountManager.getUserTaskFilePath(username));
        if (taskManager.deleteTask(taskId)) {
            std::cout << "Task deleted successfully.\n";
        } else {
            std::cout << "Failed to delete task. Task ID not found.\n";
        }
    } else {
        displayHelp();
    }

    return 0;
}
