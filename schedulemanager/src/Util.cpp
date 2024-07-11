#include "Util.h"
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <ostream>
#include <iostream>

std::string hashString(const std::string& str) {
    unsigned char hash[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)str.c_str(), str.size(), hash);
    std::stringstream ss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        ss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
    }
    return ss.str();
}

std::string timeToString(const std::time_t& time) {
    std::tm* tm = std::localtime(&time);
    char buf[80];
    strftime(buf, sizeof(buf), "%Y-%m-%d %H:%M:%S", tm);
    return std::string(buf);
}

std::time_t stringToTime(const std::string& str) {
    std::tm tm = {};
    std::istringstream ss(str);
    ss >> std::get_time(&tm, "%Y-%m-%d %H:%M:%S");
    if (ss.fail()) {
        std::cerr << "Error: Failed to parse date/time string: " << str << std::endl;
        return std::time_t(-1); // return an invalid time_t value
    }
    return std::mktime(&tm);
}

