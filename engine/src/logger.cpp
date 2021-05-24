#include <stdarg.h>
#include <stdio.h>

#include "logger.hpp"

/* TODO: Use Win32 API for coloured console
 * output on Windows, since it doesn't support
 * ANSI colour codes */
#ifdef _WIN32
	#define CONSOLE_COLOR_RESET ""
	#define CONSOLE_COLOR_GREEN ""
	#define CONSOLE_COLOR_RED ""
	#define CONSOLE_COLOR_PURPLE ""
	#define CONSOLE_COLOR_CYAN ""
#else
	#define CONSOLE_COLOR_RESET "\033[0m"
	#define CONSOLE_COLOR_GREEN "\033[1;32m"
	#define CONSOLE_COLOR_RED "\033[1;31m"
	#define CONSOLE_COLOR_PURPLE "\033[1;35m"
	#define CONSOLE_COLOR_CYAN "\033[0;36m"
#endif

void log(LogSeverity severity, const char* fmt, ...) {
	const char* type;
	const char* color;

	switch (severity) {
		case LOG_INFO:
			type = "info";
			color = CONSOLE_COLOR_GREEN;
			break;
		case LOG_ERROR:
			type = "error";
			color = CONSOLE_COLOR_RED;
			break;
		case LOG_WARNING:
			type = "warning";
			color = CONSOLE_COLOR_PURPLE;
			break;
		default:
			break; /* Unreachable */
	}

	char final_format[1024];
	snprintf(final_format, 1024, "%s%s%s: %s\n", color, type,
		CONSOLE_COLOR_RESET, fmt);

	va_list argptr;
	va_start(argptr, fmt);
	vprintf(final_format, argptr);
	va_end(argptr);
}
