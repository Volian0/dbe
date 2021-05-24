#pragma once

enum LogSeverity {
	LOG_INFO,
	LOG_WARNING,
	LOG_ERROR
};

void log(LogSeverity severity, const char* fmt, ...);
