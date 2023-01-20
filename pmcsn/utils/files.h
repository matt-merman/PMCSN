#ifndef _FILES_H_
# define _FILES_H_

#include "constants.h"

#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdlib.h>

FILE *open_file(char *p, const char *name);
void	write_result(FILE *file, double value, int index);

#endif