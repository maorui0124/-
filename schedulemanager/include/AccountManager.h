#ifndef ACCOUNT_MANAGER_H
#define ACCOUNT_MANAGER_H

#include <string>
#include <unordered_map>

class AccountManager {
public:
    AccountManager(const std::string& accountFilePath);
    bool login(const std::string& username, const std::string& password);
    bool registerAccount(const std::string& username, const std::string& password);
    std::string getUserTaskFilePath(const std::string& username) const;
private:
    std::string hashPassword(const std::string& password);
    void loadAccounts();
    void saveAccounts();

    std::string accountFilePath;
    std::unordered_map<std::string, std::string> accounts;
};

#endif // ACCOUNT_MANAGER_H



