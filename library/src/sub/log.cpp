//[INCLUDES]
#include <dogine/log.hpp>

#include <iostream>
#include <cstdarg>
#include <cstdio>
#include <chrono>
#include <iomanip>


//[PRIVATE TYPES]
enum class LogType : uint8_t
{
    Message,
    Warning,
    Error
};

//[PRIVATE VARIABLES]

//[PRIVATE FUNCTIONS]
static void PrintLog(LogType type, const char* msg, va_list args)
{
    // Obtener hora actual
    auto _now = std::chrono::system_clock::now();
    auto _t = std::chrono::system_clock::to_time_t(_now);
    std::tm _tm_now;
#if defined(_WIN32)
    localtime_s(&_tm_now, &_t);
#else
    localtime_r(&_t, &_tm_now);
#endif

    // Prefijo de color según tipo
    const char* _colorStart = "";
    const char* _colorEnd = "\033[0m";

    const char* _typeStr = "";
    switch(type)
    {
        case LogType::Message:
            _colorStart = "\033[37m"; // blanco
            _typeStr = "MESSAGE";
            break;
        case LogType::Warning:
            _colorStart = "\033[33m"; // amarillo
            _typeStr = "WARNING";
            break;
        case LogType::Error:
            _colorStart = "\033[31m"; // rojo
            _typeStr = "ERROR";
            break;
    }

    // Imprimir cabecera: [HH:MM:ss][TYPE]:
    std::printf("%02d:%02d:%02d", _tm_now.tm_hour, _tm_now.tm_min, _tm_now.tm_sec);
    std::printf("[%s%s%s]: ", _colorStart, _typeStr, _colorEnd);

    // Imprimir mensaje con formato printf
    std::vprintf(msg, args);

    // Salto de línea
    std::printf("\n");
}

//[NAMESPACE]
namespace Dogine
{
    namespace Log
    {
        //[FUNCTIONS]
        void Message(const char* _msg, ...)
        {
            va_list _args;
            va_start(_args, _msg);
            PrintLog(LogType::Message, _msg, _args);
            va_end(_args);
        }
        void Warning(const char* _msg, ...)
        {
            va_list _args;
            va_start(_args, _msg);
            PrintLog(LogType::Warning, _msg, _args);
            va_end(_args);
        }
        void Error(const char* _msg, ...)
        {
            va_list _args;
            va_start(_args, _msg);
            PrintLog(LogType::Error, _msg, _args);
            va_end(_args);
        }

    } // namespace Log
} // namespace Dogine