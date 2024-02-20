#ifndef COMMAND_LINE_H
#define COMMAND_LINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

char *get_argument_value(int argc, char *argv[], const char *flag);
bool argument_exists(int argc, char *argv[], const char *flag);

#endif // COMMAND_LINE_H