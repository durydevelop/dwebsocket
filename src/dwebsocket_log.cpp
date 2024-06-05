#include "dwebsocket_log.h"
#include <cstdarg>
#include <cstdio>

namespace dws {

    #ifdef EMSCRIPTEN
        // No colors
        const char* CL_RED      =   "";
        const char* CL_GREEN    =   "";
        const char* CL_YELLOW   =   "";
        const char* CL_BLUE     =   "";
        const char* CL_MAGENTA  =   "";
        const char* CL_CYAN     =   "";
        const char* CL_DEFAULT  =   "";        
    #else
        //! Colors defines for printf
        const char* CL_RED      =   "\x1b[31m";
        const char* CL_GREEN    =   "\x1b[32m";
        const char* CL_YELLOW   =   "\x1b[33m";
        const char* CL_BLUE     =   "\x1b[34m";
        const char* CL_MAGENTA  =   "\x1b[35m";
        const char* CL_CYAN     =   "\x1b[36m";
        const char* CL_DEFAULT  =   "\x1b[0m";
    #endif

    void Log::info(const char *tag, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        printf("[%s%s%s][%sINFO%s] ",CL_GREEN,tag,CL_DEFAULT,CL_GREEN,CL_DEFAULT);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }

    void Log::debug(const char *tag, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        printf("[%s%s%s][%sDEBUG%s] ",CL_CYAN,tag,CL_DEFAULT,CL_CYAN,CL_DEFAULT);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }

    void Log::error(const char *tag, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        printf("[%s%s%s][%sERROR%s] ",CL_RED,tag,CL_DEFAULT,CL_RED,CL_DEFAULT);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }

    void Log::warning(const char *tag, const char *fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        printf("[%s%s%s][%sWARNING%s] ",CL_YELLOW,tag,CL_DEFAULT,CL_YELLOW,CL_DEFAULT);
        vprintf(fmt, args);
        printf("\n");
        va_end(args);
    }
}