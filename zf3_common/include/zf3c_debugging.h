#pragma once

#include <stdio.h>
#include <stdarg.h>

namespace zf3 {

void log(const char* const format, ...);
void log_error(const char* const format, ...);

}
