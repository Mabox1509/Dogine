//[INCLUDES]
#include "../dogine.h"
#include <iostream>
#include <cstdarg>
#include <ctime>
#pragma warning(disable : 4996) //_CRT_SECURE_NO_WARNINGS


//[FUNCTIONS]
std::string GetTimestamp()
{
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    char buffer[10];
    std::strftime(buffer, sizeof(buffer), "%H:%M:%S", localTime);
    return std::string(buffer);
}
// Function to handle formatted output
void LogMessage(const char* type, const char* color, const char* format, va_list args)
{
    printf("\033[%sm[%s][%s]: ", color, GetTimestamp().c_str(), type);
    vprintf(format, args);
    printf("\033[0m\n");
}




//[IMPLEMETATIONS]
void Log::Message(std::string _msg, ...)
{
    va_list args;
    va_start(args, _msg);
    LogMessage("MESSAGE", "37", _msg.c_str(), args); // Light gray
    va_end(args);
}

void Log::Warning(std::string _msg, ...)
{
    va_list args;
    va_start(args, _msg);
    LogMessage("WARNING", "33", _msg.c_str(), args); // Yellow
    va_end(args);
}

void Log::Error(std::string _msg, ...)
{
    va_list args;
    va_start(args, _msg);
    LogMessage("ERROR", "31", _msg.c_str(), args); // Red
    va_end(args);
}