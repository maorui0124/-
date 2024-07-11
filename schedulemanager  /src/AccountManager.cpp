#include "AccountManager.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include <sstream>

AccountManager::AccountManager(const std::string& accountFilePath) : accountFilePath(accountFilePath) {
    loadAccounts();
}

bool AccountManager::login(const std::string& username, const std::string& password) {
    auto it = accounts.find(username);
    std::string hashedPassword = hashPassword(password);
    std::cout << "Debug: Trying to login with username: " << username << " and hashed password: " << hashedPassword << std::endl;
    if (it != accounts.end() && it->second == hashedPassword) {
        std::cout << "Debug: Login successful" << std::endl;
        return true;
    }
    std::cout << "Debug: Login failed" << std::endl;
    return false;
}

bool AccountManager::registerAccount(const std::string& username, const std::string& password) {
    if (accounts.find(username) != accounts.end()) {
        return false; // Account already exists
    }
    std::string hashedPassword = hashPassword(password);
    accounts[username] = hashedPassword;
    saveAccounts();
    std::cout << "Debug: Registered account with username: " << username << " and hashed password: " << hashedPassword << std::endl;
    return true;
}

std::string AccountManager::hashPassword(const std::string& password) {
    std::string hash = hashString(password);
    std::cout << "Debug: Hashing password: " << password << " to: " << hash << std::endl;
    return hash;
}

void AccountManager::loadAccounts() {
    std::ifstream file(accountFilePath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, hashedPassword;
        if (!(iss >> username >> hashedPassword)) { break; }
        accounts[username] = hashedPassword;
    }
    std::cout << "Debug: Loaded accounts: " << std::endl;
    for (const auto& account : accounts) {
        std::cout << "Debug: " << account.first << " -> " << account.second << std::endl;
    }
}

void AccountManager::saveAccounts() {
    std::ofstream file(accountFilePath);
    for (const auto& account : accounts) {
        file << account.first << " " << account.second << std::endl;
    }
    std::cout << "Debug: Saved accounts to file" << std::endl;
}

std::string AccountManager::getUserTaskFilePath(const std::string& username) const {
    return "data/tasks_" + username + ".txt";
}


