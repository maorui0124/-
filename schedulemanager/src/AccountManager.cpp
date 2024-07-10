#include "AccountManager.h"
#include "Util.h"
#include <fstream>
#include <iostream>
#include <sstream>

AccountManager::AccountManager(const std::string& filepath) : filepath(filepath) {
    loadAccounts();
}

bool AccountManager::login(const std::string& username, const std::string& password) {
    auto it = accounts.find(username);
    if (it != accounts.end() && it->second == hashPassword(password)) {
        return true;
    }
    return false;
}

bool AccountManager::registerAccount(const std::string& username, const std::string& password) {
    if (accounts.find(username) != accounts.end()) {
        return false; // Account already exists
    }
    accounts[username] = hashPassword(password);
    saveAccounts();
    return true;
}

std::string AccountManager::hashPassword(const std::string& password) {
    return hashString(password);
}

void AccountManager::loadAccounts() {
    std::ifstream file(filepath);
    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        std::string username, hashedPassword;
        if (!(iss >> username >> hashedPassword)) { break; }
        accounts[username] = hashedPassword;
    }
}

void AccountManager::saveAccounts() {
    std::ofstream file(filepath);
    for (const auto& account : accounts) {
        file << account.first << " " << account.second << std::endl;
    }
}
