#pragma once

//[INCLUDES]

//[NAMESPACE]
namespace Dogine
{
    namespace Log
    {
        //[FUNCTIONS]
        void Message(const char* _msg, ...);
        void Warning(const char* _msg, ...);
        void Error(const char* _msg, ...);
    }
}