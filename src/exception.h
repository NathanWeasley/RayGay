
#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
#include <stdint.h>

/**
 * This macro is used to actually throw an exception. It is used
 * in order to make sure that source code file and line is stored correctly.
 */
#define throw_exception(s) throw Exception(s,__FILE__,__LINE__)

/**
 * All exceptions throw this class.
 */
class Exception {

    public:
	Exception(std::string message, const char* srcfile, uint32_t srcline);
	std::string getMessage() { return message; };
	std::string getSourceFile() { return sourcefile; };
	uint32_t getSourceLine() { return sourceline; };

    private:
	std::string message;
	std::string sourcefile;
	uint32_t sourceline;
};

#endif
