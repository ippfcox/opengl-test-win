#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <stdlib.h>
#include "log.h"

// modified: 2024-8-22 21:16

static enum log_level log_level_ = LOG_LEVEL_WARN;
static char fmt_str[2048];
static int level_colors[] = {37, 34, 33, 31, 31};
static char *level_names[] = {"DEBUG", "INFO ", "WARN ", "ERROR", "FATAL"};
char time_str[32];

void set_log_level(enum log_level level)
{
    log_level_ = level;
}

void log_(enum log_level level, const char *file, int line, const char *func, const char *fmt, ...)
{
    if (level < log_level_)
        return;

    time_t t = time(NULL);
#if defined(_WIN32)
    struct tm tm;
    localtime_s(&tm, &t);
    strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", &tm);
#elif defined(__linux__)
    strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", localtime(&t));
#else
    strftime(time_str, 32, "%Y-%m-%d %H:%M:%S", localtime(&t));
#endif

    char *pos = strrchr(file, '/');
    snprintf(fmt_str, 2048, "\033[1;%dm[%s]\033[0m \033[32m%s \033[35m%s:%d \033[36m%s()\033[0m ",
        level_colors[level], level_names[level], time_str, pos ? pos + 1 : file, line, func);
    va_list args;

    va_start(args, fmt);
    vsnprintf(fmt_str + strlen(fmt_str), 2048, fmt, args);
    va_end(args);

    if (fmt_str[strlen(fmt_str) - 1] != '\n')
#if defined(_WIN32)
        strncat_s(fmt_str, 2048, "\n", 1);
#elif defined(__linux__)
        strncat(fmt_str, "\n", 2);
#else
        strncat(fmt_str, "\n", 2);
#endif

    fprintf(stderr, "%s", fmt_str);

    if (level == LOG_LEVEL_FATAL)
        exit(-1);
}
