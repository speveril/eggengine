#include <iostream>
#include <fstream>
#include <time.h>
#include <stdarg.h>
#include "core.h"

#define LOG_MAX_MSGLEN 4096

void Log::write(const char *msg, ...)
{
	static bool firstCall = true;

	va_list args;
	char outstr[LOG_MAX_MSGLEN];
	time_t t = time(NULL);
	tm lt;
	
	va_start(args, msg);
	vsprintf_s(outstr, LOG_MAX_MSGLEN, msg, args);
	va_end(args);

	if(firstCall) {
		firstCall = false;
		std::ofstream *f = new std::ofstream("log",std::ios::out);
		delete f;
	}

	std::ofstream *f = new std::ofstream("log",std::ios::app);
	localtime_s(&lt, &t);
	*f << "(" << (lt.tm_year+1900) << "/" << (lt.tm_mon < 9 ? "0" : "" ) << (lt.tm_mon + 1) << "/" << lt.tm_mday
		<< " " << (lt.tm_hour < 10 ? "0" : "" ) << lt.tm_hour << ":"
		<< (lt.tm_min < 10 ? "0" : "" ) << lt.tm_min << ":" << (lt.tm_sec < 10 ? "0" : "" ) << lt.tm_sec << ") "
		<< outstr << std::endl;
	*f << std::flush;

#ifdef DEBUG
	std::cout << "(" << (lt.tm_year+1900) << "/" << (lt.tm_mon < 9 ? "0" : "" ) << (lt.tm_mon + 1) << "/" << lt.tm_mday
		<< " " << (lt.tm_hour < 10 ? "0" : "" ) << lt.tm_hour << ":"
		<< (lt.tm_min < 10 ? "0" : "" ) << lt.tm_min << ":" << (lt.tm_sec < 10 ? "0" : "" ) << lt.tm_sec << ") "
		<< outstr << std::endl;
#endif

	delete f;
}

void Log::error(const char *msg, ...) {
	va_list args;
	char outstr[LOG_MAX_MSGLEN];

	va_start(args, msg);
	vsprintf_s(outstr, LOG_MAX_MSGLEN, msg, args);
	va_end(args);

	Log::write("ERROR: %s", outstr);
}

#ifdef DEBUG
void Log::debug(const char *msg, ...) {
	va_list args;
	char outstr[LOG_MAX_MSGLEN];

	va_start(args, msg);
	vsprintf_s(outstr, LOG_MAX_MSGLEN, msg, args);
	va_end(args);

	Log::write("DEBUG: %s", outstr);
}
#endif;
