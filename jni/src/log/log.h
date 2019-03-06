#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>


#define LOG_D(format, ...) zLogPrint_debug(__FILE__, __FUNCTION__, __LINE__, format, ##__VA_ARGS__)

int zLogPrint_debug(const char *filename, const char *func, int line, const char *format, ...);

