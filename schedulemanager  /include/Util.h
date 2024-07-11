#ifndef UTIL_H
#define UTIL_H

#include <ctime>
#include <string>

std::string hashString(const std::string& str);
std::string timeToString(const std::time_t& time);
std::time_t stringToTime(const std::string& str);

#endif // UTIL_H

