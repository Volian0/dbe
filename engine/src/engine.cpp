#include <stdio.h>

#include "engine.hpp"
#include "logger.hpp"

void print_hello() {
	log(LOG_INFO, "Hello, this is some info");
	log(LOG_ERROR, "Hey, this is an error");
	log(LOG_WARNING, "I'm a warning");
}
